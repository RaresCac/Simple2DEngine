#include "CollisionCalculator.h"
#include <limits>
#include <algorithm>
#include <iostream>
#include "GameObjectManager.h"
#include "Game.h"

CollisionCalculator::CollisionCalculator(TerrainManager* tm, GameObjectManager* gm) {
	_terrainManager = tm;
	_gameObjectManager = gm;
}

CollisionCalculator::~CollisionCalculator() {
}

//Based on https://www.gamedev.net/articles/programming/general-and-gameplay-programming/swept-aabb-collision-detection-and-response-r3084/
float CollisionCalculator::sweptAABB(GameObject* go1, GameObject* go2, float& normalx, float& normaly) {
	float xInvEntry, yInvEntry;
	float xInvExit, yInvExit;

	glm::vec2 pos1 = go1->getPosition();
	glm::vec2 size1 = go1->getSize();
	float vx1 = go1->getVelX();
	float vy1 = go1->getVelY();

	glm::vec2 pos2 = go2->getPosition();
	glm::vec2 size2 = go2->getSize();		//X is width, Y is height
	float vx2 = go2->getVelX();
	float vy2 = go2->getVelY();
	
	// find the distance between the objects on the near and far sides for both x and y 
	if (vx1 > 0.0f) {
		xInvEntry = pos2.x - (pos1.x + size1.x);
		xInvExit = (pos2.x + size2.x) - pos1.x;
	} else {
		xInvEntry = (pos2.x + size2.x) - pos1.x;
		xInvExit = pos2.x - (pos1.x + size1.x);
	}

	if (vy1 > 0.0f) {
		yInvEntry = pos2.y - (pos1.y + size1.y);
		yInvExit = (pos2.y + size2.y) - pos1.y;
	} else {
		yInvEntry = (pos2.y + size2.y) - pos1.y;
		yInvExit = pos2.y - (pos1.y + size1.y);
	}

	// find time of collision and time of leaving for each axis (if statement is to prevent divide by zero) 
	float xEntry, yEntry;
	float xExit, yExit;

	if (vx1 == 0.0f) {
		xEntry = -std::numeric_limits<float>::infinity();
		xExit = std::numeric_limits<float>::infinity();
	} else {
		xEntry = xInvEntry / vx1;
		xExit = xInvExit / vx1;
	}

	if (vy1 == 0.0f) {
		yEntry = -std::numeric_limits<float>::infinity();
		yExit = std::numeric_limits<float>::infinity();
	} else {
		yEntry = yInvEntry / vy1;
		yExit = yInvExit / vy1;
	}

	// find the earliest/latest times of collisionfloat 
	float entryTime = std::max(xEntry, yEntry);
	float exitTime = std::min(xExit, yExit);

	// if there was no collision
	if (entryTime > exitTime || xEntry < 0.0f && yEntry < 0.0f || xEntry > 1.0f || yEntry > 1.0f) {
		normalx = 0.0f;
		normaly = 0.0f;
		return 1.0f;
	} else {
		// calculate normal of collided surface
		if (xEntry > yEntry) {
			if (xInvEntry < 0.0f) {
				normalx = 1.0f;
				normaly = 0.0f;
			} else {
				normalx = -1.0f;
				normaly = 0.0f;
			}
		} else {
			if (yInvEntry < 0.0f) {
				normalx = 0.0f;
				normaly = 1.0f;
			} else {
				normalx = 0.0f;
				normaly = -1.0f;
			}
		} // return the time of collisionreturn entryTime; 
		//Adjust position
		go1->adjustPositionSwept(entryTime);
	}
	return entryTime;
}

