#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Other libraries:
#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>
#include <fstream> // used to load shader SPIR-V binaries
#include <array>
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES


#include "vulkan_core.h"

#include "vertex.h"
#include "swapchain_support_details.h"
#include "renderer_utility.h"

class Renderer {
public:
    void initializeRenderer();


private:
//        // Device
//        Device device
//
//        GLFWwindow *window = nullptr;
//        VkInstance instance = nullptr;
//        VkSurfaceKHR surface = nullptr;
//        VkPhysicalDevice physicalDevice = nullptr;
//        VkDevice device = nullptr;
//
//        // Queues
//
//        // Swapchain
//
//        // Buffers
//
//        // Synchronization Objects
    const char *asterismName = "asterism";
    const uint32_t WIDTH = 1024;
    const uint32_t HEIGHT = 768;

#ifndef NDEBUG
    const bool isDebug = true;
#else
    const bool isDebug  = false;
#endif

    const std::vector<const char *> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    GLFWwindow *window = nullptr;

    // Vulkan Core
    VkInstance instance = nullptr;
    VkSurfaceKHR surface = nullptr;
    VkPhysicalDevice physicalDevice = nullptr;
    VkDevice device = nullptr;
    uint32_t queueFamilyGraphics = 0;
    VkQueue graphicsQueue = nullptr;
    uint32_t queueFamilyPresent = 0;
    VkQueue presentQueue = nullptr;


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
    const int MAX_FRAMES_IN_FLIGHT = 2; //Todo: this should actually be the amount of images in the swapchain - 1?
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

    void mainLoop();

    void cleanup();

    static void frameBufferResizeCallback(GLFWwindow *window, int width, int height);

    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    void createSwapchain();

    void createImageViews();

    void createRenderPass();

    void createDescriptorSetLayout();

    static std::vector<char> readFile(const std::string &filename);

    VkShaderModule createShaderModule(const std::vector<char> &code);

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

    void drawFrame();


// This is called 'interleaving' vertex attributes (position and color are interleaved together)
    const std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f,  -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f},  {1.0f, 1.0f, 1.0f}}
    };

// Vertex indices for the index buffer
// either uint16_t or uint32_t can be used depending on the amount of vertices used (>=65535)
    const std::vector<uint16_t> vertexIndices = {
            0, 1, 2, 2, 3, 0
    };


};


struct UniformBufferObject {
    // Uniforms must be properly aligned!
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

