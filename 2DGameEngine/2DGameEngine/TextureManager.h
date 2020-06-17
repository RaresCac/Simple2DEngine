#pragma once
#include <iostream>
#include "Texture2D.h"
#include "TerrainTexture.h"
#include <vector>

class TextureManager {
public:
	TextureManager();
	~TextureManager();

	Texture2D* loadNewTexture(std::string);
	void clearTextureVector();

	Texture2D* getTexture(int);
	int getTextureIndex(Texture2D*);
	std::vector<std::string> getTextureNames() { return _textureNames; }
	std::vector<Texture2D*> getTextureList() { return _textureList; }

private:
	std::vector<Texture2D*> _textureList;
	std::vector<std::string> _textureNames; //Used for displaying in imGUI
};

