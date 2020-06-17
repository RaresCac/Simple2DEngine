#pragma once
//Enum to keep track of 2D directions, along with helper functions
enum Directions_2D {
	UP, DOWN, LEFT, RIGHT
};

static int oppositeDirection(int direction) {
	switch (direction) {
		case UP:	return DOWN;	break;
		case DOWN:	return UP;		break;
		case RIGHT: return LEFT;	break;
		case LEFT:	return RIGHT;	break;
		default: break;
	}
}