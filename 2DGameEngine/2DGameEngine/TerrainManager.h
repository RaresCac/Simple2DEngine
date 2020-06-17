#pragma once
#include "TerrainTexture.h"

//Stores pointers to the different Terrain textures loaded
class TerrainManager {

public:
	TerrainManager();
	~TerrainManager();

	TerrainTexture* loadNewTerrain(std::string);
	void clearTerrainVector();

	void setTerrainPixels(int x, int y, int w, int h, float alpha, TerrainTexture* terrain);
	void drawCircle(int x, int y, int r, float alpha, TerrainTexture* terrain);

	bool getHeightDifference(int x, int y, TerrainTexture* terrain, int* diff);
	bool getCollMapAt(int x, int y, TerrainTexture* terrain);
	bool isPosInTerrain(int x, int y, TerrainTexture* terrain);
	bool* getCollMap();
	int getTerrainIndex(Texture2D*);
	TerrainTexture* getTerrain(int index);
	std::vector<TerrainTexture*> getTerrainList() { return _terrainList; }
	std::vector<std::string> getTerrainNames() { return _terrainNames; }
	
private:
	std::string _getFileName(const std::string& s);

	std::vector<TerrainTexture*> _terrainList;
	std::vector<std::string> _terrainNames;			//Used for displaying in imGUI

};

