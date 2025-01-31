//
// Created by Igor on 27/11/2022.
//

#include <Graphics/Types/Geometry/SkinnedMesh.h>

namespace SR_GTYPES_NS {
    SR_REGISTER_COMPONENT(SkinnedMesh);

    SkinnedMesh::SkinnedMesh()
        : MeshComponent(MeshType::Skinned)
        , m_skeletonRef(GetThis())
    { }

    SkinnedMesh::~SkinnedMesh() {
        SetRawMesh(nullptr);
    }

    bool SkinnedMesh::Calculate()  {
        SR_TRACY_ZONE;

        if (IsCalculated()) {
            return true;
        }

        FreeVideoMemory();

        if (!IsCanCalculate()) {
            return false;
        }

        if (SR_UTILS_NS::Debug::Instance().GetLevel() >= SR_UTILS_NS::Debug::Level::High) {
            SR_LOG("SkinnedMesh::Calculate() : calculating \"" + m_geometryName + "\"...");
        }

        if (!CalculateVBO<Vertices::VertexType::SkinnedMeshVertex, Vertices::SkinnedMeshVertex>([this]() {
            return Vertices::CastVertices<Vertices::SkinnedMeshVertex>(GetVertices());
        })) {
            return false;
        }

        return IndexedMesh::Calculate();
    }

    void SkinnedMesh::Draw() {
        SR_TRACY_ZONE;

        auto&& pShader = GetRenderContext()->GetCurrentShader();

        if (!pShader || !IsActive()) {
            return;
        }

        if ((!IsCalculated() && !Calculate()) || m_hasErrors) {
            return;
        }

        if (!IsSkeletonUsable()) {
            return;
        }

        if (m_dirtyMaterial)
        {
            m_dirtyMaterial = false;

            m_virtualUBO = m_uboManager.ReAllocateUBO(m_virtualUBO, pShader->GetUBOBlockSize(), pShader->GetSamplersCount());

            if (m_virtualUBO != SR_ID_INVALID) {
                m_uboManager.BindUBO(m_virtualUBO);
            }
            else {
                m_pipeline->ResetDescriptorSet();
                m_hasErrors = true;
                return;
            }

            pShader->InitUBOBlock();
            pShader->Flush();

            m_material->UseSamplers();
        }

        switch (m_uboManager.BindUBO(m_virtualUBO)) {
            case Memory::UBOManager::BindResult::Duplicated:
                pShader->InitUBOBlock();
                pShader->Flush();
                m_material->UseSamplers();
                SR_FALLTHROUGH;
            case Memory::UBOManager::BindResult::Success:
                m_pipeline->DrawIndices(m_countIndices);
                break;
            case Memory::UBOManager::BindResult::Failed:
            default:
                break;
        }
    }

    SR_HTYPES_NS::Marshal::Ptr SR_GTYPES_NS::SkinnedMesh::Save(SR_HTYPES_NS::Marshal::Ptr pMarshal, SR_UTILS_NS::SavableFlags flags) const {
        pMarshal = MeshComponent::Save(pMarshal, flags);

        /// TODO: use unicode
        pMarshal->Write<std::string>(GetMeshStringPath());
        pMarshal->Write<int32_t>(GetMeshId());

        pMarshal->Write<std::string>(m_material ? m_material->GetResourceId() : "None");

        pMarshal = m_skeletonRef.Save(pMarshal);

        return pMarshal;
    }

    SR_UTILS_NS::Component* SR_GTYPES_NS::SkinnedMesh::LoadComponent(SR_HTYPES_NS::Marshal& marshal, const SR_HTYPES_NS::DataStorage *dataStorage) {
        const auto&& type = static_cast<MeshType>(marshal.Read<int32_t>());

        const auto&& path = marshal.Read<std::string>();
        const auto&& id = marshal.Read<uint32_t>();

        const auto&& material = marshal.Read<std::string>();

        if (id < 0) {
            return nullptr;
        }

        auto&& pMesh = dynamic_cast<SkinnedMesh*>(Mesh::Load(SR_UTILS_NS::Path(path, true), type, id));

        if (pMesh) {
            pMesh->GetSkeleton().Load(marshal);
        }

        if (pMesh && material != "None") {
            if (auto&& pMaterial = SR_GTYPES_NS::Material::Load(SR_UTILS_NS::Path(material, true))) {
                pMesh->SetMaterial(pMaterial);
            }
            else {
                SR_ERROR("SkinnedMesh::LoadComponent() : failed to load material! Name: " + material);
            }
        }

        return dynamic_cast<Component*>(pMesh);
    }

    std::vector<uint32_t> SkinnedMesh::GetIndices() const {
        return GetRawMesh()->GetIndices(GetMeshId());
    }

    bool SkinnedMesh::IsCanCalculate() const {
        return IsValidMeshId() && Mesh::IsCanCalculate();
    }

    bool SkinnedMesh::IsSkeletonUsable() const {
        return m_skeletonRef.GetComponent<SR_ANIMATIONS_NS::Skeleton>();
    }

    void SkinnedMesh::Update(float dt) {
        const bool usable = IsSkeletonUsable();

        if (m_skeletonIsBroken && !usable) {
            return MeshComponent::Update(dt);
        }

        if (!m_skeletonIsBroken && usable) {
            return MeshComponent::Update(dt);
        }

        m_skeletonIsBroken = !usable;
        m_renderScene->SetDirty();
        m_bonesIds.clear();

        MeshComponent::Update(dt);
    };

