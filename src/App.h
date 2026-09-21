#pragma once
#include "Camera.h"
#include <GLFW/glfw3.h>

enum class ShadingMode { Flat, Gouraud, Phong };
struct AppState {
    Camera camera;
    ShadingMode shading = ShadingMode::Phong;
    bool captured = false;
    bool firstMouse = true;
    bool screenshotRequested = false;
    double lastX = 0, lastY = 0;
};
const char* modeName(ShadingMode mode);
void handleKey(GLFWwindow* window, int key, int action);
void mouseMoved(GLFWwindow* window, double x, double y);
void scrolled(GLFWwindow* window, double y);
