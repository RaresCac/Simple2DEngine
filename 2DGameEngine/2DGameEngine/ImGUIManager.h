#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuiFileDialog.h>
#include "TextureManager.h"
#include "TerrainManager.h"
#include "GameObjectManager.h"
#include "InputManager.h"

class ImGUIManager {
public:
	ImGUIManager(TextureManager*, TerrainManager*, GameObjectManager*, int, int);
	~ImGUIManager();

	void setInputManager(InputManager*);

	void displayImGUI();
	bool saveProject(std::string filename);
	bool loadProject(std::string filename);
	void clearEverything();

private:
	void _toolsWindow();
	void _textureWindow();
	void _gameObjectsWindow();
	void _selectedGOWindow();

	std::string _getActionName(int action);
	std::string _getFileName(const std::string &s);
	std::vector<std::string> _split(std::string s, std::string delimiter);
	bool _stringToBool(std::string s);

	std::string _sep = ",";		//Separator for saving and loading

	TextureManager* _textureManager;
	TerrainManager* _terrainManager;
	GameObjectManager* _gameObjectManager;
	InputManager* _inputManager = nullptr;

	int _selectedTexIndex = 0;	//Stores the index of the texture currently selected using ImGUI
	int _selectedTerrIndex = 0; //Stores the index of the currently selected terrain texture 
	int _selectedGOIndex = -1;	//Same but for the game object

	int _screenWidth;
	int _screenHeight;
};

