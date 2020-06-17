#include "TerrainManager.h"
#include <sstream>
#include <iostream>
#include <fstream>

TerrainManager::TerrainManager() {
}

TerrainManager::~TerrainManager() {
}

TerrainTexture* TerrainManager::loadNewTerrain(std::string texPath) {
	std::cout << "Loading terrain " << texPath << std::endl;
	TerrainTexture* terrain = new TerrainTexture();
	if (!terrain->loadImage(texPath)) {
		std::cout << "Error loading image" << std::endl;
		return nullptr;
	}

	terrain->generateTexture();
	terrain->generateCollMap();
	terrain->freeTexData();
	_terrainList.push_back(terrain);

	std::string name = _getFileName(texPath);

	terrain->setName(name);
	_terrainNames.push_back(name);
	return terrain;
}

void TerrainManager::clearTerrainVector() {
	for (TerrainTexture* tt : _terrainList) {
		delete tt;
	}
	_terrainList.clear();
	_terrainNames.clear();
}

void TerrainManager::setTerrainPixels(int x, int y, int w, int h, float alpha, TerrainTexture* terrain) {
	terrain->setPixels(x, y, w, h, alpha);
}

void TerrainManager::drawCircle(int x, int y, int r, float alpha, TerrainTexture* terrain) {
	//Based on answer by user Daniel Earwicker at https://stackoverflow.com/a/1201227
	for (int dx = -r; dx < r; dx++) {
		int height = static_cast<int>(sqrtf(r * r - dx * dx));
		for (int dy = -height; dy < height; dy++) {
			terrain->setPixels(dx + x, dy + y, 1, 1, alpha);
		}
	}
}

bool TerrainManager::getHeightDifference(int x, int y, TerrainTexture* terrain, int* diff) {
	bool* collMap = terrain->getCollMap();
	int width = terrain->getWidth();
	int height = terrain->getHeight();

	//If the object is outside the terrain texture
	if (x > width || y > height || x < 0 || y < 0) {
		return false;
	}

	int pos = terrain->index(x, y);

	bool found = false;
	while (pos > 0 && !found && pos < width * height) {	//Check if it goes outside the terrain
		if (collMap[pos]) {
			found = true;
		} else {
			//Go to the y - 1 
			pos -= width;
			(*diff)++;
		}
	}

	return found;
}

bool TerrainManager::getCollMapAt(int x, int y, TerrainTexture* terrain) {
	return terrain->getCollAt(x, y);
}

bool TerrainManager::isPosInTerrain(int x, int y, TerrainTexture* terrain) {
	return (x > 0 && x < terrain->getWidth() && y > 0 && y < terrain->getHeight());
}

bool* TerrainManager::getCollMap() {
	return _terrainList.at(0)->getCollMap();
}

int TerrainManager::getTerrainIndex(Texture2D* terrain) {
	int index = 0;
	bool found = false;
	for (TerrainTexture* tt : _terrainList) {
		if (tt == terrain) {
			found = true;
			return index;
		}
		index++;
	}
	return -1;
}

TerrainTexture* TerrainManager::getTerrain(int index) {
	if (index >= _terrainList.size()) {
		return nullptr;
	}
	return _terrainList[index];
}

std::string TerrainManager::_getFileName(const std::string& s) {
	//Returns the filename of the passed absolute path
	//Based on https://stackoverflow.com/a/46931770
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string item;
	char delim = '\\';

	while (std::getline(ss, item, delim)) {
		result.push_back(item);
	}

	return result[result.size() - 1];
}
