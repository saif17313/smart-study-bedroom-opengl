#version 330 core
uniform vec3 materialColor;
out vec4 fragmentColor;
void main() { fragmentColor = vec4(pow(materialColor, vec3(1.0 / 2.2)), 1.0); }
