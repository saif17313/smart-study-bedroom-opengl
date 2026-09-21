#include "Study.h"
using namespace Materials;

void drawTable(DrawContext& c, const glm::mat4& p) {
    drawRoundedBox(c,p,{0,0.735f,0},{1.95f,0.07f,0.74f},lightWood);
    for(float x : {-0.86f,0.86f}) for(float z : {-0.26f,0.26f})
        drawBox(c,p,{x,0.35f,z},{0.065f,0.70f,0.065f},wood);
}

void drawChair(DrawContext& c, const glm::mat4& p) {
    drawRoundedBox(c,p,{0,0.49f,0},{0.56f,0.10f,0.55f},blue);
    drawRoundedBox(c,p,{0,0.81f,0.235f},{0.53f,0.64f,0.075f},wood);
    for(float x : {-0.22f,0.22f}) for(float z : {-0.21f,0.21f})
        drawBox(c,p,{x,0.22f,z},{0.055f,0.44f,0.055f},wood);
}

void drawLaptop(DrawContext& c, const glm::mat4& p) {
    drawRoundedBox(c,p,{0,0.018f,0},{0.49f,0.035f,0.32f},metal);
    const auto screen=transform(p,{0,0.037f,-0.15f},{1,1,1},-12,{1,0,0});
    drawRoundedBox(c,screen,{0,0.15f,0},{0.49f,0.30f,0.022f},blueLight);
}

void drawBookshelf(DrawContext& c, const glm::mat4& p) {
    drawBox(c,p,{0,0.40f,-0.10f},{1.67f,0.80f,0.025f},darkWood);
    for(float x : {-0.82f,0.82f})
        drawBox(c,p,{x,0.40f,0},{0.055f,0.85f,0.28f},wood);
    for(float y : {0.0f,0.40f,0.80f})
        drawBox(c,p,{0,y,0},{1.69f,0.05f,0.29f},wood);

    // Three plain books are enough to identify the shelf in the progress demo.
    drawBox(c,p,{-0.55f,0.175f,0.02f},{0.09f,0.30f,0.18f},blue);
    drawBox(c,p,{-0.43f,0.155f,0.02f},{0.09f,0.26f,0.18f},terracotta);
    drawBox(c,p,{-0.31f,0.165f,0.02f},{0.09f,0.28f,0.18f},linen);
    // TODO (full version): add richer book geometry and desk details if requested.
}
