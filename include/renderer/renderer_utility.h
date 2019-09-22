#pragma once

#include <vulkan/vulkan.h>
#include <iostream>
#include <cstring>

void VK_CHECK(VkResult result);
void VK_CHECK(VkResult result, const char *message);