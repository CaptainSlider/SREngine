//
// Created by Nikita on 17.11.2020.
//

#include "Types/Mesh.h"
#include <ResourceManager/ResourceManager.h>
#include <Render/Render.h>

#include <Debug.h>
#include <exception>
#include <Render/Shader.h>

#include <Loaders/ObjLoader.h>

#include <GUI.h>
#include <Utils/StringFormat.h>

#include <Math/Matrix4x4.h>

#include <Types/Geometry/Mesh3D.h>
#include <Types/RawMesh.h>
#include <Types/Geometry/SkinnedMesh.h>
#include <Types/Geometry/DebugWireframeMesh.h>
#include <Memory/MeshManager.h>
#include <Memory/MeshAllocator.h>

#include <Window/Window.h>

Framework::Graphics::Types::Mesh::Mesh(MeshType type, const std::string& name)
    : IResource(typeid(Mesh).name())
    , m_env(Environment::Get())
    , m_type(type)
    , m_pipeline(Environment::Get()->GetPipeLine())
{
    Component::Init<Mesh>();

    m_shader = nullptr;
    m_material = nullptr;
    m_geometryName = name;
}

Framework::Graphics::Types::Mesh::~Mesh() {
    if (m_material) {
        m_material->UnSubscribe(this);
        m_material = nullptr;
    }

    if (m_rawMesh) {
        m_rawMesh->RemoveUsePoint();
        m_rawMesh = nullptr;
    }
}

bool Framework::Graphics::Types::Mesh::Destroy() {
    if (IsDestroyed())
        return false;

    if (Debug::GetLevel() >= Debug::Level::High) {
        SR_LOG("Mesh::Destroy() : destroy \"" + m_geometryName + "\"...");
    }

    return IResource::Destroy();
}


Mesh *Mesh::Load(const std::string &localPath, MeshType type, uint32_t id) {
    auto&& pMesh = TryLoad(localPath, type, id);

    SRVerifyFalse2(pMesh, "Mesh not found! Path: " + localPath + "; Id: " + Helper::ToString(id));

    return pMesh;
}

Mesh *Mesh::TryLoad(const std::string &localPath, MeshType type, uint32_t id) {
    const auto& resourceId = Helper::Format("%s-%u|%s", EnumMeshTypeToString(type).c_str(), id, localPath.c_str());

    Mesh* mesh = nullptr;

    if (Mesh* pMesh = ResourceManager::Instance().Find<Mesh>(resourceId)) {
        SRVerifyFalse((mesh = dynamic_cast<Mesh*>(pMesh->Copy(nullptr))));
        return mesh;
    }

    auto pRaw = Helper::Types::RawMesh::Load(localPath);

    if (!pRaw) {
        return nullptr;
    }

    if (id >= pRaw->GetModelsCount()) {
        if (pRaw->GetCountUses() == 0) {
            SR_WARN("Mesh::TryLoad() : unresolved situation...");
            pRaw->Destroy();
        }
        return nullptr;
    }

    switch (type) {
        case MeshType::Static: mesh = Memory::MeshAllocator::Allocate<Mesh3D>(); break;
        case MeshType::Wireframe: mesh = Memory::MeshAllocator::Allocate<DebugWireframeMesh>(); break;
        case MeshType::Unknown:
        case MeshType::Skinned:
            SRAssert(false);
            return mesh;
    }

    if (mesh) {
        /// id меша нужно устанавливать перед id ресурса, так как когда ставится id ресурса он автоматически регистрируется
        /// и другой поток может его подхватить
        mesh->m_meshId = id;

        mesh->SetRawMesh(pRaw);
        mesh->SetGeometryName(pRaw->GetGeometryName(id));
        mesh->SetMaterial(Material::GetDefault());

        mesh->SetId(resourceId);
    }

    SRAssert(mesh);

    return mesh;

    /*
    const auto& resourceId = Helper::Format("%s-%u|%s", EnumMeshTypeToString(type).c_str(), id, localPath.c_str());
    const bool withIndices = Environment::Get()->GetPipeLine() == PipeLine::Vulkan;

    Mesh* mesh = nullptr;

    if (IResource* pResource = ResourceManager::Instance().Find<Mesh>(resourceId)) {
        SRVerifyFalse((mesh = dynamic_cast<Mesh*>(pResource->Copy(nullptr))));
        return mesh;
    }

    const auto path = ResourceManager::Instance().GetResPath().Concat("/Models/").Concat(localPath);
    //const auto ext = path.GetExtension();

    //if (ext == "obj") {
    //    mesh = ObjLoader::Load(path.ToString(), withIndices, type, id);
    //}
    //else if (ext == "fbx") {
        SRAssert(type == MeshType::Static);

        const auto resFolder = Helper::ResourceManager::Instance().GetResPath();
        auto fbx = FbxLoader::Loader::Load(
                resFolder.Concat("Utilities/FbxFormatConverter.exe"),
                resFolder.Concat("Cache"),
                resFolder.Concat("Models"),
                localPath,
                withIndices);

        if (fbx.GetShapes().size() <= id) {
            SR_ERROR("Mesh::Load() : incorrect id! \n\tPath: " + localPath + "\n\tId: " + std::to_string(id));
            return {};
        }

        mesh = LoadFbx(type, withIndices, fbx.GetShapes()[id]);
    //}

    if (mesh) {
        /// id меша нужно устанавливать перед id ресурса, так как когда ставится id ресурса он автоматически регистрируется
        /// и другой поток может его подхватить
        mesh->m_meshId = id;
        mesh->SetId(resourceId);
    }

    SRAssert(mesh);

    return mesh;*/
}

