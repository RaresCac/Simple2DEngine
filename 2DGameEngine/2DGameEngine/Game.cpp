#include "Game.h"
#include <iostream>
#include <fstream>
#include "Directions.h"

double Game::timeDelta = 0.0f;
bool Game::isPaused = false;

Game::Game() {
	_screenWidth = 1800;
	_screenHeight = 1000;
	_isFullscreen = false;
	_fps = 0;
	_frameTime = 0;
}

Game::~Game() {
	delete _inputManager;
	delete _imguiManager;
	delete _gameObjectManager;
	delete _objectRenderer;
}

int Game::init() {
	if (!glfwInit()) {
		std::cout << "Error initialising";
		return 0;
	}

	//Initialises GLFW with the OpenGL version
	//Other versions than 330 seem to give errors with ImGUI
	const char* glsl_version = "#version 330";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_window = glfwCreateWindow(_screenWidth, _screenHeight, "2D Game Engine", _isFullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);

	if (_window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(_window);
	glfwSetFramebufferSizeCallback(_window, Game::framebufferSizeCallback);	//Called when window is resized

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Initialize OpenGL loader
	bool err = gladLoadGL() == 0;
	if (err) {
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_DEPTH_TEST); Not really applicable to 2D, easier to keep track of the render order

	glfwSwapInterval(1); //Enable VSYNC

	//Initialises ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(_window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	//Initialise shaders and the managers
	_initShaders();
	_objectRenderer = new ObjectRenderer(&_shaderProgram, _screenWidth, _screenHeight);
	_objectRenderer->init();
	_textureManager = new TextureManager();
	_terrainManager = new TerrainManager();
	_gameObjectManager = new GameObjectManager(_objectRenderer, _textureManager, _terrainManager);
	_inputManager = new InputManager(_window, _objectRenderer, _gameObjectManager);
	_imguiManager = new ImGUIManager(_textureManager, _terrainManager, _gameObjectManager, _screenWidth, _screenHeight);
	_imguiManager->setInputManager(_inputManager);

	glfwSetMouseButtonCallback(_window, InputManager::manageMouseInput);
	
	glViewport(0, 0, _screenWidth, _screenHeight);
}

void Game::run() {
	double previous = glfwGetTime();
	while (!glfwWindowShouldClose(_window)) {
		//Maintain delta of the entire window, for movement calculations
		double now = glfwGetTime();
		Game::timeDelta = now - previous;
		previous = now;

		//Clear framebuffer
		glClearColor(_clearColor.x, _clearColor.y, _clearColor.z, _clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT);

		_gameObjectManager->objDrawRoutine();
		_imguiGenFrames();
		_imguiManager->displayImGUI();

		//Get input from user, handled in InputManager
		glfwPollEvents();

		//Input processing
		_inputManager->manageInput();

		//Swaps the buffers, one that contains the pixels that should be rendered on the screen, and the other prepares the pixels for the next frame.
		glfwSwapBuffers(_window);
	}

	//Delete ImGUI context
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(_window);
	glfwTerminate();
}
void Game::end() {
	glfwSetWindowShouldClose(_window, true);
}

void Game::setScreenWidth(int width) {
	_screenWidth = width;
}

void Game::setScreenHeight(int height) {
	_screenHeight = height;
}

void Game::setIsFullscreen(bool set) {
	_isFullscreen = set;
}

void Game::_calculateFPS() {
	glfwGetTime();
}

void Game::_initShaders() {
	_shaderProgram.compileShaders("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
	_shaderProgram.linkShaders();
}

void Game::_imguiGenFrames() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}


void Game::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
