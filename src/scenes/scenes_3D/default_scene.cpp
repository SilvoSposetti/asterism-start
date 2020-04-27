#include "scenes/scenes_3D/default_scene.h"
#include "drawable/shapes/quad.h"

void DefaultScene::setup() {
    logTitle("Default setup");
    // Initialize camera

//    std::shared_ptr<Quad> quad = std::make_shared<Quad>();
    Quad quad = Quad();
    // Initialize quad
    // Initialize shaders for quad, then set the shader to the quad
}

void DefaultScene::update() {
    // Update cam stuff (e.g position)

    // Update quad stuff (e.g rotation)

}

void DefaultScene::draw() {
    // Call draws here
    this->renderer->drawFrame();
    // e.g this->quad->draw(camera)
}