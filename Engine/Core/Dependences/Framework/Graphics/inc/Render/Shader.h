//
// Created by Nikita on 17.11.2020.
//

#ifndef GAMEENGINE_SHADER_H
#define GAMEENGINE_SHADER_H

#include <Types/Vertices.h>
#include <Types/Uniforms.h>

#include <Utils/Common/NonCopyable.h>
#include <Utils/Common/Hashes.h>
#include <Utils/ResourceManager/IResource.h>

#include <Memory/ShaderUBOBlock.h>

namespace SR_GTYPES_NS {
    class Texture;
}

namespace SR_GRAPH_NS {
    class Render;
    class Shader;
    class Environment;

    class Shader : public SR_UTILS_NS::IResource {
        using ShaderProgram = int32_t;
    private:
        Shader();
        ~Shader() override;

    public:
        static Shader* Load(const SR_UTILS_NS::Path& rawPath);

        bool Init();
        bool Use() noexcept;
        void UnUse() noexcept;
        bool InitUBOBlock();
        bool Flush() const;
        void FreeVideoMemory();
        bool Reload() override;

    public:
        SR_NODISCARD SR_FORCE_INLINE Render* GetRender() const { return m_render; }
        SR_NODISCARD SR_UTILS_NS::Path GetAssociatedPath() const override;
        SR_NODISCARD int32_t GetID();
        SR_NODISCARD bool Ready() const;
        SR_NODISCARD uint64_t GetUBOBlockSize() const;
        SR_NODISCARD uint32_t GetSamplersCount() const;
        SR_NODISCARD ShaderProperties GetProperties();

    public:
        template<typename T, bool shared = false> void SetValue(uint64_t hashId, const T& v) noexcept {
            if (!IsLoaded()) {
                return;
            }

            m_uniformBlock.SetField(hashId, &v);
        }

        void SR_FASTCALL SetBool(uint64_t hashId, const bool& v) noexcept;
        void SR_FASTCALL SetFloat(uint64_t hashId, const float& v) noexcept;
        void SR_FASTCALL SetInt(uint64_t hashId, const int& v) noexcept;
        void SR_FASTCALL SetMat4(uint64_t hashId, const glm::mat4& v) noexcept;
        void SR_FASTCALL SetVec4(uint64_t hashId, const glm::vec4& v) noexcept;
        void SR_FASTCALL SetVec3(uint64_t hashId, const glm::vec3& v) noexcept;
        void SR_FASTCALL SetVec2(uint64_t hashId, const glm::vec2& v) noexcept;
        void SR_FASTCALL SetIVec2(uint64_t hashId, const glm::ivec2& v) noexcept;
        void SR_FASTCALL SetSampler2D(const std::string& name, Types::Texture* sampler) noexcept;
        void SR_FASTCALL SetSamplerCube(uint64_t hashId, int32_t sampler) noexcept;
        void SR_FASTCALL SetSampler2D(uint64_t hashId, Types::Texture* sampler) noexcept;
        void SR_FASTCALL SetSampler2D(uint64_t hashId, int32_t sampler) noexcept;

    protected:
        bool Load() override;
        bool Unload() override;

    private:
        void SetSampler(uint64_t hashId, int32_t sampler) noexcept;

    private:
        ShaderProgram          m_shaderProgram        = SR_ID_INVALID;

        std::atomic<bool>      m_hasErrors            = false;
        std::atomic<bool>      m_isInit               = false;

        SRShaderCreateInfo     m_shaderCreateInfo     = { };

        Memory::ShaderUBOBlock m_uniformBlock         = Memory::ShaderUBOBlock();
        ShaderSamplers         m_samplers             = ShaderSamplers();
        ShaderProperties       m_properties           = ShaderProperties();

        Render*                m_render               = nullptr;

    };
}

#endif //GAMEENGINE_SHADER_H
