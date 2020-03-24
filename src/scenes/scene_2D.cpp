# include "scenes/scene_2D.h"

void Scene2D::initializeCore() {
    // ToDo: Initialize static cam and 2D renderer (quad or triangle) here
    this->renderer = std::make_shared<Renderer>();
    renderer->initializeRenderer();
}


void Scene2D::terminateCore() {
    // ToDo: Terminate renderer and static cam here
    this->renderer->cleanup();

}