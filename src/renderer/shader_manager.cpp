#include "renderer/shader_manager.h"


VkShaderModule ShaderManager::createShaderModule(const std::string &filePath, VkDevice device) {

    // TODO: Best put this into a constructor for shaderManager
    if (!glslangInitialized) {
        glslang::InitializeProcess();
        glslangInitialized = true;
    }

    // Load glsl source into a string:
    std::string shaderString = readShaderFile((filePath));
    const char *shaderSource = shaderString.c_str();

    // Print shader status:
//    log("Loading Shader " + filePath);
//    print(shaderSource);

    // Retrieve shader type
    EShLanguage shaderType = getShaderStage(getSuffix(filePath));

    // Create glslang shader with that type
    glslang::TShader shader(shaderType);
    shader.setStrings(&shaderSource, 1);

    // Define compilation language, client and target
    int clientInputSemanticsVersion = 110; // maps to #define VULKAN 110
    glslang::EShClient client = glslang::EShClientVulkan;
    glslang::EShTargetClientVersion clientVersion = glslang::EShTargetVulkan_1_1;
    glslang::EShTargetLanguage target = glslang::EShTargetSpv;
    glslang::EShTargetLanguageVersion targetVersion = glslang::EShTargetSpv_1_0;
    shader.setEnvInput(glslang::EShSourceGlsl, shaderType, client, clientInputSemanticsVersion);
    shader.setEnvClient(client, clientVersion);
    shader.setEnvTarget(target, targetVersion);
    const int defaultVersion = 100;

    TBuiltInResource resources = DefaultTBuiltInResource;
    auto messages = (EShMessages) (EShMsgSpvRules | EShMsgVulkanRules);


    //####################################
    // Preprocess:
    DirStackFileIncluder fileIncluder;
    fileIncluder.pushExternalLocalDirectory(filePath);

    std::string preprocessedGLSL;

    if (!shader.preprocess(&resources, defaultVersion, ENoProfile, false, false, messages, &preprocessedGLSL, fileIncluder)) {
        log("GLSL Preprocessing Failed for: " + filePath);
        print(shader.getInfoLog());
        print(shader.getInfoDebugLog());
    }
    // Store the preprocessed string into the shader and overwrite the previous one
    const char *preprocessedCStr = preprocessedGLSL.c_str();
    shader.setStrings(&preprocessedCStr, 1);

    //####################################
    // Compile:
    // First, parse the shader
    if (!shader.parse(&resources, defaultVersion, false, messages)) {
        log("GLSL Parsing Failed for: " + filePath);
        print(shader.getInfoLog());
        print(shader.getInfoDebugLog());
    }

    // Then, add the parsed shader to a glslang::TProgram and link the program:
    glslang::TProgram program;
    program.addShader(&shader);

    if (!program.link(messages)) {
        log("GLSL Linking Failed for: " + filePath);
        print(shader.getInfoLog());
        print(shader.getInfoDebugLog());
    }
    // If no errors occurred: return the SpirV:
    std::vector<unsigned int> shaderSPIR_V;
    spv::SpvBuildLogger logger;
    glslang::SpvOptions spvOptions;
    glslang::GlslangToSpv(*program.getIntermediate(shaderType), shaderSPIR_V, &logger, &spvOptions);


    // need to wrap the code in a VkShaderModule before passing it to the pipeline.
    VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize = shaderSPIR_V.size() * sizeof(unsigned int);
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t *>(shaderSPIR_V.data());

    VkShaderModule shaderModule = {};
    VK_CHECK(vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule), "Shader Module Creation");

    log("Shader Loaded: " + filePath);

    return shaderModule;
}

std::string ShaderManager::readShaderFile(const std::string &filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        log("Failed to load shader: " + filename);
        throw std::runtime_error("failed to open file: " + filename);
    }

    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

std::string ShaderManager::getSuffix(const std::string &name) {
    const size_t pos = name.rfind('.');
    return (pos == std::string::npos) ? "" : name.substr(name.rfind('.') + 1);
}

EShLanguage ShaderManager::getShaderStage(const std::string &stage) {
    if (stage == "vert") {
        return EShLangVertex;
    } else if (stage == "tesc") {
        return EShLangTessControl;
    } else if (stage == "tese") {
        return EShLangTessEvaluation;
    } else if (stage == "geom") {
        return EShLangGeometry;
    } else if (stage == "frag") {
        return EShLangFragment;
    } else if (stage == "comp") {
        return EShLangCompute;
    } else {
        log("Unknown shader stage");
        throw std::runtime_error("Unknown shader stage");
    }
}