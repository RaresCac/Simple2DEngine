#version 330 core
layout (location = 0) in vec2 verPos;
layout (location = 1) in vec2 texCoor;

out vec2 textureCoor;

uniform mat4 mvp;

void main() {
	gl_Position = mvp * vec4(verPos.x, verPos.y, 0.0, 1.0);
	textureCoor = texCoor;
}