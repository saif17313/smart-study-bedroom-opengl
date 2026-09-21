#version 330 core
flat in vec3 litColor;
out vec4 fragmentColor;
void main() {
    // Lighting is linear; encode once for the display (no framebuffer sRGB).
    fragmentColor = vec4(pow(max(litColor, vec3(0.0)), vec3(1.0 / 2.2)), 1.0);
}
