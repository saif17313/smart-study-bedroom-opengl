#pragma once
#include "Shader.h"

// One fixed source for the three shading demonstrations; no lamp controls.
void uploadLighting(Shader& shader, const glm::vec3& camera);
