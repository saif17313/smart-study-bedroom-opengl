#include "Simulation.h"
#include <algorithm>
#include <cmath>

// Degrees per second; Medium preserves the original running speed.
static constexpr float fanSpeeds[] = {0.0f, 120.0f, 240.0f, 360.0f, 480.0f};

static float approach(float value, float target, float distance) {
    return value < target ? std::min(value + distance, target) : std::max(value - distance, target);
}

void Simulation::update(float dt) {
    if (paused || !std::isfinite(dt) || dt <= 0) return;
    elapsed += dt;
    clockSeconds = std::fmod(clockSeconds + dt, 86400.0);
    // A full demonstration day takes four minutes; the wall clock keeps real seconds.
    if (dayCycle) hour = std::fmod(hour + dt * 0.1, 24.0);
    const float target = targetFanSpeed();
    const float acceleration = 120.0f;
    const float rampTime = std::min(dt, std::abs(target - fanSpeed) / acceleration);
    const float nextSpeed = approach(fanSpeed, target, acceleration * dt);
    // Integrate the acceleration ramp, including the constant-speed remainder.
    fanAngle = std::fmod(fanAngle + (fanSpeed + nextSpeed) * 0.5f * rampTime
                        + nextSpeed * (dt - rampTime), 360.0f);
    fanSpeed = nextSpeed;
    doorAngle = approach(doorAngle, doorOpen ? 100.0f : 0.0f, 70.0f * dt);
    curtainAmount = approach(curtainAmount, curtainsOpen ? 1.0f : 0.0f, 0.7f * dt);
    wardrobeAngle = approach(wardrobeAngle, wardrobeOpen ? 105.0f : 0.0f, 80.0f * dt);
    drawerAmount = approach(drawerAmount, drawerOpen ? 1.0f : 0.0f, 1.5f * dt);
    laptopAmount = approach(laptopAmount, laptopOpen ? 1.0f : 0.0f, 1.2f * dt);
}

void Simulation::changeFanSpeed(int delta) {
    const int level = std::clamp((fanOn ? fanSpeedLevel : 0) + delta, 0, maxFanSpeedLevel);
    if (level > 0) fanSpeedLevel = level;
    fanOn = level > 0;
}

float Simulation::targetFanSpeed() const {
    return fanSpeeds[fanOn ? fanSpeedLevel : 0];
}

const char* Simulation::fanSpeedName() const {
    static constexpr const char* names[] = {"OFF", "LOW", "MEDIUM", "HIGH", "MAX"};
    return names[fanOn ? fanSpeedLevel : 0];
}

float Simulation::keyboardBacklightBrightness() const {
    // Fade with the animated lid; a closed or barely exposed keyboard stays dark.
    constexpr float startOpen = 0.15f, fullyOpen = 0.35f;
    return keyboardBacklightOn ? std::clamp((laptopAmount - startOpen) / (fullyOpen - startOpen), 0.0f, 1.0f) : 0.0f;
}

void Simulation::toggleDayNight() {
    hour = daylight() > 0.5f ? 22.0 : 10.0;
    dayCycle = false;
}

float Simulation::daylight() const {
    const float sun = std::sin(static_cast<float>((hour - 6.0) * 3.141592653589793 / 12.0));
    const float t = std::clamp((sun + 0.12f) / 0.65f, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

glm::vec3 Simulation::skyColor() const {
    const auto day = weather == Weather::Rain ? glm::vec3(0.15f,0.20f,0.26f) : glm::vec3(0.23f,0.52f,0.78f);
    return glm::mix(glm::vec3(0.009f,0.016f,0.047f), day, daylight());
}

const char* weatherName(Weather weather) { return weather == Weather::Rain ? "RAIN" : "CLEAR"; }
