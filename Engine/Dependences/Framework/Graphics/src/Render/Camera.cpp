//
// Created by Nikita on 18.11.2020.
//

#include <Render/Camera.h>
#include <Window/Window.h>
#include <Debug.h>
#include <string>

#include <GUI.h>

void Framework::Graphics::Camera::UpdateShaderProjView(Framework::Graphics::Shader *shader) noexcept {
    if (!m_isCreate) {
        Debug::Warn("Camera::UpdateShader() : camera is not create! Something went wrong...");
        return;
    }

    if (m_needUpdate){
        if (!this->m_postProcessing->OnResize(m_cameraSize.x, m_cameraSize.y)){
            Debug::Error("Camera::UpdateShader() : failed recalculated frame buffers!");
            return;
        }
        m_needUpdate = false;
        this->m_isBuffCalculate = true;
    }

    shader->SetMat4("viewMat", this->m_viewMat);
    shader->SetMat4("projMat", this->m_projection);
}

void Framework::Graphics::Camera::UpdateShader(Framework::Graphics::Shader *shader) noexcept {
    if (!m_isCreate) {
        Debug::Warn("Camera::UpdateShader() : camera is not create! Something went wrong...");
        return;
    }

    if (!shader->Complete())
        return;

    if (m_needUpdate){
        if (!this->m_postProcessing->OnResize(m_cameraSize.x, m_cameraSize.y)){
            Debug::Error("Camera::UpdateShader() : failed recalculated frame buffers!");
            return;
        }
        m_needUpdate = false;
        this->m_isBuffCalculate = true;
    }

    if (m_pipeline == PipeLine::OpenGL) {
        shader->SetMat4("PVmat", this->m_projection * this->m_viewMat);
    } else {
        m_ubo.view = this->m_viewMat;
        m_ubo.proj = this->m_projection;
        this->m_env->UpdateUBO(shader->GetUBO(0), &m_ubo, sizeof(ProjViewUBO));
    }
}

bool Framework::Graphics::Camera::Create(Framework::Graphics::Window *window) {
    Debug::Graph("Camera::Create() : creating camera...");
    if (m_isCreate){
        Debug::Error("Camera::Create() : camera already create!");
        return false;
    }

    this->m_window = window;

    this->UpdateView();

    if (!m_isCalculate)
        this->Calculate();

    this->m_isCreate = true;

    return true;
}

/*
static inline glm::mat4 Mat4FromQuat(glm::quat & q) {
    glm::mat4 M;
    M[0][0] = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
    M[0][1] = 2.0f * (q.x * q.y + q.z * q.w);
    M[0][2] = 2.0f * (q.x * q.z - q.y * q.w);
    M[0][3] = 0.0f;

    M[1][0] = 2.0f * (q.x * q.y - q.z * q.w);
    M[1][1] = 1.0f - 2.0f * (q.x * q.x + q.z * q.z);
    M[1][2] = 2.0f * (q.z * q.y + q.x * q.w);
    M[1][3] = 0.0f;

    M[2][0] = 2.0f * (q.x * q.z + q.y * q.w);
    M[2][1] = 2.0f * (q.y * q.z - q.x * q.w);
    M[2][2] = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
    M[2][3] = 0.0f;

    M[3][0] = 0;
    M[3][1] = 0;
    M[3][2] = 0;
    M[3][3] = 1.0f;
    return M;
}*/

void Framework::Graphics::Camera::UpdateView() noexcept {
    glm::mat4 matrix(1.f);

    if (m_pipeline == PipeLine::OpenGL) {
        matrix = glm::rotate(matrix, -m_pitch, { 1, 0, 0 });
        matrix = glm::rotate(matrix, -m_yaw,   { 0, 1, 0 });
        matrix = glm::rotate(matrix, m_roll,   { 0, 0, 1 });

        m_viewMat = glm::translate(matrix, {
                -m_pos.x,
                -m_pos.y,
                m_pos.z//-m_pos.z
        });
    }
    else {
        matrix = glm::rotate(matrix, m_pitch, { 1, 0, 0 });
        matrix = glm::rotate(matrix, -m_yaw,  { 0, 1, 0 });
        matrix = glm::rotate(matrix, m_roll,  { 0, 0, 1 });

        m_viewMat = glm::translate(matrix, {
                -m_pos.x,
                m_pos.y,
                m_pos.z//-m_pos.z
        });
    }
}

