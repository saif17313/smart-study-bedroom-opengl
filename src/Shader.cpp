#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

static GLuint compileShader(const std::filesystem::path& path, GLenum type) {
    std::ifstream file(path);
    if (!file) throw std::runtime_error("Cannot open shader: " + path.string());
    std::ostringstream stream;
    stream << file.rdbuf();
    const std::string source = stream.str();
    const char* text = source.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &text, nullptr);
    glCompileShader(shader);
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(static_cast<size_t>(length) + 1);
        glGetShaderInfoLog(shader, length, nullptr, log.data());
        glDeleteShader(shader);
        throw std::runtime_error(path.string() + ":\n" + log.data());
    }
    return shader;
}

Shader::Shader(const std::filesystem::path& vertex, const std::filesystem::path& fragment) {
    const GLuint vs = compileShader(vertex, GL_VERTEX_SHADER);
    GLuint fs = 0;
    try { fs = compileShader(fragment, GL_FRAGMENT_SHADER); }
    catch (...) { glDeleteShader(vs); throw; }
    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);
    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(static_cast<size_t>(length) + 1);
        glGetProgramInfoLog(program, length, nullptr, log.data());
        glDeleteProgram(program);
        throw std::runtime_error("Shader link failed (" + vertex.string() + "): " + log.data());
    }
}

Shader::~Shader() { glDeleteProgram(program); }
void Shader::use() const { glUseProgram(program); }
GLint Shader::location(const std::string& name) {
    auto found = locations.find(name);
    if (found != locations.end()) return found->second;
    const GLint value = glGetUniformLocation(program, name.c_str());
    locations.emplace(name, value);
    return value;
}
void Shader::set(const std::string& name, float value) { glUniform1f(location(name), value); }
void Shader::set(const std::string& name, const glm::vec3& value) {
    glUniform3fv(location(name), 1, glm::value_ptr(value));
}
void Shader::set(const std::string& name, const glm::mat4& value) {
    glUniformMatrix4fv(location(name), 1, GL_FALSE, glm::value_ptr(value));
}
