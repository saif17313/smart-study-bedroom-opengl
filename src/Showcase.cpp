#include "Showcase.h"
#include "App.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>

namespace {
struct PhaseInfo { const char* name; double seconds; };
// Edit durations here; event offsets below are relative to their named phase.
constexpr PhaseInfo phases[] = {
    {"INTRO / ELEVATED OVERVIEW",3}, {"DAY MODE / WINDOW LIGHT",5},
    {"INDIVIDUAL LIGHTS AND COMBINATIONS",14}, {"STUDY / DRAWER",7},
    {"LAPTOP / KEYBOARD BACKLIGHT",8}, {"FAN SPEED CONTROL",16},
    {"REAL-TIME CLOCK MOTION",5}, {"ROOM DOOR",6}, {"WARDROBE",6},
    {"CURTAINS / DAYLIGHT",7}, {"AUTOMATIC DAY CYCLE / SUNSET",6},
    {"NIGHT MODE / WARM LIGHTS",6}, {"RAIN / WINDOW AND CURTAIN SWAY",5},
    {"SHADING COMPARISON / PAUSE",6}, {"32 SECOND CAMERA TOUR SAMPLE",3},
    {"FULLY ACTIVE ROOM / FINAL OVERVIEW",3}, {"SHOWCASE COMPLETE / RESTORING STATE",3}
};
constexpr double phaseStart(ShowcasePhase phase) {
    double seconds=0;
    for(int i=0;i<static_cast<int>(phase);++i) seconds+=phases[i].seconds;
    return seconds;
}
constexpr double totalDuration=phaseStart(ShowcasePhase::Count);
static_assert(std::size(phases)==static_cast<int>(ShowcasePhase::Count));
static_assert(totalDuration<=120, "The full showcase must fit within two minutes");

enum class Pose { Overview, Room, Bed, Study, Keyboard, Fan, Clock, Door, Wardrobe, Window, RainWindow, Comparison, Finale };
// All interior viewpoints remain within the room; outside poses use its cutaway.
const ShowcaseCameraPose poses[] = {
    {{0,7.4f,9.8f},{0,0.8f,-0.25f},48,true},
    {{2.65f,2.70f,2.60f},{0,1.1f,-1},76},
    {{-0.35f,2.40f,1.65f},{-2.4f,0.8f,-0.15f},60},
    {{1.0f,2.10f,-0.7f},{1.95f,0.9f,-2.18f},62},
    {{1.28f,1.07f,-1.87f},{1.60f,0.809f,-2.199f},45},
    {{1.0f,2.0f,1.65f},{0,2.73f,0.1f},65},
    {{1.8f,2.3f,-1.0f},{2.45f,2.64f,-2.75f},42},
    {{1.15f,2.35f,2.55f},{3.12f,1.08f,1.60f},65},
    {{-0.65f,2.25f,-0.4f},{-2.15f,1.1f,-2.36f},60},
    {{-0.1f,2.35f,-0.65f},{-0.05f,1.775f,-2.8f},65},
    {{-0.2f,1.95f,-1.50f},{-0.05f,1.775f,-2.8f},60},
    {{0.50f,1.8f,2.35f},{-1.8f,0.8f,-0.45f},76},
    {{3.8f,6.8f,7.8f},{0,1.0f,-0.4f},52,true}
};
enum class Action { Camera, Hour, Lights, Fan, Door, Wardrobe, Curtains, Drawer, Laptop, Keyboard, Weather, DayCycle, Pause, Shading, Tour, Help, Restore };
enum LightMask { None=0, Ceiling=1, Bedside=2, Study=4, All=7 };
struct Event {
    ShowcasePhase phase;
    double offset;
    Action action;
    float value;
    double seconds=0;
    const char* label=nullptr;
};
constexpr Event event(ShowcasePhase phase,double offset,Action action,float value,const char* label=nullptr) {
    return {phase,offset,action,value,0,label};
}
constexpr Event camera(ShowcasePhase phase,double offset,Pose pose,double seconds) {
    return {phase,offset,Action::Camera,static_cast<float>(pose),seconds};
}
constexpr Event hour(ShowcasePhase phase,double offset,float target,double seconds) {
    return {phase,offset,Action::Hour,target,seconds};
}
using P=ShowcasePhase;
using A=Action;
// Ordered events change only targets/switches, never animated object geometry.
constexpr Event events[] = {
    camera(P::Intro,0,Pose::Overview,3), hour(P::Intro,0,10,2),
    event(P::Intro,1.5,A::Help,0),
    camera(P::Day,0,Pose::Window,1.5),
    event(P::Day,0,A::Curtains,1,"DAY / CURTAINS OPEN / LIGHTS OFF"),
    camera(P::Lights,0,Pose::Room,1.2), hour(P::Lights,0,22,2),
    event(P::Lights,0,A::Lights,None,"NIGHT / ALL INDOOR LIGHTS OFF"),
    camera(P::Lights,2,Pose::Bed,0.8), event(P::Lights,2,A::Lights,Bedside,"BEDSIDE LIGHT ONLY / RELAXING"),
    camera(P::Lights,4,Pose::Study,0.8), event(P::Lights,4,A::Lights,Study,"STUDY SPOTLIGHT ONLY"),
    camera(P::Lights,6,Pose::Room,0.8), event(P::Lights,6,A::Lights,Ceiling,"CEILING LIGHT ONLY"),
    camera(P::Lights,8,Pose::Overview,2.5), event(P::Lights,8,A::Lights,Ceiling|Bedside,"CEILING + BEDSIDE LIGHTS"),
    event(P::Lights,9.5,A::Lights,Ceiling|Study,"CEILING + STUDY LIGHTS"),
    event(P::Lights,11,A::Lights,Bedside|Study,"BEDSIDE + STUDY LIGHTS"),
    event(P::Lights,12.5,A::Lights,All,"ALL INDOOR LIGHTS ON"),
    camera(P::Study,0,Pose::Study,3), event(P::Study,0,A::Lights,Study),
    event(P::Study,1,A::Laptop,1), event(P::Study,1,A::Drawer,1,"STUDY / LAPTOP OPEN / DRAWER OPENING"),
    event(P::Study,4.2,A::Drawer,0,"DRAWER CLOSING / STUDY SPOTLIGHT"),
    camera(P::Laptop,0,Pose::Keyboard,1), event(P::Laptop,0,A::Laptop,0,"LAPTOP CLOSING / KEYBOARD AUTO OFF"),
    event(P::Laptop,2,A::Keyboard,0), event(P::Laptop,2,A::Laptop,1,"LAPTOP OPENING / KEYBOARD LIGHT OFF"),
    event(P::Laptop,4,A::Keyboard,1,"KEYBOARD BACKLIGHT ON"),
    event(P::Laptop,6,A::Laptop,0,"LAPTOP CLOSING / BACKLIGHT FADES OFF"),
    camera(P::Fan,0,Pose::Fan,1.5), event(P::Fan,0,A::Lights,All),
    event(P::Fan,0,A::Fan,0,"FAN OFF"), event(P::Fan,2,A::Fan,1,"FAN LOW / SMOOTH ACCELERATION"),
    event(P::Fan,4,A::Fan,2,"FAN MEDIUM"), event(P::Fan,6,A::Fan,3,"FAN HIGH"),
    event(P::Fan,8,A::Fan,4,"FAN MAX"), event(P::Fan,10,A::Fan,2,"FAN MEDIUM / SMOOTH DECELERATION"),
    event(P::Fan,13,A::Fan,0,"FAN OFF / COASTING TO STOP"),
    camera(P::Clock,0,Pose::Clock,1.2),
    camera(P::Door,0,Pose::Door,1.2), event(P::Door,1,A::Door,1,"ROOM DOOR OPENING / HINGE MOTION"),
    event(P::Door,4,A::Door,0,"ROOM DOOR CLOSING"),
    camera(P::Wardrobe,0,Pose::Wardrobe,1.2), event(P::Wardrobe,1,A::Wardrobe,1,"WARDROBE OPENING / SHELVES AND BEDDING"),
    event(P::Wardrobe,4,A::Wardrobe,0,"WARDROBE CLOSING"),
    camera(P::Curtains,0,Pose::Window,1.2), hour(P::Curtains,0,10,1.5), event(P::Curtains,0,A::Lights,None),
    event(P::Curtains,0,A::Curtains,0,"CURTAINS CLOSED / REDUCED DAYLIGHT"),
    event(P::Curtains,1.5,A::Curtains,1,"CURTAINS OPENING / DAYLIGHT AND SWAY"),
    event(P::Curtains,5,A::Curtains,0,"CURTAINS CLOSING / PREPARING SUNSET"), hour(P::Curtains,5,17.1f,2),
    camera(P::DayCycle,0,Pose::RainWindow,1.1), event(P::DayCycle,0,A::Curtains,1),
    event(P::DayCycle,0,A::DayCycle,1,"AUTO DAY CYCLE / SUN MOON STARS SKYLINE"),
    camera(P::Night,0,Pose::Room,1.2), hour(P::Night,0,22,1.5), event(P::Night,0,A::Curtains,0),
    event(P::Night,0,A::Lights,None,"NIGHT MODE / ALL LIGHTS OFF"),
    camera(P::Night,2,Pose::Bed,0.8), event(P::Night,2,A::Lights,Bedside,"SLEEPING MODE / BEDSIDE LIGHT ONLY"),
    camera(P::Night,4,Pose::Study,0.8), event(P::Night,4,A::Lights,Bedside|Study,"NIGHT / BEDSIDE + STUDY LIGHTS"),
    event(P::Night,5,A::Lights,All,"FULLY LIT NIGHT ROOM"),
    camera(P::Rain,0,Pose::RainWindow,1), event(P::Rain,0,A::Curtains,1),
    event(P::Rain,1,A::Weather,1,"RAINY NIGHT / WARM LIGHTS / STRONGER SWAY"),
    event(P::Rain,4,A::Weather,0,"RAIN OFF / CLEAR NIGHT"), camera(P::Rain,4,Pose::Comparison,1),
    event(P::Shading,0,A::Pause,1), event(P::Shading,0,A::Shading,0,"FLAT SHADING / MOTION PAUSED"),
    event(P::Shading,2,A::Shading,1,"GOURAUD SHADING / SAME CAMERA AND LIGHTS"),
    event(P::Shading,4,A::Shading,2,"PHONG SHADING / SAME CAMERA AND LIGHTS"),
    event(P::CameraTour,0,A::Pause,0), event(P::CameraTour,0,A::Tour,1,"MOTION RESUMES / EXISTING CAMERA TOUR"),
    event(P::Finale,0,A::Tour,0), camera(P::Finale,0,Pose::Finale,2.5),
    event(P::Finale,0,A::Lights,All), event(P::Finale,0,A::Fan,2),
    event(P::Finale,0,A::Laptop,1), event(P::Finale,0,A::Keyboard,1),
    event(P::Finale,0,A::Weather,1,"FULLY ACTIVE ROOM / RAINY NIGHT"),
    event(P::Finish,0,A::Restore,0,"SHOWCASE COMPLETE / RESTORING YOUR STATE")
};
constexpr double eventTime(const Event& event) { return phaseStart(event.phase)+event.offset; }
constexpr bool validTimeline() {
    for(std::size_t i=0;i<std::size(events);++i)
        if(events[i].offset<0 || events[i].offset>=phases[static_cast<int>(events[i].phase)].seconds
            || (i>0 && eventTime(events[i])<eventTime(events[i-1]))) return false;
    return true;
}
static_assert(validTimeline(), "Showcase events must be ordered and fit their phases");

ShowcaseCameraPose cameraPose(const Camera& camera) {
    return {camera.position,camera.position+camera.forward()*5.0f,camera.fov,camera.overview};
}
void moveCamera(AppState& app,const ShowcaseCameraPose& pose,double seconds) {
    auto& c=app.showcase;
    c.cameraFrom=cameraPose(app.camera); c.cameraTo=pose;
    c.cameraTime=0; c.cameraDuration=seconds;
}
void changeHour(AppState& app,double hour,double seconds) {
    auto& c=app.showcase;
    c.hourFrom=app.simulation.hour;
    c.hourDelta=std::remainder(hour-c.hourFrom,24.0);
    c.hourTime=0; c.hourDuration=seconds;
    app.simulation.dayCycle=false;
}
void applyEvent(AppState& app,const Event& e) {
    auto& c=app.showcase; auto& s=app.simulation;
    if(e.label) c.label=e.label;
    const bool on=e.value!=0;
    const int value=static_cast<int>(e.value);
    switch(e.action) {
        case A::Camera: moveCamera(app,poses[value],e.seconds); break;
        case A::Hour: changeHour(app,e.value,e.seconds); break;
        case A::Lights: s.ceilingLight=value&Ceiling; s.bedsideLight=value&Bedside; s.studyLight=value&Study; break;
        case A::Fan: s.changeFanSpeed(value-(s.fanOn ? s.fanSpeedLevel : 0)); break;
        case A::Door: s.doorOpen=on; break;
        case A::Wardrobe: s.wardrobeOpen=on; break;
        case A::Curtains: s.curtainsOpen=on; break;
        case A::Drawer: s.drawerOpen=on; break;
        case A::Laptop: s.laptopOpen=on; break;
        case A::Keyboard: s.keyboardBacklightOn=on; break;
        case A::Weather: s.weather=on ? Weather::Rain : Weather::Clear; break;
        case A::DayCycle: s.dayCycle=on; break;
        case A::Pause: s.paused=on; break;
        case A::Shading: app.shading=static_cast<ShadingMode>(value); break;
        case A::Tour:
            app.tour=on;
            if(on) { app.tourTime=0; c.cameraFrom=cameraPose(app.camera); c.cameraTime=0; c.cameraDuration=1; }
            break;
        case A::Help: app.showHelp=on; break;
        case A::Restore: {
            const auto& original=c.saved.simulation;
            s.paused=false; s.fanOn=original.fanOn; s.fanSpeedLevel=original.fanSpeedLevel;
            s.doorOpen=original.doorOpen; s.curtainsOpen=original.curtainsOpen;
            s.wardrobeOpen=original.wardrobeOpen; s.drawerOpen=original.drawerOpen; s.laptopOpen=original.laptopOpen;
            s.keyboardBacklightOn=original.keyboardBacklightOn;
            s.ceilingLight=original.ceilingLight; s.bedsideLight=original.bedsideLight; s.studyLight=original.studyLight;
            s.weather=original.weather;
            moveCamera(app,cameraPose(c.saved.camera),phases[static_cast<int>(P::Finish)].seconds);
            changeHour(app,original.hour,phases[static_cast<int>(P::Finish)].seconds);
            break;
        }
    }
}
void synchronize(AppState& app) {
    auto& c=app.showcase;
    int phase=0;
    while(phase+1<ShowcaseState::phaseCount() && c.time>=phaseStart(static_cast<P>(phase+1))-1e-8) ++phase;
    if(phase!=static_cast<int>(c.phase)) {
        c.phase=static_cast<P>(phase); c.label=ShowcaseState::phaseName(c.phase);
        std::cout << "[Showcase] " << c.time << "s: " << c.label << '\n';
    }
    while(c.nextEvent<std::size(events) && eventTime(events[c.nextEvent])<=c.time+1e-8)
        applyEvent(app,events[c.nextEvent++]);
}
void advanceCamera(AppState& app,double dt) {
    auto& c=app.showcase;
    if(app.tour) { updateCameraTour(app,static_cast<float>(dt)); c.cameraTo=cameraPose(app.camera); }
    if(c.cameraDuration<=0) return;
    c.cameraTime=std::min(c.cameraTime+dt,c.cameraDuration);
    float t=static_cast<float>(c.cameraTime/c.cameraDuration); t=t*t*(3-2*t);
    app.camera.position=glm::mix(c.cameraFrom.position,c.cameraTo.position,t);
    const auto target=glm::mix(c.cameraFrom.target,c.cameraTo.target,t);
    if(glm::length(target-app.camera.position)>0.001f) app.camera.lookAt(target);
    app.camera.fov=glm::mix(c.cameraFrom.fov,c.cameraTo.fov,t);
    // Keep the cutaway while crossing the front wall or ceiling.
    app.camera.overview=t<1 ? c.cameraFrom.overview || c.cameraTo.overview : c.cameraTo.overview;
}
}

