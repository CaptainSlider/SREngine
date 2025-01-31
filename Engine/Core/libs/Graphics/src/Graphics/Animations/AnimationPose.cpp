//
// Created by Monika on 26.04.2023.
//

#include <Graphics/Animations/AnimationPose.h>

#include <Utils/ECS/Transform.h>

#include <Graphics/Animations/AnimationData.h>
#include <Graphics/Animations/AnimationChannel.h>
#include <Graphics/Animations/AnimationClip.h>
#include <Graphics/Animations/Skeleton.h>
#include <Graphics/Animations/Bone.h>

namespace SR_ANIMATIONS_NS {
    AnimationPose::~AnimationPose() {
        for (auto&& [boneHashName, pData] : m_data) {
            delete pData;
        }
        m_indices.clear();
        m_data.clear();
    }

    AnimationData* AnimationPose::GetData(AnimationPose::BoneHashName boneHashName) const noexcept {
        if (auto&& pIt = m_indices.find(boneHashName); pIt != m_indices.end()) {
            return pIt->second;
        }

        return nullptr;
    }

    void AnimationPose::Reset() {
        for (auto&& [boneHashName, pData] : m_data) {
            pData->Reset();
        }
    }

    void AnimationPose::Initialize(Skeleton *pSkeleton) {
        SRAssert(!m_isInitialized);

        auto&& bones = pSkeleton->GetBones();

        m_indices.reserve(bones.size());
        m_data.reserve(bones.size());

        for (auto&& pBone : bones) {
            auto&& pair = std::make_pair(
                pBone->hashName,
                new AnimationData()
            );

            m_indices.insert(pair);
            m_data.emplace_back(pair);
        }

        m_isInitialized = true;
    }

    void AnimationPose::Apply(Skeleton *pSkeleton) {
        if (!m_isInitialized) {
            Initialize(pSkeleton);
        }

        for (auto&& [boneHashName, pWorkingData] : m_data) {
            auto&& pBone = pSkeleton->TryGetBone(boneHashName);
            if (!pBone || !pBone->gameObject) {
                continue;
            }

            Apply(pWorkingData, pBone->gameObject);
        }
    }

    void AnimationPose::Apply(const AnimationData* pWorkingData, const SR_UTILS_NS::GameObject::Ptr& pGameObject) {
        auto&& pTransform = pGameObject->GetTransform();

        if (pWorkingData->translation.has_value()) {
            pTransform->SetTranslation(pWorkingData->translation.value());
        }

        if (pWorkingData->rotation.has_value()) {
            pTransform->SetRotation(pWorkingData->rotation.value());
        }

        if (pWorkingData->scale.has_value()) {
            pTransform->SetScale(pWorkingData->scale.value());
        }
    }

    void AnimationPose::Update(Skeleton* pSkeleton, AnimationPose* pWorkingPose) {
        if (!pWorkingPose) {
            SRHalt0();
            return;
        }

        if (!m_isInitialized) {
            Initialize(pSkeleton);
        }

        for (auto&& [boneHashName, pData] : m_data) {
            auto&& pBone = pSkeleton->TryGetBone(boneHashName);
            if (!pBone || !pBone->gameObject) {
                continue;
            }

            auto&& pWorkingData = pWorkingPose->GetData(boneHashName);
            if (!pWorkingData) {
                return;
            }

            Update(pData, pWorkingData, pBone->gameObject);
        }
    }

    void AnimationPose::Update(AnimationData* pStaticData, const AnimationData* pWorkingData, const SR_UTILS_NS::GameObject::Ptr& pGameObject) {
        auto&& pTransform = pGameObject->GetTransform();

        /// ------------------------------------------------------------------------------------------------------------

        if (!pStaticData->translation.has_value() || !pWorkingData->translation.has_value()) {
            pStaticData->translation = pTransform->GetTranslation();
        }
        else {
            auto&& delta = pTransform->GetTranslation() - pWorkingData->translation.value();

            if (!delta.Empty()) {
                pStaticData->translation.value() += delta;
            }
        }

        /// ------------------------------------------------------------------------------------------------------------

        if (!pStaticData->rotation.has_value() || !pWorkingData->rotation.has_value()) {
            pStaticData->rotation = pTransform->GetQuaternion();
        }
        else {
            auto&& delta = pTransform->GetQuaternion() * pWorkingData->rotation.value().Inverse();

            if (!delta.IsIdentity()) {
                pStaticData->rotation.value() *= delta;
            }
        }

        /// ------------------------------------------------------------------------------------------------------------
    }

    void AnimationPose::SetPose(AnimationClip* pClip) {
        auto&& channels = pClip->GetChannels();

        for (auto&& pChannel : channels) {
            for (auto&& [time, pKey] : pChannel->GetKeys()) {
                if (time > 0) {
                    continue;
                }

                auto&& pData = GetData(pChannel->GetGameObjectHashName());
                if (!pData) {
                    continue;
                }

                pKey->Set(1.f, pData);
            }
        }
    }
}