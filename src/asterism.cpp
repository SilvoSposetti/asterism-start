#include "renderer/renderer.h"

// External libraries:
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


#include <glm/gtc/matrix_transform.hpp>


class Asterism {
public:
    void run() {
        Renderer renderer;
        renderer.initializeRenderer();

    }

private:
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
