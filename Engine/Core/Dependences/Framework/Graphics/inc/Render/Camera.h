//
// Created by Nikita on 18.11.2020.
//

#ifndef GAMEENGINE_CAMERA_H
#define GAMEENGINE_CAMERA_H

#include <Render/Shader.h>
#include <EntityComponentSystem/Component.h>
#include <Render/PostProcessing.h>
#include <Debug.h>
#include <functional>
#include <GUI/ICanvas.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Math/Vector3.h>
#include <Math/Matrix4x4.h>
#include <Types/Uniforms.h>
#include <Events/EventManager.h>

using namespace Framework::Helper;

namespace Framework {
    class API;
};

namespace Framework::Graphics {
    class Window;

    class Camera : public Component {
        friend class ::Framework::API;
    private:
        ~Camera() = default;
        Camera() : Component("Camera"), m_env(Environment::Get()), m_pipeline(m_env->GetPipeLine()) { }
    public:
        void UpdateProjection(unsigned int w, unsigned int h);
        void UpdateProjection();

        SR_FORCE_INLINE void SetDirectOutput(bool value) noexcept {
            this->m_isEnableDirectOut.second = value;
        }
    public:
        static Camera* Allocate(uint32_t width = 0, uint32_t height = 0);
    public:
        bool Create(Window* window);

        /// \warning Call only from window class!
        bool Free();
    public:
        bool DrawOnInspector() override;

        void OnRotate(const Math::FVector3& newValue) override;
        void OnMove(const Math::FVector3& newValue) override;
        void OnReady(bool ready) override;
        void OnAttachComponent() override {
            Component::OnAttachComponent();
        }
    protected:
        void OnRemoveComponent() override {
            OnDestroyGameObject();
        }
        void OnDestroyGameObject() override;
    public:

        [[nodiscard]] SR_FORCE_INLINE bool IsAllowUpdateProjection() const { return m_allowUpdateProj;       }
        [[nodiscard]] SR_FORCE_INLINE bool IsDirectOutput()     const { return m_isEnableDirectOut.first;   }
        [[nodiscard]] SR_FORCE_INLINE bool IsNeedUpdate()       const { return m_needUpdate;                }
        [[nodiscard]] SR_FORCE_INLINE glm::vec3 GetRotation()   const { return { m_pitch, m_yaw, m_roll };  }
        [[nodiscard]] SR_FORCE_INLINE glm::mat4 GetView()       const { return this->m_viewMat;             }
        [[nodiscard]] SR_FORCE_INLINE glm::mat4 GetViewTranslate() const { return this->m_viewTranslateMat; }
        [[nodiscard]] SR_FORCE_INLINE glm::mat4 GetProjection() const { return this->m_projection;          }
        [[nodiscard]] SR_FORCE_INLINE Math::IVector2 GetSize()   const { return m_cameraSize;                }
        [[nodiscard]] SR_FORCE_INLINE PostProcessing* GetPostProcessing() const { return m_postProcessing;            }
        [[nodiscard]] SR_FORCE_INLINE glm::vec3 GetGLPosition() const { return this->m_pos.ToGLM();         }

        [[nodiscard]] glm::mat4 GetImGuizmoView() const noexcept;
        [[nodiscard]] glm::mat4 GetTranslationMatrix() const noexcept;

        void WaitCalculate() const {
            ret:
            if (!m_isCalculate)
                goto ret;
        }
        void WaitBuffersCalculate() const {
            ret:
            if (!m_isBuffCalculate)
                goto ret;
        }

        /**
         \brief Update shader parameters: proj-mat and view-mat.
         \warning Call after shader use, and before draw. */
        template <typename T> void UpdateShader(Shader* shader) noexcept {
            if (!m_isCreate) {
                Helper::Debug::Warn("Camera::UpdateShader() : camera is not create! Something went wrong...");
                return;
            }

            if (!shader->Complete())
                return;

            if (m_needUpdate) {
                if (!CompleteResize()) {
                    Helper::Debug::Error("Camera::UpdateShader() : failed to complete resize! Push exit event...");
                    Helper::EventManager::Push(EventManager::Event::Fatal);
                    return;
                }
            }

            if (m_pipeline == PipeLine::OpenGL) {
                if constexpr (std::is_same<T, ProjViewUBO>::value)
                    shader->SetMat4("PVmat", this->m_projection * this->m_viewTranslateMat);
                else if constexpr (std::is_same<T, SkyboxUBO>::value)
                    shader->SetMat4("PVmat", this->m_projection * this->m_viewMat);
            } else {
                if constexpr (std::is_same<T, ProjViewUBO>::value) {
                    ProjViewUBO ubo = {};
                    ubo.view = this->m_viewTranslateMat;
                    ubo.proj = this->m_projection;
                    m_env->UpdateUBO(shader->GetUBO(0), &ubo, sizeof(ProjViewUBO));
                } else if constexpr (std::is_same<T, SkyboxUBO>::value) {
                    SkyboxUBO ubo = {};
                    ubo.PVMat = m_projection * m_viewMat;
                    m_env->UpdateUBO(shader->GetUBO(0), &ubo, sizeof(SkyboxUBO));
                }
            }
        }
        void UpdateShaderProjView(Shader* shader) noexcept;

        bool CompleteResize();

        void PoolEvents();
    private:
        void UpdateView() noexcept;
        bool Calculate() noexcept;
    private:
        volatile bool         m_isCreate          = false;
        volatile bool         m_isCalculate       = false;
        volatile bool         m_isBuffCalculate   = false;
        volatile bool         m_needUpdate        = false;

        volatile float        m_yaw               = 0;
        volatile float        m_pitch             = 0;
        volatile float        m_roll              = 0;
    private:
        PostProcessing*       m_postProcessing    = nullptr;
        Environment*          m_env               = nullptr;
        const PipeLine        m_pipeline          = PipeLine::Unknown;

        Window*		          m_window	     	  = nullptr;
        glm::mat4	          m_projection        = glm::mat4(0);
        glm::mat4	          m_viewTranslateMat  = glm::mat4(0);
        glm::mat4	          m_viewMat           = glm::mat4(0);
        Math::FVector3	      m_pos               = { 0, 0, 0 };

        // 1 - current, 2 - new
        std::pair<bool, bool> m_isEnableDirectOut = { false, false };
        bool                  m_allowUpdateProj   = true;

        float                 m_far               = 8000.f;
        float                 m_near              = 0.01f;

        GUI::ICanvas*         m_canvas            = nullptr;

        Math::IVector2        m_cameraSize        = { 0, 0 };
    };
}

#endif //GAMEENGINE_CAMERA_H