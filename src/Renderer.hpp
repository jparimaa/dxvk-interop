#pragma once

#include "Context.hpp"
#include "DX.hpp"
#include <winnt.h>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <memory>

class Renderer final
{
public:
    Renderer(Context& context);
    ~Renderer();

    bool render();

private:
    bool update(uint32_t imageIndex);

    void createRenderPass();
    void createSwapchainImageViews();
    void createFramebuffers();
    void createSampler();
    void createTextures();
    void createTexturesDescriptorSetLayouts();
    void createGraphicsPipeline();
    void createDescriptorPool();
    void createTextureDescriptorSet();
    void updateTexturesDescriptorSet();
    void allocateCommandBuffers();

    Context& m_context;
    VkDevice m_device;

    DX m_dx;

    std::chrono::steady_clock::time_point m_lastRenderTime;
    VkRenderPass m_renderPass;
    std::vector<VkImageView> m_swapchainImageViews;
    std::vector<VkFramebuffer> m_framebuffers;
    VkSampler m_sampler;
    VkImage m_image;
    VkDeviceSize m_imageSize;
    HANDLE m_imageMemoryHandle;
    VkDeviceMemory m_imageMemory;
    VkImageView m_imageView;
    VkDescriptorSetLayout m_texturesDescriptorSetLayout;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;
    VkDescriptorPool m_descriptorPool;
    std::vector<VkDescriptorSet> m_uboDescriptorSets;
    VkDescriptorSet m_texturesDescriptorSet;
    std::vector<VkCommandBuffer> m_commandBuffers;
};
