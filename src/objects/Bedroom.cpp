#include "Bedroom.h"
using namespace Materials;

void drawBed(DrawContext& c, const glm::mat4& p) {
    for(float x : {-0.69f,0.69f}) for(float z : {-0.90f,0.90f})
        drawBox(c,p,{x,0.16f,z},{0.12f,0.32f,0.12f},darkWood);
    drawBox(c,p,{0,0.35f,0},{1.72f,0.23f,2.19f},wood);
    drawBox(c,p,{0,0.60f,-1.075f},{1.78f,1.07f,0.10f},lightWood);
    drawRoundedBox(c,p,{0,0.56f,0},{1.64f,0.25f,2.10f},linen);
    // Keep one curved pillow for a clear Flat/Gouraud/Phong comparison.
    drawSphere(c,p,{0,0.785f,-0.67f},{1.05f,0.22f,0.49f},linen);
    drawRoundedBox(c,p,{0,0.705f,0.28f},{1.61f,0.035f,1.48f},blue);
    // TODO (full version): restore draped bedding and headboard detail if requested.
}

void drawBedsideTable(DrawContext& c, const glm::mat4& p) {
    for(float x : {-0.19f,0.19f}) for(float z : {-0.19f,0.19f})
        drawBox(c,p,{x,0.255f,z},{0.055f,0.51f,0.055f},wood);
    drawBox(c,p,{0,0.55f,0},{0.52f,0.065f,0.52f},lightWood);
    drawBox(c,p,{0,0.40f,0},{0.43f,0.22f,0.43f},wood);
    drawBox(c,p,{0,0.40f,0.225f},{0.40f,0.18f,0.028f},linen);
    drawBox(c,p,{0,0.41f,0.25f},{0.11f,0.025f,0.025f},dark);
}

void drawWardrobe(DrawContext& c, const glm::mat4& p) {
    drawBox(c,p,{0,0.07f,0},{1.36f,0.14f,0.61f},darkWood);
    drawBox(c,p,{0,1.19f,0},{1.40f,2.10f,0.65f},wood);
    for(float side : {-1.0f,1.0f}) {
        drawBox(c,p,{side*0.346f,1.20f,0.34f},{0.674f,1.99f,0.038f},lightWood);
        drawBox(c,p,{side*0.062f,1.11f,0.375f},{0.025f,0.27f,0.035f},dark);
    }
}

void drawBedsideLamp(DrawContext& c, const glm::mat4& p) {
    // Model only: the lamp has no light source or toggle.
    drawCylinder(c,p,{0,0.024f,0},{0.24f,0.048f,0.24f},brass);
    drawCylinder(c,p,{0,0.21f,0},{0.035f,0.37f,0.035f},dark);
    drawFrustum(c,p,{0,0.41f,0},{0.31f,0.27f,0.31f},linen);
}
