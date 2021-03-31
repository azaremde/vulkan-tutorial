#include "CommandBuffer.hpp"

void CommandBuffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{    
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(gpu.getDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(gpu.getDevice(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(gpu.getDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(gpu.getDevice(), buffer, bufferMemory, 0);
}

void CommandBuffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(gpu.getDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(gpu.getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(gpu.getGraphicsQueue());

    vkFreeCommandBuffers(gpu.getDevice(), commandPool, 1, &commandBuffer);
}

VAO* CommandBuffer::createVertexBuffer(const std::vector<Vertex>& vertices) {
    VAO* result = new VAO();

    result->vertexCount = static_cast<uint32_t>(vertices.size());
    
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(
        bufferSize, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        stagingBuffer, 
        stagingBufferMemory
    );

    void* data;
    vkMapMemory(gpu.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(gpu.getDevice(), stagingBufferMemory);

    createBuffer(
        bufferSize, 
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        result->buffer, 
        result->memory
    );

    copyBuffer(stagingBuffer, result->buffer, bufferSize);
    
    vkDestroyBuffer(gpu.getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(gpu.getDevice(), stagingBufferMemory, nullptr);

    return result;
}

uint32_t CommandBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(gpu.getPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void CommandBuffer::createCommandPool()
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = gpu.getQueueFamilyIndices().graphics.value();
    poolInfo.flags = 0; // Optional

    VK_CHECK(
        vkCreateCommandPool(gpu.getDevice(), &poolInfo, nullptr, &commandPool),
        "Failed to create command pool."
    );

    DebugLogOut("Command pool created.");
}

void CommandBuffer::destroyCommandPool()
{
    vkDestroyCommandPool(gpu.getDevice(), commandPool, nullptr);

    DebugLogOut("Command pool destroyed.");
}

void CommandBuffer::createCommandBuffers()
{    
    commandBuffers.resize(swapChain.getFramebuffers().size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    VK_CHECK(
        vkAllocateCommandBuffers(gpu.getDevice(), &allocInfo, commandBuffers.data()),
        "Failed to allocate command buffers."
    );
}

void CommandBuffer::render(
    const VkRenderPass& renderPass, 
    const std::vector<Framebuffer*>& swapChainFramebuffers, 
    const VkExtent2D& extent, 
    const VkPipeline& graphicsPipeline,
    const std::vector<VAO*>& vaos
)
{
    for (size_t i = 0; i < commandBuffers.size(); i++)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        VK_CHECK(
            vkBeginCommandBuffer(commandBuffers[i], &beginInfo),
            "Failed to begin recording command buffer."
        );

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[i]->getFramebuffer();

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = extent;

        VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        VkDeviceSize offsets[] = {0};

        for (const VAO* vao : vaos)
        {            
            vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &vao->buffer, offsets);
            vkCmdDraw(commandBuffers[i], vao->vertexCount, 1, 0, 0);
        }

        end(commandBuffers[i]);
    }
}

void CommandBuffer::end(const VkCommandBuffer& buffer)
{
    vkCmdEndRenderPass(buffer);
    VK_CHECK(
        vkEndCommandBuffer(buffer),
        "Failed to record command buffer."
    );
}

void CommandBuffer::freeCommandBuffers()
{
    vkFreeCommandBuffers(
        gpu.getDevice(), 
        commandPool, 
        static_cast<uint32_t>(commandBuffers.size()), 
        commandBuffers.data()
    );
}

CommandBuffer::CommandBuffer(GPU& _gpu, SwapChain& _swapChain) : gpu { _gpu }, swapChain { _swapChain }
{    
    createCommandPool();
    createCommandBuffers();
}

CommandBuffer::~CommandBuffer()
{
    destroyCommandPool();
}

const VkCommandPool& CommandBuffer::getCommandPool() const
{
    return commandPool;
}

const std::vector<VkCommandBuffer>& CommandBuffer::getCommandBuffers() const
{
    return commandBuffers;
}