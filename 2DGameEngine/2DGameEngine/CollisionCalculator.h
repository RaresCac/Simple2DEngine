#pragma once
#include "GameObject.h"
#include "TerrainTexture.h"
#include "TerrainManager.h"
#include "Directions.h"

class GameObjectManager;

class CollisionCalculator {
public:
	CollisionCalculator(TerrainManager*, GameObjectManager*);
	~CollisionCalculator();

	static float sweptAABB(GameObject* go1, GameObject* go2, float& normalX, float& normalY);
	static bool AABB(GameObject*, GameObject*);

	void terrainCollision(GameObject*, GameObject* terrainObject);
	bool checkCeiling(GameObject*, GameObject* terrainObject);
	void performAction(GameObject*, GameObject* terrainObject);

private:
	TerrainManager* _terrainManager;
	GameObjectManager* _gameObjectManager;
	int _actionI = 0;

};

