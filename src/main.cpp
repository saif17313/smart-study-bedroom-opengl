#include <glad/gl.h>
#include "App.h"
#include "Lighting.h"
#include "Scene.h"
#include "Verification.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <array>
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
    title += " | 1/2/3 shading | V overview | Tab mouse | R reset";
    glfwSetWindowTitle(window,title.c_str());
}

void handleKey(GLFWwindow* window, int key, int action) {
    if(action!=GLFW_PRESS) return;
    auto& app=*static_cast<AppState*>(glfwGetWindowUserPointer(window));
    if(key>=GLFW_KEY_1 && key<=GLFW_KEY_3) app.shading=static_cast<ShadingMode>(key-GLFW_KEY_1);
    if(key==GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window,GLFW_TRUE);
    if(key==GLFW_KEY_R) app.camera.reset();
    if(key==GLFW_KEY_V) app.camera.toggleOverview();
    if(key==GLFW_KEY_F12) app.screenshotRequested=true;
    if(key==GLFW_KEY_TAB) {
        app.captured=!app.captured;
        app.firstMouse=true;
        glfwSetInputMode(window,GLFW_CURSOR,app.captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
    updateTitle(window,app);
}

void mouseMoved(GLFWwindow* window, double x, double y) {
    auto& app=*static_cast<AppState*>(glfwGetWindowUserPointer(window));
    if(!app.captured) return;
    if(app.firstMouse) { app.lastX=x; app.lastY=y; app.firstMouse=false; return; }
    app.camera.look(static_cast<float>(x-app.lastX),static_cast<float>(app.lastY-y));
    app.lastX=x; app.lastY=y;
}
void scrolled(GLFWwindow* window, double y) {
    static_cast<AppState*>(glfwGetWindowUserPointer(window))->camera.zoom(static_cast<float>(y));
}

static void processMovement(GLFWwindow* window, AppState& app, float dt) {
    if(!glfwGetWindowAttrib(window,GLFW_FOCUSED)) return;
    auto pressed=[&](int key) { return glfwGetKey(window,key)==GLFW_PRESS ? 1.0f : 0.0f; };
    app.camera.move(pressed(GLFW_KEY_W)-pressed(GLFW_KEY_S),pressed(GLFW_KEY_D)-pressed(GLFW_KEY_A),
                    pressed(GLFW_KEY_E)-pressed(GLFW_KEY_Q),dt);
}

struct Options {
    bool verify=false;
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
    return result;
}

static void runApplication(GLFWwindow* window,const Options& options) {
    AppState app;
    app.shading=options.mode;
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
    std::cout << "All four shader programs linked. Primitive uploads: " << Mesh::uploads << '\n';
    unsigned int drawCalls=0;
    auto render=[&]() {
        int width=0,height=0;
        glfwGetFramebufferSize(window,&width,&height);
        if(width<=0 || height<=0) return;
        glViewport(0,0,width,height);
        glClearColor(0.12f,0.15f,0.18f,1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        auto projection=glm::perspective(glm::radians(app.camera.fov),float(width)/height,0.05f,50.0f);
        Shader& lit=*shaders[static_cast<size_t>(app.shading)];
        lit.use(); lit.set("view",app.camera.view()); lit.set("projection",projection);
        uploadLighting(lit,app.camera.position);
        unlit.use(); unlit.set("view",app.camera.view()); unlit.set("projection",projection);
        DrawContext ctx{primitives,lit,unlit,app.shading==ShadingMode::Flat};
        drawScene(ctx,app.camera.overview);
        drawCalls=ctx.drawCalls;
    };
    if(options.verify) {
        verifyApplication(window,app,render,"docs/screenshots");
    } else if(!options.capture.empty()) {
        render(); glFinish();
        int width=0,height=0; glfwGetFramebufferSize(window,&width,&height);
        saveScreenshot(options.capture,width,height);
    } else {
        std::cout << "WASD move | Q/E down/up | Tab capture/release mouse | Wheel zoom\n"
                  << "1 Flat | 2 Gouraud | 3 Phong | V overview | R reset | F12 screenshot | Esc exit\n";
        double lastTime=glfwGetTime();
        while(!glfwWindowShouldClose(window)) {
            double now=glfwGetTime();
            const float dt=static_cast<float>(std::min(now-lastTime,0.1)); lastTime=now;
            glfwPollEvents(); processMovement(window,app,dt);
            int width=0,height=0; glfwGetFramebufferSize(window,&width,&height);
            if(width<=0 || height<=0) { glfwWaitEventsTimeout(0.05); continue; }
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
        if(options.verify || !options.capture.empty()) glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
        window=glfwCreateWindow(1280,900,"My Smart Study Bedroom",nullptr,nullptr);
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
