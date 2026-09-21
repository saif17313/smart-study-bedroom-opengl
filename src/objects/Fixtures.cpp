#include "Fixtures.h"
using namespace Materials;

void drawDoor(DrawContext& c, const glm::mat4& hinge) {
    // TODO (full version): rotate around this hinge when door animation is requested.
    drawBox(c,hinge,{0.475f,1.06f,0},{0.93f,2.12f,0.065f},wood);
    for(float x : {-0.025f,0.975f}) drawBox(c,hinge,{x,1.10f,0.015f},{0.07f,2.20f,0.13f},darkWood);
    drawBox(c,hinge,{0.475f,2.18f,0.015f},{1.06f,0.085f,0.13f},darkWood);
    drawBox(c,hinge,{0.84f,1.06f,0.05f},{0.06f,0.22f,0.019f},dark);
    drawRod(c,hinge,{0.84f,1.1f,0.065f},{0.84f,1.1f,0.12f},0.028f,metal);
    drawRod(c,hinge,{0.72f,1.1f,0.12f},{0.84f,1.1f,0.12f},0.028f,dark);
}

void drawFan(DrawContext& c, const glm::mat4& p) {
    drawFrustum(c,p,{0,-0.055f,0},{0.18f,0.11f,0.18f},dark,180,{1,0,0});
    drawRod(c,p,{0,-0.1f,0},{0,-0.31f,0},0.047f,dark);
    const auto rotor=transform(p,{0,-0.37f,0});
    // TODO (full version): animate the rotor only after the static demo.
    drawCylinder(c,rotor,{0,0,0},{0.28f,0.14f,0.28f},dark);
    for(int i=0;i<3;++i) {
        const auto blade=transform(rotor,{0,0,0},{1,1,1},i*120.0f+15.0f);
        drawRod(c,blade,{0.09f,0.015f,0},{0.24f,0.025f,0.035f},0.045f,brass);
        drawRoundedBox(c,blade,{0.50f,0.025f,0.04f},{0.65f,0.023f,0.20f},wood,-8);
    }
}

void drawCeilingLight(DrawContext& c, const glm::mat4& p) {
    drawCylinder(c,p,{0,0.04f,0},{0.38f,0.08f,0.38f},dark);
    drawCylinder(c,p,{0,-0.015f,0},{0.36f,0.045f,0.36f},brass);
    drawSphere(c,p,{0,-0.045f,0},{0.35f,0.12f,0.35f},linen);
}
