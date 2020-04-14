#include "renderer/camera.h"


Camera::Camera(ViewParams viewParams, PerspectiveParams perspectiveParams) {
    this->setupViewMatrix(viewParams);
    this->projType = perspective;

//    this->setu
    // ToDo: add setupProjectionMatrices

}

Camera::Camera(ViewParams viewParams, OrthogonalParams orthogonalParams) {
    this->setupViewMatrix(viewParams);
    this->projType = orthogonal;
}

void Camera::setupViewMatrix(ViewParams viewParams) {
    this->position = viewParams.position;
    this->target = viewParams.target;
    this->up = viewParams.up;
    this->updateViewMatrix();
}

void Camera::updateViewMatrix() {
    this->view = glm::lookAt(position, target, up);
}

void Camera::updateProjectionMatrix() {
    if (this->projType == perspective) {
        this->proj = glm::perspectiveFov<float>(
                this->vFov,
                float(this->screenSize[0]),
                float(this->screenSize[1]),
                this->nearPlane,
                this->farPlane);

    } else if (this->projType == orthogonal) {
        float aspectRatio = this->screenSize[0] / this->screenSize[1];

        this->proj = glm::ortho<float>(
                -aspectRatio,
                aspectRatio,
                -1.0f,
                1.0f,
                this->farPlane, // These two somehow need to be inverted
                this->nearPlane); // These two somehow need to be inverted
    }
    // glm was designed with OpenGL in mind, where the y coordinate of the clip coordinates is inverted
    this->proj[1][1] *= -1;

}
// ToDo: camera should write view and proj to the uniform buffer object of the drawable object
// ToDo: model should instead be written by a drawable object, assuming its position, scale and orientation