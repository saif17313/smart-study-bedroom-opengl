#include "Hud.h"
#include "App.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>

using Glyph=std::array<unsigned char,7>;
static Glyph glyph(char character) {
    static const Glyph letters[]={
        {14,17,17,31,17,17,17},{30,17,17,30,17,17,30},{14,17,16,16,16,17,14},
        {30,17,17,17,17,17,30},{31,16,16,30,16,16,31},{31,16,16,30,16,16,16},
        {14,17,16,23,17,17,15},{17,17,17,31,17,17,17},{14,4,4,4,4,4,14},
        {7,2,2,2,18,18,12},{17,18,20,24,20,18,17},{16,16,16,16,16,16,31},
        {17,27,21,21,17,17,17},{17,25,21,19,17,17,17},{14,17,17,17,17,17,14},
        {30,17,17,30,16,16,16},{14,17,17,17,21,18,13},{30,17,17,30,20,18,17},
        {15,16,16,14,1,1,30},{31,4,4,4,4,4,4},{17,17,17,17,17,17,14},
        {17,17,17,17,17,10,4},{17,17,17,21,21,21,10},{17,17,10,4,10,17,17},
        {17,17,10,4,4,4,4},{31,1,2,4,8,16,31}
    };
    static const Glyph numbers[]={
        {14,17,19,21,25,17,14},{4,12,4,4,4,4,14},{14,17,1,2,4,8,31},
        {30,1,1,14,1,1,30},{2,6,10,18,31,2,2},{31,16,16,30,1,1,30},
        {14,16,16,30,17,17,14},{31,1,2,4,8,8,8},{14,17,17,14,17,17,14},{14,17,17,15,1,1,14}
    };
    const char ch=static_cast<char>(std::toupper(static_cast<unsigned char>(character)));
    if(ch>='A' && ch<='Z') return letters[ch-'A'];
    if(ch>='0' && ch<='9') return numbers[ch-'0'];
    switch(ch) {
        case ':': return {0,4,4,0,4,4,0};
        case '/': return {1,1,2,4,8,16,16};
        case '-': return {0,0,0,31,0,0,0};
        case '+': return {0,4,4,31,4,4,0};
        case '.': return {0,0,0,0,0,12,12};
        case '[': return {14,8,8,8,8,8,14};
        case ']': return {14,2,2,2,2,2,14};
        default: return {};
    }
}

static void rectangle(std::vector<glm::vec3>& vertices,float x,float y,float w,float h) {
    vertices.insert(vertices.end(),{{x,y,0},{x+w,y,0},{x+w,y+h,0},
                                    {x,y,0},{x+w,y+h,0},{x,y+h,0}});
}

static void lettering(std::vector<glm::vec3>& vertices,const std::string& text,float x,float y,float pixel=1.5f) {
    for(char ch:text) {
        const auto bits=glyph(ch);
        for(int row=0;row<7;++row) for(int col=0;col<5;++col)
            if(bits[row] & (1<<(4-col))) rectangle(vertices,x+col*pixel,y+row*pixel,pixel,pixel);
        x+=pixel*6;
    }
}

constexpr size_t capacity=150000;
Hud::Hud() {
    glGenVertexArrays(1,&vao); glGenBuffers(1,&vbo);
    glBindVertexArray(vao); glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,capacity*sizeof(glm::vec3),nullptr,GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),nullptr);
    glBindVertexArray(0);
}
Hud::~Hud() { glDeleteVertexArrays(1,&vao); glDeleteBuffers(1,&vbo); }

void Hud::draw(Shader& shader,const AppState& app,int width,int height) {
    const auto& s=app.simulation;
    const float scale=std::min(1.0f,std::min(width/560.0f,height/510.0f));
    const float virtualHeight=height/scale;
    std::array<std::vector<glm::vec3>,3> batches;
    auto& panels=batches[0]; auto& accent=batches[1]; auto& text=batches[2];
    rectangle(panels,16,16,510,98);
    rectangle(accent,16,16,3,98);
    lettering(accent,std::string("SMART STUDY  /  ")+modeName(app.shading),32,30,1.8f);
    char time[16];
    const int minutes=static_cast<int>(s.hour*60)%1440;
    std::snprintf(time,sizeof(time),"%02d:%02d",minutes/60,minutes%60);
    lettering(text,std::string(time)+"  "+weatherName(s.weather)+"  "+(s.paused ? "PAUSED" : "LIVE")
        +"  CYCLE "+(s.dayCycle ? "ON" : "OFF"),32,54);
    lettering(text,std::string("LIGHTS  L:")+(s.ceilingLight ? "ON" : "OFF")+"  B:"
        +(s.bedsideLight ? "ON" : "OFF")+"  K:"+(s.studyLight ? "ON" : "OFF"),32,75);
    lettering(text,app.tour ? "TOUR ON  /  SPACE TO STOP" : "H CONTROLS  /  SPACE CAMERA TOUR",32,96,1.2f);

    if(app.showHelp) {
        const float y=virtualHeight-290;
        rectangle(panels,16,y,510,274);
        rectangle(accent,16,y,3,274);
        lettering(accent,"ROOM CONTROLS",32,y+14,1.8f);
        const std::string lines[]={
            "WASD MOVE   Q/E HEIGHT   TAB MOUSE LOOK",
            "WHEEL ZOOM   V OVERVIEW   R CAMERA RESET",
            "1 FLAT   2 GOURAUD   3 PHONG",
            std::string("F FAN ")+(s.fanOn ? "ON" : "OFF")+"  SPEED "+s.fanSpeedName()+"   O DOOR "+(s.doorOpen ? "OPEN" : "CLOSED"),
            "[ / ] FAN SPEED - / +",
            std::string("C CURTAINS ")+(s.curtainsOpen ? "OPEN" : "CLOSED")+"   U WARDROBE",
            "J DESK DRAWER   M LAPTOP LID",
            "L CEILING   B BEDSIDE   K STUDY LAMP",
            "N DAY/NIGHT   T DAY CYCLE   G RAIN/CLEAR",
            "P PAUSE MOTION   SPACE CAMERA TOUR",
            "F12 SCREENSHOT   H HIDE HELP   ESC EXIT"
        };
        for(size_t i=0;i<std::size(lines);++i) lettering(text,lines[i],32,y+42+20*float(i));
    }
    shader.use();
    shader.set("model",glm::mat4(1)); shader.set("view",glm::mat4(1));
    shader.set("projection",glm::ortho(0.0f,width/scale,virtualHeight,0.0f,-1.0f,1.0f));
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(vao); glBindBuffer(GL_ARRAY_BUFFER,vbo);
    const glm::vec3 colors[]={{0.007f,0.013f,0.025f},{0.20f,0.68f,0.72f},{0.66f,0.73f,0.79f}};
    for(size_t i=0;i<batches.size();++i) {
        const auto& vertices=batches[i];
        if(vertices.size()>capacity) throw std::runtime_error("HUD vertex capacity exceeded");
        shader.set("materialColor",colors[i]);
        glBufferSubData(GL_ARRAY_BUFFER,0,vertices.size()*sizeof(glm::vec3),vertices.data());
        glDrawArrays(GL_TRIANGLES,0,static_cast<GLsizei>(vertices.size()));
    }
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}
