#include <glad/gl.h>
#include "Verification.h"
#include "Mesh.h"
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

static std::vector<unsigned char> readFrame(int width, int height) {
    std::vector<unsigned char> pixels(static_cast<size_t>(width)*height*3);
    glPixelStorei(GL_PACK_ALIGNMENT,1);
    glReadBuffer(GL_BACK);
    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());
    return pixels;
}

void saveScreenshot(const std::filesystem::path& path, int width, int height) {
    if (width<=0 || height<=0) throw std::runtime_error("Cannot capture a zero-size framebuffer");
    auto pixels = readFrame(width,height);
    const uint32_t stride = (static_cast<uint32_t>(width)*3+3)&~3u;
    const uint32_t size = 54+stride*height;
    unsigned char header[54] = {'B','M'};
    auto put = [&](int offset,uint32_t value) {
        for(int b=0;b<4;++b) header[offset+b] = static_cast<unsigned char>(value>>(8*b));
    };
    put(2,size); put(10,54); put(14,40); put(18,width); put(22,height);
    header[26]=1; header[28]=24; put(34,stride*height);
    if (!path.parent_path().empty()) std::filesystem::create_directories(path.parent_path());
    std::ofstream file(path,std::ios::binary);
    if (!file) throw std::runtime_error("Cannot write screenshot: "+path.string());
    file.write(reinterpret_cast<char*>(header),54);
    std::vector<unsigned char> row(stride,0);
    for(int y=0;y<height;++y) {
        for(int x=0;x<width;++x) for(int b=0;b<3;++b)
            row[x*3+b]=pixels[(static_cast<size_t>(y)*width+x)*3+2-b];
        file.write(reinterpret_cast<char*>(row.data()),stride);
    }
    if (!file) throw std::runtime_error("Screenshot write failed: "+path.string());
    std::cout << "Screenshot: " << path.string() << '\n';
}

static void require(bool passed, const char* message) {
    if (!passed) throw std::runtime_error(std::string("Verification failed: ")+message);
    std::cout << "PASS: " << message << '\n';
}

