#pragma once
#include "ShaderLoader.h"
#include "GameObject.h"
#include "glm/ext.hpp"

class ObjectRenderer {
public:	
	ObjectRenderer(ShaderLoader*, int, int);
	~ObjectRenderer();

	void init();
	void drawObject(GameObject*);
	void moveCamera(int);
	glm::vec2 getCameraOffset() { return _cameraOffset; }

private:
	ShaderLoader* _shader;
	glm::mat4 _projection;
	glm::mat4 _view;
	glm::vec2 _cameraOffset;	//Distance to initial position
	float _cameraSpeed = 2.0f;	//2 pixels per frame

	//OpenGL rendering properties
	GLuint _vboID;		//Guaranteed to be 32 bits
	GLuint _vaoID;
	GLuint _eboID;
};