Mesh *Mesh::LoadFbx(MeshType type, bool withIndices, const FbxLoader::Geometry &geometry) {
    /*IndexedMesh* mesh = nullptr;

    if (geometry.vertices.empty()) {
        SR_ERROR("Mesh::LoadFbx() : vertices is empty! Geometry: " + geometry.name);
        return nullptr;
    }

    switch (type) {
        case MeshType::Static: {
            mesh = Memory::MeshAllocator::Allocate<Mesh3D>();
            mesh->SetVertexArray(Vertices::CastVertices<Vertices::Mesh3DVertex>(geometry.vertices));
            break;
        }
        case MeshType::Wireframe:
            mesh = Memory::MeshAllocator::Allocate<DebugWireframeMesh>();
            mesh->SetVertexArray(Vertices::CastVertices<Vertices::SkyboxVertex>(geometry.vertices));
            break;
        case MeshType::Unknown:
        case MeshType::Skinned:
            SRAssert(false);
            return mesh;
    }

    if (!mesh) {
        SRAssert(false);
        return nullptr;
    }

    mesh->SetGeometryName(geometry.name);
    mesh->SetMaterial(Material::GetDefault());

    if (withIndices)
        mesh->SetIndexArray(geometry.indices);


    return mesh;*/

    return nullptr;
}

