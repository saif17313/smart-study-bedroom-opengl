#include "Lighting.h"

void uploadLighting(Shader& shader, const glm::vec3& camera, const Simulation& s) {
    shader.set("cameraPosition",camera);
    const float daylight = s.daylight() * (s.weather == Weather::Rain ? 0.50f : 1.0f);
    const float windowTransmission = 0.15f + 0.85f * s.curtainAmount;
    shader.set("ambientColor",glm::vec3(0.022f,0.028f,0.05f)
        + glm::vec3(0.20f,0.22f,0.25f) * daylight * windowTransmission
        + glm::vec3(0.075f,0.067f,0.052f) * (s.ceilingLight ? 1.0f : 0.0f));
    const glm::vec3 positions[] = {{-0.05f,2.25f,-2.55f},{0,2.88f,-1},
                                  {-2.85f,0.97f,-0.15f},{2.40f,1.32f,-2.23f}};
    const glm::vec3 colors[] = {
        glm::vec3(0.85f,0.94f,1.0f) * daylight * windowTransmission,
        glm::vec3(1.15f,1.02f,0.82f) * (s.ceilingLight ? 1.0f : 0.0f),
        glm::vec3(1.8f,0.92f,0.35f) * (s.bedsideLight ? 1.0f : 0.0f),
        glm::vec3(1.9f,1.65f,1.18f) * (s.studyLight ? 1.0f : 0.0f)};
    const glm::vec3 attenuation[] = {{1,0.08f,0.025f},{1,0.08f,0.035f},
                                    {1,0.32f,0.45f},{1,0.20f,0.22f}};
    for (int i=0;i<4;++i) {
        const auto prefix = "lights[" + std::to_string(i) + "].";
        shader.set(prefix+"position", positions[i]);
        shader.set(prefix+"color", colors[i]);
        shader.set(prefix+"attenuation", attenuation[i]);
        shader.set(prefix+"direction", glm::vec3(0,-1,0));
        shader.set(prefix+"spot", i == 3 ? 1.0f : 0.0f);
    }
}
