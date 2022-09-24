/*This source code copyrighted by Lazy Foo' Productions (2004-2022)
and may not be redistributed without written permission.*/

#ifndef DOT_H
#define DOT_H

#include <SDL.h>

// The dot that will move around on the screen
class Dot
{
public:
	// The dimensions of the dot
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;

	// Maximum axis velocity of the dot
	static const int DOT_VEL = 10;

	// Initializes the variables
	Dot(void);

	// Takes key presses and adjusts the dot's velocity
	void handleEvent( SDL_Event& e );

	// Moves the dot
	void move(void);

	// Shows the dot on the screen
	void render(void);

private:
	// The X and Y offsets of the dot
	int mPosX, mPosY;

	// The velocity of the dot
	int mVelX, mVelY;
};

#endif 
