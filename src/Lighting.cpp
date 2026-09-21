#include "Lighting.h"

void uploadLighting(Shader& shader, const glm::vec3& camera) {
    shader.set("cameraPosition",camera);
    shader.set("ambientColor",glm::vec3(0.33f,0.35f,0.40f));
    shader.set("lightPosition",glm::vec3(0.0f,2.83f,-1.0f));
    shader.set("lightColor",glm::vec3(0.80f,0.75f,0.66f));
    // TODO (full version): add multiple lights only when explicitly requested.
}
