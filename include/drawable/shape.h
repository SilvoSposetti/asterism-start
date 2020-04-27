#pragma once

#include "drawable.h"

class Shape : public virtual Drawable {
public:

    // Constructor of derived classes defines vertices and faces for its shape


protected:
    // Either uint16_t or uint32_t can be used depending on the amount of vertices used (>=65535)
    std::vector<uint16_t> indices;
    // IndexFormat indexFormat;

private:
};