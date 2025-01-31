//
// Created by Nikita on 01.06.2021.
//

#ifndef GAMEENGINE_SKINNEDMESH_H
#define GAMEENGINE_SKINNEDMESH_H

#include <Utils/Types/IRawMeshHolder.h>
#include <Utils/ECS/EntityRef.h>

#include <Graphics/Types/Geometry/MeshComponent.h>
#include <Graphics/Animations/Skeleton.h>

namespace SR_GTYPES_NS {
    class SkinnedMesh final : public MeshComponent, public SR_HTYPES_NS::IRawMeshHolder {
        SR_ENTITY_SET_VERSION(1002);
        SR_INITIALIZE_COMPONENT(SkinnedMesh);
    public:
        SkinnedMesh();

    private:
        ~SkinnedMesh() override;

    public:
        typedef Vertices::SkinnedMeshVertex VertexType;

    public:
        static Component* LoadComponent(SR_HTYPES_NS::Marshal& marshal, const SR_HTYPES_NS::DataStorage* dataStorage);

        void Update(float dt) override;
        void UseMaterial() override;
        void UseModelMatrix() override;

        SR_NODISCARD bool IsSkeletonUsable() const;
        SR_NODISCARD bool IsCanCalculate() const override;
        SR_NODISCARD bool ExecuteInEditMode() const override { return true; }
        SR_NODISCARD SR_FORCE_INLINE bool IsCanUpdate() const noexcept override { return true; }
        SR_NODISCARD std::string GetMeshIdentifier() const override;
        SR_NODISCARD SR_UTILS_NS::EntityRef& GetSkeleton() { return m_skeletonRef; }
        SR_NODISCARD uint32_t GetMaxBones() const;

        SR_NODISCARD Component* CopyComponent() const override;

    private:
        bool PopulateSkeletonMatrices();

        void OnResourceReloaded(SR_UTILS_NS::IResource* pResource) override;
        void OnRawMeshChanged() override;
        bool Calculate() override;
        void Draw() override;

        SR_NODISCARD SR_HTYPES_NS::Marshal::Ptr Save(SR_HTYPES_NS::Marshal::Ptr pMarshal, SR_UTILS_NS::SavableFlags flags) const override;
        SR_NODISCARD std::vector<uint32_t> GetIndices() const override;

    private:
        SR_UTILS_NS::EntityRef m_skeletonRef;

        bool m_isOffsetsInitialized = false;
        bool m_skeletonIsBroken = false;

        std::vector<uint64_t> m_bonesIds;

        std::vector<SR_MATH_NS::Matrix4x4> m_skeletonMatrices;
        std::vector<SR_MATH_NS::Matrix4x4> m_skeletonOffsets;

    };
}

#endif //GAMEENGINE_SKINNEDMESH_H
