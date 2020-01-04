#include "renderer/input_manager.h"

// Global scope type qualifier
InputManager *InputManager::eventHandlingInstance;

void InputManager::keyCallback(GLFWwindow *window, int key, int scanCode, int action, int mods) {
    if (key == GLFW_KEY_E && action == GLFW_PRESS) {
//        print("Pressed E");
    } else if (key == GLFW_KEY_E && action == GLFW_RELEASE) {
//        print("Released E");
    }
//    else if (key == GLFW_KEY_E && action == GLFW_REPEAT) {
//        print("Repeated E");
//    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        this->toggleFullscreen(window);
//        print("Pressed E");
    } else if (key == GLFW_KEY_F && action == GLFW_RELEASE) {
//        print("Released E");
    }
    else if (key == GLFW_KEY_F && action == GLFW_REPEAT) {
        this->toggleFullscreen(window);
        print("Repeated E");
    }
}

void InputManager::toggleFullscreen(GLFWwindow *window) {
    this->isFullScreen = !isFullScreen;

    if (isFullScreen) {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        if (monitor) {
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            glfwGetWindowPos(window, &this->lastWindowedPosX, &this->lastWindowedPosY);
            glfwGetWindowSize(window, &this->lastWindowedWidth, &this->lastWindowedHeight);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
    } else {
        glfwSetWindowMonitor(window, nullptr, lastWindowedPosX, lastWindowedPosY, lastWindowedWidth, lastWindowedHeight, 0);
    }
}

void InputManager::mousePositionCallback(GLFWwindow *window, double _posX, double _posY) {
//    std::cout << "Mouse pos: " << posX << " - " << posY << std::endl;
    this->posX = _posX;
    this->posY = _posY;

}

void InputManager::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            this->isMouseLeftPressed = true;
//            print("Pressed left mouse button");
        } else if (action == GLFW_RELEASE) {
            this->isMouseLeftPressed = false;
//            print("Released left mouse button");
        }
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            this->isMouseRightPressed = true;
//            print("Pressed right mouse button");
        } else if (action == GLFW_RELEASE) {
            this->isMouseRightPressed = false;
//            print("Released right mouse button");
        }
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (action == GLFW_PRESS) {
            this->isMouseMiddlePressed = true;
//            print("Pressed middle mouse button");
        } else if (action == GLFW_RELEASE) {
            this->isMouseMiddlePressed = false;
//            print("Released middle mouse button");
        }
    }
}

void InputManager::mouseScrollCallback(GLFWwindow *window, double _offsetX, double _offsetY) {
//    std::cout << "Mouse scroll: " << offsetX << " - " << offsetY << std::endl;
    this->offsetX = _offsetX;
    this->offsetY = _offsetY;

}
