#include "Primitives.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <algorithm>
#include <cmath>

static void triangle(MeshData& data, unsigned int a, unsigned int b, unsigned int c) {
    const auto& va = data.vertices[a];
    const auto& vb = data.vertices[b];
    const auto& vc = data.vertices[c];
    const auto cross = glm::cross(vb.position-va.position, vc.position-va.position);
    if (glm::length(cross) < 1e-9f) return; // Sphere pole cells have one collapsed triangle.
    if (glm::dot(cross, va.normal+vb.normal+vc.normal)<0) std::swap(b,c);
    data.indices.insert(data.indices.end(), {a,b,c});
}

static void gridIndices(MeshData& data, unsigned int first, unsigned int columns, unsigned int rows) {
    for (unsigned int y=0; y<rows; ++y) for (unsigned int x=0; x<columns; ++x) {
        const unsigned int a = first+y*(columns+1)+x;
        const unsigned int b = a+1, c = a+columns+1, d = c+1;
        triangle(data,a,b,c);
        triangle(data,b,d,c);
    }
}

static MeshData boxMesh(bool rounded) {
    MeshData data;
    const glm::vec3 normals[] = {{1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1}};
    const float steps[] = {-0.5f,-0.485f,-0.455f,-0.41f,0.0f,0.41f,0.455f,0.485f,0.5f};
    const unsigned int segments = rounded ? 8 : 4;
    for (auto normal : normals) {
        const glm::vec3 u = normal.y != 0 ? glm::vec3(1,0,0) : glm::vec3(0,1,0);
        const glm::vec3 v = glm::cross(normal,u);
        const unsigned int first = static_cast<unsigned int>(data.vertices.size());
        for (unsigned int y=0; y<=segments; ++y) for (unsigned int x=0; x<=segments; ++x) {
            float px = rounded ? steps[x] : float(x)/segments-0.5f;
            float py = rounded ? steps[y] : float(y)/segments-0.5f;
            glm::vec3 point = normal*0.5f+u*px+v*py;
            glm::vec3 n = normal;
            if (rounded) {
                const glm::vec3 core = glm::clamp(point, glm::vec3(-0.41f), glm::vec3(0.41f));
                n = glm::normalize(point-core);
                point = core+n*0.09f;
            }
            data.vertices.push_back({point,n});
        }
        gridIndices(data,first,segments,segments);
    }
    return data;
}

static MeshData sphereMesh() {
    MeshData data;
    constexpr unsigned int slices=24, stacks=16;
    for (unsigned int y=0; y<=stacks; ++y) for (unsigned int x=0; x<=slices; ++x) {
        const float latitude = glm::pi<float>()*float(y)/stacks;
        const float longitude = glm::two_pi<float>()*float(x)/slices;
        const glm::vec3 n{std::sin(latitude)*std::cos(longitude),std::cos(latitude),std::sin(latitude)*std::sin(longitude)};
        data.vertices.push_back({n*0.5f,n});
    }
    gridIndices(data,0,slices,stacks);
    return data;
}

static MeshData cylinderMesh(float topRadius) {
    MeshData data;
    constexpr unsigned int slices=24;
    // The side and caps have separate vertices so their rim stays a hard edge.
    for (unsigned int y=0; y<=1; ++y) for (unsigned int x=0; x<=slices; ++x) {
        const float angle = glm::two_pi<float>()*float(x)/slices;
        const float radius = y ? topRadius : 0.5f;
        const glm::vec3 n = glm::normalize(glm::vec3(std::cos(angle),0.5f-topRadius,std::sin(angle)));
        data.vertices.push_back({{radius*std::cos(angle),float(y)-0.5f,radius*std::sin(angle)},n});
    }
    gridIndices(data,0,slices,1);
    for (int cap=0; cap<2; ++cap) {
        const auto first = static_cast<unsigned int>(data.vertices.size());
        const float radius = cap ? topRadius : 0.5f;
        const glm::vec3 n{0,cap ? 1.0f : -1.0f,0};
        data.vertices.push_back({{0,float(cap)-0.5f,0},n});
        for (unsigned int x=0; x<=slices; ++x) {
            const float a = glm::two_pi<float>()*float(x)/slices;
            data.vertices.push_back({{radius*std::cos(a),float(cap)-0.5f,radius*std::sin(a)},n});
        }
        for (unsigned int x=0; x<slices; ++x) triangle(data,first,first+x+1,first+x+2);
    }
    return data;
}

