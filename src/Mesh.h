#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex { glm::vec3 position; glm::vec3 normal; };
struct MeshData { std::vector<Vertex> vertices; std::vector<unsigned int> indices; };

class Mesh {
public:
    explicit Mesh(const MeshData& data);
    ~Mesh();
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    void draw(bool flat) const;
    static unsigned int uploads;
private:
    GLuint vao = 0, vbo = 0, ebo = 0;
    GLuint flatVao = 0, flatVbo = 0;
    GLsizei indexCount = 0, flatCount = 0;
};
