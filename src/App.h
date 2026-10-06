#pragma once
#include "Camera.h"
#include "Simulation.h"
#include "Showcase.h"
#include <GLFW/glfw3.h>

enum class ShadingMode { Flat, Gouraud, Phong };
struct AppState {
    Camera camera;
    ShadingMode shading = ShadingMode::Phong;
    bool captured = false;
    bool firstMouse = true;
    bool screenshotRequested = false;
    bool showHelp = true;
    bool showHud = true;
    bool tour = false;
    double tourTime = 0;
    Simulation simulation;
    ShowcaseState showcase;
    double lastX = 0, lastY = 0;
};
const char* modeName(ShadingMode mode);
void handleKey(GLFWwindow* window, int key, int action);
void mouseMoved(GLFWwindow* window, double x, double y);
void scrolled(GLFWwindow* window, double y);
void updateApplication(AppState& app, float dt);
void updateCameraTour(AppState& app, float dt);
