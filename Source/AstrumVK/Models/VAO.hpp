#ifndef __AstrumVK_Models_VAO_hpp__
#define __AstrumVK_Models_VAO_hpp__

#pragma once

#include "Pch.hpp"

#include "AstrumVK/GPU/GPU.hpp"

class VAO
{
public:
    VkBuffer buffer;
    VkDeviceMemory memory;

    uint32_t vertexCount;

    void destroy(GPU& gpu);

    VAO(const VAO&) = delete;
    VAO& operator=(const VAO&) = delete;

public:
    VAO();
    ~VAO();
};

#endif