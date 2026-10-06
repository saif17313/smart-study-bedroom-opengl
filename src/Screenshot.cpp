#include <glad/gl.h>
#include "Screenshot.h"
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_WRITE_NO_STDIO
#include <stb_image_write.h>

std::vector<unsigned char> readFrame(int width, int height) {
    if(width<=0 || height<=0) throw std::runtime_error("Cannot capture a zero-size framebuffer");
    std::vector<unsigned char> pixels(static_cast<size_t>(width)*height*3);
    glPixelStorei(GL_PACK_ALIGNMENT,1);
    glReadBuffer(GL_BACK);
    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());
    return pixels;
}

void saveScreenshot(const std::filesystem::path& path, int width, int height) {
    auto pixels=readFrame(width,height);
    if(!path.parent_path().empty()) std::filesystem::create_directories(path.parent_path());
    std::ofstream file(path,std::ios::binary);
    if(!file) throw std::runtime_error("Cannot write screenshot: "+path.string());
    if(path.extension()==".png") {
        const size_t stride=static_cast<size_t>(width)*3;
        for(int y=0;y<height/2;++y)
            std::swap_ranges(pixels.begin()+y*stride,pixels.begin()+(y+1)*stride,
                             pixels.begin()+(height-1-y)*stride);
        auto write=[](void* context,void* bytes,int count) {
            static_cast<std::ofstream*>(context)->write(static_cast<const char*>(bytes),count);
        };
        if(!stbi_write_png_to_func(write,&file,width,height,3,pixels.data(),width*3))
            throw std::runtime_error("PNG encoding failed: "+path.string());
    } else {
        // Preserve the existing BMP format used by F12 and --verify.
        const uint32_t stride=(static_cast<uint32_t>(width)*3+3)&~3u;
        const uint32_t size=54+stride*height;
        unsigned char header[54]={'B','M'};
        auto put=[&](int offset,uint32_t value) {
            for(int b=0;b<4;++b) header[offset+b]=static_cast<unsigned char>(value>>(8*b));
        };
        put(2,size); put(10,54); put(14,40); put(18,width); put(22,height);
        header[26]=1; header[28]=24; put(34,stride*height);
        file.write(reinterpret_cast<char*>(header),54);
        std::vector<unsigned char> row(stride,0);
        for(int y=0;y<height;++y) {
            for(int x=0;x<width;++x) for(int b=0;b<3;++b)
                row[x*3+b]=pixels[(static_cast<size_t>(y)*width+x)*3+2-b];
            file.write(reinterpret_cast<char*>(row.data()),stride);
        }
    }
    file.flush();
    if(!file) throw std::runtime_error("Screenshot write failed: "+path.string());
    std::cout << "Screenshot: " << path.string() << '\n';
}
