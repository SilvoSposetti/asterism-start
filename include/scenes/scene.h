#pragma once

#include "renderer/renderer.h"
#include "renderer/renderer_utility.h"

class Scene {
public:


    std::shared_ptr<Renderer> renderer;
    uint32_t frameCount;
    double_t dt;

    void run();

    // Submits draw/other commands to the Renderer, maybe better in the subclasses
    //    virtual void submit(Camera c, Drawable d) = 0;

private:



    // These functions differ for each type of scene (e.g 2D, 3D, etc...)
    virtual void initializeCore() = 0;

    virtual void terminateCore() = 0;

    // These ones instead differ for each scene, independent of the type
    virtual void setup() = 0;

    virtual void update() = 0;

    virtual void draw() = 0;

    void core();
};
