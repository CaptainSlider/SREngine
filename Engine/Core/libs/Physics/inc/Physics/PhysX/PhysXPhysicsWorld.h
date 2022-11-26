//
// Created by Monika on 24.11.2022.
//

#ifndef SRENGINE_PHYSXPHYSICSWORLD_H
#define SRENGINE_PHYSXPHYSICSWORLD_H

#include <Physics/PhysX/PhysXUtils.h>
#include <Physics/PhysicsWorld.h>

namespace SR_PHYSICS_NS {
    class PhysXPhysicsWorld : public PhysicsWorld {
        using Super = PhysicsWorld;
    public:
        explicit PhysXPhysicsWorld(LibraryPtr pLibrary, Space space);
        ~PhysXPhysicsWorld() override;

    public:
        bool Initialize() override;
        bool ClearForces() override;
        bool Synchronize() override;

        bool StepSimulation(float_t step) override;

        bool AddRigidbody(RigidbodyPtr pRigidbody) override;
        bool RemoveRigidbody(RigidbodyPtr pRigidbody) override;

    private:
        physx::PxScene* m_scene = nullptr;

    };
}

#endif //SRENGINE_PHYSXPHYSICSWORLD_H
