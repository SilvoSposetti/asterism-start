#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <set>

#include "renderer_utility.h"
#include "queue_manager.h"

class VulkanCore {
public:

    static VkInstance createInstance(std::string asterismName, bool isDebug);

    static VkSurfaceKHR createSurface(VkInstance instance, GLFWwindow *window);

    static VkPhysicalDevice createPhysicalDevice(VkInstance instance);

    static VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice,
                                        std::vector<const char *> deviceExtensions,
                                        QueueManager queues);

private:

    static bool isDiscreteGPU(VkPhysicalDevice device);

    static bool isIntegratedGPU(VkPhysicalDevice device);

};