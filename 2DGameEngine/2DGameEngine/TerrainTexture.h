#pragma once
#include "Texture2D.h"
#include <string>
#include <vector>

class TerrainTexture : public Texture2D {

public:
	TerrainTexture();
	~TerrainTexture();

	void generateCollMap();
	void setPixels(int x, int y, int w, int h, float alpha);

	bool getCollAt(int x, int y);
	bool* getCollMap() { return _collisionMap; }

private:
	bool _validatePosition(int x, int y, int w, int h);
	void _setPixels(int x, int y, int w, int h, GLubyte* pixelBytes);
	void _setCollisionMap(int x, int y, int w, int h, GLboolean value);

	bool* _collisionMap;	//Loaded left to right, bottom to top
};

