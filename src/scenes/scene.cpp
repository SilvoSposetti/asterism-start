#include "scenes/scene.h"
//#include <chrono>

void Scene::run() {
    this->frameCount = 0;
    this->dt = 0.0;

    this->setup();
    this->initializeCore();
    this->core();
}


void Scene::core() {

    while (this->renderer->checkLoop()) {
        this->renderer->rendererPollEvents();

        auto start = std::chrono::high_resolution_clock::now();
//        logTitle("NEW FRAME");

        this->update();
        this->draw();

        auto end = std::chrono::high_resolution_clock::now();
        this->dt = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
//        log("dt: " + std::to_string(this->dt));
//        log("frameCount: " + std::to_string(this->frameCount));

        this->frameCount++;
    }
    this->renderer->afterLoop();

    this->terminateCore();
}

