#include <glad/gl.h>
#include "Verification.h"
#include "Mesh.h"
#include <algorithm>
#include <cstdint>
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

void verifyApplication(GLFWwindow* window, AppState& app, const std::function<void()>& render,
                       const std::filesystem::path& output) {
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
    glfwSetWindowSize(window,960,640); glfwPollEvents(); render();
    glfwGetFramebufferSize(window,&width,&height);
    require(width==960 && height==640,"resize updates the framebuffer");
    glfwSetWindowSize(window,1280,900); glfwPollEvents();
    app.camera.reset(); render();
    glfwGetFramebufferSize(window,&width,&height);
    const auto first=readFrame(width,height);
    const double startTime=glfwGetTime();
    unsigned int frames=0;
    // A real 30-second render soak confirms the Phase 1 scene has no time-driven motion.
    while(glfwGetTime()-startTime<30.0) { render(); glfwPollEvents(); ++frames; }
    glFinish();
    const auto last=readFrame(width,height);
    require(first==last,"scene pixels remain static after 30 seconds");
    require(Mesh::uploads==uploadCount,"navigation and shading switches never re-upload meshes");
    require(glGetError()==GL_NO_ERROR,"no OpenGL error during verification");
    std::cout << "Render soak: " << frames << " frames in " << glfwGetTime()-startTime
              << " seconds (offscreen, no swap/vsync); mesh uploads: " << uploadCount << '\n';
    handleKey(window,GLFW_KEY_ESCAPE,GLFW_PRESS);
    require(glfwWindowShouldClose(window),"Escape requests clean shutdown");
}
