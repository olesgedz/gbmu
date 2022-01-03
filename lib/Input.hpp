#ifndef INCLUDED_INPUT_H
#define INCLUDED_INPUT_H

#include <cstdint>
#include <SDL.h>
struct Application;
struct Input
{
	// Up-reference
	Application *emu;

	uint8_t row[2];
	uint8_t wire;

	void initialize();

	void handleGameInput(int state, SDL_Keycode key);
};

#endif