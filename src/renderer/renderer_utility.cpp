#include "renderer/renderer_utility.h"

void VK_CHECK(VkResult result) {
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Something went wrong");
    }
}

void VK_CHECK(VkResult result, const char *message) {
    if (result != VK_SUCCESS) {
        char error_msg[8] = "Error: ";
        throw std::runtime_error(strcat(error_msg, message));
    }
//    else{
//        char success_msg[10] = "Success: ";
//        std::cout <<strcat(success_msg, message) << std::endl;
//    }
}