//
// Created by Monika on 11.03.2023.
//

#ifndef SRENGINE_EVOSCRIPTRESOURCERELOADER_H
#define SRENGINE_EVOSCRIPTRESOURCERELOADER_H

#include <Utils/ResourceManager/IResourceReloader.h>

namespace SR_SCRIPTING_NS {
    class SR_DLL_EXPORT EvoScriptResourceReloader final : public SR_UTILS_NS::IResourceReloader {
        using StashedProperties = std::vector<std::pair<Behaviour*, SR_HTYPES_NS::Marshal::Ptr>>;
    public:
        SR_NODISCARD bool Reload(const SR_UTILS_NS::Path& path, SR_UTILS_NS::ResourceInfo* pResourceInfo) override;

    private:
        void FreeStashedProperties(const StashedProperties& properties);

    };
}

#endif //SRENGINE_EVOSCRIPTRESOURCERELOADER_H
