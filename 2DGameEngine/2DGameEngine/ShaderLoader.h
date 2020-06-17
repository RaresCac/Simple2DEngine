#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>

class ShaderLoader {
public:
	ShaderLoader();
	~ShaderLoader();

	void compileShaders(std::string vertexFile, std::string fragmentFile);
	void addAttribute(std::string attribName);
	void linkShaders();
	void useProgram();
	void unuseProgram();
	int getUniformLocation(std::string);

	void setBoolean(const GLchar*, const GLfloat&);
	void setVector2f(const GLchar*, const glm::vec2&);
	void setVector3f(const GLchar*, const glm::vec3&);
	void setMat4(const  GLchar*, const glm::mat4&);

private:
	void error(std::string error);

	GLint _programID;
	GLint _vertexShadID;
	GLint _fragShadID;

	GLint _numAttrib;
};

