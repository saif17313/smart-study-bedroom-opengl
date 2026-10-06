#pragma once
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Simulation.h"

struct Primitives {
    Mesh box, roundedBox, sphere, cylinder, frustum, curtain;
    Primitives();
};

// This is just the shared drawing data, not a scene graph or renderer hierarchy.
struct DrawContext {
    Primitives& shapes;
    Shader& lit;
    Shader& unlit;
    bool flat;
    const Simulation& simulation;
    unsigned int drawCalls = 0;
};

glm::mat4 transform(const glm::mat4& parent, glm::vec3 position,
                    glm::vec3 size = glm::vec3(1), float angle = 0,
                    glm::vec3 axis = glm::vec3(0,1,0));
void drawPart(DrawContext& ctx, const Mesh& mesh, const glm::mat4& model, const Material& material);
void drawBox(DrawContext& ctx, const glm::mat4& parent, glm::vec3 position, glm::vec3 size,
             const Material& material, float angle = 0, glm::vec3 axis = glm::vec3(0,1,0));
void drawRoundedBox(DrawContext& ctx, const glm::mat4& parent, glm::vec3 position, glm::vec3 size,
                    const Material& material, float angle = 0, glm::vec3 axis = glm::vec3(0,1,0));
void drawSphere(DrawContext& ctx, const glm::mat4& parent, glm::vec3 position, glm::vec3 size,
                const Material& material, float angle = 0, glm::vec3 axis = glm::vec3(0,1,0));
void drawCylinder(DrawContext& ctx, const glm::mat4& parent, glm::vec3 position, glm::vec3 size,
                  const Material& material, float angle = 0, glm::vec3 axis = glm::vec3(0,1,0));
void drawFrustum(DrawContext& ctx, const glm::mat4& parent, glm::vec3 position, glm::vec3 size,
                 const Material& material, float angle = 0, glm::vec3 axis = glm::vec3(0,1,0));
void drawRod(DrawContext& ctx, const glm::mat4& parent, glm::vec3 start, glm::vec3 end,
             float diameter, const Material& material);