std::vector<Mesh *> Framework::Graphics::Types::Mesh::Load(const std::string& localPath, MeshType type) {
    std::vector<Mesh*> meshes;

    uint32_t id = 0;
    while(auto&& pMesh = TryLoad(localPath, type, id)) {
        meshes.emplace_back(pMesh);
        ++id;
    }

    if (meshes.empty()) {
        SR_ERROR("Mesh::Load() : failed to load mesh! Path: " + localPath);
    }

    return meshes;

    /*auto path = ResourceManager::Instance().GetResPath().Concat("/Models/").Concat(localPath);
    auto meshes = std::vector<Mesh*>();
    const bool withIndices = Environment::Get()->GetPipeLine() == PipeLine::Vulkan;

    uint32_t counter = 0;
ret:
    const auto& id = Helper::Format("%s-%u|%s", EnumMeshTypeToString(type).c_str(), counter, localPath.c_str());
    if (IResource* find = ResourceManager::Instance().Find<Mesh>(id)) {
        if (IResource* copy = ((Mesh*)(find))->Copy(nullptr)) {
            meshes.emplace_back(dynamic_cast<Mesh*>(copy));
            counter++;
            goto ret;
        }
        else {
            SRAssert2(false, "Mesh::Load() : [FATAL] An unforeseen situation has arisen, apparently, it is necessary to work out this piece of code...");
        }
    }
    else if (counter > 0)
        return meshes;

    std::string ext = StringUtils::GetExtensionFromFilePath(path);

    //if (ext == "obj") {
    //    meshes = ObjLoader::Load(path, withIndices, type);
    //}
    //else if (ext == "fbx") {
        //SRAssert(type == MeshType::Static);

        const auto resFolder = Helper::ResourceManager::Instance().GetResPath();

        auto fbx = FbxLoader::Loader::Load(
                resFolder.Concat("Utilities/FbxFormatConverter.exe"),
                resFolder.Concat("Cache"),
                resFolder.Concat("Models"),
                localPath,
                withIndices);

        if (fbx.GetShapes().empty()) {
            SR_ERROR("Mesh::Load() : file not found or need triangulate! \n\tPath: " + localPath);
            return {};
        }

        for (const auto& shape : fbx.GetShapes()) {
            if (auto&& mesh = LoadFbx(type, withIndices, shape))
                meshes.emplace_back(mesh);
        }
    //}
    //else {
    //    SR_ERROR("Mesh::Load() : unknown \"" + ext + "\" format!");
    //    meshes = std::vector<Mesh*>();
    //}

    for (uint32_t i = 0; i < static_cast<uint32_t>(meshes.size()); ++i) {
        /// id меша нужно устанавливать перед id ресурса, так как когда ставится id ресурса он автоматически регистрируется
        /// и другой поток может его подхватить
        meshes[i]->m_meshId = i;
        meshes[i]->SetId(Helper::Format("%s-%u|%s", EnumMeshTypeToString(type).c_str(), i, localPath.c_str()));
    }

    return meshes;*/
}

void Mesh::OnDestroyGameObject() {
    Destroy();

    if (!m_render) {
        SR_ERROR("Mesh::OnDestroyGameObject() : render is not set! Something went wrong...");
    }
    else
        m_render->RemoveMesh(this);
}

bool Mesh::IsCanCalculate() const {
    if (!m_render) {
        SR_ERROR("Mesh::IsCanCalculate() : mesh is not register in render!");
        return false;
    }

    if (!m_shader) {
        SR_ERROR("Mesh::IsCanCalculate() : mesh have not shader!");
        return false;
    }

    if (!m_material) {
        SR_ERROR("Mesh::IsCanCalculate() : mesh have not material!");
        return false;
    }

    return true;
}

void Mesh::OnSelected(bool value) {
    if (value == IsSelected())
        return;

    Component::OnSelected(value);
}

Math::FVector3 Mesh::GetBarycenter() const {
    auto baryMat = Math::Matrix4x4(m_barycenter, Math::FVector3(), 1.0);
    auto rotateMat = Math::Matrix4x4(0.0, m_rotation.InverseAxis(2).ToQuat(), 1.0);

    return (rotateMat * baryMat).GetTranslate();
}

IResource *Mesh::Copy(IResource* destination) const {
    if (IsDestroyed()) {
        SR_ERROR("Mesh::Copy() : mesh already destroyed!");
        return nullptr;
    }

    Mesh* mesh = dynamic_cast<Mesh*>(destination);
    if (!mesh) {
        SR_ERROR("Mesh::Copy() : impossible to copy basic mesh!");
        return nullptr;
    }

    if (Debug::GetLevel() >= Debug::Level::Full)
        SR_LOG("Mesh::Copy() : copy \"" + GetResourceId() + "\" mesh...");

    mesh->m_meshId        = m_meshId;

    mesh->SetMaterial(m_material);
    mesh->SetShader(nullptr);
    mesh->SetRawMesh(m_rawMesh);

    mesh->m_barycenter = m_barycenter;
    mesh->m_position   = m_position;
    mesh->m_rotation   = m_rotation;
    mesh->m_scale      = m_scale;
    mesh->m_skew       = m_skew;

    mesh->m_modelMat      = m_modelMat;
    mesh->m_isCalculated.store(m_isCalculated);
    mesh->m_hasErrors.store(false);

    return Helper::IResource::Copy(mesh);
}

