#include <iostream>
#include <cstddef>
#include "Directions.h"
#include "Game.h"
#include "Vertex.h"
#include "GameObject.h"

GameObject::GameObject() {
	_position = glm::vec2(1.0f);
	_lastPosition = _position;
	_lastActionPos = glm::vec2(1.0f);
	_size = glm::vec2(1.0f);
	_rotation = 0.0f;
	_colour = glm::vec3(1.0f);
}

GameObject::~GameObject() {
}

void GameObject::init(glm::vec2 position, glm::vec2 size, GLfloat rotation, glm::vec3 colour) {
	_position = position;
	_size = size;
	_rotation = rotation;
	_colour = colour;
}
bool GameObject::canMove(int direction) {
	return _canMoveInDir[direction];
}

void GameObject::adjustPositionSwept(float collisionTime) {
	//For SweptAABB collision
	_position.x += _velX * collisionTime;
	_position.y += _velY * collisionTime;
}

void GameObject::returnToPrevPos() {
	_position.x -= _velX;
	_position.y -= _velY;
}

void GameObject::stopAutoMove() {
	_autoMoving = false;
	for (int i = 0; i < 4; i++) {
		_autoMoveDir[i] = false;
	}
}

void GameObject::incrementActionIter() {
	++_currentActionIter;
}

std::string GameObject::getColourAsString() {
	ImVec4 colour(_colour.x, _colour.y, _colour.z, 1.0);
	return std::to_string(ImGui::ColorConvertFloat4ToU32(colour));
}

int GameObject::getCurrentAutoMoveDir() {
	for (int i = 0; i < 4; i++) {
		if (_autoMoveDir[i]) {
			return i;
		}
	}
	return -1;
}

void GameObject::setTexture(Texture2D* tex) {
	_texture = tex;
}

void GameObject::setPosition(glm::vec2 position) {
	_position = position;
}

void GameObject::setSize(glm::vec2 size) {
	_size = size;
}

void GameObject::setWidth(int w) {
	_size = glm::vec2(w, _size.y);
}

void GameObject::setHeight(int h) {
	_size = glm::vec2(_size.x, h);
}

void GameObject::setColour(glm::vec3 colour) {
	_colour = colour;
}

void GameObject::setColour(ImU32 colour) {
	ImVec4 vec = ImGui::ColorConvertU32ToFloat4(colour);
	_colour = glm::vec3(vec.x, vec.y, vec.z);
}

void GameObject::setMovementSpeed(float speed) {
	_movementSpeed = speed;
}

void GameObject::setCanMove(bool set) {
	_canMove = set;
}

void GameObject::setCanMoveDir(int direction, bool set) {
	_canMoveInDir[direction] = set;
}

void GameObject::setRecentlyReversedDir(bool set) {
	_recentlyReversedDir = set;
}

void GameObject::setHasCollision(bool set) {
	_hasCollision = set;
}

void GameObject::setHasTerrainCollision(bool set) {
	_hasTerrainCollision = set;
}

void GameObject::setMovingDir(int dir, bool set) {
	if (_movingInDir[dir] != set) {
		_movingInDir[dir] = set;
	}
}

void GameObject::setAutoMoveDir(int dir, bool move) {
	//If move is false it might mean that a direction is just being disabled in that direction
	//not completely stopped
	if (move)
		_autoMoving = true;
	_autoMoveDir[dir] = move;
}

void GameObject::setFalling(bool fall) {
	_isFalling = fall;
}

void GameObject::setName(std::string name) {
	_name = name;
}

void GameObject::setAction(int action) {
	//Set movement speed and reset _actioniterations depending on the action
	switch (action) {
	case ACT_DIG_V: 
		_movementSpeed = 0; break;
	case ACT_CLIMB:
		_movementSpeed = _autoMovSpeed; break;
	case NO_ACTION: 
		_movementSpeed = _autoMovSpeed; 
		_currentActionIter = 0;
		break;
	default: _movementSpeed = _actionMovSpeed;
	}

	_action = action;
}

void GameObject::setActionIterations(int limit) {
	_actionIterations = limit;
}

void GameObject::setActionMovSpeed(float speed) {
	_actionMovSpeed = speed;
}

void GameObject::setIsPerfAction(bool set) {
	_isPerformingAction = set;
}

void GameObject::setLastActionPos(glm::vec2 pos) {
	_lastActionPos = pos;
}

void GameObject::setLastActionTime(double time) {
	_lastActionTime = time;
}

void GameObject::setIsBlocker(bool set) {
	_isBlocker = set;
}

void GameObject::setIsGenerator(bool set) {
	if (set) {
		setAction(GameObjectActions::ACT_GENERATE);
	} else {
		setAction(GameObjectActions::NO_ACTION);
	}
	_isGenerator = set;
}

void GameObject::setGenerationRate(float set) {
	_generationRate = set;
}

void GameObject::setGenerationAmount(int set) {
	_generationAmount = set;
}

void GameObject::setIsExit(bool set) {
	_isExit = set;
}

void GameObject::_performMovement() {
	_velX = 0.0f;
	_velY = 0.0f;
	_lastPosition = _position;

	float movement = _movementSpeed * Game::timeDelta;


	//Logic to allow movement
	//If the user is manually moving an object, allow it even if game is paused but not if the object is currently falling
	//If it is an automatic movement, do not allow if the game is paused or object is falling
	if ((_movingInDir[UP] || (_autoMoveDir[UP] && !Game::isPaused)) && !_isFalling) {
		_position = glm::vec2(_position.x, _position.y + movement);
		_velY = movement;
	}

	if ((_movingInDir[DOWN] && !_isFalling) || ((_autoMoveDir[DOWN] || _isFalling) && !Game::isPaused)) {
		_position = glm::vec2(_position.x, _position.y - movement);
		_velY = -movement;
	} 

	if ((_movingInDir[LEFT] || _autoMoveDir[LEFT] && !Game::isPaused) && !_isFalling) {
		_position = glm::vec2(_position.x - movement, _position.y);
		_velX = -movement;
	}

	if ((_movingInDir[RIGHT] || _autoMoveDir[RIGHT] && !Game::isPaused) && !_isFalling) {
		_position = glm::vec2(_position.x + movement, _position.y);
		_velX = movement;
	}
}

void GameObject::moveObject() {
	_performMovement();
}
