#pragma once
#include "Texture2D.h"
#include <string>
#include <vector>

class Terrain : public Texture2D {

public:
	Terrain();
	~Terrain();

	void generateCollMap();
	void setPixels(int x, int y, int w, int h, float alpha);
	int index(int x, int y, int w = 0, int h = 0);

	bool getCollAt(int x, int y);
	bool* getCollMap() { return _collisionMap; }

private:
	bool _validatePosition(int x, int y, int w, int h);
	void _setPixels(int x, int y, int w, int h, GLubyte* pixelBytes);
	void _setCollisionMap(int x, int y, int w, int h, GLboolean value);

	bool* _collisionMap;	//Loaded left to right, bottom to top
};

