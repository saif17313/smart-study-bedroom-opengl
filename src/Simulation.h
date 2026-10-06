#pragma once
#include <glm/glm.hpp>

enum class Weather { Clear, Rain };

// All motion is advanced explicitly, so captures and paused shading comparisons
// render the same state without consulting the wall clock inside draw functions.
struct Simulation {
    static constexpr int maxFanSpeedLevel = 4;
    bool paused = false;
    bool fanOn = true, doorOpen = false, curtainsOpen = true;
    int fanSpeedLevel = 2; // Remember the last non-zero level while the fan is off.
    bool wardrobeOpen = false, drawerOpen = false, laptopOpen = true;
    bool keyboardBacklightOn = true;
    bool ceilingLight = true, bedsideLight = true, studyLight = true;
    bool dayCycle = true;
    Weather weather = Weather::Clear;
    double elapsed = 0;
    double clockSeconds = 10 * 3600 + 10 * 60;
    double hour = 10.0 + 10.0 / 60.0;
    float fanAngle = 0, fanSpeed = 0;
    float doorAngle = 0, curtainAmount = 1;
    float wardrobeAngle = 0, drawerAmount = 0, laptopAmount = 1;

    void update(float dt);
    void changeFanSpeed(int delta);
    float targetFanSpeed() const;
    const char* fanSpeedName() const;
    float keyboardBacklightBrightness() const;
    void toggleDayNight();
    float daylight() const;
    glm::vec3 skyColor() const;
};
const char* weatherName(Weather weather);
