#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>
#include <renderer/camera.h>
#include <vector>
#include "vertex.h"

class Drawable {
public:
//    virtual void draw(Camera camera) = 0;

    void setShader(int shader);

    glm::mat4 getModelMatrix();

    void setPosition(glm::vec3 position);

    void setRotation(float angle, glm::vec3 axis);

    void setScaling(glm::vec3 scale);

protected:
    std::vector<Vertex> vertices;

private:
    // pointer to a shaderModule created by shaderManager

    // ToDo: compute modelMatrix only if any of the matrices above is changed (e.g. during an update())
    glm::vec3 position;
    glm::mat4 positionMatrix;

    float angle;
    glm::vec3 axis;
    glm::mat4 rotationMatrix;

    glm::vec3 scale;
    glm::mat4 scalingMatrix;


};