bool Mesh::FreeVideoMemory() {
    if (m_pipeline == PipeLine::Vulkan) {
        if (m_descriptorSet >= 0) {
            m_env->FreeDescriptorSet(m_descriptorSet);
            m_descriptorSet = -5;
        }

        if (m_UBO >= 0) {
            if (!m_env->FreeUBO(m_UBO)) {
                Helper::Debug::Error("Mesh::FreeVideoMemory() : failed to free uniform buffer object!");
                return false;
            }
        }
    }

    m_isCalculated = false;

    return true;
}

void Mesh::ReCalcModel() {
    Math::Matrix4x4 modelMat = Math::Matrix4x4::FromTranslate(m_position);

    if (m_pipeline == PipeLine::OpenGL) {
        modelMat *= Math::Matrix4x4::FromScale(m_skew.InverseAxis(0));
        modelMat *= Math::Matrix4x4::FromEulers(m_rotation.InverseAxis(1));
    }
    else {
        modelMat *= Math::Matrix4x4::FromScale(m_skew);
        modelMat *= Math::Matrix4x4::FromEulers(m_rotation);
    }

    modelMat *= Math::Matrix4x4::FromScale(m_inverse ? -m_scale : m_scale);

    m_modelMat = modelMat.ToGLM();

    UpdateUBO();
}

void Mesh::WaitCalculate() const  {
    ret:
    if (!m_render) {
        SRAssert2(false, Helper::Format("Mesh::WaitCalculate() : There is no destination render!"
                             " The geometry will never be calculated. \nName: %s", m_geometryName.c_str()).c_str());
        return;
    }

    if (m_isCalculated)
        return;

    goto ret;
}

bool Mesh::Calculate()  {
    m_isCalculated = true;
    return true;
}

void Mesh::SetMaterial(Material *material) {
    if (m_material)
        m_material->UnSubscribe(this);

    if ((m_material = material)) {
        m_material->Subscribe(this);
    }
    else
        SR_WARN("Mesh::SetMaterial() : the material is nullptr!");

    if (m_material && m_render)
        m_material->Register(m_render);

    if (m_isCalculated)
        Environment::Get()->SetBuildState(false);
}

void Mesh::SetShader(Framework::Graphics::Shader *shader) {
    if (m_isCalculated) {
        Environment::Get()->SetBuildState(false);
        m_isCalculated = false;
    }
    m_shader = shader;
}

void Mesh::OnMove(const Math::FVector3 &newValue) {
    m_position = newValue;
    ReCalcModel();
}

void Mesh::OnRotate(const Math::FVector3 &newValue) {
    m_rotation = newValue;
    ReCalcModel();
}

void Mesh::OnScaled(const Math::FVector3 &newValue) {
    m_scale = newValue;
    ReCalcModel();
}

void Mesh::OnSkewed(const Math::FVector3 &newValue) {
    m_skew = newValue;
    ReCalcModel();
}

void Mesh::OnTransparencyChanged() {
    if (m_render) {
        m_render->ReRegisterMesh(this);
    }
}

std::string Mesh::GetPath() const {
    return StringUtils::Substring(GetResourceId(), '|', 1);
}

void Mesh::SetRawMesh(Helper::Types::RawMesh *pRaw) {
    if (m_rawMesh && pRaw) {
        SRAssert(false);
        return;
    }

    if (m_rawMesh) {
        m_rawMesh->RemoveUsePoint();
    }

    if (pRaw) {
        pRaw->AddUsePoint();
    }

    m_rawMesh = pRaw;
}



