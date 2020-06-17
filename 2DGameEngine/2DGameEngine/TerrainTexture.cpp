#include "TerrainTexture.h"

TerrainTexture::TerrainTexture() {
	_collisionMap = nullptr;
}

TerrainTexture::~TerrainTexture() {
	delete[] _collisionMap;
}

void TerrainTexture::generateCollMap() {
	if (_texData && _nrChan == 4) {
		_collisionMap = new bool[_width * _height];

		for (int i = 0; i < _width * _height; i++) {
			int pos = (i * 4) + 3;
			GLubyte alpha = _texData[pos];			//Get the alpha component of the ith pixel
			_collisionMap[i] = (alpha != 0x00);		//Set the mask boolean value to 1 if alpha is not 0
		}
	}
}

void TerrainTexture::setPixels(int x, int y, int w, int h, float alpha) {
	if (_validatePosition(x, y, w, h)) {
		//Hardcoded
		GLubyte rComp = 212;
		GLubyte gComp = 255;
		GLubyte bComp = 0;
		GLubyte aComp = (int)255 * alpha;

		std::vector<GLubyte> pixelBytes;
		for (int i = 0; i < (w * h); i++) {
			pixelBytes.push_back(rComp);
			pixelBytes.push_back(gComp);
			pixelBytes.push_back(bComp);
			pixelBytes.push_back(aComp);
		}

		_setPixels(x, y, w, h, &pixelBytes[0]);
		_setCollisionMap(x, y, w, h, (alpha != 0.0f));
	}
}

bool TerrainTexture::getCollAt(int x, int y) {
	if (x < _width && y < _height) {
		return _collisionMap[index(x, y)];
	} else {
		return false;
	}
}

bool TerrainTexture::_validatePosition(int x, int y, int w, int h) {
	//Checks if the positions of a potential edit are within the boundaries of the texture
	return (x + w) < _width && (y + h) < _height;
}

void TerrainTexture::_setPixels(int x, int y, int w, int h, GLubyte* pixelBytes) {
	glBindTexture(GL_TEXTURE_2D, _texID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixelBytes);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TerrainTexture::_setCollisionMap(int x, int y, int w, int h, GLboolean value) {
	if (_validatePosition(x, y, w, h)) {
		for (int i = 0; i < w; i++) {
			for (int j = 0; j < h; j++) {
				int pixelLocation = (_width * (y + j)) + (x + i);
				//Prevents crashes when x and y are outside the boundaries of the terrain game object
				if (pixelLocation < _width * _height && pixelLocation >= 0) {
					_collisionMap[pixelLocation] = value;
				}
			}
		}
	}
}