bool CollisionCalculator::AABB(GameObject* movingObj, GameObject* blocker) {
	//Axis aligned bounding box collision check
	glm::vec2 blockPos = blocker->getPosition();
	glm::vec2 blockSize = blocker->getSize();

	glm::vec2 movingPos = movingObj->getPosition();
	glm::vec2 movingSize = movingObj->getSize();


	bool collisionX = movingPos.x + movingSize.x >= blockPos.x
		&& blockPos.x + blockSize.x >= movingPos.x;
	bool collisionY = movingPos.y + movingSize.y >= blockPos.y
		&& blockPos.y + blockSize.y >= movingPos.y;

	/*
	if (collisionX && collisionY) {
		//Adjust position of movingObject when it collides

		//Distance between the right side of the movingObj and the left side of the collided object
		float xDiffRL = position.x + size.x - object->getPosition().x; 
		//Distance between the left side of the movingObj and the right side of the collided object
		float xDiffLR = object->getPosition().x + object->getSize().x - position.x;
		bool leftSideCollision = xDiffRL < xDiffLR;

		//Distance between the top side of the movingObj and the bottom side of the collided object
		float yDiffTB = position.y + size.y - object->getPosition().y;
		//Distance between the bottom side of the movingObj and the top side of the collided object
		float yDiffBT = object->getPosition().y + object->getSize().y - position.y;
		bool bottomSideCollision = yDiffTB < yDiffBT;

		movingObj->setPosition(position);
		result = true;
	}

	*/
	return collisionX && collisionY;
}

void CollisionCalculator::terrainCollision(GameObject* go, GameObject* terrainObject) {
	glm::vec2 objPos = go->getPosition();
	int xColl = go->getCollAnchorX();
	int yColl = go->getCollAnchorY() + go->getSize().y; //Check top of object, so it can climb

	int heightDiff = -go->getHeight();			//Negative as positive value indicates distance to lower terrain relative to the position of GO
	TerrainTexture* terrain = dynamic_cast<TerrainTexture*>(terrainObject->getTexture());
	bool floorExists = _terrainManager->getHeightDifference(xColl, yColl, terrain, &heightDiff);
	int action = go->getAction();

	if (!floorExists) {
		//Object is outside the terrain
		_gameObjectManager->deleteGameObject(go);
	} else {
		if (go->isFalling()) {
			if (abs(heightDiff) < 2) {
				go->setFalling(false);
				objPos.y = objPos.y - heightDiff;
				go->setPosition(objPos);
			}
		} else {
			if (abs(heightDiff) < go->getHeight()) {
				//If an object is climbing and it can be repositioned, it means that the object can stop climbing 
				if (go->isPerformingAction()) {
					if (action == GameObjectActions::ACT_CLIMB) {
						go->setAction(NO_ACTION);
						go->setIsPerfAction(false);
					} else if (action == GameObjectActions::ACT_STAIRS) {
						//Check if it's building stairs one after another
						if (abs(heightDiff) != go->getBuildHeight() && heightDiff != 0) {
							go->setAction(NO_ACTION);
							go->setIsPerfAction(false);
						}
					}
				}

				//Reposition the object if the difference is smaller than the size of the object
				objPos.y = objPos.y - heightDiff;
				go->setPosition(objPos);
				go->setRecentlyReversedDir(false);
			} else if (heightDiff > 0){
				//If the difference is against a lower terrain it should fall
				go->setFalling(true);
				go->setAction(NO_ACTION);
				go->setRecentlyReversedDir(false);
			} else {
				//Has hit a wall and can't go up
				if (go->getAction() == GameObjectActions::ACT_CLIMB) {
					objPos.y += 1;
					objPos.x -= go->getVelX();
					go->setPosition(objPos);
					go->setIsPerfAction(true);
					go->setRecentlyReversedDir(false);

					//Check if the object hits against terrain above it
					if (checkCeiling(go, terrainObject)) {
						go->setAction(NO_ACTION);
						go->setIsPerfAction(false);
					}
				} else {
					//Stop ACT_STAIRS 
					if (action == GameObjectActions::ACT_STAIRS) {
						go->setAction(NO_ACTION);
						go->setIsPerfAction(false);
					}
					objPos.y -= go->getVelY();
					objPos.x -= go->getVelX();
					go->setPosition(objPos);
					if (!go->hasRecentlyRveresedDir())		//Avoid getting stuck in a wall
						_gameObjectManager->reverseAutoDirection(go);
				}

			}
		}
	}	
}

