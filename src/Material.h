#pragma once
#include <glm/glm.hpp>

struct Material {
    glm::vec3 color;
    float specular = 0.12f;
    float shininess = 24.0f;
    bool unlit = false;
};

namespace Materials {
inline const Material wall{{0.76f,0.73f,0.66f},0.03f,8};
inline const Material ceiling{{0.84f,0.81f,0.75f},0.02f,8};
inline const Material trim{{0.88f,0.85f,0.78f},0.12f,24};
inline const Material wood{{0.34f,0.19f,0.095f},0.23f,40};
inline const Material lightWood{{0.51f,0.32f,0.17f},0.18f,32};
inline const Material darkWood{{0.19f,0.10f,0.055f},0.18f,32};
inline const Material linen{{0.85f,0.80f,0.69f},0.02f,8};
inline const Material blue{{0.23f,0.31f,0.43f},0.04f,12};
inline const Material blueLight{{0.39f,0.48f,0.61f},0.03f,12};
inline const Material dark{{0.045f,0.055f,0.065f},0.25f,48};
inline const Material metal{{0.26f,0.29f,0.30f},0.65f,72};
inline const Material brass{{0.55f,0.39f,0.16f},0.48f,48};
inline const Material terracotta{{0.56f,0.27f,0.16f},0.08f,12};
inline const Material sky{{0.13f,0.28f,0.47f},0,1,true};
}
