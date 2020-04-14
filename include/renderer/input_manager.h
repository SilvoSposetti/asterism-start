#pragma once

#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "renderer_utility.h"

class InputManager {
public:

    // The class stores state about the input events.
    // Need to store a reference to itself, such that the static dispatcher functions can call the correct callbacks.
    // The callbacks operate using the state of the eventHandlingInstance, which must be set before the callbacks are hooked.
    // See more about this here: https://stackoverflow.com/questions/21799746/how-to-glfwsetkeycallback-for-different-classes
    static InputManager *eventHandlingInstance;

    void setEventHandling() { eventHandlingInstance = this; }

    // Static functions that get called when GLFW events are triggered.
    static void GLFWAPI keyCallbackDispatcher(GLFWwindow *window, int key, int scanCode, int action, int mods) {
        if (eventHandlingInstance)
            eventHandlingInstance->keyCallback(window, key, scanCode, action, mods);
    }

    static void mousePositionCallbackDispatcher(GLFWwindow *window, double posX, double posY) {
        if (eventHandlingInstance)
            eventHandlingInstance->mousePositionCallback(window, posX, posY);
    }

    static void mouseButtonCallbackDispatcher(GLFWwindow *window, int button, int action, int mods) {
        if (eventHandlingInstance)
            eventHandlingInstance->mouseButtonCallback(window, button, action, mods);
    }

    static void mouseScrollCallbackDispatcher(GLFWwindow *window, double offsetX, double offsetY) {
        if (eventHandlingInstance)
            eventHandlingInstance->mouseScrollCallback(window, offsetX, offsetY);
    }

    // Mouse position
    double posX;
    double posY;

    // Mouse Clicks
    bool isMouseLeftPressed = false;
    bool isMouseRightPressed = false;
    bool isMouseMiddlePressed = false;

    // Mouse scroll
    double offsetX;
    double offsetY;

private:
    // Fullscreen functionality
    bool isFullScreen = false;
    int lastWindowedPosY;
    int lastWindowedPosX;
    int lastWindowedWidth;
    int lastWindowedHeight;

    void toggleFullscreen(GLFWwindow *window);



    // Member functions that are called from the dispatchers with the proper InputManager handling instance
    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    void mousePositionCallback(GLFWwindow *window, double posX, double posY);

    void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

    void mouseScrollCallback(GLFWwindow *window, double offsetX, double offsetY);


};