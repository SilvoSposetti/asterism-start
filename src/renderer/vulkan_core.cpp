#include "renderer/vulkan_core.h"

VkInstance VulkanCore::createInstance(const char *asterismName, bool isDebug) {
    std::cout << "*** Running " << asterismName << " in ";
    if (isDebug) {
        std::cout << "DEBUG";
    } else {
        std::cout << "RELEASE";
    }
    std::cout << " mode ***" << std::endl;


    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = asterismName;
    applicationInfo.apiVersion = VK_VERSION_1_1;

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;

    // Activate validation layer only in debug:
    if (isDebug) {
        const char *debugLayers[] = {
                "VK_LAYER_LUNARG_standard_validation"
        };
        instanceCreateInfo.ppEnabledLayerNames = debugLayers;
        instanceCreateInfo.enabledLayerCount = sizeof(debugLayers) / sizeof(debugLayers[0]);
        std::cout << ("Validation Layers ENABLED") << std::endl;
    }

    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;
    instanceCreateInfo.enabledExtensionCount = glfwExtensionCount;

    VkInstance instance;
    VK_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));

    return instance;
}


VkSurfaceKHR VulkanCore::createSurface(VkInstance instance, GLFWwindow *window) {
    VkSurfaceKHR surface = nullptr;
    // GLFW should handle all platform specific code to generate a surface
    VK_CHECK(glfwCreateWindowSurface(instance, window, nullptr, &surface));
    return surface;
}


VkPhysicalDevice VulkanCore::createPhysicalDevice(VkInstance instance, VkSurfaceKHR surface) {
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

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
        throw std::runtime_error("No GPU found");
    }
    return physicalDevice;

}


bool VulkanCore::isDiscreteGPU(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);

    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        printf("Using discrete GPU: %s\n", properties.deviceName);
        return true;
    }
    return false;
}

bool VulkanCore::isIntegratedGPU(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);

    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
        printf("Using integrated GPU: %s\n", properties.deviceName);
        return true;
    }
    return false;
}


VkDevice VulkanCore::createLogicalDevice(VkPhysicalDevice physicalDevice,
                                         std::vector<const char *> deviceExtensions,
                                         QueueManager queues) {
    std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {queues.getFamilyIndex(GRAPHICS), queues.getFamilyIndex(PRESENT)};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily: uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.queueFamilyIndex = queueFamily;
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

    VkDevice device;
    VK_CHECK(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device), "Failed to create device");

    return device;
}
