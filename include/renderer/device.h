#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

class Device {
public:
     void initializeDevice();
     void getVkInstance();
     void getSurface();
     void getPhysicalDevice();
     void getLogicalDevice();

private:
    void createInstance();

    void createSurface();

    void pickPhysicalDevice();

    void createLogicalDevice();
};