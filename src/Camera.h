#pragma once
#include <glm/glm.hpp>

class Camera {
public:
    glm::vec3 position{2.65f, 1.85f, 2.6f};
    float yaw = -125.0f;
    float pitch = -8.0f;
    float fov = 76.0f;
    bool overview = false;
    Camera();
    glm::vec3 forward() const;
    glm::mat4 view() const;
    void lookAt(const glm::vec3& target);
    void reset();
    void toggleOverview();
    void move(float forwardAmount, float rightAmount, float upAmount, float dt);
    void look(float dx, float dy);
    void zoom(float amount);
};
