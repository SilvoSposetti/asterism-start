# include "scenes/scene_3D.h"


void Scene3D::initializeCore() {
    // ToDo: Initialize dynamic cam and 3D renderer
    this->renderer = std::make_shared<Renderer>();
    renderer->initializeRenderer();
}


void Scene3D::terminateCore() {
    // ToDo: Terminate 3D renderer and dynamic cam here
    this->renderer->cleanup();
}