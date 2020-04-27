#include "drawable/shapes/quad.h"

Quad::Quad(void) {
    // This is called 'interleaving' vertex attributes (position and color are interleaved together)
    this->vertices = {
            {{-0.5f, 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f,  0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f,  0.0f, 0.5f},  {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.0f, 0.5f},  {1.0f, 1.0f, 1.0f}}
    };


    this->indices = {0, 2, 1, 0, 3, 2};;
}
