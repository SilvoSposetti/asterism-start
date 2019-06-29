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
    const int WIDTH = 1024;
    const int HEIGHT = 768;

    GLFWwindow *window = nullptr;

    VkInstance instance = nullptr;
    VkSurfaceKHR surface = nullptr;
    uint32_t queueFamilyGraphics = 0;
    VkQueue graphicsQueue = nullptr;
    uint32_t queueFamilyPresent = 0;
    VkQueue presentQueue = nullptr;
    VkPhysicalDevice physicalDevice = nullptr;
    VkDevice device = nullptr;


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
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());
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


    void initializeVulkan() {
        createInstance();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
    }


    void mainLoop() {
        while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
            glfwPollEvents();

        }
    }

    void cleanup() {
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