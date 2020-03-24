#pragma once

#include "scenes/scene.h"

class Scene2D : public virtual Scene {
public:

private:
    void initializeCore() final;

    void terminateCore() final;

};
