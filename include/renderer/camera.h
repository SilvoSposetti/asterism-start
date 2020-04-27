#pragma once

#include <glm/gtc/matrix_transform.hpp>

struct ViewParams {
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
};

enum ProjType {
    perspective,
    orthogonal
};

struct PerspectiveParams {
    float vFov;
    glm::vec2 screensize;
    float nearPlane;
    float farPlane;
};

struct OrthogonalParams {
    glm::vec2 screensize;
    float nearPlane;
    float farPlane;
};

class Camera {
public:
    Camera(ViewParams viewParams, PerspectiveParams perspectiveParams);
    Camera(ViewParams viewParams, OrthogonalParams orthogonalParams);

private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;

    glm::mat4 view;

    void setupViewMatrix(ViewParams viewParams);

    void updateViewMatrix();

    ProjType projType;
    float vFov;
    glm::vec2 screenSize;
    float nearPlane;
    float farPlane;
    glm::mat4 proj;

    void updateProjectionMatrix();

};