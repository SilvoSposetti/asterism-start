#include <iostream>
#include <GLFW/glfw3.h>
#include <assert.h>

int main() {
    std::cout << "Hello, Vulkan with a window!\n" << std::endl;

    int rc = glfwInit();
    assert(rc);

    GLFWwindow *window = glfwCreateWindow(1024, 768, "Hello, Asterism!", 0, 0);
    assert(window);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    return 0;
}