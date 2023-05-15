//
// Created by Nikita on 01.07.2021.
//

#ifndef GAMEENGINE_VULKANIMGUI_H
#define GAMEENGINE_VULKANIMGUI_H

#include <Utils/GUI.h>

#include <EvoVulkan/VulkanKernel.h>
#include <EvoVulkan/DescriptorManager.h>
#include <EvoVulkan/Types/RenderPass.h>

namespace SR_GRAPH_NS::VulkanTypes {
    class VkImGUI : public EvoVulkan::Types::IVkObject {
        using PipelinePtr = Environment*;
        using Super = EvoVulkan::Types::IVkObject;
    public:
        VkImGUI(const PipelinePtr& pPipeline);
        ~VkImGUI() override;

    private:
        const std::vector<VkDescriptorPoolSize> m_poolSizes = {
                { VK_DESCRIPTOR_TYPE_SAMPLER,                1000 },
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
                { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
                { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000 }
        };
    public:
        bool ReSize(uint32_t width, uint32_t height);
        void SetSurfaceDirty();

        SR_NODISCARD VkCommandBuffer Render(uint32_t frame);
        SR_NODISCARD bool IsSurfaceDirty() const { return m_surfaceDirty; }
        SR_NODISCARD bool IsUndockingActive() const;

    public:
        bool Init();
        bool ReInit();

    private:
        bool InitializeRenderer();
        void DeInitializeRenderer();

    private:
        PipelinePtr m_pipeline = nullptr;

        bool m_surfaceDirty = true;

        VkCommandBufferBeginInfo m_cmdBuffBI;
        VkRenderPassBeginInfo m_renderPassBI;
        std::vector<VkClearValue> m_clearValues;

        std::vector<VkFramebuffer> m_frameBuffs;
        std::vector<VkCommandPool> m_cmdPools;
        std::vector<VkCommandBuffer> m_cmdBuffs;

        EvoVulkan::Types::RenderPass m_renderPass;

        EvoVulkan::Types::DescriptorPool* m_pool = nullptr;
        EvoVulkan::Types::Device* m_device = nullptr;
        EvoVulkan::Types::Swapchain* m_swapchain = nullptr;

        EvoVulkan::Types::MultisampleTarget* m_multisample = nullptr;

    };
}

#endif //GAMEENGINE_VULKANIMGUI_H
