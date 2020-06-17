#include "GameObjectManager.h"
#include "CollisionCalculator.h"
#include "Directions.h"
#include "Game.h"
#include <iostream>

GameObjectManager::GameObjectManager(ObjectRenderer* renderer, TextureManager* textureM, TerrainManager* terrainM) {
	_objectRenderer = renderer;
	_textureManager = textureM;
	_terrainManager = terrainM;
	_collisionCalculator = new CollisionCalculator(_terrainManager, this);

	Texture2D* tex = _textureManager->loadNewTexture("textures\\cardboard.jpg");
	TerrainTexture* terr = _terrainManager->loadNewTerrain("textures\\terraintest.png");

	addGameObject(600, 350, 20, 20);
	addGameObject(400, 400, 25, 25);
	addGameObject(500, 450, 20, 20);
	addTerrainObject(0, 0, terr);
}

GameObjectManager::~GameObjectManager() {
	delete _collisionCalculator;
}

void GameObjectManager::addGameObject(int x, int y, int w, int h, Texture2D* texture) {
	GameObject * go = new GameObject();
	std::string name = "gameObject" + std::to_string(_gameObjects.size());
	go->setName(name);
	_gameObjectsNames.push_back(name);

	go->init(glm::vec2(x, y), glm::vec2(w, h), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f) /*Colour*/);
	if (texture) 
		go->setTexture(texture);
	_gameObjects.push_back(go);
}

void GameObjectManager::addGameObject(GameObject* go) {
	_gameObjectsNames.push_back(go->getName());
	_gameObjects.push_back(go);
}

void GameObjectManager::addTerrainObject(int x, int y, TerrainTexture* terrain) {
	GameObject* go = new GameObject();
	_terrainObjects.push_back(go);
	int w = terrain->getWidth();
	int h = terrain->getHeight();
	go->init(glm::vec2(x, y), glm::vec2(w, h), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f) /*Colour*/);
	go->setTexture(terrain);
}

void GameObjectManager::addTerrainObject(GameObject* go) {
	_terrainObjects.push_back(go);
}

void GameObjectManager::deleteGameObject(int index) {
	_objectsToDelete.push_back(index);
}

void GameObjectManager::deleteGameObject(GameObject* go) {
	deleteGameObject(_getGameObjectIndex(go, _gameObjects));
}

void GameObjectManager::setObjectTexture(int index, Texture2D* texture) {
	if (index >= 0) {
		GameObject* currentObj = _gameObjects.at(index);
		currentObj->setTexture(texture);
	}
}

void GameObjectManager::setTerrainObjectTexture(TerrainTexture* terrain) {
	if (_terrainObjects.size() == 0) {
		addTerrainObject(0, 0, terrain);
	} else {
		GameObject* terrainObject = getTerrainObject();
		int width = terrain->getWidth();
		int height = terrain->getHeight();

		terrainObject->setSize(glm::vec2(width, height));
		terrainObject->setTexture(terrain);
	}
}

void GameObjectManager::stopObjMovement(int index, int direction) {
	if (index > -1 && index < _gameObjects.size()) {
		GameObject* go = _gameObjects[index];
		go->setMovingDir(direction, false);
	}
}

void GameObjectManager::startObjMovement(int index, int direction) {
	if (index > -1 && index < _gameObjects.size()) {
		GameObject* go = _gameObjects[index];
		go->setMovingDir(direction, true);
	}
}

int GameObjectManager::_getGameObjectIndex(GameObject* go, std::vector<GameObject*>& vector) {
	//Iterates through the vector
	//Returns -1 if object not found, its index if found
	int i = 0;
	for (GameObject* currentGO : vector) {
		if (currentGO == go) {
			return i;
		}
		i++;
	}
	return -1;
}

bool GameObjectManager::_actionConstraints(GameObject* go) {
	//UNUSED
	//Check if the current action can be performed
	bool result = false;

	if (go->getAction() == GameObjectActions::ACT_STAIRS) {
		if (!go->isPerformingAction()) {
			result = true;
		} else {
			glm::vec2 currentPos = go->getPosition();
			glm::vec2 lastActionPos = go->getLastActionPos();
			int xDiff = abs(currentPos.x - lastActionPos.x);
			result = xDiff >= go->getBuildWidth();
		}
	}

	return result;
}

void GameObjectManager::explodeGameObject(int index) {
	GameObject* go = _gameObjects[index];
	int x = go->getCollAnchorX();
	int y = go->getCollAnchorY();
	int explosionRadius = std::max(go->getWidth(), go->getHeight());

	TerrainTexture* terrain = dynamic_cast<TerrainTexture*>(_terrainObjects[0]->getTexture());
	_terrainManager->drawCircle(x, y, explosionRadius, 0.0f, terrain);
	removeBlockingObject(go);
	deleteGameObject(index);
}

