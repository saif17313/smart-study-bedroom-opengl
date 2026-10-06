#include "Bedroom.h"
using namespace Materials;

void drawBed(DrawContext& c, const glm::mat4& p) {
    for(float x : {-0.69f,0.69f}) for(float z : {-0.90f,0.90f})
        drawBox(c,p,{x,0.16f,z},{0.12f,0.32f,0.12f},darkWood);
    drawBox(c,p,{0,0.35f,0},{1.72f,0.23f,2.19f},wood);
    drawBox(c,p,{0,0.60f,-1.075f},{1.78f,1.07f,0.10f},lightWood);
    drawRoundedBox(c,p,{0,0.56f,0},{1.64f,0.25f,2.10f},linen);
    for(float x : {-0.40f,0.40f})
        drawSphere(c,p,{x,0.79f,-0.69f},{0.75f,0.22f,0.47f},linen,x*8);
    for(float x : {-0.60f,-0.30f,0.0f,0.30f,0.60f})
        drawRoundedBox(c,p,{x,0.89f,-1.007f},{0.24f,0.35f,0.06f},blue);
    drawBox(c,p,{0,1.15f,-1.075f},{1.85f,0.065f,0.15f},darkWood);
    drawRoundedBox(c,p,{0,0.705f,0.28f},{1.61f,0.035f,1.48f},blue);
    for(float side : {-1.0f,1.0f}) {
        for(int i=0;i<10;++i) {
            const float z=-0.39f+i*0.145f;
            drawRoundedBox(c,p,{side*0.818f,0.555f,z},{0.055f,0.32f,0.155f},
                           i%3==0 ? blueLight : blue,side*5,{0,0,1});
        }
    }
    drawRoundedBox(c,p,{0,0.56f,1.043f},{1.66f,0.31f,0.06f},blue);
    for(float z : {-0.32f,-0.26f,0.83f,0.89f})
        drawBox(c,p,{0,0.727f,z},{1.59f,0.007f,0.022f},blueLight);
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
    drawBox(c,p,{0,1.19f,-0.30f},{1.40f,2.10f,0.06f},darkWood);
    for(float side : {-1.0f,1.0f})
        drawBox(c,p,{side*0.67f,1.19f,0},{0.06f,2.10f,0.65f},wood);
    drawBox(c,p,{0,2.26f,0},{1.47f,0.09f,0.71f},darkWood);
    for(float y : {0.16f,0.78f,1.48f,2.21f})
        drawBox(c,p,{0,y,0},{1.32f,0.05f,0.60f},lightWood);
    for(int i=0;i<3;++i)
        drawRoundedBox(c,p,{0.27f,0.86f+i*0.1f,0},{0.48f,0.085f,0.42f},i%2 ? blue : linen);
    drawRoundedBox(c,p,{-0.27f,1.67f,0},{0.45f,0.30f,0.43f},terracotta);
    for(float side : {-1.0f,1.0f}) {
        const auto door=transform(p,{side*0.685f,0,0.34f},{1,1,1},side*c.simulation.wardrobeAngle);
        drawBox(c,door,{-side*0.339f,1.20f,0},{0.674f,1.99f,0.038f},lightWood);
        for(float y : {0.72f,1.65f})
            drawBox(c,door,{-side*0.339f,y,0.026f},{0.52f,0.78f,0.018f},wood);
        drawRod(c,door,{-side*0.623f,0.98f,0.05f},{-side*0.623f,1.25f,0.05f},0.025f,brass);
    }
}

void drawBedsideLamp(DrawContext& c, const glm::mat4& p) {
    drawCylinder(c,p,{0,0.024f,0},{0.24f,0.048f,0.24f},brass);
    drawCylinder(c,p,{0,0.21f,0},{0.035f,0.37f,0.035f},dark);
    const Material shade{{0.93f,0.67f,0.32f},0,1,true};
    drawFrustum(c,p,{0,0.41f,0},{0.31f,0.27f,0.31f},c.simulation.bedsideLight ? shade : linen);
    drawCylinder(c,p,{0,0.274f,0},{0.315f,0.015f,0.315f},brass);
}
