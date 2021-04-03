#ifndef __AstrumVK_GPU_CommandBuffer_hpp__
#define __AstrumVK_GPU_CommandBuffer_hpp__

#pragma once

#include "Pch.hpp"

#include "GPU.hpp"
#include "AstrumVK/SwapChain/SwapChain.hpp"
#include "AstrumVK/Entities/Entity.hpp"
#include "AstrumVK/Pipeline/Pipeline.hpp"
#include "AstrumVK/UBO/UniformBufferObject.hpp"
#include "AstrumVK/UBO/UniformBuffer.hpp"

#include "Assets/MeshAsset.hpp"

class CommandBuffer
{
private:
    VkCommandPool commandPool;
    void createCommandPool();
    void destroyCommandPool();

    std::vector<VkCommandBuffer> commandBuffers;

    SwapChain& swapChain;
    Pipeline& pipeline;

    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

public:

    // Todo: Move it somewhere
    void createVertexBuffer(Entity* vao, const std::vector<Vert>& vertices);
    void createIndexBuffer(Entity* vao, const std::vector<uint32_t>& indices);

    // Todo: Move it somewhere
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    // Todo: Consider a simplification
    void render(
        const VkRenderPass& renderPass, 
        const std::vector<Framebuffer*>& swapChainFramebuffers, 
        const VkExtent2D& extent, 
        const VkPipeline& graphicsPipeline,
        const UniformBuffer& uniformBuffer,
        const std::vector<Entity*>& vaos
    );

    void createCommandBuffers();
    void freeCommandBuffers();

    const VkCommandPool& getCommandPool() const;
    const std::vector<VkCommandBuffer>& getCommandBuffers() const;

    CommandBuffer(SwapChain& _swapChain, Pipeline& _pipeline);
    ~CommandBuffer();
};

#endif