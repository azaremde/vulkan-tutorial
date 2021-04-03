#ifndef __AstrumVK_Models_VAO_hpp__
#define __AstrumVK_Models_VAO_hpp__

#pragma once

#include "Pch.hpp"

#include "AstrumVK/GPU/GPU.hpp"

class Texture2D;

class Entity
{
public:
    VkBuffer buffer;
    VkDeviceMemory memory;

    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    Texture2D* albedoColor;

    uint32_t vertexCount { 0 };
    uint32_t indexCount { 0 };

    std::vector<VkDescriptorSet> descriptorSets;

    void destroy(GPU& gpu);

    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

public:
    Entity();
    ~Entity();
};

#endif