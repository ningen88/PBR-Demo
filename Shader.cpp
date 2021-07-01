#include "Shader.h"

#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

Shader::Shader(const std::string& filename) : m_filepath(filename), m_rendererId(0) {
    ShaderProgramSource source = parseShader(filename);
    m_rendererId = createShader(source.vertex_source, source.fragment_source);
}

Shader::~Shader(){
    glDeleteProgram(m_rendererId);
}

void Shader::bind() const {
    glUseProgram(m_rendererId);
}

void Shader::unbind() const {
    glUseProgram(0);
}

int Shader::getUniformLocation(const std::string name) {
    if (m_uniformlocationCache.find(name) != m_uniformlocationCache.end()) {
        return m_uniformlocationCache[name]; 
    }

    int location = glGetUniformLocation(m_rendererId, name.c_str());
    if (location == -1) {
        std::cout << "Warning uniform " << name << " doesn't exist!" << std::endl;
    } 

    m_uniformlocationCache[name] = location;
    return location;
}

ShaderProgramSource Shader::parseShader(const std::string& filepath) {

    std::ifstream stream(filepath);

    enum class ShaderType {

        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line)) {

        if (line.find("#shader") != std::string::npos) {

            if (line.find("vertex") != std::string::npos) {

                type = ShaderType::VERTEX;
            }

            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;

            }

        }
        else {
            ss[(int)type] << line << '\n';
        }

    }
    return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source) {

    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();                                // is like &source[0];
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);                                           // compile the shader identify by id

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);            //ERROR HANDLING... iv stands for integer vector

    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);

        std::cout << "Failed to compile Shader: " << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader) {

    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);                    //gli shader sono ormai "caricati" in program possiamo eliminarli.

    return program;
}

void Shader::setUniform1i(const std::string& name, int value) {
    int location = getUniformLocation(name);
    glUniform1i(location, value);

}

void Shader::setUniform1iv(const std::string& name, int count, const int value[3]) {
    int location = getUniformLocation(name);
    glUniform1iv(location, count, &value[0]);
}

void Shader::setUniform1f(const std::string& name, float value) {
    int location = getUniformLocation(name);
    glUniform1f(location, value);
}

void Shader::setUniform2f(const std::string& name, float v0, float v1) {
    int location = getUniformLocation(name);
    glUniform2f(location, v0, v1);
}

void Shader::setUniform3f(const std::string& name, float v0, float v1, float v2) {
    int location = getUniformLocation(name);
    glUniform3f(location, v0, v1, v2);
}

void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
    int location = getUniformLocation(name);
    glUniform4f(location, v0, v1, v2, v3);
}

void Shader::setUniformMat2f(const std::string& name, const glm::mat2& matrix) {
    int location = getUniformLocation(name);
    glUniformMatrix2fv(location, 1, GL_FALSE, &matrix[0][0]);
}

void Shader::setUniformMat3f(const std::string& name, const glm::mat3& matrix) {
    int location = getUniformLocation(name);
    glUniformMatrix3fv(location, 1, GL_FALSE, &matrix[0][0]);
}

void Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix) {
    int location = getUniformLocation(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
}