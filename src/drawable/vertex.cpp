#include "drawable/vertex.h"

VkVertexInputBindingDescription Vertex::getBindingDescription() {
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

std::array<VkVertexInputAttributeDescription, 2> Vertex::getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

    // Position:
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    // Typical formats used:
    // float: VK_FORMAT_R32_SFLOAT
    // vec2: VK_FORMAT_R32G32_SFLOAT
    // vec3: VK_FORMAT_R32G32B32_SFLOAT
    // vec4: VK_FORMAT_R32G32B32A32_SFLOAT
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    // 'offset' specifies the offset from the beginning of the struct
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    // Color:
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    return attributeDescriptions;
}
