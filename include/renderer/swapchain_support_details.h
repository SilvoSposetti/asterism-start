#pragma once

#include <vulkan/vulkan.h>
#include <vector>

struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;

    static SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface);
};

