#pragma once
#include <GLFW/glfw3.h>

struct Position {
	float x;
	float y;
};

struct Colour {
	GLubyte r;
	GLubyte g;
	GLubyte b;
	GLubyte a;
} colour;

struct Vertex {
	Position position;
	Colour colour; 
};