//
// Created by Monika on 28.07.2022.
//

#include <Physics/Rigidbody.h>

#include <Utils/ECS/Transform.h>
#include <Utils/World/Scene.h>
#include <Utils/DebugDraw.h>
#include <Utils/Types/RawMesh.h>

#include <Physics/LibraryImpl.h>
#include <Physics/PhysicsScene.h>
#include <Physics/PhysicsMaterial.h>

namespace SR_PTYPES_NS {
    Rigidbody::Rigidbody(LibraryPtr pLibrary)
        : SR_UTILS_NS::Component()
        , m_library(pLibrary)
        , m_shape(pLibrary->CreateCollisionShape())
        , m_isBodyDirty(true)
        , m_scale(SR_MATH_NS::FVector3::One())
    {
        m_shape->SetRigidbody(this);
    }

    Rigidbody::~Rigidbody() {
        SR_SAFE_DELETE_PTR(m_shape);

        SetMaterial(nullptr);
        SetRawMesh(nullptr);
    }

    std::string Rigidbody::GetEntityInfo() const {
        return Super::GetEntityInfo() + " | " + SR_UTILS_NS::EnumReflector::ToString(m_shape->GetType());
    }

    SR_UTILS_NS::Component* Rigidbody::LoadComponent(SR_UTILS_NS::Measurement measurement, SR_HTYPES_NS::Marshal &marshal, const SR_HTYPES_NS::DataStorage* pDataStorage) {
        const auto&& type = static_cast<ShapeType>(marshal.Read<int32_t>());

        const auto&& center = marshal.Read<SR_MATH_NS::FVector3>(SR_MATH_NS::FVector3(0.f));
        const auto&& size = marshal.Read<SR_MATH_NS::FVector3>(SR_MATH_NS::FVector3(1.f));

        const auto&& mass = marshal.Read<float_t>();
        const auto&& isTrigger = marshal.Read<bool>();
        const auto&& isStatic = marshal.Read<bool>();

        const auto&& material = marshal.Read<std::string>();

        static auto&& verifyType = [](LibraryImpl* pLibrary, ShapeType shapeType) -> ShapeType {
            if (!pLibrary->IsShapeSupported(shapeType)) {
                SR_WARN("Rigidbody::LoadComponent() : rigidbody has unsupported shape! Replace to default...");
                shapeType = pLibrary->GetDefaultShape();
            }

            return shapeType;
        };

        Rigidbody* pComponent = nullptr;
        LibraryImpl* pLibrary = nullptr;

        switch (measurement) {
            case SR_UTILS_NS::Measurement::Space3D: {
                pLibrary = pDataStorage->GetPointer<LibraryImpl>("3DPLib");
                pComponent = pLibrary->CreateRigidbody3D();
                break;
            }
            default:
                SRHalt("Unsupported measurement!");
                return pComponent;
        }

        if (!pComponent) {
            SR_ERROR("Rigidbody::LoadComponent() : failed to create rigidbody!");
            return nullptr;
        }

        if (!pComponent->GetCollisionShape()) {
            SR_ERROR("Rigidbody::LoadComponent() : rigidbody have not collision shape!");
            pComponent->AutoFree([](auto&& pData) {
                delete pData;
            });
            return nullptr;
        }

        if (material.empty()) {
            pComponent->SetMaterial(SR_PHYSICS_NS::PhysicsLibrary::Instance().GetDefaultMaterial());
        }
        else {
            pComponent->SetMaterial(PhysicsMaterial::Load(material));
        }

        pComponent->SetType(verifyType(pLibrary, type));
        pComponent->SetCenter(center);
        pComponent->GetCollisionShape()->SetSize(size);
        pComponent->SetMass(mass);
        pComponent->SetIsTrigger(isTrigger);
        pComponent->SetIsStatic(isStatic);

        return pComponent;
    }

    SR_HTYPES_NS::Marshal::Ptr Rigidbody::Save(SR_HTYPES_NS::Marshal::Ptr pMarshal, SR_UTILS_NS::SavableFlags flags) const {
        pMarshal = Super::Save(pMarshal, flags);

        pMarshal->Write<int32_t>(static_cast<int32_t>(m_shape->GetType()));

        pMarshal->Write<SR_MATH_NS::FVector3>(m_center, SR_MATH_NS::FVector3(0.f));
        pMarshal->Write<SR_MATH_NS::FVector3>(m_shape->GetSize(), SR_MATH_NS::FVector3(1.f));

        pMarshal->Write<float_t>(m_mass);
        pMarshal->Write<bool>(IsTrigger());
        pMarshal->Write<bool>(IsStatic());

        if (m_material) {
            pMarshal->Write<std::string>(m_material->GetResourcePath().ToStringRef());
        }
        else {
            pMarshal->Write<std::string>("");
        }

        return pMarshal;
    }

    void Rigidbody::OnDestroy() {
        m_shape->RemoveDebugShape();

        /// получаем указатель обязательно до OnDestroy
        PhysicsScene::Ptr physicsScene = GetPhysicsScene();

        Super::OnDestroy();

        if (physicsScene) {
            physicsScene->Remove(this);
        }
        else {
           AutoFree([](auto&& pData) {
               delete pData;
           });
        }
    }

    void Rigidbody::OnAttached() {
        Component::OnAttached();
        GetCollisionShape()->UpdateDebugShape();
    }

