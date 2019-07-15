#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <set>

#include "renderer_utility.h"

class VulkanCore {
public:

    static VkInstance createInstance(const char *asterismName, bool isDebug);

    static VkSurfaceKHR createSurface(VkInstance instance, GLFWwindow *window);

    static VkPhysicalDevice createPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);

    static void getQueueIndices(VkPhysicalDevice physicalDevice,
                                VkSurfaceKHR surface,
                                uint32_t &queueFamilyGraphics,
                                uint32_t &queueFamilyPresent);

    static VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice,
                                        std::vector<const char *> deviceExtensions,
                                        uint32_t &queueFamilyGraphics,
                                        uint32_t &queueFamilyPresent,
                                        VkQueue &graphicsQueue,
                                        VkQueue &presentQueue);

private:

    static bool isDiscreteGPU(VkPhysicalDevice device);

    static bool isIntegratedGPU(VkPhysicalDevice device);

};