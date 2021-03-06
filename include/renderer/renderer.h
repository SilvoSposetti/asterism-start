#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

// Other libraries:
#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>
#include <array>
#include <chrono>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include "vulkan_core.h"
#include "queue_manager.h"
#include "drawable/vertex.h"
#include "shader_manager.h"
#include "swapchain_support_details.h"
#include "renderer_utility.h"
#include "input_manager.h"


class Renderer {
public:
    void initializeRenderer();

    bool checkLoop();

    static void rendererPollEvents();

    void drawFrame();

    void afterLoop();

    void cleanup();


private:

    std::string asterismName = "asterism";
    const uint32_t WIDTH = 1024;
    const uint32_t HEIGHT = 768;

#ifndef NDEBUG
    const bool isDebug = true;
#else
    const bool isDebug = false;
#endif

    const std::vector<QueueType> requiredQueues = {GRAPHICS_QUEUE, COMPUTE_QUEUE};
    const std::vector<ShaderType> requiredShaders = {VERTEX_SHADER, FRAGMENT_SHADER};

    const std::vector<const char *> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    GLFWwindow *window = nullptr;

    // Input Manager
    std::shared_ptr<InputManager> inputManager;

    // Vulkan Core
    VkInstance instance = nullptr;
    VkSurfaceKHR surface = nullptr;
    VkPhysicalDevice physicalDevice = nullptr;
    VkDevice device = nullptr;

    // QueueManager:
    QueueManager queues = QueueManager(requiredQueues);


    VkSwapchainKHR swapchain = nullptr;

    std::vector<VkImage> swapchainImages;
    VkFormat swapchainImageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D swapchainExtent = {0, 0};

    std::vector<VkImageView> swapchainImageViews;

    VkRenderPass renderPass = nullptr;
    VkDescriptorSetLayout descriptorSetLayout = nullptr;
    VkPipelineLayout pipelineLayout = nullptr;

    VkPipeline graphicsPipeline = nullptr;

    std::vector<VkFramebuffer> swapchainFrameBuffers;

    VkCommandPool commandPool = nullptr;

    VkDescriptorPool descriptorPool = nullptr;
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<VkCommandBuffer> commandBuffers;


    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    const int MAX_FRAMES_IN_FLIGHT = 2; // Todo: this should actually be the amount of images in the swapchain - 1?
    size_t currentFrame = 0;
    std::vector<VkFence> inFlightFences;

    // Buffers:
    VkBuffer vertexBuffer = nullptr;
    VkDeviceMemory vertexBufferMemory = nullptr;
    VkBuffer indexBuffer = nullptr;
    VkDeviceMemory indexBufferMemory = nullptr;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;

    bool frameBufferResized = false;

    void initializeWindow();

    void initializeVulkan();


    static void frameBufferResizeCallback(GLFWwindow *window, int width, int height);

    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    void createSwapchain();

    void createImageViews();

    void createRenderPass();

    void createDescriptorSetLayout();

//    static std::vector<char> readFile(const std::string &filename);

//    VkShaderModule createShaderModule(const std::vector<char> &code);

    void createGraphicsPipeline();

    void createFramebuffers();

    void createCommandPool();

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);


    void createBuffer(VkDeviceSize size,
                      VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties,
                      VkBuffer &buffer,
                      VkDeviceMemory &bufferMemory);

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    void createVertexBuffer();

    void createIndexBuffer();

    void createUniformBuffers();

    void createDescriptorPool();

    void createDescriptorSets();

    void createCommandBuffers();

    void createSyncObjects();

    void recreateSwapchain();

    void cleanupSwapchain();

    void updateUniformBuffer(uint32_t currentImageIndex);


// This is called 'interleaving' vertex attributes (position and color are interleaved together)
    const std::vector<Vertex> vertices = {
            {{-0.5f, 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f,  0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f,  0.0f, 0.5f},  {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.0f, 0.5f},  {1.0f, 1.0f, 1.0f}}
    };

// Vertex indices for the index buffer
// either uint16_t or uint32_t can be used depending on the amount of vertices used (>=65535)
    const std::vector<uint16_t> vertexIndices = {
            0, 2, 1, 0, 3, 2
    };


};


struct UniformBufferObject {
    // Uniforms must be properly aligned!
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

