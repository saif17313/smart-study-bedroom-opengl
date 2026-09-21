#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

Camera::Camera() { reset(); }
glm::vec3 Camera::forward() const {
    const float y = glm::radians(yaw), p = glm::radians(pitch);
    return glm::normalize(glm::vec3(std::cos(y)*std::cos(p), std::sin(p), std::sin(y)*std::cos(p)));
}
glm::mat4 Camera::view() const { return glm::lookAt(position, position + forward(), {0,1,0}); }
void Camera::lookAt(const glm::vec3& target) {
    const glm::vec3 direction = glm::normalize(target - position);
    yaw = glm::degrees(std::atan2(direction.z, direction.x));
    pitch = glm::degrees(std::asin(direction.y));
}
void Camera::reset() {
    overview = false;
    position = {2.65f, 1.85f, 2.60f};
    fov = 76.0f;
    lookAt({0.0f, 1.05f, -1.0f});
}
void Camera::toggleOverview() {
    if (overview) { reset(); return; }
    overview = true;
    position = {0.0f, 7.4f, 9.8f};
    fov = 48.0f;
    lookAt({0.0f, 0.8f, -0.25f});
}
void Camera::move(float ahead, float right, float up, float dt) {
    glm::vec3 direction = forward()*ahead + glm::normalize(glm::cross(forward(), glm::vec3(0,1,0)))*right;
    direction += glm::vec3(0,up,0);
    if (glm::length(direction) > 0.001f) position += glm::normalize(direction)*2.0f*dt;
}
void Camera::look(float dx, float dy) {
    yaw += dx*0.10f;
    pitch = std::clamp(pitch + dy*0.10f, -89.0f, 89.0f);
}
void Camera::zoom(float amount) { fov = std::clamp(fov - amount*2.0f, 30.0f, 85.0f); }
