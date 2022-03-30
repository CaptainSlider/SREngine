//
// Created by Nikita on 17.11.2020.
//

#ifndef GAMEENGINE_MATERIAL_H
#define GAMEENGINE_MATERIAL_H

#include <Types/Texture.h>

#include <Render/Shader.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>

#include <unordered_set>
#include <ResourceManager/ResourceManager.h>

namespace Framework::Graphics::Types {
    class Mesh;
    class Mesh3D;

    class Material : public Helper::IResource {
        friend class Mesh;
        friend class Mesh3D;

    private:
        Material();
        Material(Texture* diffuse, Texture* normal, Texture* specular, Texture* glossiness);

        ~Material() override;

    public:
        static Material* GetDefault();
        static bool FreeDefault();
        static bool InitDefault(Render* render);

        static Material* Load(const std::string& name);

    public:
        Helper::IResource* Copy(Helper::IResource* destination) const override;

        SR_NODISCARD bool GetBloomEnabled() const { return m_bloom;  }
        SR_NODISCARD bool IsTransparent() const { return m_transparent;  }
        SR_NODISCARD Helper::Math::FColor GetColor() const { return m_color; }
        SR_NODISCARD uint32_t GetCountSubscriptions() const;

        SR_NODISCARD Texture* GetDiffuse() const { return m_diffuse; }
        SR_NODISCARD Texture* GetNormal() const { return m_normal; }
        SR_NODISCARD Texture* GetSpecular() const { return m_specular; }
        SR_NODISCARD Texture* GetGlossiness() const { return m_glossiness; }

        void SetColor(float r, float g, float b) { SetColor(Helper::Math::FColor(r, g, b, 1.f)); }
        void SetColor(Helper::Math::FVector3 color) { SetColor(color.x, color.y, color.z); }
        void SetColor(glm::vec4 color);
        void SetColor(const Helper::Math::FColor& color);
        void SetBloom(bool value) { this->m_bloom = value; };
        bool SetTransparent(bool value);

        void SetDiffuse(Texture* tex);
        void SetNormal(Texture* tex);
        void SetSpecular(Texture* tex);
        void SetGlossiness(Texture* tex);

        void UseVulkan();
        void UseOpenGL() const;

        void Subscribe(Mesh* mesh);
        void UnSubscribe(Mesh* mesh);

        bool Register(Render* render);

    private:
        bool Destroy() override;
        void UpdateSubscribers();

    private:
        SR_INLINE static Material* m_default       = nullptr;

        std::unordered_set<Mesh*>  m_subscriptions = {};

        const Environment*         m_env           = nullptr;
        Render*                    m_render        = nullptr;

        std::atomic<bool>          m_transparent   = false;
        std::atomic<bool>          m_bloom         = false;
        mutable std::mutex         m_mutex         = std::mutex();

        Helper::Math::FColor       m_color         = Helper::Math::FColor(1.f);

        Texture*                   m_diffuse       = nullptr;
        Texture*                   m_normal        = nullptr;
        Texture*                   m_specular      = nullptr;
        Texture*                   m_glossiness    = nullptr;

    };
}

#endif //GAMEENGINE_MATERIAL_H
