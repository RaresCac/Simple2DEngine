#include "TextureManager.h"
#include <iostream>

TextureManager::TextureManager() {
}

TextureManager::~TextureManager() {
	for (Texture2D* t : _textureList) {
		delete t;
	}
}

Texture2D* TextureManager::loadNewTexture(std::string texPath) {
	std::cout << "Loading " << texPath << std::endl;
	Texture2D* texture = new Texture2D();
	if (!texture->loadImage(texPath)) {
		std::cout << "Error loading image" << std::endl;
		return nullptr;
	}

	texture->generateTexture();
	texture->freeTexData();

	_textureList.push_back(texture);

	std::string name = "texture" + std::to_string(_textureList.size());

	texture->setName(name);
	_textureNames.push_back(name);
	return texture;
}

void TextureManager::clearTextureVector() {
	for (Texture2D* t : _textureList) {
		delete t;
	}
	_textureList.clear();
	_textureNames.clear();
}

Texture2D* TextureManager::getTexture(int i) {
	if (i >= _textureList.size()) {
		return nullptr;
	}
	return _textureList[i];
}

int TextureManager::getTextureIndex(Texture2D* texture) {
	int index = 0;
	bool found = false;
	for (Texture2D* tex : _textureList) {
		if (texture == tex) {
			found = true;
			return index;
		}
		index++;
	}
	return -1;
}

