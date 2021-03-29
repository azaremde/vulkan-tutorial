#ifndef __AstrumVK_SwapChain_SwapChain_hpp__
#define __AstrumVK_SwapChain_SwapChain_hpp__

#pragma once

#include "Pch.hpp"

#include "AstrumVK/GPU/GPU.hpp"
#include "AstrumVK/Surface/Surface.hpp"
#include "AstrumVK/Framebuffers/Framebuffer.hpp"

#include "Core/Window.hpp"

class SwapChain
{
private:
    VkSwapchainKHR swapChain;
    void createSwapChain();
    void destroySwapChain();

    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    void createImageViews();
    void destroyImageViews();

    std::vector<Framebuffer*> framebuffers;

    VkSurfaceFormatKHR surfaceFormat;
    void chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    VkPresentModeKHR presentMode;
    void choosePresentationMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

    VkExtent2D extent;
    void chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    GPU& gpu;
    Surface& surface;
    Window& window;

    SwapChain(const SwapChain&) = delete;
    SwapChain& operator=(const SwapChain&) = delete;

public:
    void createFramebuffers(const VkRenderPass& renderPass);
    void destroyFramebuffers();
    
    const VkSurfaceFormatKHR& getSurfaceFormat() const;
    const VkExtent2D& getExtent() const;

    SwapChain(GPU& _gpu, Surface& _surface, Window& _window);
    ~SwapChain();
};

#endif