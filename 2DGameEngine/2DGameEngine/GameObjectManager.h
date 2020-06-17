#pragma once
#include <memory>
#include "GameObject.h"
#include "CollisionCalculator.h"
#include "ObjectRenderer.h"
#include "TextureManager.h"
#include "TerrainManager.h"

class GameObjectManager {
public:
	GameObjectManager(ObjectRenderer*, TextureManager*, TerrainManager*);
	~GameObjectManager();
	
	void objDrawRoutine();
	void clearVectors();

	void addGameObject(int x, int y, int w, int h, Texture2D* texture = nullptr);
	void addGameObject(GameObject* go);
	void addTerrainObject(int x, int y, TerrainTexture*);
	void addTerrainObject(GameObject* go);

	void deleteGameObject(int index);
	void deleteGameObject(GameObject* go);
	void setBlockingObject(int index);
	void setBlockingObject(GameObject*);
	void removeBlockingObject(int index);
	void removeBlockingObject(GameObject*);

	void stopObjMovement(int, int);
	void startObjMovement(int, int);

	void explodeGameObject(int index);			
	void generateGameObject(GameObject* generator);
	void reverseAutoDirection(GameObject*);	

	int getGameObjectAtPos(int x, int y);
	GameObject* getTerrainObject() { return _terrainObjects[0]; }
	std::vector<GameObject*>* getGameObjectList() { return &_gameObjects; }
	std::vector<GameObject*>* getTerrainObjectList() { return &_terrainObjects; }
	std::vector<std::string> getGameObjectNames() { return _gameObjectsNames; }
	GameObject* getGameObject(int index) { return _gameObjects[index]; }
	glm::vec2 getCameraOffset() { return _objectRenderer->getCameraOffset(); }

	void setObjectTexture(int, Texture2D*);
	void setTerrainObjectTexture(TerrainTexture*);

	bool recentlyRemovedObject = false;

private:
	int _getGameObjectIndex(GameObject* go, std::vector<GameObject*> &vector);
	bool _actionConstraints(GameObject* go);
	int _exitTargetNumber = 20;						//How many objects must get to the exit

	std::vector<GameObject*> _gameObjects;			//Stores all the game objects except terrain
	std::vector<GameObject*> _terrainObjects;		//Game objects that hold a terrain texture and are immobile
	std::vector<GameObject*> _blockingObjects;		//Objects that should have collision

	std::vector<GameObject*> _generatedObjects;		//Temporarily stores objects created by Generators, added to _gameObjects after frame is rendered completely.
	std::vector<int> _objectsToDelete;				//Temporarily stores indexes of objects in _gameObjects that will be deleted at the end of a frame
	std::vector<std::string> _gameObjectsNames;

	CollisionCalculator* _collisionCalculator;
	ObjectRenderer* _objectRenderer;
	TextureManager* _textureManager;
	TerrainManager* _terrainManager;
};

