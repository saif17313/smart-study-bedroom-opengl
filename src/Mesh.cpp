#include "Mesh.h"
#include <cstddef>
#include <cmath>
#include <stdexcept>

unsigned int Mesh::uploads = 0;

static void vertexLayout() {
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
}

Mesh::Mesh(const MeshData& data) {
    if (data.indices.empty() || data.indices.size()%3 != 0) throw std::runtime_error("Mesh requires triangles");
    std::vector<Vertex> flatVertices;
    flatVertices.reserve(data.indices.size());
    for (size_t i=0; i<data.indices.size(); i+=3) {
        const Vertex a = data.vertices.at(data.indices[i]);
        const Vertex b = data.vertices.at(data.indices[i+1]);
        const Vertex c = data.vertices.at(data.indices[i+2]);
        glm::vec3 normal = glm::cross(b.position-a.position, c.position-a.position);
        if (glm::length(normal) < 1e-9f) throw std::runtime_error("Degenerate mesh triangle");
        normal = glm::normalize(normal);
        for (const auto& vertex : {a,b,c}) {
            if (!std::isfinite(glm::length(vertex.normal)) || glm::length(vertex.normal)<0.9f)
                throw std::runtime_error("Invalid vertex normal");
            flatVertices.push_back({vertex.position, normal});
        }
    }
    indexCount = static_cast<GLsizei>(data.indices.size());
    flatCount = static_cast<GLsizei>(flatVertices.size());
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.vertices.size()*sizeof(Vertex), data.vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size()*sizeof(unsigned int), data.indices.data(), GL_STATIC_DRAW);
    vertexLayout();
    glGenVertexArrays(1, &flatVao);
    glGenBuffers(1, &flatVbo);
    glBindVertexArray(flatVao);
    glBindBuffer(GL_ARRAY_BUFFER, flatVbo);
    glBufferData(GL_ARRAY_BUFFER, flatVertices.size()*sizeof(Vertex), flatVertices.data(), GL_STATIC_DRAW);
    vertexLayout();
    glBindVertexArray(0);
    ++uploads;
}
Mesh::~Mesh() {
    glDeleteVertexArrays(1, &vao);
    glDeleteVertexArrays(1, &flatVao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &flatVbo);
}
void Mesh::draw(bool flat) const {
    glBindVertexArray(flat ? flatVao : vao);
    if (flat) glDrawArrays(GL_TRIANGLES, 0, flatCount);
    else glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}
