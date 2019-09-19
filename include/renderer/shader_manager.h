#pragma once

#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <fstream> // used to load shader SPIR-V binaries
#include <stdlib.h>
#include"renderer_utility.h"


enum ShaderType {
    VERTEX_SHADER = VK_SHADER_STAGE_VERTEX_BIT,
    TESSELLATION_CONTROL_SHADER = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
    TESSELLATION_EVALUATION_SHADER = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
    GEOMETRY_SHADER = VK_SHADER_STAGE_GEOMETRY_BIT,
    FRAGMENT_SHADER = VK_SHADER_STAGE_FRAGMENT_BIT,
    COMPUTE_SHADER = VK_SHADER_STAGE_COMPUTE_BIT,
};

class Shader_Manager {
public:

    static VkShaderModule createShaderModule(const std::string &filename, VkDevice device);

private:
    static std::vector<char> readShaderFile(const std::string &filename);
};