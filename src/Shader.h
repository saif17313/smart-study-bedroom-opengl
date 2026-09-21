#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <filesystem>
#include <string>
#include <unordered_map>

class Shader {
public:
    Shader(const std::filesystem::path& vertex, const std::filesystem::path& fragment);
    ~Shader();
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    void use() const;
    void set(const std::string& name, float value);
    void set(const std::string& name, const glm::vec3& value);
    void set(const std::string& name, const glm::mat4& value);
private:
    GLuint program = 0;
    std::unordered_map<std::string, GLint> locations;
    GLint location(const std::string& name);
};
