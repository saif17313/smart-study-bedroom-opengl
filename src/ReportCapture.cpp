#include <glad/gl.h>
#include "ReportCapture.h"
#include "Screenshot.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

namespace {
enum class Pose { Overview, BirdEye, Lighting, Shading, Desk, Keyboard, Fan, Door, Window, Weather, Wardrobe, Clock, Laptop };
struct ReportCameraPose { const char* name; glm::vec3 position,target; float fov; bool overview=false; };
const ReportCameraPose poses[]={
    {"Overview",{2.65f,2.50f,2.60f},{0,1.05f,-1.0f},70},
    {"BirdEye",{0,7.4f,9.8f},{0,0.8f,-0.25f},38,true},
    {"LightingComparison",{2.65f,2.50f,2.60f},{0,1.05f,-1.0f},70},
    {"ShadingComparison",{0.50f,1.8f,2.35f},{-1.8f,0.8f,-0.45f},65},
    {"Desk",{1.0f,2.10f,-0.7f},{1.95f,0.9f,-2.18f},62},
    {"Keyboard",{1.28f,1.07f,-1.87f},{1.60f,0.809f,-2.199f},45},
    {"Fan",{1.0f,2.0f,1.65f},{0,2.73f,0.1f},58},
    {"Door",{1.15f,2.35f,2.55f},{3.12f,1.08f,1.60f},65},
    {"Window",{-0.1f,2.35f,-0.65f},{-0.05f,1.775f,-2.8f},65},
    {"Weather",{0.35f,1.90f,-1.30f},{0.25f,1.65f,-2.70f},68},
    {"Wardrobe",{-0.65f,2.25f,-0.4f},{-2.15f,1.1f,-2.36f},60},
    {"Clock",{2.2f,2.5f,-1.30f},{2.45f,2.64f,-2.75f},38},
    {"Laptop",{1.0f,1.55f,-1.15f},{1.60f,0.95f,-2.16f},48}
};
enum class State { Balanced,Day,Night,Rain,LightsOff,Ceiling,Bedside,Study,AllLights,Fan,FanMax,
                   Door,Curtains,Wardrobe,Drawer,Laptop,Keyboard,Clock,Showcase,DayCycle,Tour,NightSky,ClosedCurtains,KeyboardOff,Paused };
struct Shot { const char* filename; Pose pose; State state; ShadingMode mode=ShadingMode::Phong; bool hud=false,help=false; };
const Shot shots[]={
    {"01_overview.png",Pose::Overview,State::Balanced},
    {"02_birds_eye.png",Pose::BirdEye,State::Balanced},
    {"03_day.png",Pose::Overview,State::Day},
    {"04_night.png",Pose::Overview,State::Night},
    {"05_rain.png",Pose::Weather,State::Rain},
    {"06_lights_off.png",Pose::Lighting,State::LightsOff},
    {"07_ceiling_light.png",Pose::Lighting,State::Ceiling},
    {"08_bedside_light.png",Pose::Lighting,State::Bedside},
    {"09_study_light.png",Pose::Lighting,State::Study},
    {"10_all_lights.png",Pose::Lighting,State::AllLights},
    {"11_flat.png",Pose::Shading,State::Balanced,ShadingMode::Flat},
    {"12_gouraud.png",Pose::Shading,State::Balanced,ShadingMode::Gouraud},
    {"13_phong.png",Pose::Shading,State::Balanced},
    {"14_fan.png",Pose::Fan,State::Fan},
    {"15_fan_speed.png",Pose::Fan,State::FanMax,ShadingMode::Phong,true,true},
    {"16_door_open.png",Pose::Door,State::Door},
    {"17_curtain_open.png",Pose::Window,State::Curtains},
    {"18_wardrobe_open.png",Pose::Wardrobe,State::Wardrobe},
    {"19_drawer_open.png",Pose::Desk,State::Drawer},
    {"20_laptop_open.png",Pose::Laptop,State::Laptop},
    {"21_keyboard_backlight.png",Pose::Keyboard,State::Keyboard},
    {"22_clock.png",Pose::Clock,State::Clock},
    {"23_showcase.png",Pose::BirdEye,State::Showcase,ShadingMode::Phong,true},
    {"24_automatic_day_cycle.png",Pose::Window,State::DayCycle,ShadingMode::Phong,true},
    {"25_camera_tour.png",Pose::Overview,State::Tour,ShadingMode::Phong,true},
    {"26_night_sky.png",Pose::Window,State::NightSky},
    {"27_curtains_closed.png",Pose::Window,State::ClosedCurtains},
    {"28_keyboard_backlight_off.png",Pose::Keyboard,State::KeyboardOff},
    {"29_animation_paused.png",Pose::Overview,State::Paused,ShadingMode::Phong,true}
};

void advance(AppState& app,double seconds) {
    // Exercise production motion until hinges, slider, lid and fan reach targets.
    while(seconds>1e-8) {
        const double step=std::min(seconds,1.0/60.0);
        updateApplication(app,static_cast<float>(step)); seconds-=step;
    }
}
void configure(AppState& app,const Shot& shot) {
    app=AppState{};
    app.showHud=shot.hud; app.showHelp=shot.help; app.shading=shot.mode;
    const auto& p=poses[static_cast<int>(shot.pose)];
    app.camera.position=p.position; app.camera.lookAt(p.target); app.camera.fov=p.fov; app.camera.overview=p.overview;
    auto& s=app.simulation;
    s.paused=false; s.dayCycle=false; s.hour=10;
    s.weather=Weather::Clear; s.elapsed=0; s.clockSeconds=10*3600+10*60;
    s.fanOn=false; s.fanSpeedLevel=2; s.fanAngle=0; s.fanSpeed=0;
    s.doorOpen=false; s.doorAngle=0; s.wardrobeOpen=false; s.wardrobeAngle=0;
    s.drawerOpen=false; s.drawerAmount=0; s.curtainsOpen=true; s.curtainAmount=0;
    s.laptopOpen=true; s.laptopAmount=0; s.keyboardBacklightOn=true;
    int lights=7;
    double settle=2;
    switch(shot.state) {
        case State::Day: lights=0; break;
        case State::Night: s.hour=22; lights=6; break;
        case State::Rain: s.hour=17.5; s.weather=Weather::Rain; lights=6; break;
        case State::LightsOff: case State::Ceiling: case State::Bedside: case State::Study: case State::AllLights:
            s.hour=22; s.curtainsOpen=false; s.laptopOpen=false; s.keyboardBacklightOn=false;
            lights=shot.state==State::Ceiling ? 1 : shot.state==State::Bedside ? 2 : shot.state==State::Study ? 4 : shot.state==State::AllLights ? 7 : 0;
            break;
        case State::Fan: s.changeFanSpeed(2); settle=3.4; break;
        case State::FanMax: s.changeFanSpeed(4); settle=5.35; break;
        case State::Door: s.doorOpen=true; break;
        case State::Curtains: lights=0; break;
        case State::Wardrobe: s.wardrobeOpen=true; break;
        case State::Drawer: s.drawerOpen=true; break;
        case State::Laptop: break;
        case State::Keyboard: case State::KeyboardOff:
            s.hour=22; lights=6; s.keyboardBacklightOn=shot.state==State::Keyboard;
            break;
        case State::Clock: settle=17.25; break;
        case State::DayCycle: s.hour=17.1; lights=0; break;
        case State::NightSky: s.hour=22; lights=6; break;
        case State::ClosedCurtains: lights=0; s.curtainsOpen=false; break;
        case State::Paused: s.hour=22; s.changeFanSpeed(2); break;
        case State::Balanced: case State::Tour: case State::Showcase: break;
    }
    s.ceilingLight=(lights&1)!=0; s.bedsideLight=(lights&2)!=0; s.studyLight=(lights&4)!=0;
    advance(app,settle);
    if(shot.state==State::DayCycle) { s.dayCycle=true; advance(app,6); }
    if(shot.state==State::Tour) { app.tour=true; app.tourTime=0; advance(app,11.5); }
    if(shot.state==State::Paused) s.paused=true;
    if(shot.state==State::Showcase) { app.showcase.start(app); advance(app,105.75); }
    if(s.doorAngle!=(s.doorOpen ? 100.0f : 0.0f) || s.wardrobeAngle!=(s.wardrobeOpen ? 105.0f : 0.0f)
        || s.drawerAmount!=(s.drawerOpen ? 1.0f : 0.0f) || s.laptopAmount!=(s.laptopOpen ? 1.0f : 0.0f)
        || s.curtainAmount!=(s.curtainsOpen ? 1.0f : 0.0f) || std::abs(s.fanSpeed-s.targetFanSpeed())>0.01f)
        throw std::runtime_error(std::string("Report state did not settle: ")+shot.filename);
}
}

