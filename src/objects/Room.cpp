#include "Room.h"
using namespace Materials;

void drawRoom(DrawContext& c, bool overview) {
    const glm::mat4 root(1);
    const Material floor{{0.48f,0.43f,0.35f},0.10f,24};
    drawBox(c,root,{0,-0.11f,0},{6.64f,0.22f,5.84f},floor);
    for(int row=0;row<14;++row) for(int col=0;col<8;++col) {
        const float x=-2.8f+col*0.80f, z=-2.6f+row*0.40f;
        const float tint=0.025f*float((row*3+col*7)%5);
        const Material plank{{0.33f+tint,0.225f+tint*0.8f,0.135f+tint*0.5f},0.13f,32};
        drawBox(c,root,{x,0.002f,z},{0.793f,0.009f,0.393f},plank);
    }
    drawBox(c,root,{-3.26f,1.55f,0},{0.12f,3.1f,5.84f},wall);
    // Rear wall is built AROUND the window, not behind it.
    drawBox(c,root,{-1.95f,1.55f,-2.86f},{2.5f,3.1f,0.12f},wall);
    drawBox(c,root,{1.90f,1.55f,-2.86f},{2.6f,3.1f,0.12f},wall);
    drawBox(c,root,{-0.05f,0.525f,-2.86f},{1.3f,1.05f,0.12f},wall);
    drawBox(c,root,{-0.05f,2.8f,-2.86f},{1.3f,0.6f,0.12f},wall);
    // Right wall has a real 0.95 m doorway centered at Z=1.6.
    drawBox(c,root,{3.26f,1.55f,-0.8375f},{0.12f,3.1f,3.925f},wall);
    drawBox(c,root,{3.26f,1.55f,2.4375f},{0.12f,3.1f,0.725f},wall);
    drawBox(c,root,{3.26f,2.625f,1.6f},{0.12f,0.95f,0.95f},wall);
    if (!overview) {
        drawBox(c,root,{0,1.55f,2.86f},{6.4f,3.1f,0.12f},wall);
        drawBox(c,root,{0,3.16f,0},{6.64f,0.12f,5.84f},ceiling);
        drawBox(c,root,{0,0.08f,2.785f},{6.4f,0.16f,0.035f},trim);
    }
    drawBox(c,root,{0,0.08f,-2.785f},{6.4f,0.16f,0.035f},trim);
    drawBox(c,root,{-3.185f,0.08f,0},{0.035f,0.16f,5.6f},trim);
    drawBox(c,root,{3.185f,0.08f,-0.8375f},{0.035f,0.16f,3.925f},trim);
    drawBox(c,root,{3.185f,0.08f,2.4375f},{0.035f,0.16f,0.725f},trim);
    drawBox(c,root,{0,3.01f,-2.76f},{6.4f,0.12f,0.10f},trim);
    drawBox(c,root,{-3.16f,3.01f,0},{0.10f,0.12f,5.6f},trim);
    drawBox(c,root,{3.16f,3.01f,0},{0.10f,0.12f,5.6f},trim);
}
