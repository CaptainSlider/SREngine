//
// Created by Monika on 20.03.2023.
//

#include <Graphics/Utils/MeshUtils.h>

#include <Graphics/Types/Geometry/Mesh3D.h>
#include <Graphics/Types/Geometry/DebugWireframeMesh.h>
#include <Graphics/Types/Geometry/SkinnedMesh.h>

#include <Graphics/UI/Sprite2D.h>

namespace SR_GRAPH_NS {
    SR_GTYPES_NS::Mesh* CreateMeshByType(MeshType type) {
        switch (type) {
            case MeshType::Static:
                return new SR_GTYPES_NS::Mesh3D();

            case MeshType::Wireframe:
                return new SR_GTYPES_NS::DebugWireframeMesh();

            case MeshType::Sprite2D:
                return new UI::Sprite2D();

            case MeshType::Skinned:
                return new SR_GTYPES_NS::SkinnedMesh();

            case MeshType::Unknown:
            default:
                break;
        }

        SRHalt("Unknown mesh type!");

        return nullptr;
    }
}