static void verifyFanSpeedControls(GLFWwindow* window, AppState& app) {
    app.simulation=Simulation{};
    auto& s=app.simulation;
    require(s.fanOn && s.fanSpeedLevel==2 && s.targetFanSpeed()==240,
        "fan starts at Medium with the original target speed");
    handleKey(window,GLFW_KEY_RIGHT_BRACKET,GLFW_PRESS);
    require(s.fanSpeedLevel==3 && s.targetFanSpeed()==360 && s.fanSpeed==0,
        "right bracket selects High without jumping angular velocity");
    for(int i=0;i<20;++i) handleKey(window,GLFW_KEY_RIGHT_BRACKET,GLFW_PRESS);
    require(s.fanSpeedLevel==4 && s.targetFanSpeed()==480,
        "repeated right bracket presses clamp at Max");
    s.update(0.5f);
    require(s.fanSpeed==60 && s.fanAngle==15,"fan accelerates with the original integrated ramp");
    s.update(10);
    require(s.fanSpeed==480,"fan settles exactly at the maximum angular speed");
    handleKey(window,GLFW_KEY_LEFT_BRACKET,GLFW_PRESS);
    require(s.fanSpeedLevel==3 && s.targetFanSpeed()==360 && s.fanSpeed==480,
        "left bracket selects High without jumping angular velocity");
    s.update(0.5f);
    require(s.fanSpeed==420,"lower speed selection smoothly decelerates the fan");
    s.update(5);
    handleKey(window,GLFW_KEY_F,GLFW_PRESS);
    require(!s.fanOn && s.fanSpeedLevel==3 && s.targetFanSpeed()==0 && s.fanSpeed==360,
        "F turns the fan off and remembers High without stopping instantly");
    s.update(0.5f);
    require(s.fanSpeed==300,"fan coasts smoothly after switching power off");
    s.update(10);
    const float stopped=s.fanAngle;
    s.update(1);
    require(s.fanSpeed==0 && s.fanAngle==stopped,"powered-off fan reaches zero and stays stopped");
    handleKey(window,GLFW_KEY_F,GLFW_PRESS);
    require(s.fanOn && s.fanSpeedLevel==3 && s.targetFanSpeed()==360,
        "F restores the previously selected High speed");
    s.update(0.5f);
    require(s.fanSpeed==60,"restored fan smoothly accelerates again");
    handleKey(window,GLFW_KEY_RIGHT_BRACKET,GLFW_REPEAT);
    handleKey(window,GLFW_KEY_LEFT_BRACKET,GLFW_RELEASE);
    require(s.fanSpeedLevel==3,"held and released bracket keys do not change speed levels");
    for(int i=0;i<20;++i) handleKey(window,GLFW_KEY_LEFT_BRACKET,GLFW_PRESS);
    require(!s.fanOn && s.targetFanSpeed()==0 && s.fanSpeedLevel==1 && s.fanSpeed==60,
        "repeated left bracket presses clamp at Off and preserve Low for F");
    handleKey(window,GLFW_KEY_F,GLFW_PRESS);
    require(s.fanOn && s.fanSpeedLevel==1 && s.targetFanSpeed()==120,
        "F restores Low after stepping down to Off");
    handleKey(window,GLFW_KEY_F,GLFW_PRESS);
    handleKey(window,GLFW_KEY_LEFT_BRACKET,GLFW_PRESS);
    require(!s.fanOn && s.targetFanSpeed()==0,"left bracket keeps a powered-off fan at Off");
    handleKey(window,GLFW_KEY_RIGHT_BRACKET,GLFW_PRESS);
    require(s.fanOn && s.fanSpeedLevel==1 && s.targetFanSpeed()==120,
        "right bracket starts a powered-off fan at Low");
    handleKey(window,GLFW_KEY_P,GLFW_PRESS);
    const float pausedAngle=s.fanAngle, pausedSpeed=s.fanSpeed;
    handleKey(window,GLFW_KEY_RIGHT_BRACKET,GLFW_PRESS);
    s.update(5);
    require(s.targetFanSpeed()==240 && s.fanAngle==pausedAngle && s.fanSpeed==pausedSpeed,
        "speed selection while paused changes the target and keeps motion frozen");
    handleKey(window,GLFW_KEY_P,GLFW_PRESS);
    s.update(0.5f);
    require(s.fanSpeed==120,"unpause smoothly approaches the selected Medium speed");

    Simulation oneStep,slow,fast;
    oneStep.changeFanSpeed(2); slow.changeFanSpeed(2); fast.changeFanSpeed(2);
    oneStep.update(4.5f);
    for(int i=0;i<135;++i) slow.update(1.0f/30.0f);
    for(int i=0;i<648;++i) fast.update(1.0f/144.0f);
    require(std::abs(oneStep.fanAngle-120)<0.01f && std::abs(slow.fanAngle-oneStep.fanAngle)<0.05f
        && std::abs(fast.fanAngle-oneStep.fanAngle)<0.05f
        && slow.fanSpeed==480 && fast.fanSpeed==480,
        "Max acceleration and constant-speed remainder agree across frame rates");
    oneStep.changeFanSpeed(-3); slow.changeFanSpeed(-3); fast.changeFanSpeed(-3);
    oneStep.update(3.5f);
    for(int i=0;i<105;++i) slow.update(1.0f/30.0f);
    for(int i=0;i<504;++i) fast.update(1.0f/144.0f);
    require(std::abs(oneStep.fanAngle)<0.01f && std::abs(std::remainder(slow.fanAngle-oneStep.fanAngle,360.0f))<0.05f
        && std::abs(std::remainder(fast.fanAngle-oneStep.fanAngle,360.0f))<0.05f
        && slow.fanSpeed==120 && fast.fanSpeed==120,
        "Max-to-Low deceleration and constant-speed remainder agree across frame rates");
}

