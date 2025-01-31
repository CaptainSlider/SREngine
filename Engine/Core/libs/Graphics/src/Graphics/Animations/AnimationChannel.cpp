//
// Created by Monika on 01.05.2023.
//

#include <Graphics/Animations/AnimationChannel.h>

namespace SR_ANIMATIONS_NS {
    AnimationChannel::~AnimationChannel() {
        for (auto&& [time, pKey] : m_keys) {
            delete pKey;
        }
    }

    uint32_t AnimationChannel::UpdateChannel(uint32_t keyIndex,
            float_t time,
            float_t weight,
            const AnimationPose* pStaticPose,
            AnimationPose* pWorkingPose
    ) const {
        auto&& pWorkingData = pWorkingPose->GetData(GetGameObjectHashName());
        auto&& pStaticData = pStaticPose->GetData(GetGameObjectHashName());

        if (!pWorkingData || !pStaticData) {
            return keyIndex;
        }

    skipKey:
        if (keyIndex == m_keys.size()) {
            return keyIndex;
        }

        auto&& [keyTime, pKey] = m_keys.at(keyIndex);

        if (time > keyTime) {
            if (keyIndex == 0) {
                pKey->Update(0.f, weight, nullptr, pWorkingData, pStaticData);
            }
            else {
                pKey->Update(1.f, weight, m_keys.at(keyIndex - 1).second, pWorkingData, pStaticData);
            }

            ++keyIndex;

            goto skipKey;
        }

        if (keyIndex == 0) {
            pKey->Update(0.f, weight, nullptr, pWorkingData, pStaticData);
        }
        else {
            auto&& [prevTime, prevKey] = m_keys.at(keyIndex - 1);

            const double_t currentTime = time - prevTime;
            const double_t keyCurrTime = keyTime - prevTime;
            const double_t progress = currentTime / keyCurrTime;

            pKey->Update(progress, weight, prevKey, pWorkingData, pStaticData);
        }

        return keyIndex;
    }

    void AnimationChannel::Load(aiNodeAnim *pChannel, double_t ticksPerSecond, std::vector<AnimationChannel*>& channels) {
        if (pChannel->mNumPositionKeys > 0) {
            static constexpr float_t mul = 0.01;

            auto&& pTranslationChannel = new AnimationChannel();
            auto&& first = AiV3ToFV3(pChannel->mPositionKeys[0].mValue, mul);

            pTranslationChannel->SetName(pChannel->mNodeName.C_Str());

            for (uint16_t positionKeyIndex = 0; positionKeyIndex < pChannel->mNumPositionKeys; ++positionKeyIndex) {
                auto&& pPositionKey = pChannel->mPositionKeys[positionKeyIndex];

                auto&& translation = AiV3ToFV3(pPositionKey.mValue, mul);

                pTranslationChannel->AddKey(pPositionKey.mTime / ticksPerSecond,
                    new TranslationKey(
                        pTranslationChannel,
                        translation,
                        translation - first
                    )
                );
            }

            channels.emplace_back(pTranslationChannel);
        }

        /// --------------------------------------------------------------------------------------------------------

        if (pChannel->mNumRotationKeys > 0) {
            auto&& pRotationChannel = new AnimationChannel();
            auto&& first = AiQToQ(pChannel->mRotationKeys[0].mValue).Inverse();

            pRotationChannel->SetName(pChannel->mNodeName.C_Str());

            for (uint16_t rotationKeyIndex = 0; rotationKeyIndex < pChannel->mNumRotationKeys; ++rotationKeyIndex) {
                auto&& pRotationKey = pChannel->mRotationKeys[rotationKeyIndex];

                auto&& q = AiQToQ(pRotationKey.mValue);

                auto&& delta = q * first;

                pRotationChannel->AddKey(pRotationKey.mTime / ticksPerSecond,
                     new RotationKey(pRotationChannel, q, delta)
                );
            }

            channels.emplace_back(pRotationChannel);
        }

        /// --------------------------------------------------------------------------------------------------------

        if (pChannel->mNumScalingKeys > 0) {
            auto&& pScalingChannel = new AnimationChannel();
            auto&& first = AiV3ToFV3(pChannel->mScalingKeys[0].mValue, 1.f);

            pScalingChannel->SetName(pChannel->mNodeName.C_Str());

            for (uint16_t scalingKeyIndex = 0; scalingKeyIndex < pChannel->mNumScalingKeys; ++scalingKeyIndex) {
                auto&& pScalingKey = pChannel->mScalingKeys[scalingKeyIndex];

                auto&& scale = AiV3ToFV3(pScalingKey.mValue, 1.f);

                pScalingChannel->AddKey(pScalingKey.mTime /ticksPerSecond,
                    new ScalingKey(
                        pScalingChannel,
                        scale,
                        scale / first
                    )
                );
            }

            channels.emplace_back(pScalingChannel);
        }
    }

    void AnimationChannel::SetName(const std::string_view& name) {
        m_hashName = SR_HASH_STR_VIEW(name);
        if (m_hashName == 0) {
            SRHalt0();
        }
    }

    void AnimationChannel::AddKey(double_t timePoint, AnimationKey* pKey) {
        m_keys.emplace_back(std::make_pair(timePoint, pKey));
    }
}