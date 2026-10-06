#include "Study.h"
using namespace Materials;

void drawTable(DrawContext& c, const glm::mat4& p) {
    drawRoundedBox(c,p,{0,0.735f,0},{1.95f,0.07f,0.74f},lightWood);
    for(float x : {-0.86f,0.86f}) for(float z : {-0.26f,0.26f})
        drawBox(c,p,{x,0.35f,z},{0.065f,0.70f,0.065f},wood);
    for(float x : {0.38f,0.82f})
        drawBox(c,p,{x,0.60f,0},{0.035f,0.21f,0.65f},wood);
    const auto drawer=transform(p,{0.60f,0.52f,c.simulation.drawerAmount*0.40f});
    drawBox(c,drawer,{0,0,0},{0.40f,0.025f,0.59f},darkWood);
    for(float x : {-0.2f,0.2f})
        drawBox(c,drawer,{x,0.075f,0},{0.025f,0.15f,0.59f},lightWood);
    drawBox(c,drawer,{0,0.075f,-0.28f},{0.40f,0.15f,0.025f},wood);
    drawBox(c,drawer,{0,0.075f,0.30f},{0.47f,0.19f,0.04f},lightWood);
    drawRod(c,drawer,{-0.075f,0.075f,0.35f},{0.075f,0.075f,0.35f},0.025f,brass);
    drawBox(c,drawer,{0,0.026f,0},{0.27f,0.025f,0.35f},linen);
}

void drawChair(DrawContext& c, const glm::mat4& p) {
    drawRoundedBox(c,p,{0,0.49f,0},{0.56f,0.10f,0.55f},blue);
    drawRoundedBox(c,p,{0,0.81f,0.235f},{0.53f,0.64f,0.075f},wood);
    for(float x : {-0.22f,0.22f}) for(float z : {-0.21f,0.21f})
        drawBox(c,p,{x,0.22f,z},{0.055f,0.44f,0.055f},wood);
}

void drawLaptop(DrawContext& c, const glm::mat4& p) {
    drawRoundedBox(c,p,{0,0.018f,0},{0.49f,0.035f,0.32f},metal);
    for(int row=0;row<4;++row) for(int col=0;col<10;++col)
        drawBox(c,p,{-0.196f+col*0.0435f,0.038f,-0.09f+row*0.034f},{0.035f,0.004f,0.024f},dark);
    drawBox(c,p,{0,0.038f,0.09f},{0.14f,0.004f,0.075f},dark);
    const auto screen=transform(p,{0,0.047f,-0.15f},{1,1,1},90-102*c.simulation.laptopAmount,{1,0,0});
    drawRoundedBox(c,screen,{0,0.15f,0},{0.49f,0.30f,0.022f},dark);
    const Material display{{0.035f,0.13f,0.22f},0,1,true};
    drawBox(c,screen,{0,0.15f,0.014f},{0.45f,0.26f,0.006f},display);
    const Material cyan{{0.19f,0.75f,0.83f},0,1,true};
    const Material screenText{{0.68f,0.79f,0.84f},0,1,true};
    drawBox(c,screen,{-0.15f,0.15f,0.019f},{0.09f,0.22f,0.003f},blue);
    for(int i=0;i<7;++i)
        drawBox(c,screen,{-0.025f+(i%2)*0.022f,0.24f-i*0.029f,0.020f},
                {0.16f+(i%3)*0.02f,0.008f,0.003f},i%3==0 ? cyan : screenText);
}

void drawBookshelf(DrawContext& c, const glm::mat4& p) {
    drawBox(c,p,{0,0.40f,-0.10f},{1.67f,0.80f,0.025f},darkWood);
    for(float x : {-0.82f,0.82f})
        drawBox(c,p,{x,0.40f,0},{0.055f,0.85f,0.28f},wood);
    for(float y : {0.0f,0.40f,0.80f})
        drawBox(c,p,{0,y,0},{1.69f,0.05f,0.29f},wood);

    const Material covers[]={blue,terracotta,linen,blueLight,lightWood};
    for(int shelf=0;shelf<2;++shelf) for(int i=0;i<7;++i) {
        const float h=0.23f+(i%3)*0.033f;
        const auto book=transform(p,{-0.66f+i*0.13f+shelf*0.12f,0.026f+shelf*0.4f,0.035f});
        drawBox(c,book,{0,h/2,0},{0.105f,h,0.19f},covers[(i+shelf)%5]);
        drawBox(c,book,{0,h/2,0.101f},{0.07f,h-0.035f,0.01f},linen);
        for(float y : {0.05f,h-0.05f})
            drawBox(c,book,{0,y,0.11f},{0.075f,0.011f,0.008f},covers[(i+shelf)%5]);
    }
    drawSphere(c,p,{0.61f,0.145f,0},{0.20f,0.20f,0.20f},brass);
    drawBox(c,p,{0.61f,0.04f,0},{0.24f,0.04f,0.22f},dark);
}

void drawStudyLamp(DrawContext& c, const glm::mat4& p) {
    drawCylinder(c,p,{0,0.018f,0},{0.27f,0.035f,0.27f},dark);
    drawRod(c,p,{0,0.03f,0},{0,0.31f,-0.06f},0.028f,brass);
    drawRod(c,p,{0,0.31f,-0.06f},{-0.12f,0.63f,-0.03f},0.028f,brass);
    drawSphere(c,p,{0,0.31f,-0.06f},{0.065f,0.065f,0.065f},metal);
    drawFrustum(c,p,{-0.12f,0.60f,-0.03f},{0.25f,0.18f,0.25f},blue);
    const Material glow{{1.0f,0.85f,0.56f},0,1,true};
    drawCylinder(c,p,{-0.12f,0.506f,-0.03f},{0.215f,0.008f,0.215f},c.simulation.studyLight ? glow : linen);
}

void drawDeskAccessories(DrawContext& c, const glm::mat4& p) {
    drawBox(c,p,{0,0.018f,0},{0.23f,0.036f,0.29f},terracotta,-8);
    drawBox(c,p,{0,0.038f,0},{0.21f,0.006f,0.27f},linen,-8);
    drawRod(c,p,{-0.07f,0.05f,0.10f},{0.04f,0.05f,-0.08f},0.012f,dark);
    drawCylinder(c,p,{0.23f,0.067f,-0.06f},{0.10f,0.13f,0.10f},blueLight);
    drawCylinder(c,p,{0.23f,0.134f,-0.06f},{0.079f,0.004f,0.079f},darkWood);
    for(int i=0;i<3;++i)
        drawRod(c,p,{0.215f+i*0.016f,0.10f,-0.06f},{0.20f+i*0.024f,0.25f,-0.06f},0.009f,i%2 ? brass : terracotta);
}