double ShowcaseState::duration() { return totalDuration; }
int ShowcaseState::phaseCount() { return static_cast<int>(P::Count); }
const char* ShowcaseState::phaseName(ShowcasePhase phase) { return phases[static_cast<int>(phase)].name; }

void ShowcaseState::start(AppState& app) {
    if(active) return;
    saved={app.camera,app.simulation,app.shading,app.tour,app.showHud,app.showHelp,app.tourTime};
    active=true; paused=false; time=0; phase=P::Intro; nextEvent=0;
    label="AUTOMATED PROJECT SHOWCASE";
    cameraDuration=hourDuration=0;
    app.tour=false; app.showHud=true; app.showHelp=true; app.shading=ShadingMode::Phong;
    app.simulation=Simulation{};
    auto& s=app.simulation;
    s.fanOn=false; s.curtainsOpen=false; s.curtainAmount=0;
    s.laptopOpen=false; s.laptopAmount=0;
    s.ceilingLight=s.bedsideLight=s.studyLight=false;
    s.hour=saved.simulation.hour; s.dayCycle=false;
    std::cout << "[Showcase] Started; total timeline: " << totalDuration << " seconds, " << phaseCount() << " phases.\n";
    synchronize(app);
}
void ShowcaseState::stop(AppState& app,bool completed) {
    if(!active) return;
    app.camera=saved.camera; app.simulation=saved.simulation; app.shading=saved.shading;
    app.tour=saved.tour; app.tourTime=saved.tourTime;
    app.showHud=saved.showHud; app.showHelp=saved.showHelp;
    app.firstMouse=true;
    active=false; paused=false;
    std::cout << "[Showcase] " << (completed ? "Complete" : "Cancelled") << " at " << time
              << "s; original state restored.\n";
}
void ShowcaseState::update(AppState& app,float dt) {
    if(!active || paused || !std::isfinite(dt) || dt<=0) return;
    double remaining=dt;
    while(active && remaining>1e-8) {
        synchronize(app);
        double boundary=std::min(totalDuration,phaseStart(static_cast<P>(static_cast<int>(phase)+1)));
        if(nextEvent<std::size(events)) boundary=std::min(boundary,eventTime(events[nextEvent]));
        const double step=std::min({remaining,boundary-time,1.0/60.0});
        app.simulation.update(static_cast<float>(step));
        if(hourDuration>0 && hourTime<hourDuration) {
            hourTime=std::min(hourTime+step,hourDuration);
            double t=hourTime/hourDuration; t=t*t*(3-2*t);
            app.simulation.hour=std::fmod(hourFrom+hourDelta*t+24,24);
        }
        advanceCamera(app,step);
        time+=step; remaining-=step;
        if(time>=totalDuration-1e-8) { time=totalDuration; stop(app,true); return; }
    }
    synchronize(app);
}
