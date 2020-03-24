#include "renderer/renderer.h"
#include "scenes/scenes_3D/default_scene.h"


class Asterism {
public:
    static void run() {
//        Renderer renderer;
//        renderer.initializeRenderer();
        std::shared_ptr<DefaultScene> defaultScene = std::make_shared<DefaultScene>();
        defaultScene->run();
    }

private:
};

int main() {
    try {
        Asterism::run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
