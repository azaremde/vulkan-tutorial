#ifndef __Vulkan_Shaders_Shader_hpp__
#define __Vulkan_Shaders_Shader_hpp__

#pragma once

#include "Pch.hpp"

#include "Vulkan/GPU/GPU.hpp"

class Shader
{
private:
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	GPU& gpu;

	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;

public:
	Shader(GPU& _gpu, const std::string& vertShaderPath, const std::string& fragShaderPath);
	~Shader();

	VkShaderModule& GetVertexShaderModule();
	VkShaderModule& GetFragmentShaderModule();

	void RetrieveCreateInfoStruct(std::vector<VkPipelineShaderStageCreateInfo>& result);
};

namespace ShaderUtil
{
	static std::vector<char> ReadFile(const std::string& filename);
	static VkShaderModule CreateShaderModule(const VkDevice& device, const std::vector<char>& code);
}

#endif