void Framework::Graphics::Camera::OnRotate(glm::vec3 newValue) noexcept {
    this->m_yaw   = float(newValue.y * 3.14 / 45.f / 4.f);
    this->m_pitch = float(newValue.x * 3.14 / 45.f / 4.f);
    this->m_roll  = float(newValue.z * 3.14 / 45.f / 4.f);

    this->UpdateView();
}

void Framework::Graphics::Camera::OnMove(glm::vec3 newValue) noexcept {
    this->m_pos = {
             newValue.x,
             newValue.y,
            -newValue.z
    };
    this->UpdateView();
}

void Framework::Graphics::Camera::UpdateProjection() {
    m_projection = glm::perspective(glm::radians(45.f), (float)m_cameraSize.x / (float)m_cameraSize.y, m_near, m_far);
}
void Framework::Graphics::Camera::UpdateProjection(unsigned int w, unsigned int h) {
    this->m_cameraSize = {w, h};
    m_projection = glm::perspective(glm::radians(45.f), (float)w / (float)h, m_near, m_far);
    m_needUpdate = true;
}

bool Framework::Graphics::Camera::Calculate() noexcept {
    if (m_isCalculate)
        return false;

    Debug::Graph("Camera::Calculate() : calculating camera...");

    this->m_postProcessing->Init(m_window->GetRender());
    m_needUpdate = true;
    m_isCalculate = true;

    return true;
}

void Framework::Graphics::Camera::OnDestroyGameObject() noexcept {
    if (m_window) {
        m_window->DestroyCamera(this);
        m_window = nullptr;
    }
}

nlohmann::json Framework::Graphics::Camera::Save() {
    nlohmann::json camera;
    camera["Camera"]["Far"] = m_far;
    camera["Camera"]["Near"] = m_near;

    camera["Camera"]["PostProcessing"]["ColorCorrection"] = {
            m_postProcessing->GetColorCorrection().r,
            m_postProcessing->GetColorCorrection().g,
            m_postProcessing->GetColorCorrection().b
        };
    camera["Camera"]["PostProcessing"]["Gamma"] = m_postProcessing->GetGamma();

    return camera;
}

bool Framework::Graphics::Camera::Free() {
    Debug::Graph("Camera::Free() : free camera pointer...");
    this->m_postProcessing->Destroy();
    this->m_postProcessing->Free();
    delete this;
    return true;
}

bool Framework::Graphics::Camera::DrawOnInspector() {
    if (ImGui::InputFloat("Far", &m_far, 50) || ImGui::InputFloat("Near", &m_near, 0.1))
        this->UpdateProjection();

    if (!m_window)
        ImGui::TextColored({1,0,0,1}, "Window is missing!");

    ImGui::Separator();

    Helper::GUI::DrawTextOnCenter("PostProcessing");

    PostProcessing* post = m_postProcessing;


    float gamma = post->GetGamma();
    if (ImGui::InputFloat("Gamma", &gamma, 0.05))
        post->SetGamma(gamma);

    float exposure = post->GetExposure();
    if (ImGui::InputFloat("Exposure", &exposure, 0.05))
        post->SetExposure(exposure);

    float saturation = post->GetSaturation();
    if (ImGui::InputFloat("Saturation", &saturation, 0.05))
        post->SetSaturation(saturation);

    glm::vec3 color = post->GetColorCorrection();
    if (ImGui::InputFloat3("Color correction", &color[0]))
        post->SetColorCorrection(color);

    ImGui::NewLine();

    bool enabled = post->GetBloomEnabled();
    if (ImGui::Checkbox("Bloom", &enabled))
        post->SetBloom(enabled);

    ImGui::NewLine();

    float bloom_intensity = post->GetBloomIntensity();
    if (ImGui::InputFloat("Bloom intensity", &bloom_intensity, 0.1))
        post->SetBloomIntensity(bloom_intensity);

    color = post->GetBloomColor();
    if (ImGui::InputFloat3("Bloom color", &color[0]))
        post->SetBloomColor(color);

    int bloom_amount = post->GetBloomAmount();
    if (ImGui::InputInt("Bloom amount", &bloom_amount)) {
        if (bloom_amount == 0)
            bloom_amount = 1;
        post->SetBloomAmount(bloom_amount);
    }

    return true;
}

Framework::Graphics::Camera *Framework::Graphics::Camera::Allocate() {
    auto camera = new Camera();
    camera->m_postProcessing = PostProcessing::Allocate(camera);
    if (!camera->m_postProcessing) {
        Debug::Error("Camera::Allocate() : failed to allocate post processing!");
        return nullptr;
    }
    return camera;
}


