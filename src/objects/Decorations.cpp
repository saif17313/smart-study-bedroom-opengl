#include "Decorations.h"
#include <cmath>
using namespace Materials;

void drawWindow(DrawContext& c, const glm::mat4& p) {
    drawBox(c,p,{0,0,-0.07f},{1.30f,1.45f,0.02f},sky);
    for(float x : {-0.67f,0.67f})
        drawBox(c,p,{x,0,0},{0.08f,1.58f,0.15f},wood);
    for(float y : {-0.765f,0.765f})
        drawBox(c,p,{0,y,0},{1.42f,0.08f,0.15f},wood);
    drawBox(c,p,{0,0,0.025f},{0.048f,1.47f,0.07f},darkWood);
    drawBox(c,p,{0,-0.79f,0.065f},{1.51f,0.075f,0.25f},lightWood);
}

void drawCurtains(DrawContext& c, const glm::mat4& p) {
    drawRod(c,p,{-1.08f,0.84f,0.13f},{1.08f,0.84f,0.13f},0.028f,dark);
    for(float side : {-1.0f,1.0f})
        drawBox(c,p,{side*0.78f,-0.04f,0.16f},{0.47f,1.65f,0.035f},blue);
    // TODO (full version): add folds or movement to these two panels.
}

void drawClock(DrawContext& c, const glm::mat4& p) {
    drawCylinder(c,p,{0,0,0},{0.47f,0.055f,0.47f},dark,90,{1,0,0});
    drawCylinder(c,p,{0,0,0.034f},{0.427f,0.015f,0.427f},linen,90,{1,0,0});
    for(int i=0;i<12;++i) {
        float angle=float(i)*30.0f, rad=glm::radians(angle);
        drawBox(c,p,{0.181f*std::sin(rad),0.181f*std::cos(rad),0.047f},
                {0.009f,0.025f,0.005f},dark,-angle,{0,0,1});
    }
    // The two hands stay at 10:10; their shared center is a future rotation pivot.
    auto hour=transform(p,{0,0,0.051f},{1,1,1},60,{0,0,1});
    auto minute=transform(p,{0,0,0.059f},{1,1,1},-60,{0,0,1});
    drawBox(c,hour,{0,0.053f,0},{0.016f,0.106f,0.006f},dark);
    drawBox(c,minute,{0,0.077f,0},{0.010f,0.154f,0.006f},dark);
}

void drawWallFrame(DrawContext& c, const glm::mat4& p) {
    drawBox(c,p,{0,0,0},{0.65f,0.91f,0.045f},darkWood);
    drawBox(c,p,{0,0,0.029f},{0.58f,0.84f,0.016f},linen);
    drawBox(c,p,{0,0,0.042f},{0.44f,0.67f,0.008f},blueLight);
}

void drawRug(DrawContext& c, const glm::mat4& p) {
    drawBox(c,p,{0,0.012f,0},{2.80f,0.024f,2.20f},blue);
}
