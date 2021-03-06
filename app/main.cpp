#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "Application.hpp"

// Default settings
settings_t initSettings()
{
	settings_t settings;

	settings.debug = 0;
	settings.keymap.start = SDLK_RETURN;
	settings.keymap.select = SDLK_RSHIFT;
	settings.keymap.left = SDLK_LEFT;
	settings.keymap.up = SDLK_UP;
	settings.keymap.right = SDLK_RIGHT;
	settings.keymap.down = SDLK_DOWN;
	settings.keymap.b = SDLK_z;
	settings.keymap.a = SDLK_x;

	return settings;
}

int main(int argc, char *argv[])
{
	char *filename = nullptr;

	// Optional filename argument
	if (argc == 2) {
		filename = argv[1];
        std::string fl(filename);
        if (fl.find(".gb") == std::string::npos) {
            std::cout << "Not a valid file" << std::endl;
            return 0;
        }
	}
	
	// Initialize emulator with settings
	settings_t settings = initSettings();
	Application emu(settings);

	if (filename) {
		// Initialize components and try to load ROM file.
		if (emu.initializeWithRom(argv[1])) {
//			std::cout << "Succesfully loaded ROM '" << argv[1]
//			          << "' of size " << emu.memory.romLen << " into memory.\n";
		std::cout << std::endl;
		} else {
			std::cerr << "Error loading rom, exiting.\n";
			return -1;
		}
	}

	// Start the emulation (synchronous)
	emu.run();

	return 0;
}


