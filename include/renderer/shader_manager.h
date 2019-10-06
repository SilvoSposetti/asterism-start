#pragma once

#include <vulkan/vulkan.h>

#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/DirStackFileIncluder.h>

#include <iostream>
#include <vector>
#include <fstream> // used to read shader source
#include <cstdlib>

#include"renderer_utility.h"


enum ShaderType {
    VERTEX_SHADER = VK_SHADER_STAGE_VERTEX_BIT,
    TESSELLATION_CONTROL_SHADER = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
    TESSELLATION_EVALUATION_SHADER = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
    GEOMETRY_SHADER = VK_SHADER_STAGE_GEOMETRY_BIT,
    FRAGMENT_SHADER = VK_SHADER_STAGE_FRAGMENT_BIT,
    COMPUTE_SHADER = VK_SHADER_STAGE_COMPUTE_BIT,
};

class ShaderManager {
public:

    VkShaderModule createShaderModule(const std::string &filePath, VkDevice device);

private:
    static std::string readShaderFile(const std::string &filename);

    static std::string getSuffix(const std::string &name);

    static EShLanguage getShaderStage(const std::string &stage);

    bool glslangInitialized = false;


    const TBuiltInResource DefaultTBuiltInResource = {
            32,
            6,
            32,
            32,
            64,
            4096,
            64,
            32,
            80,
            32,
            4096,
            32,
            128,
            8,
            16,
            16,
            15,
            -8,
            7,
            8,
            65535,
            65535,
            65535,
            1024,
            1024,
            64,
            1024,
            16,
            8,
            8,
            1,
            60,
            64,
            64,
            128,
            128,
            8,
            8,
            8,
            0,
            0,
            0,
            0,
            0,
            8,
            8,
            16,
            256,
            1024,
            1024,
            64,
            128,
            128,
            16,
            1024,
            4096,
            128,
            128,
            16,
            1024,
            120,
            32,
            64,
            16,
            0,
            0,
            0,
            0,
            8,
            8,
            1,
            0,
            0,
            0,
            0,
            1,
            1,
            16384,
            4,
            64,
            8,
            8,
            4,
            // These until next comment might not be correct
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            true,
            true,
            true,
            true,
            true,
            true,
            true,
            true,
            true

    };


};