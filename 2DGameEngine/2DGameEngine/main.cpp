#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Game.h"

int main(void) {
	Game gameWindow;

	int windowWidth;
	int windowHeight;

	int selection;
	bool fullScreen;

	std::cout << "Select a window size:" << std::endl;
	std::cout << "  1. 1920 x 1080 and fullscreen." << std::endl;
	std::cout << "  2. Start with default settings (1800 x 1000)." << std::endl;

	std::cin >> selection;

	while (true) {
		if (std::cin.fail() || selection > 2) {
			std::cin >> selection;
		} else {
			break;
		}
	}

	if (selection == 1) {
		gameWindow.setIsFullscreen(true);
		gameWindow.setScreenWidth(1920);
		gameWindow.setScreenWidth(1080);
	}

	gameWindow.init();
	gameWindow.run();

	return 0;
};