    void SkinnedMesh::UseMaterial() {
        MeshComponent::UseMaterial();
        UseModelMatrix();
    }

    void SkinnedMesh::UseModelMatrix() {
        SR_TRACY_ZONE;

        /// TODO: А не стоило бы изменить ColorBufferPass так, чтобы он вызывал не UseModelMatrix, а более обощённый метод?
        /// Нет, не стоило бы.
        if (!PopulateSkeletonMatrices()) {
            return;
        }

        switch (GetMaxBones()) {
            case 128:
                GetRenderContext()->GetCurrentShader()->SetCustom(SHADER_SKELETON_MATRICES_128, m_skeletonMatrices.data());
                GetRenderContext()->GetCurrentShader()->SetCustom(SHADER_SKELETON_MATRIX_OFFSETS_128, m_skeletonOffsets.data());
                break;
            case 256:
                GetRenderContext()->GetCurrentShader()->SetCustom(SHADER_SKELETON_MATRICES_256, m_skeletonMatrices.data());
                GetRenderContext()->GetCurrentShader()->SetCustom(SHADER_SKELETON_MATRIX_OFFSETS_256, m_skeletonOffsets.data());
                break;
            case 384:
                GetRenderContext()->GetCurrentShader()->SetCustom(SHADER_SKELETON_MATRICES_384, m_skeletonMatrices.data());
                GetRenderContext()->GetCurrentShader()->SetCustom(SHADER_SKELETON_MATRIX_OFFSETS_384, m_skeletonOffsets.data());
                break;
            case 0:
                break;
            default:
                SRHaltOnce0();
                return;
        }

        GetRenderContext()->GetCurrentShader()->SetMat4(SHADER_MODEL_MATRIX, m_modelMatrix);
    }

    void SkinnedMesh::OnResourceReloaded(SR_UTILS_NS::IResource* pResource) {
        if (GetRawMesh() == pResource) {
            OnRawMeshChanged();
            return;
        }
        Mesh::OnResourceReloaded(pResource);
    }

    bool SkinnedMesh::PopulateSkeletonMatrices() {
        SR_TRACY_ZONE;

        static SR_MATH_NS::Matrix4x4 identityMatrix = SR_MATH_NS::Matrix4x4().Identity();

        auto&& bones = GetRawMesh()->GetBones(GetMeshId());

        if (bones.empty()) {
            return false;
        }

        if (m_bonesIds.empty()) {
            const uint64_t bonesCount = SR_MAX(GetMaxBones(), bones.size());

            m_bonesIds.resize(bonesCount);
            m_skeletonOffsets.resize(bonesCount);
            m_skeletonMatrices.resize(bonesCount);

            /// for (uint64_t i = 0; i < bonesCount; ++i) {
            ///     m_skeletonMatrices[i] = identityMatrix;
            ///     m_skeletonOffsets[i] = identityMatrix;
            /// }
        }

        auto&& pSkeleton = m_skeletonRef.GetComponent<SR_ANIMATIONS_NS::Skeleton>();

        if (!pSkeleton || !pSkeleton->GetRootBone()) {
            return false;
        }

        if (!m_isOffsetsInitialized) {
            for (auto&& [hashName, boneId] : bones) {
                m_skeletonOffsets[boneId] = GetRawMesh()->GetBoneOffset(hashName);
                m_bonesIds[boneId] = pSkeleton->GetBoneIndex(hashName);
            }
            m_isOffsetsInitialized = true;
        }

        for (uint64_t boneId = 0; boneId < m_bonesIds.size(); ++boneId) {
            if (auto&& bone = pSkeleton->GetBoneByIndex(m_bonesIds[boneId]); bone && bone->gameObject) {
                m_skeletonMatrices[boneId] = bone->gameObject->GetTransform()->GetMatrix();
            }
            else {
                m_skeletonMatrices[boneId] = identityMatrix;
            }
        }

        return true;
    }

    void SkinnedMesh::OnRawMeshChanged() {
        IRawMeshHolder::OnRawMeshChanged();

        if (GetRawMesh() && IsValidMeshId()) {
            SetGeometryName(GetRawMesh()->GetGeometryName(GetMeshId()));
        }

        m_isOffsetsInitialized = false;

        MarkPipelineUnBuild();

        m_dirtyMaterial = true;
        m_isCalculated = false;
    }

    SR_UTILS_NS::Component* SkinnedMesh::CopyComponent() const {
        if (auto&& pComponent = dynamic_cast<SkinnedMesh*>(MeshComponent::CopyComponent())) {
            pComponent->SetRawMesh(GetRawMesh());
            pComponent->SetMeshId(GetMeshId());
            pComponent->m_skeletonRef = m_skeletonRef.Copy(pComponent->GetThis());
            return pComponent;
        }

        return nullptr;
    }

    std::string SkinnedMesh::GetMeshIdentifier() const {
        if (auto&& pRawMesh = GetRawMesh()) {
            return SR_UTILS_NS::Format("%s|%i|%i", pRawMesh->GetResourceId().c_str(), GetMeshId(), pRawMesh->GetReloadCount());
        }

        return MeshComponent::GetMeshIdentifier();
    }

    uint32_t SkinnedMesh::GetMaxBones() const {
        if (!GetRawMesh()) {
            return 0;
        }

        uint32_t bonesCount = GetRawMesh()->GetBones(GetMeshId()).size();

        if (bonesCount > 256) {
            return 384;
        }
        else if (bonesCount > SR_HUMANOID_MAX_BONES) {
            return 256;
        }
        else {
            return SR_HUMANOID_MAX_BONES;
        }
    }
}