#include <iostream>
#include <fstream>
#include <imgui.h>
#include "InputManager.h"
#include "Directions.h"

GameObjectManager* InputManager::_gameObjectManager;
int InputManager::_clickState = SELECT_OBJECT;
int InputManager::_currentGOIndex = -1;

InputManager::InputManager(GLFWwindow* window, ObjectRenderer* renderer, GameObjectManager* goManager) {
	_window = window;
	_objectRenderer = renderer;
	_gameObjectManager = goManager;
}

InputManager::~InputManager() {
}

void InputManager::manageInput() {
	if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(_window, true);

	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureKeyboard) {		//Prevent actions while writing to an Imgui textbox
		if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
			_objectRenderer->moveCamera(UP);
		if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
			_objectRenderer->moveCamera(DOWN);
		if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
			_objectRenderer->moveCamera(LEFT);
		if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
			_objectRenderer->moveCamera(RIGHT);

		if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS) {
			_pressed[UP] = true;
			_gameObjectManager->startObjMovement(_currentGOIndex, UP);
		}
		if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			_pressed[DOWN] = true;
			_gameObjectManager->startObjMovement(_currentGOIndex, DOWN);
		}
		if (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			_pressed[LEFT] = true;
			_gameObjectManager->startObjMovement(_currentGOIndex, LEFT);
		}
		if (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			_pressed[RIGHT] = true;
			_gameObjectManager->startObjMovement(_currentGOIndex, RIGHT);
		}

		if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_RELEASE)
			if (_pressed[UP]) {
				_pressed[UP] = false;
				_gameObjectManager->stopObjMovement(_currentGOIndex, UP);
			}
		if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_RELEASE) {
			if (_pressed[DOWN]) {
				_pressed[DOWN] = false;
				_gameObjectManager->stopObjMovement(_currentGOIndex, DOWN);
			}
		}
		if (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_RELEASE) {
			if (_pressed[LEFT]) {
				_pressed[LEFT] = false;
				_gameObjectManager->stopObjMovement(_currentGOIndex, LEFT);
			}
		}
		if (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_RELEASE) {
			if (_pressed[RIGHT]) {
				_pressed[RIGHT] = false;
				_gameObjectManager->stopObjMovement(_currentGOIndex, RIGHT);
			}
		}
	}
}

void InputManager::manageMouseInput(GLFWwindow* window, int button, int action, int mods) {
	ImGuiIO& io = ImGui::GetIO();
	//https://github.com/eliasdaler/imgui-sfml/issues/24#issuecomment-427003384
	if (!io.WantCaptureMouse) {		//Prevents clicking through any ImGui window
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			int winW, winH;
			glfwGetWindowSize(window, &winW, &winH);
			//Adjust to bottom-left alignment
			int adjYPos = winH - ypos;
			int cameraAdjXPos = xpos + _gameObjectManager->getCameraOffset().x;
			int cameraAdjYPos = adjYPos + _gameObjectManager->getCameraOffset().y;
			switch (_clickState) {
			case SELECT_OBJECT:
				_currentGOIndex = _gameObjectManager->getGameObjectAtPos(cameraAdjXPos, cameraAdjYPos);
				break;
			case ADD_OBJECT:
				_gameObjectManager->addGameObject(cameraAdjXPos, cameraAdjYPos, 20, 20); break;
			case EDIT_TERR_ADD:
				//_gameObjectManager->setTerrainPixel(xpos, adjYPos, 1.0f); break;
			case EDIT_TERR_REM:
				//_gameObjectManager->setTerrainPixel(xpos, adjYPos, 0.0f); break;
			default:
				break;
			}
		}
	}
}

void InputManager::setClickState(int clickState) {
	_clickState = clickState;
}

void InputManager::setCurrentGOIndex(int index) {
	_currentGOIndex = index;
}
