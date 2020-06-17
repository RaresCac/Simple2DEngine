#pragma once
#include <vector>
#include <glad/glad.h>  // Initialize with gladLoadGL()
#include <GLFW/glfw3.h>
#include "GameObject.h"
#include "ShaderLoader.h"
#include "ObjectRenderer.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "TerrainManager.h"
#include "ImGUIManager.h"

class Game {
public:
	Game();
	~Game();

	int init();
	void run();
	void end();

	int getScreenWidth() { return _screenWidth; }
	int getScreenHeight() { return _screenHeight; }
	bool isFullscreen() { return _isFullscreen; }

	void setScreenWidth(int width);
	void setScreenHeight(int height);
	void setIsFullscreen(bool set);

	static double timeDelta;
	static bool isPaused;

private:
	static void framebufferSizeCallback(GLFWwindow*, int, int);
	
	void _calculateFPS();
	void _initShaders();
	void _imguiGenFrames();

	GLFWwindow* _window = nullptr;	
	ShaderLoader _shaderProgram;
	ObjectRenderer* _objectRenderer = nullptr;

	TextureManager* _textureManager = nullptr;
	TerrainManager* _terrainManager = nullptr;
	GameObjectManager* _gameObjectManager = nullptr;
	InputManager* _inputManager = nullptr;
	ImGUIManager* _imguiManager = nullptr;

	int _screenWidth;
	int _screenHeight;
	bool _isFullscreen;
	float _fps;
	float _frameTime;

	ImVec4 _clearColor = ImVec4(0.4f, 0.0f, 0.4f, 1.0f);
};