static MeshData curtainMesh() {
    MeshData data;
    constexpr unsigned int columns=96, rows=16;
    const float frequency=glm::two_pi<float>()*8;
    for(unsigned int y=0;y<=rows;++y) for(unsigned int x=0;x<=columns;++x) {
        const float u=float(x)/columns, v=float(y)/rows;
        const float wave=frequency*u;
        const glm::vec3 point{u-0.5f,0.5f-v+0.012f*std::cos(wave)*v*v*v*v,0.05f*std::sin(wave)};
        const glm::vec3 tangentU{1,-0.012f*frequency*std::sin(wave)*v*v*v*v,0.05f*frequency*std::cos(wave)};
        const glm::vec3 tangentV{0,-1+0.048f*std::cos(wave)*v*v*v,0};
        data.vertices.push_back({point,glm::normalize(glm::cross(tangentV,tangentU))});
    }
    gridIndices(data,0,columns,rows);
    return data;
}

Primitives::Primitives() : box(boxMesh(false)), roundedBox(boxMesh(true)), sphere(sphereMesh()),
    cylinder(cylinderMesh(0.5f)), frustum(cylinderMesh(0.28f)), curtain(curtainMesh()) {}

glm::mat4 transform(const glm::mat4& parent, glm::vec3 position, glm::vec3 size, float angle, glm::vec3 axis) {
    // Parent * translation * rotation * scale: local parts move with their object.
    auto model = glm::translate(parent,position);
    if (angle != 0) model = glm::rotate(model,glm::radians(angle),axis);
    return glm::scale(model,size);
}

void drawPart(DrawContext& ctx, const Mesh& mesh, const glm::mat4& model, const Material& material) {
    Shader& shader = material.unlit ? ctx.unlit : ctx.lit;
    shader.use();
    shader.set("model",model);
    shader.set("materialColor",material.color);
    if (!material.unlit) {
        shader.set("materialSpecular",material.specular);
        shader.set("shininess",material.shininess);
    }
    mesh.draw(ctx.flat && !material.unlit);
    ++ctx.drawCalls;
}

void drawBox(DrawContext& c, const glm::mat4& p, glm::vec3 pos, glm::vec3 size, const Material& m, float a, glm::vec3 axis) {
    drawPart(c,c.shapes.box,transform(p,pos,size,a,axis),m);
}
void drawRoundedBox(DrawContext& c, const glm::mat4& p, glm::vec3 pos, glm::vec3 size, const Material& m, float a, glm::vec3 axis) {
    drawPart(c,c.shapes.roundedBox,transform(p,pos,size,a,axis),m);
}
void drawSphere(DrawContext& c, const glm::mat4& p, glm::vec3 pos, glm::vec3 size, const Material& m, float a, glm::vec3 axis) {
    drawPart(c,c.shapes.sphere,transform(p,pos,size,a,axis),m);
}
void drawCylinder(DrawContext& c, const glm::mat4& p, glm::vec3 pos, glm::vec3 size, const Material& m, float a, glm::vec3 axis) {
    drawPart(c,c.shapes.cylinder,transform(p,pos,size,a,axis),m);
}
void drawFrustum(DrawContext& c, const glm::mat4& p, glm::vec3 pos, glm::vec3 size, const Material& m, float a, glm::vec3 axis) {
    drawPart(c,c.shapes.frustum,transform(p,pos,size,a,axis),m);
}
void drawRod(DrawContext& c, const glm::mat4& p, glm::vec3 start, glm::vec3 end, float diameter, const Material& m) {
    const auto direction = end-start;
    const float length = glm::length(direction);
    if (length<0.0001f) return;
    const auto n = direction/length;
    auto axis = glm::cross(glm::vec3(0,1,0),n);
    if (glm::length(axis)<0.001f) axis = {1,0,0};
    else axis = glm::normalize(axis);
    const float angle = glm::degrees(std::acos(std::clamp(n.y,-1.0f,1.0f)));
    drawCylinder(c,p,(start+end)*0.5f,{diameter,length,diameter},m,angle,axis);
}
