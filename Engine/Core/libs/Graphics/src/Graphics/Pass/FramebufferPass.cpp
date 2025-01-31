//
// Created by Monika on 22.07.2022.
//

#include <Graphics/Pass/FramebufferPass.h>
#include <Graphics/Types/Framebuffer.h>

namespace SR_GRAPH_NS {
    SR_REGISTER_RENDER_PASS(FramebufferPass)

    FramebufferPass::FramebufferPass(RenderTechnique *pTechnique, BasePass* pParent)
        : GroupPass(pTechnique, pParent)
        , IFramebufferPass()
    { }

    bool FramebufferPass::Load(const SR_XML_NS::Node &passNode) {
        LoadFramebufferSettings(passNode.TryGetNode("FramebufferSettings"));

        bool result = GroupPass::Load(passNode.TryGetNode("Passes"));

        SetName(passNode.TryGetAttribute("Name").ToString(passNode.Name()));

        return result;
    }

    bool FramebufferPass::PreRender() {
        GroupPass::PreRender();
        return false;
    }

    bool FramebufferPass::Render() {
        if (!m_framebuffer) {
            m_isRendered = false;
            return false;
        }

        if (!m_framebuffer->Bind()) {
            m_isRendered = false;
            return false;
        }

        if (!m_framebuffer->BeginCmdBuffer(m_clearColors, m_depth)) {
            m_isRendered = false;
            return false;
        }

        if (m_framebuffer->BeginRender()) {
            GroupPass::Render();
            m_framebuffer->EndRender();
            m_framebuffer->EndCmdBuffer();
        }

        m_pipeline->SetCurrentFramebuffer(nullptr);

        m_isRendered = true;

        /// Независимо от того, отрисовали мы что-то в кадровый буффер или нет,
        /// все равно возвращаем false (hasDrawData), так как технически, кадровый буффер
        /// не несет данных для рендера.
        return false;
    }

    bool FramebufferPass::PostRender() {
        GroupPass::PostRender();
        return false;
    }

    void FramebufferPass::OnResize(const SR_MATH_NS::UVector2 &size) {
        IFramebufferPass::ResizeFrameBuffer(size);
        GroupPass::OnResize(size);
    }

    bool FramebufferPass::Init() {
        return GroupPass::Init() && InitializeFramebuffer(GetContext());
    }

    void FramebufferPass::Update() {
        if (!m_framebuffer || m_framebuffer->IsDirty()) {
            return;
        }

        if (!m_isRendered) {
            return;
        }

        m_pipeline->SetCurrentFramebuffer(m_framebuffer);

        GroupPass::Update();

        m_pipeline->SetCurrentFramebuffer(nullptr);
    }
}