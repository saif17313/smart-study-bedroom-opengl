#include <glad/gl.h>
#include "App.h"
#include "Lighting.h"
#include "Scene.h"
#include "Verification.h"
#include "Hud.h"
#include "ReportCapture.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

const char* modeName(ShadingMode mode) {
    switch(mode) {
        case ShadingMode::Flat: return "Flat";
        case ShadingMode::Gouraud: return "Gouraud";
        default: return "Phong";
    }
}

static void updateTitle(GLFWwindow* window, const AppState& app) {
    std::string title="My Smart Study Bedroom | ";
    title += modeName(app.shading);
    title += app.camera.overview ? " | Overview" : " | Interior";
    title += app.simulation.paused ? " | PAUSED" : " | LIVE";
    if(app.showcase.active) title += std::string(" | SHOWCASE: ")+ShowcaseState::phaseName(app.showcase.phase);
    title += " | H controls | 1/2/3 shading | N day/night";
    glfwSetWindowTitle(window,title.c_str());
}

void handleKey(GLFWwindow* window, int key, int action) {
    if(action!=GLFW_PRESS) return;
    auto& app=*static_cast<AppState*>(glfwGetWindowUserPointer(window));
    if(key==GLFW_KEY_Y) {
        if(app.showcase.active) app.showcase.stop(app); else app.showcase.start(app);
        updateTitle(window,app); return;
    }
    if(app.showcase.active) {
        if(key==GLFW_KEY_ESCAPE) { app.showcase.stop(app); glfwSetWindowShouldClose(window,GLFW_TRUE); }
        if(key==GLFW_KEY_P) app.showcase.paused=!app.showcase.paused;
        if(key==GLFW_KEY_H) app.showHelp=!app.showHelp;
        if(key==GLFW_KEY_F12) app.screenshotRequested=true;
        updateTitle(window,app); return;
    }
    if(key>=GLFW_KEY_1 && key<=GLFW_KEY_3) app.shading=static_cast<ShadingMode>(key-GLFW_KEY_1);
    if(key==GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window,GLFW_TRUE);
    if(key==GLFW_KEY_R) { app.tour=false; app.camera.reset(); }
    if(key==GLFW_KEY_V) { app.tour=false; app.camera.toggleOverview(); }
    if(key==GLFW_KEY_F12) app.screenshotRequested=true;
    auto& s=app.simulation;
    if(key==GLFW_KEY_F) s.fanOn=!s.fanOn;
    if(key==GLFW_KEY_RIGHT_BRACKET) s.changeFanSpeed(1);
    if(key==GLFW_KEY_LEFT_BRACKET) s.changeFanSpeed(-1);
    if(key==GLFW_KEY_O) s.doorOpen=!s.doorOpen;
    if(key==GLFW_KEY_C) s.curtainsOpen=!s.curtainsOpen;
    if(key==GLFW_KEY_U) s.wardrobeOpen=!s.wardrobeOpen;
    if(key==GLFW_KEY_J) s.drawerOpen=!s.drawerOpen;
    if(key==GLFW_KEY_M) s.laptopOpen=!s.laptopOpen;
    if(key==GLFW_KEY_I) s.keyboardBacklightOn=!s.keyboardBacklightOn;
    if(key==GLFW_KEY_L) s.ceilingLight=!s.ceilingLight;
    if(key==GLFW_KEY_B) s.bedsideLight=!s.bedsideLight;
    if(key==GLFW_KEY_K) s.studyLight=!s.studyLight;
    if(key==GLFW_KEY_N) s.toggleDayNight();
    if(key==GLFW_KEY_T) s.dayCycle=!s.dayCycle;
    if(key==GLFW_KEY_G) s.weather=s.weather==Weather::Clear ? Weather::Rain : Weather::Clear;
    if(key==GLFW_KEY_P) s.paused=!s.paused;
    if(key==GLFW_KEY_H) app.showHelp=!app.showHelp;
    if(key==GLFW_KEY_SPACE) {
        app.tour=!app.tour; app.tourTime=0;
        if(app.tour) app.camera.reset();
    }
    if(key==GLFW_KEY_TAB) {
        app.captured=!app.captured;
        app.firstMouse=true;
        glfwSetInputMode(window,GLFW_CURSOR,app.captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
    updateTitle(window,app);
}

void mouseMoved(GLFWwindow* window, double x, double y) {
    auto& app=*static_cast<AppState*>(glfwGetWindowUserPointer(window));
    if(!app.captured || app.showcase.active) return;
    if(app.firstMouse) { app.lastX=x; app.lastY=y; app.firstMouse=false; return; }
    app.tour=false;
    app.camera.look(static_cast<float>(x-app.lastX),static_cast<float>(app.lastY-y));
    app.lastX=x; app.lastY=y;
}
void scrolled(GLFWwindow* window, double y) {
    auto& app=*static_cast<AppState*>(glfwGetWindowUserPointer(window));
    if(app.showcase.active) return;
    app.tour=false;
    app.camera.zoom(static_cast<float>(y));
}

static void processMovement(GLFWwindow* window, AppState& app, float dt) {
    if(app.showcase.active) return;
    if(!glfwGetWindowAttrib(window,GLFW_FOCUSED)) return;
    auto pressed=[&](int key) { return glfwGetKey(window,key)==GLFW_PRESS ? 1.0f : 0.0f; };
    float ahead=pressed(GLFW_KEY_W)-pressed(GLFW_KEY_S), right=pressed(GLFW_KEY_D)-pressed(GLFW_KEY_A);
    float up=pressed(GLFW_KEY_E)-pressed(GLFW_KEY_Q);
    if(ahead!=0 || right!=0 || up!=0) app.tour=false;
    app.camera.move(ahead,right,up,dt);
}

void updateCameraTour(AppState& app, float dt) {
    if (!app.tour || app.simulation.paused) return;
    app.tourTime=std::fmod(app.tourTime+dt,32.0);
    const glm::vec3 positions[]={{2.65f,1.85f,2.60f},{0.5f,1.8f,2.35f},
                                {0.6f,1.65f,0.1f},{1.0f,2.0f,1.95f},{2.65f,1.85f,2.60f}};
    const glm::vec3 targets[]={{0,1.05f,-1},{-1.8f,0.8f,-0.45f},
                              {1.9f,1.1f,-2.2f},{-0.3f,1.6f,-2.2f},{0,1.05f,-1}};
    const int segment=static_cast<int>(app.tourTime/8.0);
    float t=static_cast<float>(app.tourTime/8.0-segment);
    t=t*t*(3-2*t);
    app.camera.overview=false;
    app.camera.fov=76;
    app.camera.position=glm::mix(positions[segment],positions[segment+1],t);
    app.camera.lookAt(glm::mix(targets[segment],targets[segment+1],t));
}

void updateApplication(AppState& app, float dt) {
    if(!std::isfinite(dt) || dt<=0) return;
    if(app.showcase.active) { app.showcase.update(app,dt); return; }
    app.simulation.update(dt);
    updateCameraTour(app,dt);
}

struct Options {
    bool verify=false, report=false;
    bool night=false, rain=false, noHud=false;
    std::filesystem::path capture;
    std::filesystem::path shaders;
    std::string view="interior";
    ShadingMode mode=ShadingMode::Phong;
};

static Options parseOptions(int argc,char** argv) {
    Options result;
    result.shaders=std::filesystem::absolute(argv[0]).parent_path()/"shaders";
    for(int i=1;i<argc;++i) {
        const std::string arg=argv[i];
        if(arg=="--verify") result.verify=true;
        else if(arg=="--capture-report") result.report=true;
        else if(arg=="--night") result.night=true;
        else if(arg=="--rain") result.rain=true;
        else if(arg=="--no-hud") result.noHud=true;
        else if(i+1<argc && arg=="--capture") result.capture=argv[++i];
        else if(i+1<argc && arg=="--shaders") result.shaders=argv[++i];
        else if(i+1<argc && arg=="--view") result.view=argv[++i];
        else if(i+1<argc && arg=="--mode") {
            const std::string mode=argv[++i];
            if(mode=="flat") result.mode=ShadingMode::Flat;
            else if(mode=="gouraud") result.mode=ShadingMode::Gouraud;
            else if(mode=="phong") result.mode=ShadingMode::Phong;
            else throw std::runtime_error("Mode must be flat, gouraud, or phong");
        } else throw std::runtime_error("Unknown or incomplete option: "+arg);
    }
    if(result.report && (result.verify || !result.capture.empty() || result.night || result.rain || result.noHud
        || result.view!="interior" || result.mode!=ShadingMode::Phong))
        throw std::runtime_error("--capture-report defines its own states; use it separately from other scene/capture options");
    return result;
}

static void runApplication(GLFWwindow* window,const Options& options) {
    AppState app;
    app.shading=options.mode;
    app.showHud=!options.noHud;
    if(options.night) { app.simulation.hour=22; app.simulation.dayCycle=false; }
    if(options.rain) app.simulation.weather=Weather::Rain;
    if(options.view=="overview") app.camera.toggleOverview();
    else if(options.view=="bedroom") {
        app.camera.position={0.50f,1.8f,2.35f}; app.camera.lookAt({-1.8f,0.8f,-0.45f});
    } else if(options.view=="study") {
        app.camera.position={0.6f,1.65f,0.1f}; app.camera.lookAt({1.9f,1.1f,-2.2f});
    } else if(options.view!="interior") throw std::runtime_error("View must be interior, overview, bedroom, or study");
    glfwSetWindowUserPointer(window,&app);
    glfwSetKeyCallback(window,[](GLFWwindow* w,int key,int,int action,int) { handleKey(w,key,action); });
    glfwSetCursorPosCallback(window,mouseMoved);
    glfwSetScrollCallback(window,[](GLFWwindow* w,double,double y) { scrolled(w,y); });
    glfwSetWindowFocusCallback(window,[](GLFWwindow* w,int) {
        static_cast<AppState*>(glfwGetWindowUserPointer(w))->firstMouse=true;
    });
    updateTitle(window,app);
    const std::array<std::string,3> names={"flat","gouraud","phong"};
    std::array<std::unique_ptr<Shader>,3> shaders;
    for(size_t i=0;i<names.size();++i)
        shaders[i]=std::make_unique<Shader>(options.shaders/(names[i]+".vert"),options.shaders/(names[i]+".frag"));
    Shader unlit(options.shaders/"unlit.vert",options.shaders/"unlit.frag");
    Primitives primitives;
    Hud hud;
    std::cout << "All four shader programs linked. Primitive uploads: " << Mesh::uploads << '\n';
    unsigned int drawCalls=0;
    auto render=[&]() {
        int width=0,height=0;
        glfwGetFramebufferSize(window,&width,&height);
        if(width<=0 || height<=0) return;
        glViewport(0,0,width,height);
        const auto sky=app.simulation.skyColor()*0.30f;
        glClearColor(sky.r,sky.g,sky.b,1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        auto projection=glm::perspective(glm::radians(app.camera.fov),float(width)/height,0.05f,50.0f);
        Shader& lit=*shaders[static_cast<size_t>(app.shading)];
        lit.use(); lit.set("view",app.camera.view()); lit.set("projection",projection);
        uploadLighting(lit,app.camera.position,app.simulation);
        unlit.use(); unlit.set("view",app.camera.view()); unlit.set("projection",projection);
        DrawContext ctx{primitives,lit,unlit,app.shading==ShadingMode::Flat,app.simulation};
        drawScene(ctx,app.camera.overview);
        drawCalls=ctx.drawCalls;
        if(app.showHud) hud.draw(unlit,app,width,height);
    };
    if(options.report) {
        captureReport(window,app,render,"report_screenshots");
    } else if(options.verify) {
        verifyApplication(window,app,render,"docs/screenshots");
    } else if(!options.capture.empty()) {
        render(); glFinish();
        int width=0,height=0; glfwGetFramebufferSize(window,&width,&height);
        saveScreenshot(options.capture,width,height);
    } else {
        std::cout << "WASD move | Q/E down/up | Tab capture/release mouse | Wheel zoom\n"
                  << "1 Flat | 2 Gouraud | 3 Phong | V overview | R reset | F12 screenshot | Esc exit\n"
                  << "F fan | [ / ] fan speed - / + | O door | C curtains | U wardrobe | J drawer | M laptop\n"
                  << "I keyboard backlight (automatic fade with laptop lid)\n"
                  << "L ceiling | B bedside | K study lamp | N day/night | T day cycle | G weather\n"
                  << "P pause | Space camera tour | H help | Y automated showcase (109 seconds)\n";
        double lastTime=glfwGetTime();
        while(!glfwWindowShouldClose(window)) {
            double now=glfwGetTime();
            const float frameDt=static_cast<float>(now-lastTime);
            const float dt=std::min(frameDt,0.1f); lastTime=now;
            glfwPollEvents(); processMovement(window,app,dt);
            int width=0,height=0; glfwGetFramebufferSize(window,&width,&height);
            if(width<=0 || height<=0) {
                if(app.showcase.active) updateApplication(app,frameDt);
                glfwWaitEventsTimeout(0.05); continue;
            }
            const bool wasShowcase=app.showcase.active;
            const auto oldPhase=app.showcase.phase;
            updateApplication(app,app.showcase.active ? frameDt : dt);
            if(wasShowcase!=app.showcase.active || oldPhase!=app.showcase.phase) updateTitle(window,app);
            render();
            if(app.screenshotRequested) {
                saveScreenshot(std::filesystem::path("screenshots")/(std::string(modeName(app.shading))+"-"+std::to_string(static_cast<int>(now*1000))+".bmp"),width,height);
                app.screenshotRequested=false;
            }
            glfwSwapBuffers(window);
        }
    }
    const auto error=glGetError();
    if(error!=GL_NO_ERROR) throw std::runtime_error("OpenGL error: "+std::to_string(error));
    std::cout << "Draw calls in final frame: " << drawCalls << "; clean shutdown.\n";
    // Local Shader/Mesh destructors run here, BEFORE glfwDestroyWindow.
}

int main(int argc,char** argv) {
    GLFWwindow* window=nullptr;
    try {
        const auto options=parseOptions(argc,argv);
        glfwSetErrorCallback([](int code,const char* message) { std::cerr<<"GLFW "<<code<<": "<<message<<'\n'; });
        if(!glfwInit()) throw std::runtime_error("GLFW initialization failed");
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
        glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif
        glfwWindowHint(GLFW_SAMPLES,4);
        if(options.verify || options.report || !options.capture.empty()) glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
        if(options.report) { glfwWindowHint(GLFW_SCALE_TO_MONITOR,GLFW_FALSE); glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE); }
        window=glfwCreateWindow(options.report ? 1920 : 1280,options.report ? 1080 : 900,"My Smart Study Bedroom",nullptr,nullptr);
        if(!window) throw std::runtime_error("Cannot create an OpenGL 3.3 core window; check graphics drivers");
        glfwMakeContextCurrent(window);
        if(!gladLoadGL(glfwGetProcAddress) || !GLAD_GL_VERSION_3_3) throw std::runtime_error("GLAD cannot load OpenGL 3.3");
        glfwSwapInterval(1);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        // No face culling: thin fabric and room interiors remain inspectable.
        std::cout<<"Vendor: "<<glGetString(GL_VENDOR)<<"\nRenderer: "<<glGetString(GL_RENDERER)
                 <<"\nOpenGL: "<<glGetString(GL_VERSION)<<"\nGLSL: "<<glGetString(GL_SHADING_LANGUAGE_VERSION)<<'\n';
        GLint profile=0; glGetIntegerv(GL_CONTEXT_PROFILE_MASK,&profile);
        if(!(profile&GL_CONTEXT_CORE_PROFILE_BIT)) throw std::runtime_error("The graphics context is not core profile");
        std::cout<<"Core profile confirmed; API usage restricted to OpenGL 3.3.\n";
        runApplication(window,options);
        glfwDestroyWindow(window); glfwTerminate();
        return 0;
    } catch(const std::exception& error) {
        std::cerr<<"Error: "<<error.what()<<'\n';
        if(window) glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
}
