#include "ShaderLoader.h"
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

ShaderLoader::ShaderLoader() {
	_vertexShadID = 0;
	_fragShadID = 0;
	_programID = 0;
	_numAttrib = 0;
}

ShaderLoader::~ShaderLoader() {
	glDeleteProgram(_programID);
}

void ShaderLoader::compileShaders(std::string vertexFilePath, std::string fragmentFilePath) {
	//Program for the shader
	_programID = glCreateProgram();
	//Vertex shader
	_vertexShadID = glCreateShader(GL_VERTEX_SHADER);
	//Error checking

	if (_vertexShadID == 0) 
		error("Vertex shader not created");
	
	std::ifstream vertexFile(vertexFilePath);
	if (vertexFile.fail()) 
		error("Vertex shader file not found");
	
	std::string fileContents = "";
	std::string line;
	while (std::getline(vertexFile, line)) 
		fileContents += line + "\n";
	
	vertexFile.close();
	
	const char* contentsPtr = fileContents.c_str();
	glShaderSource(_vertexShadID, 1, &contentsPtr, nullptr);
	glCompileShader(_vertexShadID);

	//Check status of compilation
	int success;
	char infoLog[512];
	glGetShaderiv(_vertexShadID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(_vertexShadID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	_fragShadID = glCreateShader(GL_FRAGMENT_SHADER);
	if (_fragShadID == 0) 
		error("Fragment shader not created");
	
	std::ifstream fragmentFile(fragmentFilePath);
	if (fragmentFile.fail()) 
		error("Fragment shader file not found");
	
	fileContents = "";
	line = "";
	while (std::getline(fragmentFile, line)) 
		fileContents += line + "\n";
	
	fragmentFile.close();

	contentsPtr = fileContents.c_str();
	glShaderSource(_fragShadID, 1, &contentsPtr, nullptr);
	glCompileShader(_fragShadID);

	glGetShaderiv(_fragShadID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(_fragShadID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}

void ShaderLoader::addAttribute(std::string attribName) {
	glBindAttribLocation(_programID, _numAttrib++, attribName.c_str());
}

void ShaderLoader::linkShaders() {
	glAttachShader(_programID, _vertexShadID);
	glAttachShader(_programID, _fragShadID);
	glLinkProgram(_programID);

	int success;
	char infoLog[512];
	glGetProgramiv(_programID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(_programID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDetachShader(_programID, _vertexShadID);
	glDetachShader(_programID, _fragShadID);
	glDeleteShader(_vertexShadID);
	glDeleteShader(_fragShadID);
}

void ShaderLoader::useProgram() {
	glUseProgram(_programID);
}

void ShaderLoader::unuseProgram() {
	glUseProgram(0);
}

int ShaderLoader::getUniformLocation(std::string uniformName) {
	return glGetUniformLocation(_programID, uniformName.c_str());
}

void ShaderLoader::setBoolean(const GLchar* name, const GLfloat& value) {
	//value will get converted to bool by the shader, zero values will be false, else will be true
	glUniform1f(glGetUniformLocation(_programID, name), value);
}

void ShaderLoader::setVector2f(const GLchar* name, const glm::vec2& value) {
	glUniform2f(glGetUniformLocation(_programID, name), value.x, value.y);
}

void ShaderLoader::setVector3f(const GLchar* name, const glm::vec3& value) {
	glUniform3f(glGetUniformLocation(_programID, name), value.x, value.y, value.z);
}

void ShaderLoader::setMat4(const GLchar* name, const glm::mat4& mat) {
	glUniformMatrix4fv(glGetUniformLocation(_programID, name), 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderLoader::error(std::string error) {
	std::cout << error << std::endl;
}
