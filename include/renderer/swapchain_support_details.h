#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class SwapChainSupportDetails {
public:
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};


