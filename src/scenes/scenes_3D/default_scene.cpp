# include "scenes/scenes_3D/default_scene.h"


void DefaultScene::setup() {
    logTitle("Default setup");
}

void DefaultScene::update() {
    // Update stuff here

}

void DefaultScene::draw() {
    // Call draws here
    this->renderer->drawFrame();
}