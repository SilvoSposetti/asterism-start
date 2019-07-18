#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>
#include <iostream>

enum QueueType {
    GRAPHICS,
    COMPUTE,
    PRESENT
};

class QueueManager {
public:
    explicit QueueManager(const std::vector<QueueType> &flags);

    void retrieveAvailableQueueIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

    void setQueues(VkDevice device);

    uint32_t getFamilyIndex(QueueType flag);

    VkQueue *getQueue(QueueType flag);


private:
    const uint32_t invalidQueueIndex = 999;

    std::vector<QueueType> queueFlags;
    size_t numOfQueues;
    std::vector<uint32_t> families;
    std::vector<VkQueue> vkQueues;

    uint32_t getFlagIndex(QueueType flag);

    void printQueueInfo();

};