void captureReport(GLFWwindow* window,AppState& app,const std::function<void()>& render,
                   const std::filesystem::path& directory) {
    int width=0,height=0; glfwGetFramebufferSize(window,&width,&height);
    if(width!=1920 || height!=1080) throw std::runtime_error("Report capture requires a 1920x1080 framebuffer");
    GLint samples=0; glGetIntegerv(GL_SAMPLES,&samples);
    std::cout << "[Report] 1920x1080 | MSAA samples: " << samples << " | existing depth/gamma/shader pipeline\n";
    const AppState original=app;
    std::filesystem::create_directories(directory);
    std::ofstream manifest(directory/"manifest.csv");
    if(!manifest) throw std::runtime_error("Cannot write report manifest");
    manifest << "filename,width,height,bytes,pose,x,y,z,target_x,target_y,target_z,fov,overview,shading,hour,weather,ceiling,bedside,study,day_cycle,fan_on,fan_level,fan_speed,fan_angle,door_angle,curtain_amount,wardrobe_angle,drawer_amount,laptop_amount,keyboard_light,keyboard_brightness,clock_seconds,elapsed,paused,tour,tour_time,showcase,showcase_time,hud,help\n";
    manifest << std::setprecision(10);
    try {
        for(const auto& shot:shots) {
            configure(app,shot);
            // Render several identical frames; do not advance simulation between comparisons.
            for(int frame=0;frame<3;++frame) render();
            glFinish();
            if(glGetError()!=GL_NO_ERROR) throw std::runtime_error(std::string("OpenGL report capture error: ")+shot.filename);
            saveScreenshot(directory/shot.filename,width,height);
            const auto bytes=std::filesystem::file_size(directory/shot.filename);
            const auto& c=app.camera; const auto& s=app.simulation;
            const auto target=c.position+c.forward();
            manifest << shot.filename << ',' << width << ',' << height << ',' << bytes << ','
                << (app.showcase.active ? "ShowcaseFinale" : app.tour ? "OriginalTour" : poses[static_cast<int>(shot.pose)].name) << ','
                << c.position.x << ',' << c.position.y << ',' << c.position.z << ','
                << target.x << ',' << target.y << ',' << target.z << ',' << c.fov << ',' << c.overview << ',' << modeName(app.shading) << ','
                << s.hour << ',' << weatherName(s.weather) << ',' << s.ceilingLight << ',' << s.bedsideLight << ',' << s.studyLight << ',' << s.dayCycle << ','
                << s.fanOn << ',' << (s.fanOn ? s.fanSpeedLevel : 0) << ',' << s.fanSpeed << ',' << s.fanAngle << ','
                << s.doorAngle << ',' << s.curtainAmount << ',' << s.wardrobeAngle << ',' << s.drawerAmount << ',' << s.laptopAmount << ','
                << s.keyboardBacklightOn << ',' << s.keyboardBacklightBrightness() << ',' << s.clockSeconds << ',' << s.elapsed << ','
                << s.paused << ',' << app.tour << ',' << app.tourTime << ',' << app.showcase.active << ',' << app.showcase.time << ',' << app.showHud << ',' << app.showHelp << '\n';
            std::cout << "[OK] " << shot.filename << " | " << width << 'x' << height << " | " << bytes << " bytes\n";
        }
        manifest.flush();
        if(!manifest) throw std::runtime_error("Report manifest write failed");
    } catch(...) { app=original; throw; }
    app=original;
    std::cout << "Report capture complete: " << std::size(shots) << " PNG images in " << directory.string() << '\n';
}
