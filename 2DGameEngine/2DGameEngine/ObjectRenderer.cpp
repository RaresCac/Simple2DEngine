#include "ObjectRenderer.h"
#include <iostream>
#include "Directions.h"

ObjectRenderer::ObjectRenderer(ShaderLoader* shader, int sWidth, int sHeight) {
	_shader = shader;
	//Orthographic projection, for 2D
	_cameraOffset = glm::vec2(0, 0);
	_projection = glm::ortho(0.0f, (float)sWidth, 0.0f, (float)sHeight, 0.0f, 1.0f);
	_view = glm::mat4(1.0f);				//Used for the camera
	_eboID = 0;
	_vboID = 0;
	_vaoID = 0;
}

ObjectRenderer::~ObjectRenderer() {
	//Delete buffers
	glDeleteBuffers(1, &_eboID);
	glDeleteBuffers(1, &_vboID);
	glDeleteVertexArrays(1, &_vaoID);
}

void ObjectRenderer::init() {
	//Generates the various buffers
	if (_vaoID == 0) {
		glGenVertexArrays(1, &_vaoID);
	}

	if (_vboID == 0) {
		glGenBuffers(1, &_vboID);
	}

	if (_eboID == 0) {
		glGenBuffers(1, &_eboID);
	}

	//Local space coordinates
	GLfloat vertexData[] = {
		//Positions		//Texture coord
		//x    y		 x     y
		0.0f, 0.0f,		0.0f, 0.0f,		//Bottom left
		0.0f, 1.0f,		0.0f, 1.0f,		//Top let
		1.0f, 0.0f,		1.0f, 0.0f,		//Bottom right						
		1.0f, 1.0f,		1.0f, 1.0f		//Top right
	};

	GLuint elements[] = {
		0, 1, 2,		//First triangle
		1, 2, 3			//Second triangle, vertices with indices 1 and 2 shared
	};
	
	//VAO will store the configurations so it will only be needed to bind to the VAO at drawing time
	glBindVertexArray(_vaoID);

	//Binds and copies the vertex array data into the VBO
	glBindBuffer(GL_ARRAY_BUFFER, _vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	//Same but for the EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _eboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	//Position for the position attribute, relates to (position = 0) in the vertex shader
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);							//Position attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * (sizeof(GLfloat))));	//Tex coordinate att

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//Sets the bindings to default
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ObjectRenderer::drawObject(GameObject* go) {
	//Sets the uniforms for the object
	_shader->useProgram();
	glm::mat4 model = glm::mat4(1.0f);		//Size of the object
	glm::vec2 position = go->getPosition();
	glm::vec2 size = go->getSize();

	model = glm::translate(model, glm::vec3(position, 0.0f));
	model = glm::scale(model, glm::vec3(size, 1.0f));

	//Matrix multiplication read from right
	glm::mat4 mvp = _projection * _view * model;	

	_shader->setMat4("mvp", mvp);
	_shader->setVector3f("colour", go->getColour());
	Texture2D* texture = go->getTexture();
	if (texture) {
		_shader->setBoolean("hasTexture", 1.0f);
		texture->bind();
	} else {
		_shader->setBoolean("hasTexture", 0.0f);
	}

	glBindVertexArray(_vaoID);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);				//Not really necesarry for current configuration, but better for future proofness if more VAOs are used
	glBindTexture(GL_TEXTURE_2D, 0);	//Unbinds the texture

}

void ObjectRenderer::moveCamera(int direction) {
	switch (direction) {								//  x, y, z
	case UP:	_view = glm::translate(_view, glm::vec3(0.0f, _cameraSpeed, 0.0f));	 _cameraOffset.y -= _cameraSpeed; break;
	case DOWN:	_view = glm::translate(_view, glm::vec3(0.0f, -_cameraSpeed, 0.0f)); _cameraOffset.y += _cameraSpeed; break;
	case LEFT:	_view = glm::translate(_view, glm::vec3(-_cameraSpeed, 0.0f, 0.0f)); _cameraOffset.x += _cameraSpeed; break;
	case RIGHT: _view = glm::translate(_view, glm::vec3(_cameraSpeed, 0.0f, 0.0f));	 _cameraOffset.x -= _cameraSpeed; break;
	} 
}
