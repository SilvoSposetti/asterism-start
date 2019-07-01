// External libraries:
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

// Other libraries:
#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>

class Asterism {
public:
    void run() {
        initializeWindow();
        initializeVulkan();
        mainLoop();
        cleanup();
    }

private:
    const char *asterismName = "asterism";
    const uint32_t WIDTH = 1024;
    const uint32_t HEIGHT = 768;

    const std::vector<const char *> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    GLFWwindow *window = nullptr;

    VkInstance instance = nullptr;

    VkSurfaceKHR surface = nullptr;

    uint32_t queueFamilyGraphics = 0;
    VkQueue graphicsQueue = nullptr;
    uint32_t queueFamilyPresent = 0;
    VkQueue presentQueue = nullptr;

    VkPhysicalDevice physicalDevice = nullptr;

    VkDevice device = nullptr;

    VkSwapchainKHR swapchain = nullptr;

    std::vector<VkImage> swapchainImages;
    VkFormat swapchainImageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D swapchainExtent = {0, 0};

    std::vector<VkImageView> swapchainImageViews;

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };


    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice vkPhysicalDevice) {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, surface, &details.capabilities);

        // Formats:
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, surface, &formatCount, nullptr);
        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, surface, &formatCount, details.formats.data());
        }

        // Present modes:
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, surface, &presentModeCount,
                                                      details.presentModes.data());
        }

        return details;
    }

    static void VK_CHECK(VkResult result) {
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Something Went Wrong");
        }
    }

    void initializeWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Tell GLFW not to create an OpenGL context.
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window = glfwCreateWindow(WIDTH, HEIGHT, asterismName, nullptr, nullptr);
    }

    void createInstance() {
        VkApplicationInfo applicationInfo = {};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = asterismName;
        applicationInfo.apiVersion = VK_VERSION_1_1;

        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;

        // Activate validation layer only in debug:
#ifndef NDEBUG
        const char *debugLayers[] = {
                "VK_LAYER_LUNARG_standard_validation"
        };
        instanceCreateInfo.ppEnabledLayerNames = debugLayers;
        instanceCreateInfo.enabledLayerCount = sizeof(debugLayers) / sizeof(debugLayers[0]);
        printf("Debug mode: Vulkan Validation Layers ENABLED\n");
#endif

        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;
        instanceCreateInfo.enabledExtensionCount = glfwExtensionCount;

        VK_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));

    }


    static bool isDiscreteGPU(VkPhysicalDevice device) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);

        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            printf("Found a discrete GPU: %s\n", properties.deviceName);
            return true;
        }
        return false;
    }

    static bool isIntegratedGPU(VkPhysicalDevice device) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);

        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
            printf("Found an integrated GPU: %s\n", properties.deviceName);
            return true;
        }
        return false;
    }


    void pickPhysicalDevice() {
        physicalDevice = VK_NULL_HANDLE;

        // Retrieve available physical devices
        uint32_t physicalDevicesCount = 0;
        vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, nullptr);

        if (physicalDevicesCount == 0) {
            throw std::runtime_error("Failed to find GPUs with Vulkan support.");
        }

        std::vector<VkPhysicalDevice> physicalDevices(physicalDevicesCount);
        vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, physicalDevices.data());

        // First look for a discrete GPU
        for (const VkPhysicalDevice &possibleDevice : physicalDevices) {
            if (isDiscreteGPU(possibleDevice)) {
                physicalDevice = possibleDevice;
                break;
            }
        }

        // If a discrete GPU is not available, pick the first one
        if (physicalDevice == VK_NULL_HANDLE) {
            for (const VkPhysicalDevice &possibleDevice : physicalDevices) {
                if (isIntegratedGPU(possibleDevice)) {
                    physicalDevice = possibleDevice;
                    break;
                }
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("Failed to find a GPU.");
        }

        // Check that picked device supports all necessary queues
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        int queueFamilyGraphicsIndexFound = -1;
        for (int i = 0; i < queueFamilyCount; ++i) {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
                queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
                queueFamilyGraphicsIndexFound = i;
            }
        }

        int queueFamilyPresentIndexFound = -1;
        VkBool32 presentSupport = false;
        for (int i = 0; i < queueFamilyCount; ++i) {
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
            if (presentSupport) {
                queueFamilyPresentIndexFound = i;
                break;
            }
        }

        if (queueFamilyGraphicsIndexFound == -1 || queueFamilyPresentIndexFound == -1) {
            throw std::runtime_error("GPU doesn't support necessary queues!");
        } else {
            // Note that it's very likely that these queues end up beign the same after all
            queueFamilyGraphics = queueFamilyGraphicsIndexFound;
            std::cout << "Chosen graphics and compute queue indexed at " << queueFamilyGraphicsIndexFound << std::endl;
            queueFamilyPresent = queueFamilyPresentIndexFound;
            std::cout << "Chosen present queue indexed at " << queueFamilyPresentIndexFound << std::endl;
        }


    }

    void createLogicalDevice() {
        std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {queueFamilyGraphics, queueFamilyPresent};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily: uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
            deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            deviceQueueCreateInfo.queueFamilyIndex = queueFamilyGraphics;
            deviceQueueCreateInfo.queueCount = 1;
            deviceQueueCreateInfo.pQueuePriorities = &queuePriority;
            deviceQueueCreateInfos.push_back(deviceQueueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures = {};

        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;


        VK_CHECK(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device));

        // Retrieve queue handles as soon as the device is created
        vkGetDeviceQueue(device, queueFamilyGraphics, 0, &graphicsQueue);
        vkGetDeviceQueue(device, queueFamilyPresent, 0, &presentQueue);
    }

    void createSurface() {
        // GLFW should handle all platform specific code to generate a surface
        VK_CHECK(glfwCreateWindowSurface(instance, window, nullptr, &surface));
    }

    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
        // Best case is the surface has no preferred format:
        if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
            return {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
        }
        // If best case fails, then try and choose the best format:
        for (const auto &availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_R8G8B8A8_UNORM &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }
        // Else, return the first that is found:
        return availableFormats[0];
    }

    static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
        // VK_PRESENT_MODE_IMMEDIATE_KHR transfers images right away (can result in tearing) Also, only one to be always supported
        // VK_PRESENT_MODE_FIFO_KHR swapchain becomes a queue and the display pops an image when it refreshes (V-Sync)
        // VK_PRESENT_MODE_FIFO_RELAXED_KHR if queue is empty, then it transfers the image as soon as it arrives
        // VK_PRESENT_MODE_MAILBOX_KHR queued images are replaced with newer ones when queue is full
        VkPresentModeKHR bestMode = VK_PRESENT_MODE_MAILBOX_KHR;
        VkPresentModeKHR secondBestMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        VkPresentModeKHR fallbackMode = VK_PRESENT_MODE_FIFO_KHR;

        for (const auto &availablePresentMode :availablePresentModes) {
            if (availablePresentMode == bestMode) {
                return availablePresentMode;
            } else if (availablePresentMode == secondBestMode) {
                fallbackMode = availablePresentMode;
            }
        }
        return fallbackMode;

    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
        if (capabilities.currentExtent.width !=
            std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            VkExtent2D actualExtent = {WIDTH, HEIGHT};

            actualExtent.width =
                    std::max(capabilities.minImageExtent.width,
                             std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height =
                    std::max(capabilities.minImageExtent.height,
                             std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    void createSwapchain() {
        // Check that picked device supports a swapchain
        // If it supports queues, then it also probably supports a swapchain, but checking just to be sure
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto &extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        if (requiredExtensions.empty()) {
            std::cout << "Swapchain is supported";
        } else {
            throw std::runtime_error("Swapchain not supported");
        }

        // Then check that the swapchain supports necessary details
        bool swapChainAdequate;
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        if (swapChainAdequate) {
            std::cout << ", and itself it supports some capabilities, surface formats, and presentation modes"
                      << std::endl;
        } else {
            throw std::runtime_error("Swapchain doesn't support capabilities, surface formats, or presentation modes");
        }

        // Choose surface format (Color depth)
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        // Choose presentation mode (conditions for "swapping" images to the screen)
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        // Choose swap extent (resolution of images in the swapchain)
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        // pick 1 image more than minimum to not have to wait on driver operations before rendering on next image
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 &&
            imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = surface;
        swapchainCreateInfo.minImageCount = imageCount;
        swapchainCreateInfo.imageFormat = surfaceFormat.format;
        swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent = extent;
        swapchainCreateInfo.imageArrayLayers = 1; // more than 1 if creating a stereoscopic view application.
        // If first want to render and then apply post-processing to the image, then need to use VK_IMAGE_USAGE_TRANSFER_DST_BIT as imageUse
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        if (queueFamilyGraphics != queueFamilyPresent) {
            // In VK_SHARING_MODE_CONCURRENT images can be used across multiple queue families without explicit ownership transfer.
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainCreateInfo.queueFamilyIndexCount = 2;
            uint32_t queueFamilyIndices[] = {queueFamilyGraphics, queueFamilyPresent};
            swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            //VK_SHARING_MODE_EXCLUSIVE: An image is owned by one queue family at a time and ownership must be explicitly transferred before using it in another queue family.
            //This option offers the best performance
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreateInfo.queueFamilyIndexCount = 0; // Optional
            swapchainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
        }
        // Can set this to have automatic 90 degree rotation or horizontal flip
        swapchainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        // Specify if alpha channel is used for blending with other windows (not in this case)
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = presentMode;
        // 'clipped' specifies if pixels that are obscured by another window need to be ignored
        swapchainCreateInfo.clipped = VK_TRUE;
        // If a new swapchain is created store a reference to the old one here
        swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

        VK_CHECK(vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain));
        std::cout << "Amount of swapchain images: " << imageCount << std::endl;

        // Retrieve swapchain images handles:
        vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
        swapchainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());

        // Save format and extent of the created swapchain:
        swapchainImageFormat = surfaceFormat.format;
        swapchainExtent = extent;

    }

    void createImageViews() {
        // Amount of views is the same as the amount of images
        swapchainImageViews.resize(swapchainImages.size());

        for (size_t i = 0; i < swapchainImages.size(); i++) {
            VkImageViewCreateInfo imageViewCreateInfo = {};
            imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewCreateInfo.image = swapchainImages[i];
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewCreateInfo.format = swapchainImageFormat;

            // Here, can swizzle color channels around or set some as constant values
            imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
            imageViewCreateInfo.subresourceRange.levelCount = 1;
            imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            imageViewCreateInfo.subresourceRange.layerCount = 1;

            VK_CHECK(vkCreateImageView(device, &imageViewCreateInfo, nullptr, &swapchainImageViews[i]));

        }

    }

    void initializeVulkan() {
        createInstance();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapchain();
        createImageViews();
    }


    void mainLoop() {
        while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
            glfwPollEvents();

        }
    }

    void cleanup() {
        for (auto imageView : swapchainImageViews) {
            vkDestroyImageView(device, imageView, nullptr);
        }
        vkDestroySwapchainKHR(device, swapchain, nullptr);
        vkDestroyDevice(device, nullptr);
        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

int main() {
    Asterism asterism;
    try {
        asterism.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}