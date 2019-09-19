#include "renderer/shader_manager.h"

std::vector<char> Shader_Manager::readShaderFile(const std::string &filename) {
    // ate flag: start reading at the end of the file
    // binary flag: read t as binary (avoid text transformations)
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file " + filename);
    }
    // The advantage from reading at the end is that we can use the read position to determine file size
    // and allocate a buffer
    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    // Then go back at the beginning of the file and read all of the bytes
    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;

}

VkShaderModule Shader_Manager::createShaderModule(const std::string &filename, VkDevice device) {

    std::vector<char> shaderSourceChar = readShaderFile(filename);
    std::string shaderSource(shaderSourceChar.begin(), shaderSourceChar.end());
    //std::cout << shaderSource << std::endl;

    // ToDo: Check: use glslLangValidator or shaderc / glslang, see THIS: https://forestsharp.com/glslang-cpp/

    std::vector<char> code = readShaderFile(filename);

    // need to wrap the code in a VkShaderModule before passing it to the pipeline.
    VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize = code.size();
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule = {};
    VK_CHECK(vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule));

    return shaderModule;
}