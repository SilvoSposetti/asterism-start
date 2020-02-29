#include "renderer/vulkan_core.h"

VkInstance VulkanCore::createInstance(std::string asterismName, bool isDebug) {
    std::string mode = isDebug ? "DEBUG" : "RELEASE";
    logTitle("Running " + std::string(asterismName) + " in " + mode + " mode");


    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = asterismName.c_str();
    applicationInfo.apiVersion = VK_VERSION_1_2;

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;

    // Activate validation layer only in debug:
    if (isDebug) {
        const char *debugLayers[] = {
                "VK_LAYER_LUNARG_standard_validation",
                "VK_LAYER_LUNARG_monitor"
        };
        instanceCreateInfo.ppEnabledLayerNames = debugLayers;
        instanceCreateInfo.enabledLayerCount = sizeof(debugLayers) / sizeof(debugLayers[0]);
        log("Validation Layers ENABLED");
    }

    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;
    instanceCreateInfo.enabledExtensionCount = glfwExtensionCount;

    // Best practices validation extension:
    if(isDebug){
        VkValidationFeatureEnableEXT enables[] = {VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
                                                  // The next feature requires a PhysicalDevice that has the feature fragmentStoresAndAtomics and vertexPipelineStoresAndAtomics
//                                                  VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT// increase featureCount if this is not commented
        };
        VkValidationFeaturesEXT features = {};
        features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        features.enabledValidationFeatureCount = 1;
        features.pEnabledValidationFeatures = enables;

        instanceCreateInfo.pNext = &features;
    }

    VkInstance instance;
    VK_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &instance), "Instance Creation");

    return instance;
}


VkSurfaceKHR VulkanCore::createSurface(VkInstance instance, GLFWwindow *window) {
    VkSurfaceKHR surface = nullptr;
    // GLFW should handle all platform specific code to generate a surface
    VK_CHECK(glfwCreateWindowSurface(instance, window, nullptr, &surface), "GLFW Window Surface Creation");
    return surface;
}


VkPhysicalDevice VulkanCore::createPhysicalDevice(VkInstance instance) {
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

//     If a discrete GPU is not available, pick the first one
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

    uint32_t version = properties.apiVersion;
    std::cout << std::endl;

    uint32_t major = VK_VERSION_MAJOR(version);
    uint32_t minor = VK_VERSION_MINOR(version);
    uint32_t patch = VK_VERSION_PATCH(version);

    std::cout << major << " " << minor << " " << patch << std::endl;

    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        log(std::string("Using discrete GPU: ") + properties.deviceName);
        return true;
    }
    return false;
}

bool VulkanCore::isIntegratedGPU(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);

    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
        log(std::string("Using integrated GPU: ") + properties.deviceName);
        return true;
    }
    return false;
}


VkDevice VulkanCore::createLogicalDevice(VkPhysicalDevice physicalDevice,
                                         std::vector<const char *> deviceExtensions,
                                         QueueManager queues) {
    std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {queues.getFamilyIndex(GRAPHICS_QUEUE), queues.getFamilyIndex(PRESENT_QUEUE)};

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
    VK_CHECK(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device), "Logical Device Creation");

    return device;
}
