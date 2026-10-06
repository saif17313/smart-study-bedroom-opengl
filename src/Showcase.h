#pragma once
#include "Camera.h"
#include "Simulation.h"
#include <cstddef>

struct AppState;
enum class ShadingMode;
enum class ShowcasePhase {
    Intro, Day, Lights, Study, Laptop, Fan, Clock, Door, Wardrobe,
    Curtains, DayCycle, Night, Rain, Shading, CameraTour, Finale, Finish, Count
};

struct ShowcaseCameraPose {
    glm::vec3 position, target;
    float fov = 65;
    bool overview = false;
};

struct ShowcaseSnapshot {
    Camera camera;
    Simulation simulation;
    ShadingMode shading{};
    bool tour = false, showHud = true, showHelp = true;
    double tourTime = 0;
};

// The controller changes production targets; Simulation owns every object animation.
struct ShowcaseState {
    bool active = false, paused = false;
    double time = 0;
    ShowcasePhase phase = ShowcasePhase::Intro;
    const char* label = "AUTOMATED PROJECT SHOWCASE";
    ShowcaseSnapshot saved;
    std::size_t nextEvent = 0;
    ShowcaseCameraPose cameraFrom, cameraTo;
    double cameraTime = 0, cameraDuration = 0;
    double hourFrom = 0, hourDelta = 0, hourTime = 0, hourDuration = 0;

    void start(AppState& app);
    void stop(AppState& app, bool completed = false);
    void update(AppState& app, float dt);
    static double duration();
    static int phaseCount();
    static const char* phaseName(ShowcasePhase phase);
};
