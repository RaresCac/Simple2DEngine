#pragma once
#include <iostream>
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Texture2D {
public:
	Texture2D();
	~Texture2D();

	bool loadImage(std::string);
	void generateTexture();
	void freeTexData();
	void bind();
	int index(int x, int y, int w = 0, int h = 0);

	void setName(std::string);

	int getWidth() { return _width; }
	int getHeight() { return _height; }
	std::string getName() { return _name; }
	std::string getPath() { return _path; }

	//Virtual functions to allow dynamic_cast in CollisionCalculator
	virtual bool* getCollMap();
	virtual void setPixels(int x, int y, int w, int h, float alpha);
	virtual bool getCollAt(int x, int y);

protected:
	GLuint _texID;
	GLubyte* _texData = nullptr;
	int _height;
	int _width;
	int _nrChan;	
	
private:
	std::string _name;
	std::string _path;
	std::string _ext;
};

