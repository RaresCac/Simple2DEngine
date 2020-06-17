#include "Texture2D.h"
#include <iomanip>

Texture2D::Texture2D() {
	_texID = 0;
	_texData = NULL;
	
	_width = 0;
	_height = 0;
	_nrChan = 0;
}

Texture2D::~Texture2D() {
	//Deletes the texture associated with this object
	glDeleteTextures(1, &_texID);
}

bool Texture2D::loadImage(std::string imgPath) {
	//Get the extension of the image
	int dot = imgPath.find_last_of(".");
	_ext = imgPath.substr(dot+1, imgPath.length() - 1); 


	//Sets the desired channels for jpg or png textures
	int typeSTBI;
	if (_ext == "png") {
		typeSTBI = STBI_rgb_alpha;
	} else {
		typeSTBI = STBI_rgb;
	}

	//Loads an image into the _texData byte array
	//Data in _texData contains 4 bytes per pixel (rgba). Images are loaded bottom to top, left to right
	stbi_set_flip_vertically_on_load(true);
	_texData = stbi_load(imgPath.c_str(), &_width, &_height, &_nrChan, typeSTBI);

	_path = imgPath;
	return true;
}

void Texture2D::generateTexture() {
	glGenTextures(1, &_texID);
	glBindTexture(GL_TEXTURE_2D, _texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//Generate the appropiate texture with or without Alpha channel
	if (_nrChan == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _texData);
	} else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, _texData);
	}
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::freeTexData() {
	stbi_image_free(_texData);
}

void Texture2D::bind() {
	glBindTexture(GL_TEXTURE_2D, _texID);
}

int Texture2D::index(int x, int y, int w, int h) {
	//W and H represents the current width if index is used with a object with width and height
	//and not a simple position.
	//Useful for getting pixelLocation in _setCollisionMap (TextureTerrain) - UNUSED
	return ((y + h) * _width) + (x + h);
}

void Texture2D::setName(std::string name) {
	_name = name;
}

bool* Texture2D::getCollMap() {
	return nullptr;
}

void Texture2D::setPixels(int x, int y, int w, int h, float alpha) {
}

bool Texture2D::getCollAt(int x, int y) {
	return false;
}
