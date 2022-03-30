//
// Created by Nikita on 17.11.2020.
//

#ifndef GAMEENGINE_RENDER_H
#define GAMEENGINE_RENDER_H

#include <Render/Shader.h>
#include <Types/Mesh.h>
#include <Environment/Environment.h>
#include <Types/EditorGrid.h>
#include <Render/ColorBuffer.h>
#include <Types/List.h>

#include <Render/MeshCluster.h>

#include <Types/Geometry/IndexedMesh.h>

namespace SR_GRAPH_NS::Types {
    class Skybox;
    class Texture;
}

namespace SR_GRAPH_NS {
    // TODO: to refactoring

    // first - current, second - new
    struct RenderSkybox {
        Types::Skybox* m_current;
        Types::Skybox* m_new;
    };

    class Light;
    class Window;
    class Camera;

    class Render : public Helper::NonCopyable {
    protected:
        Render(std::string name);

    public:
        ~Render() override = default;

    public:
        static Render* Allocate(std::string name);

    public:
        [[nodiscard]] SR_FORCE_INLINE bool IsRun() const noexcept { return m_isRun; }
        [[nodiscard]] SR_FORCE_INLINE bool IsInit() const noexcept { return m_isInit; }
    public:
        [[nodiscard]] SR_FORCE_INLINE ColorBuffer* GetColorBuffer() const noexcept { return this->m_colorBuffer; }

        SR_FORCE_INLINE void SetWireFrameEnabled(const bool& value) noexcept { this->m_wireFrame = value; }
        [[nodiscard]] SR_FORCE_INLINE bool GetWireFrameEnabled() const noexcept { return this->m_wireFrame; }
        SR_FORCE_INLINE void SetGridEnabled(bool value) { this->m_gridEnabled = value; }
        void SetCurrentCamera(Camera* camera) noexcept;
        [[nodiscard]] SR_FORCE_INLINE Camera* GetCurrentCamera() const noexcept { return this->m_currentCamera; }
    public:
        void Synchronize();
        bool IsClean();

        void ReRegisterMesh(Types::Mesh* mesh);
        void RemoveMesh(Types::Mesh* mesh);
        void RegisterMesh(Types::Mesh* mesh);
        void RegisterMeshes(const Helper::Types::List<Types::Mesh*>& meshes) {
            for (size_t i = 0; i < meshes.Size(); i++)
                this->RegisterMesh(meshes[i]);
        }

        bool InsertShader(uint32_t id, Shader* shader);
        Shader* FindShader(uint32_t id) const;

        /** \brief Can get a nullptr value for removing skybox */
        void SetSkybox(Types::Skybox* skybox);
        bool FreeSkyboxMemory(Types::Skybox* skybox);
        [[nodiscard]] Types::Skybox* GetSkybox() const { return m_skybox.m_current; }

        void RegisterTexture(Types::Texture* texture);
        void FreeTexture(Types::Texture* texture);
    public:
        [[nodiscard]] inline Window* GetWindow() const noexcept { return m_window; }
    public:
        bool Create(Window* window); //, Camera* camera
        /** \warning call only from window thread */
        bool Init();
        /** \warning call only from window thread */
        bool Run();
        /** \warning call only from window thread */
        bool Close();
    public:
        /** \brief Check all render events. For example: new meshes, remove old meshes */
        void PollEvents();
    public:
        virtual void UpdateUBOs() { }

        virtual void CalculateAll() = 0;
        virtual void DrawGeometry() = 0;
        virtual void DrawSkybox()   = 0;
    public:
        virtual void DrawGrid()                 = 0;
        virtual void DrawSingleColors()         = 0;
        virtual void DrawTransparentGeometry()  = 0;
        virtual void DrawSettingsPanel()        = 0;

    protected:
        std::atomic<bool>             m_isCreate                 = false;
        std::atomic<bool>             m_isInit                   = false;
        std::atomic<bool>             m_isRun                    = false;
        std::atomic<bool>             m_isClose                  = false;

        bool                          m_gridEnabled              = false;
        bool                          m_skyboxEnabled            = true;
        bool                          m_wireFrame                = false;

        Window*                       m_window                   = nullptr;
        Camera*                       m_currentCamera            = nullptr;
        mutable std::recursive_mutex  m_mutex                    = std::recursive_mutex();

        // TO_REFACTORING
        std::vector<Types::Mesh*>     m_newMeshes                = std::vector<Types::Mesh*>();
        std::queue<Types::Mesh*>      m_removeMeshes             = std::queue<Types::Mesh*>();
        std::vector<Types::Texture*>  m_texturesToFree           = std::vector<Types::Texture*>();
        std::vector<Types::Skybox*>   m_skyboxesToFreeVidMem     = std::vector<Types::Skybox*>();
        std::unordered_set<Types::Texture*>  m_textures                 = std::unordered_set<Types::Texture*>();

        MeshCluster                   m_geometry                 = { };
        MeshCluster                   m_transparentGeometry      = { };

        RenderSkybox                  m_skybox                   = { nullptr, nullptr };

        std::vector<Shader*>          m_shaders                  = {};

        ColorBuffer*                  m_colorBuffer              = nullptr;
        EditorGrid*                   m_grid                     = nullptr;
        Environment*                  m_env                      = nullptr;

        const PipeLine                m_pipeLine                 = PipeLine::Unknown;
        const std::string             m_renderName               = "Unnamed";

    };
}

#endif //GAMEENGINE_RENDER_H
