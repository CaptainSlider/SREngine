//
// Created by Monika on 05.04.2022.
//

#ifndef SRENGINE_PROCEDURALMESH_H
#define SRENGINE_PROCEDURALMESH_H

#include <Types/Geometry/IndexedMesh.h>

namespace SR_GTYPES_NS {
    class ProceduralMesh : public IndexedMesh {
        friend class Mesh;
        SR_ENTITY_SET_VERSION(1000);
    public:
        ProceduralMesh();

    public:
        typedef Vertices::StaticMeshVertex VertexType;

    public:
        IResource* Copy(IResource* destination) const override;

        static Component* LoadComponent(SR_HTYPES_NS::Marshal& marshal, const SR_HTYPES_NS::DataStorage* dataStorage);

        void SetVertices(const std::vector<Vertices::StaticMeshVertex>& vertices);

        SR_NODISCARD bool IsCanCalculate() const override;

    private:
        bool Calculate() override;
        void FreeVideoMemory() override;
        void Draw() override;

        SR_NODISCARD SR_HTYPES_NS::Marshal Save(SR_UTILS_NS::SavableFlags flags) const override;
        SR_NODISCARD std::vector<uint32_t> GetIndices() const override;

    private:
        std::vector<Vertices::StaticMeshVertex> m_vertices;
        std::vector<uint32_t> m_indices;

    };
}

#endif //SRENGINE_PROCEDURALMESH_H
