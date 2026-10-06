#include "Decorations.h"
#include <cmath>
using namespace Materials;

void drawWindow(DrawContext& c, const glm::mat4& p) {
    const auto& s=c.simulation;
    const Material skyNow{s.skyColor(),0,1,true};
    drawBox(c,p,{0,0,-0.24f},{1.30f,1.45f,0.02f},skyNow);
    const Material skyline{glm::mix(glm::vec3(0.012f,0.024f,0.045f),glm::vec3(0.12f,0.22f,0.29f),s.daylight()),0,1,true};
    const Material skyLight{s.daylight()>0.5f ? glm::vec3(1.0f,0.81f,0.41f) : glm::vec3(0.64f,0.75f,0.93f),0,1,true};
    if(s.weather==Weather::Clear) {
        drawSphere(c,p,{0.34f,0.43f,-0.19f},{0.19f,0.19f,0.018f},skyLight);
        if(s.daylight()<0.3f) for(int i=0;i<16;++i) {
            const float x=-0.58f+float((i*37)%113)*0.01f;
            const float y=-0.10f+float((i*29)%76)*0.01f;
            drawBox(c,p,{x,y,-0.18f},{0.008f,0.008f,0.005f},skyLight);
        }
    }
    for(int i=0;i<8;++i) {
        float h=0.17f+float((i*7)%5)*0.055f;
        const float x=-0.57f+i*0.16f;
        drawBox(c,p,{x,-0.72f+h/2,-0.15f},{0.145f,h,0.035f},skyline);
        if(s.daylight()<0.5f) for(int row=0;row<2;++row)
            drawBox(c,p,{x,-0.65f+row*0.075f,-0.129f},{0.027f,0.028f,0.005f},skyLight);
    }
    if(s.weather==Weather::Rain) {
        const Material rain{{0.41f,0.56f,0.66f},0,1,true};
        for(int i=0;i<64;++i) {
            const float x=-0.60f+float((i*43)%121)*0.01f;
            const float phase=static_cast<float>(std::fmod(s.elapsed*1.4+i*0.137,1.0));
            const float y=0.65f-phase*1.30f;
            drawRod(c,p,{x,y,-0.085f},{x+0.018f,y+0.060f,-0.085f},0.003f,rain);
        }
    }
    for(float x : {-0.67f,0.67f})
        drawBox(c,p,{x,0,0},{0.08f,1.58f,0.15f},wood);
    for(float y : {-0.765f,0.765f})
        drawBox(c,p,{0,y,0},{1.42f,0.08f,0.15f},wood);
    drawBox(c,p,{0,0,0.025f},{0.048f,1.47f,0.07f},darkWood);
    drawBox(c,p,{0,-0.79f,0.065f},{1.51f,0.075f,0.25f},lightWood);
}

void drawCurtains(DrawContext& c, const glm::mat4& p) {
    drawRod(c,p,{-1.08f,0.84f,0.13f},{1.08f,0.84f,0.13f},0.028f,dark);
    const auto& s=c.simulation;
    for(float side : {-1.0f,1.0f}) {
        const float width=0.69f-0.27f*s.curtainAmount;
        const float center=side*(0.335f+0.465f*s.curtainAmount);
        const float sway=std::sin(static_cast<float>(std::fmod(s.elapsed*1.4,6.283185307179586))+side)
            * (s.weather==Weather::Rain ? 1.4f : 0.5f) * (0.3f+0.7f*s.curtainAmount);
        const auto top=transform(p,{center,0.77f,0.16f},{1,1,1},sway,{0,0,1});
        drawPart(c,c.shapes.curtain,transform(top,{0,-0.825f,0},{width,1.65f,1}),blue);
        for(int i=0;i<8;++i)
            drawCylinder(c,p,{center-width*0.44f+i*width*0.125f,0.825f,0.13f},{0.022f,0.065f,0.022f},brass);
    }
}

void drawClock(DrawContext& c, const glm::mat4& p) {
    drawCylinder(c,p,{0,0,0},{0.47f,0.055f,0.47f},dark,90,{1,0,0});
    drawCylinder(c,p,{0,0,0.034f},{0.427f,0.015f,0.427f},linen,90,{1,0,0});
    for(int i=0;i<12;++i) {
        float angle=float(i)*30.0f, rad=glm::radians(angle);
        drawBox(c,p,{0.181f*std::sin(rad),0.181f*std::cos(rad),0.047f},
                {0.009f,0.025f,0.005f},dark,-angle,{0,0,1});
    }
    const double seconds=c.simulation.clockSeconds;
    auto hour=transform(p,{0,0,0.051f},{1,1,1},-static_cast<float>(std::fmod(seconds/120.0,360.0)),{0,0,1});
    auto minute=transform(p,{0,0,0.059f},{1,1,1},-static_cast<float>(std::fmod(seconds/10.0,360.0)),{0,0,1});
    auto second=transform(p,{0,0,0.067f},{1,1,1},-static_cast<float>(std::fmod(seconds*6.0,360.0)),{0,0,1});
    drawBox(c,hour,{0,0.053f,0},{0.016f,0.106f,0.006f},dark);
    drawBox(c,minute,{0,0.077f,0},{0.010f,0.154f,0.006f},dark);
    drawBox(c,second,{0,0.065f,0},{0.005f,0.185f,0.005f},terracotta);
    drawSphere(c,p,{0,0,0.073f},{0.024f,0.024f,0.015f},brass);
}

void drawWallFrame(DrawContext& c, const glm::mat4& p) {
    drawBox(c,p,{0,0,0},{0.65f,0.91f,0.045f},darkWood);
    drawBox(c,p,{0,0,0.029f},{0.58f,0.84f,0.016f},linen);
    drawBox(c,p,{0,0,0.042f},{0.44f,0.67f,0.008f},blueLight);
    drawSphere(c,p,{0.105f,0.18f,0.05f},{0.11f,0.11f,0.008f},brass);
    for(int i=0;i<3;++i)
        drawBox(c,p,{-0.08f+i*0.10f,-0.14f-i*0.04f,0.052f+i*0.003f},
                {0.18f,0.14f,0.004f},i%2 ? blue : linen,-35+i*12,{0,0,1});
}

void drawRug(DrawContext& c, const glm::mat4& p) {
    drawBox(c,p,{0,0.012f,0},{2.80f,0.024f,2.20f},blue);
    drawBox(c,p,{0,0.025f,0},{2.64f,0.006f,2.04f},linen);
    drawBox(c,p,{0,0.030f,0},{2.52f,0.006f,1.92f},blue);
    for(int i=-3;i<=3;++i) for(int j=-2;j<=2;++j)
        drawBox(c,p,{i*0.32f,0.035f,j*0.31f},{0.095f,0.005f,0.095f},blueLight,45);
    for(float side : {-1.0f,1.0f}) for(int i=0;i<24;++i)
        drawBox(c,p,{-1.32f+i*0.115f,0.014f,side*1.14f},{0.012f,0.009f,0.11f},linen);
}