void verifyApplication(GLFWwindow* window, AppState& app, const std::function<void()>& render,
                       const std::filesystem::path& output) {
    app.simulation=Simulation{};
    app.simulation.dayCycle=false;
    app.showHud=false;
    const unsigned int uploadCount=Mesh::uploads;
    int width=0,height=0;
    glfwGetFramebufferSize(window,&width,&height);
    std::vector<unsigned char> previous;
    for (int key=GLFW_KEY_1;key<=GLFW_KEY_3;++key) {
        handleKey(window,key,GLFW_PRESS);
        render(); glFinish();
        auto frame=readFrame(width,height);
        if (!previous.empty()) require(frame!=previous,"shading modes produce different rendered pixels");
        previous=frame;
        saveScreenshot(output/(std::string(modeName(app.shading))+".bmp"),width,height);
    }
    require(app.shading==ShadingMode::Phong,"1/2/3 dispatch selects all three shading modes");
    handleKey(window,GLFW_KEY_V,GLFW_PRESS);
    require(app.camera.overview,"overview keyboard preset");
    render(); saveScreenshot(output/"Overview.bmp",width,height);
    handleKey(window,GLFW_KEY_V,GLFW_REPEAT);
    require(app.camera.overview,"held toggle key does not toggle repeatedly");
    handleKey(window,GLFW_KEY_R,GLFW_PRESS);
    require(!app.camera.overview,"reset returns to interior view");
    const auto start=app.camera.position;
    app.camera.move(1,0,0,0.5f);
    require(glm::length(app.camera.position-start)>0.9f,"camera movement uses delta time");
    app.camera.reset();
    for(int i=0;i<10;++i) app.camera.move(1,0,0,0.05f);
    require(std::abs(glm::length(app.camera.position-start)-1.0f)<0.001f,"camera travel agrees across frame rates");
    app.camera.reset();
    handleKey(window,GLFW_KEY_TAB,GLFW_PRESS);
    const float yaw=app.camera.yaw;
    mouseMoved(window,200,200);
    require(app.camera.yaw==yaw,"first mouse sample prevents capture jump");
    mouseMoved(window,300,150);
    require(app.camera.yaw!=yaw,"mouse callback updates camera orientation");
    scrolled(window,1000);
    require(app.camera.fov==30,"zoom is bounded");
    app.camera.look(0,10000);
    require(app.camera.pitch==89,"pitch is bounded");
    handleKey(window,GLFW_KEY_TAB,GLFW_PRESS);
    app.camera.reset();
    // Advance two identical scenes by the same amount, changing only the tested
    // control. This prevents clock/fan motion or HUD text from faking success.
    auto capture=[&]() { render(); glFinish(); return readFrame(width,height); };
    auto interaction=[&](int key,const char* message,const char* screenshot,float dt=0.8f) {
        app.simulation=Simulation{};
        app.simulation.dayCycle=false;
        auto baseline=app.simulation;
        baseline.update(dt);
        handleKey(window,key,GLFW_PRESS);
        app.simulation.update(dt);
        const auto changed=app.simulation;
        app.simulation=baseline;
        const auto before=capture();
        app.simulation=changed;
        const auto after=capture();
        require(before!=after,message);
        if(screenshot) saveScreenshot(output/screenshot,width,height);
    };
    verifyFanSpeedControls(window,app);
    interaction(GLFW_KEY_F,"fan switch changes the rendered rotor",nullptr);
    interaction(GLFW_KEY_RIGHT_BRACKET,"increasing fan speed changes the rendered rotor",nullptr,3.0f);
    interaction(GLFW_KEY_LEFT_BRACKET,"decreasing fan speed changes the rendered rotor",nullptr,3.0f);
    interaction(GLFW_KEY_C,"curtain movement changes geometry and daylight", "Curtains.bmp");
    interaction(GLFW_KEY_N,"day/night preset changes sky and room lighting", "Night.bmp");
    interaction(GLFW_KEY_G,"rain changes the exterior and daylight", "Rain.bmp");
    app.camera.position={0.7f,1.65f,2.4f}; app.camera.lookAt({3.1f,1.05f,1.6f});
    interaction(GLFW_KEY_O,"door opens around its hinge", "Door.bmp");
    app.camera.position={0.5f,1.8f,0.7f}; app.camera.lookAt({-2.15f,1.1f,-2.36f});
    interaction(GLFW_KEY_U,"wardrobe doors reveal the shelves", "Wardrobe.bmp");
    app.camera.position={3.0f,1.05f,-0.9f}; app.camera.lookAt({2.45f,0.58f,-2.0f});
    interaction(GLFW_KEY_J,"desk drawer slides out", "Drawer.bmp");
    app.camera.position={1.5f,1.6f,-1.0f}; app.camera.lookAt({1.6f,0.9f,-2.16f});
    interaction(GLFW_KEY_M,"laptop lid rotates around its hinge", "Laptop.bmp");

    // Each actual light must affect room pixels in every shading path, even
    // with all other lights off. Geometry-only emissive toggles are insufficient.
    for(int mode=0;mode<3;++mode) {
        app.shading=static_cast<ShadingMode>(mode);
        for(int key : {GLFW_KEY_L,GLFW_KEY_B,GLFW_KEY_K}) {
            app.simulation=Simulation{};
            app.simulation.hour=22;
            app.simulation.dayCycle=false;
            app.simulation.ceilingLight=false;
            app.simulation.bedsideLight=false;
            app.simulation.studyLight=false;
            // Aim at surfaces below the sources, with their glowing models out of view.
            if(key==GLFW_KEY_B) {
                app.camera.position={-2.85f,0.40f,0.3f}; app.camera.lookAt({-2.85f,0.05f,0.2f});
            } else if(key==GLFW_KEY_K) {
                app.camera.position={2.25f,1.08f,-2.10f}; app.camera.lookAt({2.25f,0.77f,-2.20f});
            } else {
                app.camera.position={0.0f,1.30f,0.5f}; app.camera.lookAt({0.0f,0.0f,0.0f});
            }
            const auto dark=capture();
            handleKey(window,key,GLFW_PRESS);
            require(dark!=capture(),"independent light illuminates surfaces in the selected shading mode");
        }
    }
    app.shading=ShadingMode::Phong;
    app.camera.reset();
    app.simulation=Simulation{};
    app.simulation.hour=22; app.simulation.dayCycle=false;
    app.simulation.ceilingLight=false;
    capture(); saveScreenshot(output/"Lamps.bmp",width,height);

    // Independent motion regressions: clock only, rain only, and fan coasting.
    app.simulation=Simulation{};
    app.camera.position={1.8f,2.3f,-1.0f}; app.camera.lookAt({2.45f,2.64f,-2.75f});
    const auto clockBefore=capture();
    app.simulation.clockSeconds+=15;
    require(clockBefore!=capture(),"clock hands advance in rendered pixels");
    app.simulation=Simulation{}; app.simulation.weather=Weather::Rain;
    app.camera.position={-0.05f,1.8f,-2.1f}; app.camera.lookAt({-0.05f,1.8f,-2.8f});
    app.camera.fov=45;
    const auto rainBefore=capture();
    app.simulation.elapsed=0.23;
    require(rainBefore!=capture(),"rain streaks move outside the window");

    Simulation slow,fast;
    slow.doorOpen=fast.doorOpen=true;
    slow.curtainsOpen=fast.curtainsOpen=false;
    slow.wardrobeOpen=fast.wardrobeOpen=true;
    slow.drawerOpen=fast.drawerOpen=true;
    slow.laptopOpen=fast.laptopOpen=false;
    for(int i=0;i<30;++i) slow.update(1.0f/30.0f);
    for(int i=0;i<144;++i) fast.update(1.0f/144.0f);
    require(std::abs(slow.fanAngle-fast.fanAngle)<0.01f && std::abs(slow.doorAngle-fast.doorAngle)<0.01f
        && std::abs(slow.curtainAmount-fast.curtainAmount)<0.001f
        && std::abs(slow.wardrobeAngle-fast.wardrobeAngle)<0.01f
        && std::abs(slow.drawerAmount-fast.drawerAmount)<0.001f
        && std::abs(slow.laptopAmount-fast.laptopAmount)<0.001f
        && std::abs(slow.hour-fast.hour)<0.0001,"motion agrees at 30 and 144 updates per second");
    slow.update(5);
    require(slow.doorAngle==100 && slow.wardrobeAngle==105 && slow.curtainAmount==0
        && slow.drawerAmount==1 && slow.laptopAmount==0,"interactions stop exactly at open/closed limits");
    slow.fanOn=false; slow.doorOpen=false; slow.wardrobeOpen=false;
    slow.curtainsOpen=true; slow.drawerOpen=false; slow.laptopOpen=true;
    slow.update(5);
    const float stoppedAngle=slow.fanAngle;
    slow.update(1);
    require(slow.fanSpeed==0 && slow.fanAngle==stoppedAngle,"fan decelerates and stops");
    require(slow.doorAngle==0 && slow.wardrobeAngle==0 && slow.curtainAmount==1
        && slow.drawerAmount==0 && slow.laptopAmount==1,"interactions return to their original limits");
    slow.hour=23.99; slow.clockSeconds=86399.5; slow.update(1);
    require(slow.hour<1 && slow.clockSeconds<1,"environment and clock wrap safely at midnight");

    app.simulation=Simulation{};
    handleKey(window,GLFW_KEY_N,GLFW_PRESS);
    require(app.simulation.hour==22 && !app.simulation.dayCycle,"night preset disables the automatic day cycle");
    handleKey(window,GLFW_KEY_N,GLFW_PRESS);
    require(app.simulation.hour==10,"day preset restores daylight");
    handleKey(window,GLFW_KEY_T,GLFW_PRESS);
    updateApplication(app,5);
    require(app.simulation.hour>10,"automatic day cycle resumes with T");
    handleKey(window,GLFW_KEY_F,GLFW_PRESS);
    handleKey(window,GLFW_KEY_F,GLFW_REPEAT);
    require(!app.simulation.fanOn,"held interaction keys do not retrigger toggles");
    app.camera.reset();
    handleKey(window,GLFW_KEY_SPACE,GLFW_PRESS);
    const auto tourStart=app.camera.position;
    updateApplication(app,4);
    require(app.tour && glm::length(app.camera.position-tourStart)>0.1f,"camera tour moves through room viewpoints");
    handleKey(window,GLFW_KEY_P,GLFW_PRESS);
    const auto tourPaused=app.camera.position;
    updateApplication(app,2);
    require(app.camera.position==tourPaused,"pause also freezes the camera tour");
    handleKey(window,GLFW_KEY_P,GLFW_PRESS);
    scrolled(window,1);
    require(!app.tour,"manual zoom cancels the camera tour");
    handleKey(window,GLFW_KEY_SPACE,GLFW_PRESS);
    handleKey(window,GLFW_KEY_R,GLFW_PRESS);
    require(!app.tour && !app.camera.overview,"camera reset cancels the tour");

    app.simulation=Simulation{};
    handleKey(window,GLFW_KEY_P,GLFW_PRESS);
    const auto frozen=capture();
    updateApplication(app,30);
    require(app.simulation.paused && app.simulation.elapsed==0 && frozen==capture(),"pause freezes all motion and rendered pixels");
    handleKey(window,GLFW_KEY_P,GLFW_PRESS);
    updateApplication(app,0.25f);
    require(frozen!=capture(),"unpause resumes visible animation");
    app.showHud=true; app.showHelp=true;
    capture(); saveScreenshot(output/"Controls.bmp",width,height);
    const auto help=capture();
    handleKey(window,GLFW_KEY_H,GLFW_PRESS);
    require(!app.showHelp && help!=capture(),"H hides the help panel");
    glfwSetWindowSize(window,960,640); glfwPollEvents(); render();
    glfwGetFramebufferSize(window,&width,&height);
    require(width==960 && height==640,"resize updates the framebuffer");
    glfwSetWindowSize(window,480,360); glfwPollEvents(); render();
    app.showHelp=true; render();
    glfwGetFramebufferSize(window,&width,&height);
    require(width==480 && height==360,"HUD renders in a small framebuffer");
    saveScreenshot(output/"SmallWindow.bmp",width,height);
    glfwSetWindowSize(window,1280,900); glfwPollEvents();
    app.camera.reset(); app.showHud=false;
    app.simulation=Simulation{};
    app.simulation.weather=Weather::Rain;
    render();
    glfwGetFramebufferSize(window,&width,&height);
    const auto first=readFrame(width,height);
    const double startTime=glfwGetTime();
    unsigned int frames=0;
    // Exercise the live simulation and renderer for a real 30-second soak.
    double lastTime=startTime;
    while(glfwGetTime()-startTime<30.0) {
        const double now=glfwGetTime();
        updateApplication(app,static_cast<float>(std::min(now-lastTime,0.1)));
        lastTime=now;
        render(); glfwPollEvents(); ++frames;
    }
    glFinish();
    const auto last=readFrame(width,height);
    require(first!=last && app.simulation.elapsed>0,"scene animates throughout the 30-second render soak");
    require(Mesh::uploads==uploadCount,"navigation and shading switches never re-upload meshes");
    require(glGetError()==GL_NO_ERROR,"no OpenGL error during verification");
    std::cout << "Render soak: " << frames << " frames in " << glfwGetTime()-startTime
              << " seconds (offscreen, no swap/vsync); mesh uploads: " << uploadCount << '\n';
    handleKey(window,GLFW_KEY_ESCAPE,GLFW_PRESS);
    require(glfwWindowShouldClose(window),"Escape requests clean shutdown");
}
