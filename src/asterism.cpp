// External libraries:
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// Other libraries:
#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>
#include <fstream> // used to load shader SPIR-V binaries
#include <array>

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

    VkRenderPass renderPass = nullptr;
    VkPipelineLayout pipelineLayout = nullptr;

    VkPipeline graphicsPipeline = nullptr;

    std::vector<VkFramebuffer> swapchainFrameBuffers;

    VkCommandPool commandPool = nullptr;

    std::vector<VkCommandBuffer> commandBuffers;


    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    const int MAX_FRAMES_IN_FLIGHT = 2; //Todo: this should actually be the amount of images in the swapchain - 1?
    size_t currentFrame = 0;
    std::vector<VkFence> inFlightFences;

    VkBuffer vertexBuffer = nullptr;
    VkDeviceMemory vertexBufferMemory = nullptr;
    VkBuffer indexBuffer = nullptr;
    VkDeviceMemory indexBufferMemory = nullptr;

    bool frameBufferResized = false;

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };


    struct Vertex {
        glm::vec2 pos;
        glm::vec3 color;

        static VkVertexInputBindingDescription getBindingDescription() {
            // All of the per-vertex data is packed into one array, so only one binding
            VkVertexInputBindingDescription bindingDescription = {};
            // 'binding' specifies the index of the binding in the array of bindings
            bindingDescription.binding = 0;
            // 'stride' defines the number of bytes from one entry to the next
            bindingDescription.stride = sizeof(Vertex);
            // VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each vertex
            // VK_VERTEX_INPUT_RATE_INSTANCE: Move to the next data entry after each instance
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

            // Position:
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            // Typical formats used:
            // float: VK_FORMAT_R32_SFLOAT
            // vec2: VK_FORMAT_R32G32_SFLOAT
            // vec3: VK_FORMAT_R32G32B32_SFLOAT
            // vec4: VK_FORMAT_R32G32B32A32_SFLOAT
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            // 'offset' specifies the offset from the beginning of the struct
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            // Color:
            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            return attributeDescriptions;
        }
    };

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
            throw std::runtime_error("Something went wrong");
        }
    }

    void initializeWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Tell GLFW not to create an OpenGL context.
        // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Block window resize

        window = glfwCreateWindow(WIDTH, HEIGHT, asterismName, nullptr, nullptr);

        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
    }

    static void frameBufferResizeCallback(GLFWwindow *window, int width, int height) {
        auto *app = reinterpret_cast<Asterism *>(glfwGetWindowUserPointer(window));
        app->frameBufferResized = true;

        if (width == 0 or height == 0) {
            std::cout << "Window has been minimized" << std::endl;
        } else {
            std::cout << "Window has been resized (" << width << "x" << height << ")" << std::endl;
        }
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
            // Note that it's very likely that these queues end up being the same after all
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
            int width;
            int height;
            glfwGetFramebufferSize(window, &width, &height);
            VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

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
            std::cout << ", and itself it supports surface formats and presentation modes"
                      << std::endl;
        } else {
            throw std::runtime_error("Swapchain doesn't support surface formats or presentation modes");
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
        swapchainCreateInfo.imageArrayLayers = 1; // more than 1 if creating a stereoscopic view application. (e.g. VR)
        // If first want to render and then apply post-processing to the image, then need to use VK_IMAGE_USAGE_TRANSFER_DST_BIT as imageUsage
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

    void createRenderPass() {

        //###################################################
        // Attachment description:
        VkAttachmentDescription attachmentDescription = {};
        // Format should match that of the swapchain
        attachmentDescription.format = swapchainImageFormat;
        // Increase if multisampling is needed
        attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        // loadOp: what to do before rendering
        // VK_ATTACHMENT_LOAD_OP_CLEAR clears the values to a constant at the start
        // VK_ATTACHMENT_LOAD_OP_LOAD preserves the existing contents of the attachment
        // VK_ATTACHMENT_LOAD_OP_DONT_CARE is self explanatory
        attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        // storeOp: what to do after rendering
        // VK_ATTACHMENT_STORE_OP_STORE contents of the attachment are stored in memory to read later (e.g for displaying them on the screen)
        // VK_ATTACHMENT_STORE_OP_DONT_CARE contents of the attachment will be undefined after rendering
        attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        // Don't do anything with stencil, so don't care
        attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        // The layout of the images in memory can change based on the purpose of the images
        // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL Images used as color attachment
        // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR Images to be presented in the swap chain
        // VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: Images to be used as destination for a memory copy operation
        attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        //###################################################
        // Subpasses and attachment references:
        // Every subpass of a render pass references to one or more attachments.
        VkAttachmentReference attachmentRef = {};
        // the reference is specified by the index of the attachment into an attachment descriptions array
        attachmentRef.attachment = 0;
        attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        // Specify for what should the subpass be used:
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &attachmentRef;

        //###################################################
        // Subpass dependencies:
        // Basically ensures that the render passes can't begin until the image is available
        VkSubpassDependency subpassDependency = {};
        // VK_SUBPASS_EXTERNAL refers to the implicit subpass before or after the render pass
        subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpassDependency.dstSubpass = 0;
        // Specify the operations that need to wait in the stages that these operations occur
        subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.srcAccessMask = 0;
        subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


        //###################################################
        // Render pass:
        VkRenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = 1;
        renderPassCreateInfo.pAttachments = &attachmentDescription;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;
        renderPassCreateInfo.dependencyCount = 1;
        renderPassCreateInfo.pDependencies = &subpassDependency;

        VK_CHECK(vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass));


    }


    static std::vector<char> readFile(const std::string &filename) {
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

    VkShaderModule createShaderModule(const std::vector<char> &code) {
        // need to wrap the code in a VkShaderModule before passing it to the pipeline.
        VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
        shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCreateInfo.codeSize = code.size();
        shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

        VkShaderModule shaderModule = {};
        VK_CHECK(vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule));

        return shaderModule;
    }

    void createGraphicsPipeline() {
        //###################################################
        // Shader modules:
        std::vector<char> vertShaderCode = readFile("F:/Git Repo/asterism/shaders/vert.spv");
        std::vector<char> fragShaderCode = readFile("F:/Git Repo/asterism/shaders/frag.spv");

        VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

        VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo = {};
        vertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageCreateInfo.module = vertShaderModule;
        // Specify the entry point for the shader
        // (can have for example different entry points for different shader behaviours in the same .spv)
        vertShaderStageCreateInfo.pName = "main";
        // (Optional)
        vertShaderStageCreateInfo.pSpecializationInfo = nullptr;

        VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo = {};
        fragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageCreateInfo.module = fragShaderModule;
        // Specify the entry point for the shader
        // (can have for example different entry points for different shader behaviours in the same .spv)
        fragShaderStageCreateInfo.pName = "main";
        // (Optional)
        fragShaderStageCreateInfo.pSpecializationInfo = nullptr;

        VkPipelineShaderStageCreateInfo shaderStageCreateInfos[] = {vertShaderStageCreateInfo,
                                                                    fragShaderStageCreateInfo};

        // Above here: Shaders are programmable functions
        // Below here: Apply the correct parameters to the fixed function stages:

        //###################################################
        // Vertex shader input:
        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        // Bindings indicate the spacing between data and whether the data is per-vertex or per-instance (instancing)
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        // Attribute descriptions: type of the attributes passed to the vertex shader
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();


        //###################################################
        // Input assembly:
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
        inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        // Set whet kind of do the vertices represent
        inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        // It's possible to break up lines and triangles having the '_STRIP' topology if this is set to true
        inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

        //###################################################
        // Viewport and scissors:
        // Viewport describes the region of the framebuffer that the output will be rendered to (usually the entire screen)
        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapchainExtent.width;
        viewport.height = (float) swapchainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        // Scissor rectangles define in which regions pixels will actually be stored
        // (any pixel outside of the scissor rectangles will be discarded by the rasterizer)
        VkRect2D scissor = {};
        scissor.offset = {0, 0};
        scissor.extent = swapchainExtent;

        VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
        viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateCreateInfo.viewportCount = 1;
        viewportStateCreateInfo.pViewports = &viewport;
        viewportStateCreateInfo.scissorCount = 1;
        viewportStateCreateInfo.pScissors = &scissor;

        //###################################################
        // Rasterizer:
        // Takes the geometry and turns it into fragments. Also does depth testing, face culling and scissor testing.
        VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
        rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        // If depthClampEnable is true, then fragments that are beyond the near and far planes are clamped to them
        // as opposed as being discarded (useful for shadow maps)
        rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
        // If rasterizerDiscardEnable is set to true, then geometry never passes through the rasterizer stage
        // (basically disables any output to the framebuffer
        rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        // Polygon modes available: (using any mode other than fill requires enabling a GPU feature)
        // VK_POLYGON_MODE_FILL : draw the whole triangle given vertices
        // VK_POLYGON_MODE_LINE : draw only the edges of the triangles
        // VK_POLYGON_MODE_POINT : draw only the vertices of the triangles
        rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
        // Line thickness: any value above 1.0 requires the 'widelines' GPU feature
        rasterizationStateCreateInfo.lineWidth = 1.0f;
        // Cull mode: can cull back-facing triangles, front-facing, both, or none
        rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        // Front face: choose if front-face is going to be generated clockwise or counterclockwise
        rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        // Depth bias: can alter the depth values by adding a constant value or biasing them based on a fragment's slope
        // (Sometime used for shadow mapping)
        rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
        rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f; // Optional
        rasterizationStateCreateInfo.depthBiasClamp = 0.0f; // Optional
        rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f; // Optional

        //###################################################
        // Multisampling: (Enabling it requires a GPU feature)
        VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
        multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
        multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampleStateCreateInfo.minSampleShading = 1.0f; // Optional
        multisampleStateCreateInfo.pSampleMask = nullptr; // Optional
        multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE; // Optional

        //###################################################
        // Depth and stencil testing:
        // (not used at the moment, so pass a nullptr)

        //###################################################
        // Color blending: (the fragment shader returns a color, how should this color replace what currently is in the frame buffer?)
        VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
        colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                                   VK_COLOR_COMPONENT_G_BIT |
                                                   VK_COLOR_COMPONENT_B_BIT |
                                                   VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachmentState.blendEnable = VK_FALSE;
        colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

        // The operations performed follow this pseudo-code: (for more see page 115/297 of the tutorial)
        // if (blendEnable) {
        //     finalColor.rgb = (srcColorBlendFactor * newColor.rgb)
        //                        <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
        //     finalColor.a = (srcAlphaBlendFactor * newColor.a)
        //     <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
        //     } else {
        //     finalColor = newColor;
        //     }
        // finalColor = finalColor & colorWriteMask;

        VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
        colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
        colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlendStateCreateInfo.attachmentCount = 1;
        colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
        colorBlendStateCreateInfo.blendConstants[0] = 0.0f; // Optional
        colorBlendStateCreateInfo.blendConstants[1] = 0.0f; // Optional
        colorBlendStateCreateInfo.blendConstants[2] = 0.0f; // Optional
        colorBlendStateCreateInfo.blendConstants[3] = 0.0f; // Optional

        //###################################################
        // Dynamic state:
        // Some of the pipeline parameters can be updated without re-creating the whole pipeline.
        // For doing so, the following structure needs to be filled, otherwise a nullptr can be passed to the pipeline

        // VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT,
        //                                   VK_DYNAMIC_STATE_LINE_WIDTH};
        // VkPipelineDynamicStateCreateInfo dynamicState = {};
        // dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        // dynamicState.dynamicStateCount = 2;
        // dynamicState.pDynamicStates = dynamicStates;

        //###################################################
        // Pipeline layout: (pass variables to shaders at draw time (uniforms))
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout));

        //###################################################
        // Pipeline:
        VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.stageCount = 2;
        pipelineCreateInfo.pStages = shaderStageCreateInfos;
        // Fixed function stages:
        pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
        pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
        pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
        pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
        pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
        pipelineCreateInfo.pDepthStencilState = nullptr; // Optional
        pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
        pipelineCreateInfo.pDynamicState = nullptr;
        // pipelineLayout handle:
        pipelineCreateInfo.layout = pipelineLayout;
        // render pass and index of the graphics subpass where the graphics pipeline will be used
        pipelineCreateInfo.renderPass = renderPass;
        pipelineCreateInfo.subpass = 0;
        // Can create a new pipeline derived from an existing one with these two parameters
        //These values are only used if the VK_PIPELINE_CREATE_DERIVATIVE_BIT flag is also specified
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineCreateInfo.basePipelineIndex = -1; // Optional

        // vkCreateGraphicsPipelines is actually designed to handle multiple pipelineCreateInfos and
        // consequently it can create multiple pipelines
        VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphicsPipeline));

        // Shader modules can be destroyed as soon as the pipeline is created
        vkDestroyShaderModule(device, fragShaderModule, nullptr);
        vkDestroyShaderModule(device, vertShaderModule, nullptr);


    }

    void createFramebuffers() {
        // resize the container to hold all the framebuffers
        swapchainFrameBuffers.resize(swapchainImageViews.size());
        // Create a framebuffer for each image view:
        for (size_t i = 0; i < swapchainImageViews.size(); ++i) {
            VkFramebufferCreateInfo framebufferCreateInfo = {};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.renderPass = renderPass;
            framebufferCreateInfo.attachmentCount = 1;
            framebufferCreateInfo.pAttachments = &swapchainImageViews[i];
            framebufferCreateInfo.width = swapchainExtent.width;
            framebufferCreateInfo.height = swapchainExtent.height;
            framebufferCreateInfo.layers = 1;

            VK_CHECK(vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &swapchainFrameBuffers[i]));
        }
    }

    void createCommandPool() {
        VkCommandPoolCreateInfo commandPoolCreateInfo = {};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.queueFamilyIndex = queueFamilyGraphics;
        // Possible flags:
        // VK_COMMAND_POOL_CREATE_TRANSIENT_BIT: Hint that command buffers are rerecorded with new commands very often (may change memory allocation behavior)
        // VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT: Allow command buffers to be rerecorded individually, without this flag they all have to be reset together
        commandPoolCreateInfo.flags = 0; // Optional (none of the above are used)

        VK_CHECK(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool));
    }


    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if (typeFilter & ((uint32_t) 1 << i) &&
                (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void createBuffer(VkDeviceSize size,
                      VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties,
                      VkBuffer &buffer,
                      VkDeviceMemory &bufferMemory) {

        // Buffer Creation:
        VkBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        // 'size' specifies the size of the buffer in bytes
        bufferCreateInfo.size = size;
        // multiple usages can be specified with a bitwise OR
        bufferCreateInfo.usage = usage;
        // like images in swapchain, buffers can be owned by a specific queue family or shared across some.
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // Create vertex buffer:
        VK_CHECK(vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer));

        // Memory Requirements and Allocation:
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memRequirements);


        VkMemoryAllocateInfo memoryAllocateInfo = {};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memRequirements.size;
        // VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ensures that memory used by CPU and GPU is coherent
        memoryAllocateInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        VK_CHECK(vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &bufferMemory));

        // If allocation is successful, then the memory can be associated with the buffer.
        // The fourth parameter is the offset within the region of memory. If the offset is non-zero, then it is
        // required to be divisible by memRequirements.alignment
        vkBindBufferMemory(device, buffer, bufferMemory, 0);


    }

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
        // Memory transfer operations are executed using command buffers, therefore a temporary command buffer allocation is needed.
        VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandPool = commandPool;
        commandBufferAllocateInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);

        // Record the temporary command buffer:
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // command buffer is only used once
        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        // This command buffer only contains the copy command, so can stop recording after it.
        vkEndCommandBuffer(commandBuffer);

        // Now, submit it for execution:
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);

        // Clean up the command buffer used for transfer operation
        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }

    void createVertexBuffer() {
        // Here the objective is to use a vertex buffer using the most optimal memory type, the VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT flag.
        // Usually this type of memory (GPU local memory) is not accessible to the CPU on dedicated graphics card.
        // The solution is to first fill a staging buffer accessible to the CPU, and then copy the content of the
        // staging buffer into the other local buffer, the one which is actually used.

        // Thus the staging buffer has the VK_BUFFER_USAGE_TRANSFER_SRC_BIT flag as it is used as source in the memory transfer operation,
        // the actual buffer has the VK_BUFFER_USAGE_TRANSFER_DST_BIT flag as it is used as destination in the memory transfer operation.

        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        // 'stagingBuffer' is a host visible buffer and is only temporary
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        createBuffer(bufferSize,
                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     stagingBuffer,
                     stagingBufferMemory);

        // Filling the Staging Buffer:
        void *data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        // 'vertexBuffer' is the device local buffer and is used as vertex buffer
        createBuffer(bufferSize,
                     VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     vertexBuffer,
                     vertexBufferMemory);

        // Copy the content from one buffer to the other:
        copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }

    void createIndexBuffer() {
        // The same that's said for the vertex buffer also applies to the index buffer
        // The usage of the actual buffer is different (VK_BUFFER_USAGE_INDEX_BUFFER_BIT instead of VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)

        VkDeviceSize bufferSize = sizeof(vertexIndices[0]) * vertexIndices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     stagingBuffer,
                     stagingBufferMemory);

        void *data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0,
                    &data);
        memcpy(data, vertexIndices.data(), (size_t) bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     indexBuffer,
                     indexBufferMemory);

        copyBuffer(stagingBuffer, indexBuffer, bufferSize);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);

    }

    void createCommandBuffers() {
        commandBuffers.resize(swapchainFrameBuffers.size());

        // Command buffer allocation:
        VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.commandPool = commandPool;
        // VK_COMMAND_BUFFER_LEVEL_PRIMARY: Can be submitted to a queue for execution, but cannot be called from other command buffers
        // VK_COMMAND_BUFFER_LEVEL_SECONDARY: Cannot be submitted directly, but can be called from primary command buffers
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandBufferCount = (uint32_t) commandBuffers.size();

        VK_CHECK(vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers.data()));

        for (size_t i = 0; i < commandBuffers.size(); i++) {
            // Starting command buffer recording:
            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            // Flags specify how the command buffer is going to be used
            // VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT: The command buffer will be rerecorded right after executing it once.
            // VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT: This is a secondary command buffer that will be entirely within a single render pass.
            // VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT: The command buffer can be resubmitted while it is also already pending execution.
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
            beginInfo.pInheritanceInfo = nullptr; // Optional

            VK_CHECK(vkBeginCommandBuffer(commandBuffers[i], &beginInfo));

            // Starting a render pass:
            VkRenderPassBeginInfo renderPassInfo = {};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = swapchainFrameBuffers[i];
            // Render area defines where shader loads and stores will take place (match size of attachment for best performance)
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = swapchainExtent;
            // Clear color for the VK_ATTACHMENT_LOAD_OP_CLEAR
            VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;

            // VK_SUBPASS_CONTENTS_INLINE: The render pass commands will be embedded in the primary command buffer itself and no secondary command buffers will be executed.
            // VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS: The render pass commands will be executed from secondary command buffers.
            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            // Can now bind the graphics pipeline:
            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

            // Also, can bind the vertexBuffer:
            VkBuffer vertexBuffers[] = {vertexBuffer};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

            // And the index buffer: (index type is decided by the index type)
            vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT16);

            // Draw command:
            // Inputs are: (in order)
            // vertexCount defines how many vertices need to be drawn
            // instanceCount used for instance rendering, 1 if instance rendering isn't used
            // firstVertex defines the lowest value of gl_VertexIndex
            // firstInstance defines the lowest value of gl_InstanceIndex
            vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(vertexIndices.size()), 1, 0, 0, 0);
            // End render pass:
            vkCmdEndRenderPass(commandBuffers[i]);

            // Finished recording the command buffer:
            VK_CHECK(vkEndCommandBuffer(commandBuffers[i]));
        }


    }

    void createSyncObjects() {
        // Semaphores are best used for GPU <--> GPU synchronization
        // Fences are best used for CPU <--> GPU synchronization
        // Each frame should have its own semaphore and fence

        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        // Start up the fence in a signaled state, so that the first frame can be directly drawn
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]));
            VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]));
            VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]));
        }
    }

    void initializeVulkan() {
        createInstance();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapchain();
        createImageViews();
        createRenderPass();
        createGraphicsPipeline();
        createFramebuffers();
        createCommandPool();
        createVertexBuffer();
        createIndexBuffer();
        createCommandBuffers();
        createSyncObjects();
    }

    void recreateSwapchain() {
        // Handle minimization:
        int width = 0;
        int height = 0;
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }

        // Wait for resources to be available
        vkDeviceWaitIdle(device);

        // Destroy all VK entities that have to do with the current swapchain
        cleanupSwapchain();

        // Create them with the correct values again
        createSwapchain();
        createImageViews();
        createRenderPass();
        createGraphicsPipeline();
        createFramebuffers();
        createCommandBuffers();
    }

    void cleanupSwapchain() {
        for (VkFramebuffer frameBuffer : swapchainFrameBuffers) {
            vkDestroyFramebuffer(device, frameBuffer, nullptr);
        }
        vkDestroyPipeline(device, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        vkDestroyRenderPass(device, renderPass, nullptr);
        for (VkImageView imageView : swapchainImageViews) {
            vkDestroyImageView(device, imageView, nullptr);
        }
        vkDestroySwapchainKHR(device, swapchain, nullptr);
    }


    void drawFrame() {
        // Wait for the frame to be finished:
        // The VK_TRUE passed here indicates that all fences need to be signaled before continuing
        // (in this case we have a single one so it doesn't really matter)
        // The last parameter is a timeout for the next frame to become available and setting it to the maximum 64-bit
        // unsigned int disables the timeout
        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

        //###################################################
        // 1. Acquire an image from the swapchain (swapchain is an extension, so we require the vk*KHR naming convention)
        uint32_t imageIndex;
        // The third parameter specifies a timeout in nanoseconds for an image to become available,
        // using the maximum value of 64 bit unsigned integer disables the timeout.
        VkResult acquireNextImageResult = vkAcquireNextImageKHR(device, swapchain, std::numeric_limits<uint64_t>::max(),
                                                                imageAvailableSemaphores[currentFrame],
                                                                VK_NULL_HANDLE, &imageIndex);

        if (acquireNextImageResult == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapchain();
            return;
        } else if (acquireNextImageResult != VK_SUCCESS && acquireNextImageResult != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swapchjain image");
        }

        //###################################################
        // 2. Submitting and executing the command buffer with that image as attachment in the framebuffer
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        // Need to wait with writing colors to the image until it's available
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        // Specify which command buffers to actually submit for execution
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
        // pSignalSemaphores specifies which semaphore to signal when the command buffers have finished execution
        VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        // Submit commands to the queue: (last parameter defines an optional fence to be signaled when the command buffer finishes execution)
        // This fence corresponds to the inFlightFence that is used to synchronise the CPU with the GPU

        vkResetFences(device, 1, &inFlightFences[currentFrame]);

        VK_CHECK(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]));

        //###################################################
        // 3. Return the image to the swapchain for presentation
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        // Specify which semaphores ot wait on before presentation can happen
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        // Specify which swapchains to present images to and the index of the image for each swapchain
        VkSwapchainKHR swapChains[] = {swapchain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        // Can additionally specify an array of VkResult values to check if every swapchain presentation was succesfull
        // which is not really necessary for a single swapchain, since the return value of the present function can be used
        presentInfo.pResults = nullptr; // Optional

        VkResult queuePresentResult = vkQueuePresentKHR(presentQueue, &presentInfo);

        if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || queuePresentResult == VK_SUBOPTIMAL_KHR ||
            frameBufferResized) {
            frameBufferResized = false;
            recreateSwapchain();
        } else if (queuePresentResult != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image");
        }

        // If the CPU is submitting work faster than the GPU can keep up, the queue will slowly fill up with work.
        // To avoid this, we can allow a certain amount of frames to be 'in-flight' while still bounding the amount
        // of work that piles up. Currentframe thus is a value
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
            glfwPollEvents();
            drawFrame();
        }
        // When the window is closed, there might still be operations going on. Need to wait until all operations are done
        // before cleaning up resources
        vkDeviceWaitIdle(device);
    }

    void cleanup() {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device, inFlightFences[i], nullptr);
        }
        vkDestroyCommandPool(device, commandPool, nullptr);

        cleanupSwapchain();

        vkDestroyBuffer(device, vertexBuffer, nullptr);
        vkFreeMemory(device, vertexBufferMemory, nullptr);

        vkDestroyBuffer(device, indexBuffer, nullptr);
        vkFreeMemory(device, indexBufferMemory, nullptr);

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