void GameObjectManager::generateGameObject(GameObject* generator) {
	int width = 15;
	int height = width;

	int x = generator->getCollAnchorX() - (width/2);	//Adjust to middle of generated object
	int y = generator->getCollAnchorY();

	GameObject* go = new GameObject();
	std::string name = "gameObject" + std::to_string(_gameObjects.size());

	go->setName(name);
	go->init(glm::vec2(x, y), glm::vec2(width, height), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f) /*Colour*/);
	go->setCanMove(true);
	go->setAutoMoveDir(RIGHT, true);
	go->setHasTerrainCollision(true);
	go->setHasCollision(true);

	_gameObjectsNames.push_back(name);
	_generatedObjects.push_back(go);
}

void GameObjectManager::reverseAutoDirection(GameObject* go) {
	int dir = go->getCurrentAutoMoveDir();
	go->stopAutoMove();

	if (dir != -1)
		go->setAutoMoveDir(oppositeDirection(dir), true);
	
	go->setRecentlyReversedDir(true);
}

int GameObjectManager::getGameObjectAtPos(int x, int y) {
	int index = 0;
	for (GameObject * go : _gameObjects) {
		glm::vec2 size = go->getSize();
		glm::vec2 position = go->getPosition();
		if (x < position.x + size.x && x > position.x&&
			y < position.y + size.x && y > position.y) {
			return index;
		}
		index++;
	}
	return -1;
}


void GameObjectManager::setBlockingObject(int index) {
	GameObject* go = _gameObjects[index];
	setBlockingObject(go);
}

void GameObjectManager::setBlockingObject(GameObject* go) {
	go->setIsBlocker(true);
	int index = _getGameObjectIndex(go, _blockingObjects);
	if (index == -1) _blockingObjects.push_back(go); //Add object if not found
	
}

void GameObjectManager::removeBlockingObject(int index) {
	GameObject* go = _gameObjects[index];
	removeBlockingObject(go);
}

void GameObjectManager::removeBlockingObject(GameObject* go) {
	go->setIsBlocker(false);
	int index = _getGameObjectIndex(go, _blockingObjects);
	if (index >= 0) _blockingObjects.erase(_blockingObjects.begin() + index);
}

void GameObjectManager::objDrawRoutine() {
	//Will perform movement of objects, collision checking and then drawing them on the screen
	//Loop performed per frame

	//First render the terrain
	for (GameObject* tGo : _terrainObjects) {
		_objectRenderer->drawObject(tGo);
	}

	int i = 0;			//Keep track of index of current GameObject

	for (GameObject* go : _gameObjects) {
		if (!go->isBlocker()) {	//Paused check here so objects are still rendered
			if (go->canMove()) go->moveObject();

			if (!Game::isPaused) {
				if (_terrainObjects.size() > 0) {
					_collisionCalculator->performAction(go, _terrainObjects[0]);

					if (go->hasTerrainCollision())
						_collisionCalculator->terrainCollision(go, _terrainObjects[0]);
				}

				//Blocker collision 
				if (go->hasCollision()) {
					for (GameObject* block : _blockingObjects) {
						if (_collisionCalculator->AABB(go, block)) {
							if (block->isExit()) {
								deleteGameObject(i);		//todo: add score
							} else {
								if (go->isAutoMoving()) {
									reverseAutoDirection(go);
								} else {
									go->returnToPrevPos();		//Block the object from going forward
								}
							}
						}
					}
				}
			}

		}
		_objectRenderer->drawObject(go);
		i++;
	}

	//Delete all objects that need to be deleted
	if (_objectsToDelete.size() > 0) {
		//Sort indexes in descending order
		std::sort(_objectsToDelete.rbegin(), _objectsToDelete.rend());
		for (int i : _objectsToDelete) {
			GameObject* go = _gameObjects[i];
			if (go->isBlocker()) {
				removeBlockingObject(go);
			}
			delete go;
			_gameObjects.erase(_gameObjects.begin() + i);
		}
		_objectsToDelete.clear();
	}

	//Add all generated objects to _gameObjects
	if (_generatedObjects.size() > 0) {
		_gameObjects.insert(_gameObjects.end(), _generatedObjects.begin(), _generatedObjects.end());
		_generatedObjects.clear();
	}
}

void GameObjectManager::clearVectors() {
	for (GameObject* go : _gameObjects) {
		delete go;
	}
	_gameObjects.clear();
	_blockingObjects.clear();

	for (GameObject* go : _generatedObjects) {
		delete go;
	}

	_generatedObjects.clear();
	_objectsToDelete.clear();
	_gameObjectsNames.clear();

	for (GameObject* go : _terrainObjects) {
		delete go;
	}
	_terrainObjects.clear();
}

