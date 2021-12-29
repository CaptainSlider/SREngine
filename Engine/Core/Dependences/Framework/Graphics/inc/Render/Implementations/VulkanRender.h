//
// Created by Nikita on 27.05.2021.
//

#ifndef GAMEENGINE_VULKANRENDER_H
#define GAMEENGINE_VULKANRENDER_H

#include <Render/Render.h>

#include <Types/Skybox.h>
#include <Render/Camera.h>

namespace Framework::Graphics::Impl {
    class VulkanRender : public Render {
        void DrawSingleColors() override {

        }

        void DrawSettingsPanel() override {

        }

        void UpdateUBOs() override {
            if (m_currentCamera) {
                this->m_currentCamera->UpdateShader<ProjViewUBO>(m_shaders[Shader::StandardID::DebugWireframe]);
                this->m_currentCamera->UpdateShader<ProjViewUBO>(m_shaders[Shader::StandardID::Geometry]);
                this->m_currentCamera->UpdateShader<ProjViewUBO>(m_shaders[Shader::StandardID::Transparent]);
                this->m_currentCamera->UpdateShader<SkyboxUBO>(m_shaders[Shader::StandardID::Skybox]);
            }
        }

        void DrawGeometry() override {
            //SRDrawMeshCluster(m_geometry, Vulkan, ;)

            static Environment* env = Environment::Get();

            for (auto const& [shader, subCluster] : m_geometry.m_subClusters) {
                if (shader) shader->Use();
                else
                    continue;

                for (auto const& [key, meshGroup] : subCluster.m_groups) {
                    env->BindVBO(meshGroup[0]->GetVBO<true>());
                    env->BindIBO(meshGroup[0]->GetIBO<true>());

                    for (const auto &mesh : meshGroup)
                        mesh->DrawVulkan();
                }

                env->UnUseShader();
            }
        }

        void DrawSkybox() override {
            if (m_skybox.m_current && m_skyboxEnabled) {
                m_shaders[Shader::StandardID::Skybox]->Use();

                m_skybox.m_current->DrawVulkan();

                m_env->UnUseShader();
            }
        }

        void DrawTransparentGeometry() override {

        }

        void DrawGrid() override {

        }
    };
}

#endif //GAMEENGINE_VULKANRENDER_H