bool CollisionCalculator::checkCeiling(GameObject* go, GameObject* terrainObject) {
	//Checks if the terrain is empty or not at the top of the GameObject
	int x = go->getCollAnchorX();
	int y = go->getCollAnchorY() + go->getHeight();

	TerrainTexture* terrain = dynamic_cast<TerrainTexture*>(terrainObject->getTexture());
	return _terrainManager->getCollMapAt(x, y, terrain);
}

void CollisionCalculator::performAction(GameObject* go, GameObject* terrainObject) {
	int action = go->getAction();
	int direction = go->getCurrentAutoMoveDir();
	TerrainTexture* terrain = dynamic_cast<TerrainTexture*>(terrainObject->getTexture());
	bool putStair = false;
	if (action != NO_ACTION) {
		glm::vec2 objPos = go->getPosition();
		glm::vec2 objSize = go->getSize();
		int collX = go->getCollAnchorX();
		int collY = go->getCollAnchorY();
		switch (action) {
		case ACT_DIG_H: {
			int digBoxWidth = objSize.x * 0.75;
			int digBoxHeight = objSize.y;

			int digBoxX = direction == RIGHT ? collX : collX - digBoxWidth;
			int digBoxY = collY + 1;		//Adjust y to match diplayed gameObject
			_terrainManager->setTerrainPixels(digBoxX, digBoxY, digBoxWidth, digBoxHeight, 0.0f, terrain);
			break;
		}
		case ACT_DIG_V: {
			int digBoxWidth = objSize.x;
			int digBoxHeight = objSize.y * 0.25;

			int digBoxX = objPos.x;
			int digBoxY = collY;
			_terrainManager->setTerrainPixels(digBoxX, digBoxY, digBoxWidth, digBoxHeight, 0.0f, terrain);
			break;
		}
		case ACT_DIG_DIAG: {
			int digBoxWidth = objSize.x * 0.75;
			int digBoxHeight = objSize.y;

			int digBoxX = direction == RIGHT ? collX + 1: collX - digBoxWidth;
			int digBoxY = collY;		//Removes one pixel under the object
			_terrainManager->setTerrainPixels(digBoxX, digBoxY, digBoxWidth, digBoxHeight, 0.0f, terrain);
			break;
		}
		case ACT_STAIRS: 
			if (go->getActionIterations() >= go->getCurrentActionIter()) {
				if (go->isPerformingAction()) {
					glm::vec2 lastActionPos = go->getLastActionPos();
					int xDiff = abs(objPos.x - lastActionPos.x);
					putStair = xDiff >= (go->getBuildWidth() / 2);
				} else {
					putStair = true;
					go->setIsPerfAction(true);
				}
				if (putStair) {
					int stairWidth = go->getBuildWidth();
					int stairHeight = go->getBuildHeight();

					int stairX = direction == RIGHT ? collX : collX - stairWidth;
					int stairY = objPos.y + 1;
					_terrainManager->setTerrainPixels(stairX, stairY, stairWidth, stairHeight, 1.0f, terrain);
					go->setLastActionPos(objPos);
					go->incrementActionIter();
				}
			}
			break;

		case ACT_GENERATE: 
			if (go->getCurrentActionIter() <= go->getGenerationAmount()) {
				double now = glfwGetTime();
				double timeDiff = now - go->getLastActionTime();
				if (timeDiff >= go->getGenerationRate()) {
					_gameObjectManager->generateGameObject(go);
					go->setLastActionTime(now);
					go->incrementActionIter();
				}
			}
			break;
		}
	}
}