#pragma once
#include "Shader.h"

struct AppState;
// Tiny built-in bitmap lettering, batched into three draws. No font dependency.
class Hud {
public:
    Hud();
    ~Hud();
    Hud(const Hud&) = delete;
    Hud& operator=(const Hud&) = delete;
    void draw(Shader& shader, const AppState& app, int width, int height);
private:
    GLuint vao=0, vbo=0;
};
