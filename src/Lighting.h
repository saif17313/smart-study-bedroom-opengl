#pragma once
#include "Shader.h"
#include "Simulation.h"

// Window fill, ceiling point light, bedside point light and desk spotlight.
void uploadLighting(Shader& shader, const glm::vec3& camera, const Simulation& simulation);
