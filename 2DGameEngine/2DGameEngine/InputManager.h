#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "ObjectRenderer.h"
#include "GameObjectManager.h"

class InputManager {
public:
	InputManager(GLFWwindow*, ObjectRenderer*, GameObjectManager*);
	~InputManager();

	void manageInput();
	static void manageMouseInput(GLFWwindow*, int, int, int);

	int getClickState() { return _clickState; }
	int getCurrentGOIndex() { return _currentGOIndex; }
	void setClickState(int clickState);
	void setCurrentGOIndex(int index);

	enum ClickState {
		SELECT_OBJECT,
		ADD_OBJECT,
		EDIT_TERR_ADD,
		EDIT_TERR_REM
	};

private:
	static GameObjectManager* _gameObjectManager;
	GLFWwindow* _window;
	ObjectRenderer* _objectRenderer;

	static int _currentGOIndex;	//Holds the index of the current object that is selected
	static int _clickState;		//Holds what should happen when a click is detected on the screen

	//Initially the keys will all be in state GLFW_RELEASE so keeps track if directional keys
	//have been in state GLFW_PRESSED before handling the GLFW_RELEASE state
	bool _pressed[4] = { false, false, false, false };
};

