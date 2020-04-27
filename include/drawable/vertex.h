#pragma once

#include <vulkan/vulkan.h>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

class Vertex {
public:
    glm::vec3 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription();

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
};