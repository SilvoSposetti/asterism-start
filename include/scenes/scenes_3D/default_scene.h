#pragma once

#include "scenes/scene_3D.h"

class DefaultScene : public virtual Scene3D {
public:

private:
    void setup() final;

    void update() final;

    void draw() final;

};
