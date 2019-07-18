#include "renderer/queue_manager.h"

QueueManager::QueueManager(const std::vector<QueueType> &flags) {
    queueFlags = flags;
    // add the present queue to the necessary queues:
    queueFlags.push_back(PRESENT);

    numOfQueues = queueFlags.size();
    families.resize(numOfQueues);
    vkQueues.resize(numOfQueues);
}

void QueueManager::retrieveAvailableQueueIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {

    // Check that picked device supports all necessary queues
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    // Find all required queues:
    std::vector<uint32_t> indicesFound(numOfQueues, invalidQueueIndex);
    for (uint32_t i = 0; i < numOfQueues; i++) {
        if (queueFlags[i] == GRAPHICS) {
            for (int j = 0; j < queueFamilyCount; ++j) {
                if (queueFamilies[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    indicesFound[i] = j;
                }
            }
        } else if (queueFlags[i] == COMPUTE) {
            for (int j = 0; j < queueFamilyCount; ++j) {
                if (queueFamilies[j].queueFlags & VK_QUEUE_COMPUTE_BIT) {
                    indicesFound[i] = j;
                }
            }
        } else if (queueFlags[i] == PRESENT) {
            VkBool32 presentSupport = false;
            for (int j = 0; j < queueFamilyCount; ++j) {
                vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, j, surface, &presentSupport);
                if (presentSupport) {
                    indicesFound[i] = j;
                    break;
                }
            }
        }
    }

    for (uint32_t i = 0; i < numOfQueues; ++i) {
        if (indicesFound[i] == invalidQueueIndex) {
            throw std::runtime_error("GPU doesn't support some necessary queues!");
        }
    }
    families = indicesFound;
}


// Setters:
void QueueManager::setQueues(VkDevice device) {
    for (uint32_t i = 0; i < numOfQueues; ++i) {
        VkQueue queue = VK_NULL_HANDLE;
        vkGetDeviceQueue(device, families[i], 0, &queue);
        vkQueues[i] = queue;
    }
    printQueueInfo();
}

//Getters:
uint32_t QueueManager::getFamilyIndex(QueueType flag) {
    uint32_t i = getFlagIndex(flag);
    return families[i];
}

VkQueue *QueueManager::getQueue(QueueType flag) {
    uint32_t i = getFlagIndex(flag);
    return &vkQueues[i];
}


uint32_t QueueManager::getFlagIndex(QueueType flag) {
    for (uint32_t i = 0; i < queueFlags.size(); ++i) {
        if (queueFlags[i] == flag) {
            return i;
        }
    }
    throw std::runtime_error("Flag not found in queue");
}

// Other:
void QueueManager::printQueueInfo() {
    std::cout << "Queue family indices: " << std::endl;
    for (uint32_t i = 0; i < numOfQueues; ++i) {
        std::cout << "\t";
        if (queueFlags[i] == GRAPHICS) {
            std::cout << "Graphics";
        } else if (queueFlags[i] == COMPUTE) {
            std::cout << "Compute";
        } else if (queueFlags[i] == PRESENT) {
            std::cout << "Present";
        }
        std::cout << " - " << families[i];
    }
    std::cout << std::endl;
}