    const Rigidbody::PhysicsScenePtr& Rigidbody::GetPhysicsScene() const {
        if (!m_physicsScene.Valid()) {
            auto&& pScene = TryGetScene();
            if (!pScene) {
                static Rigidbody::PhysicsScenePtr empty;
                return empty;
            }

            m_physicsScene = pScene->Do<PhysicsScenePtr>([](SR_WORLD_NS::Scene* ptr) {
                return ptr->GetDataStorage().GetValue<PhysicsScenePtr>();
            }, PhysicsScenePtr());
        }

        return m_physicsScene;
    }

    void Rigidbody::OnMatrixDirty() {
        if (auto&& pTransform = GetTransform()) {
            pTransform->GetMatrix().Decompose(
                m_translation,
                m_rotation,
                m_scale
            );

            m_shape->UpdateDebugShape();
        }

        SetMatrixDirty(true);

        Component::OnMatrixDirty();
    }

    bool Rigidbody::UpdateMatrix(bool force) {
        if ((!force && !IsMatrixDirty())) {
            return false;
        }

        SetMatrixDirty(false);

        m_shape->SetScale(m_scale);
        m_shape->UpdateMatrix();

        return true;
    }

    SR_MATH_NS::FVector3 Rigidbody::GetCenter() const noexcept {
        return m_center;
    }

    float_t Rigidbody::GetMass() const noexcept {
        return m_mass;
    }

    void Rigidbody::SetCenter(const SR_MATH_NS::FVector3& center) {
        m_center = center;
        SetMatrixDirty(true);
        m_shape->UpdateDebugShape();
    }

    void Rigidbody::SetMass(float_t mass) {
        m_mass = mass;
        UpdateInertia();
    }

    SR_MATH_NS::FVector3 Rigidbody::GetCenterDirection() const noexcept {
        /// TODO: cache direction
        return m_rotation * (m_scale * m_center);
    }

    ShapeType Rigidbody::GetType() const noexcept {
        return m_shape->GetType();
    }

    void Rigidbody::SetType(ShapeType type) {
        if (m_shape->GetType() == type) {
            return;
        }

        m_shape->SetType(type);

        SetShapeDirty(true);
    }

    void Rigidbody::OnEnable() {
        if (auto&& physicsScene = GetPhysicsScene()) {
            if (!IsBodyDirty()) {
                UpdateInertia();
            }

            physicsScene->Register(this);
        }
        else {
            SRHalt("Failed to get physics scene!");
        }

        m_shape->UpdateDebugShape();

        Super::OnEnable();
    }

    void Rigidbody::OnDisable() {
        if (auto&& physicsScene = GetPhysicsScene()) {
            physicsScene->Remove(this);
        }
        else {
            SRHalt("Failed to get physics scene!");
        }

        m_shape->RemoveDebugShape();

        Super::OnDisable();
    }

    SR_UTILS_NS::Measurement Rigidbody::GetMeasurement() const {
        return SR_UTILS_NS::Measurement::Unknown;
    }

    void Rigidbody::SetIsTrigger(bool value) {
        m_isTrigger = value;
        m_isBodyDirty = true;
    }

    void Rigidbody::SetIsStatic(bool value) {
        m_isStatic = value;
        m_isBodyDirty = true;
    }

    RBUpdShapeRes Rigidbody::UpdateShape() {
        if (!IsShapeDirty()) {
            return RBUpdShapeRes::Nothing;
        }

        m_shape->RemoveDebugShape();

        if (!m_shape->UpdateShape()) {
            SR_ERROR("Rigidbody::UpdateShape() : failed to update shape!");
            return RBUpdShapeRes::Error;
        }

        if (!UpdateShapeInternal()) {
            SR_ERROR("Rigidbody::UpdateShape() : failed to internal update shape!");
            return RBUpdShapeRes::Error;
        }

        m_shape->UpdateDebugShape();

        UpdateMatrix(true);

        SetShapeDirty(false);

        return RBUpdShapeRes::Updated;
    }

    bool Rigidbody::InitBody() {
        if (!m_isBodyDirty) {
            SRHalt("Rigidbody::InitBody() : body is not dirty!");
            return false;
        }

        m_isBodyDirty = false;

        return true;
    }

    void Rigidbody::SetMaterial(PhysicsMaterial* pMaterial) {
        if (pMaterial == m_material) {
            return;
        }

        if (m_material) {
            m_material->RemoveUsePoint();
        }

        if ((m_material = pMaterial)) {
            m_material->AddUsePoint();
        }
    }

    void Rigidbody::SetRawMesh(SR_HTYPES_NS::RawMesh* pRawMesh) {
        if (pRawMesh == m_rawMesh) {
            return;
        }

        if (m_rawMesh) {
            m_rawMesh->RemoveUsePoint();
        }

        if ((m_rawMesh = pRawMesh)) {
            m_rawMesh->AddUsePoint();
        }
    }

    bool Rigidbody::IsDebugEnabled() const noexcept {
        if (auto&& pPhysicsScene = GetPhysicsScene()) {
            return pPhysicsScene->IsDebugEnabled();
        }

        return false;
    }

    void Rigidbody::Update(float_t dt) {
        m_shape->Update(dt);
        Super::Update(dt);
    }
}
