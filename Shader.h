#pragma once

#include <string>
#include <unordered_map>
#include "glm/glm/glm.hpp"

struct ShaderProgramSource {
	std::string vertex_source;
	std::string fragment_source;
};

class Shader {

private:
	unsigned int m_rendererId;
	std::string m_filepath;
	std::unordered_map<std::string, int> m_uniformlocationCache;

public:
	Shader(const std::string& filename);
	~Shader();
	void bind() const;
	void unbind() const;

	void setUniform1i(const std::string& name, int value);
	void setUniform1iv(const std::string& name, int count, const int value[3]);

	void setUniform1f(const std::string& name, float value);
	void setUniform2f(const std::string& name, float v0, float v1);
	void setUniform3f(const std::string& name, float v0, float v1, float v2);
	void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);

	void setUniformMat2f(const std::string& name, const glm::mat2& matrix);
	void setUniformMat3f(const std::string& name, const glm::mat3& matrix);
	void setUniformMat4f(const std::string& name, const glm::mat4& matrix);

private:
	unsigned int compileShader(unsigned int type, const std::string& source);
	unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);
	ShaderProgramSource parseShader(const std::string& filepath);
	int getUniformLocation(const std::string name);
};