#include "renderer/renderer.h"


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
