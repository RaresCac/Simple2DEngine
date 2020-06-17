#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <imgui.h>
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderLoader.h"
#include "Texture2D.h"
#include "GameObjectActions.h"

class GameObject
{
public:
	GameObject();
	~GameObject();

	void init(glm::vec2 position, glm::vec2 size, GLfloat rotation, glm::vec3 colour);	
	void moveObject();
	bool canMove(int);
	void adjustPositionSwept(float);
	void returnToPrevPos();
	void stopAutoMove();
	void incrementActionIter();

	//------------Getters----------------
	//Rendering properties
	Texture2D* getTexture() { return _texture; }
	glm::vec2 getPosition() { return _position; }
	glm::vec2 getLastPosition() { return _lastPosition; }
	glm::vec2 getSize() { return _size; }
	glm::vec3 getColour() { return _colour; }
	std::string getColourAsString();
	std::string getName() { return _name; }
	float getWidth() { return _size.x; };
	float getHeight() { return _size.y; };	

	//Movement properties
	float getVelX() { return _velX; }
	float getVelY() { return _velY; }
	float getMovementSpeed() { return _movementSpeed; }	
	int getCollAnchorX() { return (int)(_position.x + (_size.x / 2)); }
	int getCollAnchorY() { return (int)(_position.y); }
	int getCurrentAutoMoveDir();
	std::vector<bool> getAutoMoveDir() { return _autoMoveDir; }
	bool canMove() { return _canMove; }
	bool isFalling() { return _isFalling; }	
	bool isAutoMoving() { return _autoMoving; }
	bool hasRecentlyRveresedDir() { return _recentlyReversedDir; }

	//Collision properties
	bool hasTerrainCollision() { return _hasTerrainCollision; }
	bool hasCollision() { return _hasCollision; }

	//Type of GameObject
	bool isExit() { return _isExit; }
	bool isBlocker() { return _isBlocker; }
	bool isGenerator() { return _isGenerator; }
	float getGenerationRate() { return _generationRate; }
	int getGenerationAmount() { return _generationAmount; }

	//Action properties
	int getAction() { return _action; }	
	bool isPerformingAction() { return _isPerformingAction; }
	int getActionIterations() { return _actionIterations; }
	int getCurrentActionIter() { return _currentActionIter; }
	float getActionMovSpeed() { return _actionMovSpeed; }
	glm::vec2 getLastActionPos() { return _lastActionPos; }
	double getLastActionTime() { return _lastActionTime; }

	//For ACT_STAIRS
	int getBuildWidth() { return _buildWidth; }
	int getBuildHeight() { return _buildHeight; }

	//--------Setters---------
	//General properties
	void setTexture(Texture2D*);
	void setPosition(glm::vec2);
	void setSize(glm::vec2);
	void setWidth(int);
	void setHeight(int);
	void setColour(glm::vec3);
	void setColour(ImU32);
	void setName(std::string);

	//Movement properties
	void setMovementSpeed(float);
	void setMovingDir(int, bool);
	void setAutoMoveDir(int, bool);
	void setCanMove(bool);
	void setCanMoveDir(int, bool);
	void setRecentlyReversedDir(bool);

	//Collision properties
	void setHasCollision(bool);
	void setHasTerrainCollision(bool);
	void setFalling(bool);

	//Type of GameObject
	void setIsBlocker(bool);
	void setIsGenerator(bool);
	void setGenerationRate(float);
	void setGenerationAmount(int);
	void setIsExit(bool);

	//Action properties
	void setAction(int action);
	void setActionIterations(int limit);
	void setActionMovSpeed(float speed);
	void setIsPerfAction(bool set);
	void setLastActionPos(glm::vec2 pos);
	void setLastActionTime(double time);

private:
	void _performMovement();

	//Object properties within the game
	Texture2D* _texture = nullptr;
	glm::vec2 _position;	
	glm::vec2 _lastPosition;			//For collision resolution
	glm::vec2 _size;					//x = width, y = height
	GLfloat _rotation;
	glm::vec3 _colour;				
	
	bool _canMove = true;				//If object can move (modify its _position via _performMovement)
	std::vector<bool> _canMoveInDir = { true, true, true, true };		//If object can move in the 2D directions up, down, left, right
	std::vector<bool> _movingInDir = { false, false, false, false };	//Which directions the object will move this frame
	std::vector<bool> _autoMoveDir = { false, false, false, false };	//In which direction the object should move this frame, not input by user
	bool _autoMoving = false;								//If object is moving in any direction automatically
	float _autoMovSpeed = 100.0f;							//Movement speed when auto moving
	float _movementSpeed = 100.0f;		//Current movement speed, pixels per second
	float _velX = 0.0f;					//Stores the current speed (i.e. last movement in the current frame)
	float _velY = 0.0f;

	bool _isFalling = false;			//If it should move towards the terrain below. Modified by the terrain collision method
	bool _hasCollision = false;			//If collision is checked against blockers/exits
	bool _hasTerrainCollision = false;	//If collision is checked against terrain
	bool _recentlyReversedDir = false;	//If auto direction has been swapped recently, to avoid getting stuck in a wall
	bool _isBlocker = false;			//If it should block other moving objects
	bool _isExit = false;				//If is an exit point for the objects, i.e. the 
	bool _isGenerator = false;			//If it should spawn other game objects
	float _generationRate = 1.0f;		//Rate at which to spawn new GameObjects, in seconds
	int _generationAmount = 10;			//How many GameObjects should be generated.
	
	int _action = NO_ACTION;			//Special action that the object should perform (like removing some terrain)
	int _actionIterations = 15;			//Used for ACT_STAIRS, indicates number of stairs to be placed
	float _actionMovSpeed = 40.0f;	
	int _currentActionIter = 0;
	bool _isPerformingAction = false;	//If currently performing action
	glm::vec2 _lastActionPos;
	double _lastActionTime = 0;			//Time when action was performed, tracked by glfwGetTime()
	int _buildWidth = 20;				//For ACT_STAIRS, width of the placed pixels under them
	int _buildHeight = 4;				//Same as before, but width

	//Editor properties
	std::string _name = "";
	std::string _id = "";
};

