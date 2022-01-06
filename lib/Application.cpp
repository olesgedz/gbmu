#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <fstream>
#include "Application.hpp"


// Give all components an up-reference to us.
Application::Application(settings_t settings)
{
	cpu.emu = this;
	graphics.emu = this;
	input.emu = this;
	memory.emu = this;
	audio.emu = this;
	gui.emu = this;

	// Save the settings
	this->settings = settings;
	skipLogo = false;
}

bool Application::initializeWithRom(std::string const fn)
{
	// Save the ROM filename
	this->filename = fn;

	// Reset and re-initialize state
	reset();

	// Load the ROM from file into memory
	return memory.loadRom(this->filename);
}

void Application::unloadRom() {
	memory.unloadRom();
}

void Application::reset()
{
	// (re-)initialize GB components
	cpu.initialize();
	graphics.initialize();
	input.initialize();
	memory.initialize();
	audio.initialize();
}

void Application::saveState(uint8_t slot)
{
  uint8_t state[sizeof(Audio) + sizeof(CPU) + sizeof(Graphics) + sizeof(Input) + sizeof(Memory)];

  // Serialization, hardcore mode
  uint8_t *dest = state;
  memcpy(dest, (uint8_t *)&audio, sizeof(Audio)); dest += sizeof(Audio);
  memcpy(dest, (uint8_t *)&cpu, sizeof(CPU)); dest += sizeof(CPU);
  memcpy(dest, (uint8_t *)&graphics, sizeof(Graphics)); dest += sizeof(Graphics);
  memcpy(dest, (uint8_t *)&input, sizeof(Input)); dest += sizeof(Input);
  memcpy(dest, (uint8_t *)&memory, sizeof(Memory)); dest += sizeof(Memory);

  // Write to file
  char fn[1048];
  sprintf(fn, "%s_%d.bin", filename.c_str(), slot);
  std::ofstream file(fn, std::ios::binary);
  file.write((const char *)state, sizeof(state));
}

bool Application::loadState(uint8_t slot)
{
  size_t expectedLen = sizeof(Audio) + sizeof(CPU) + sizeof(Graphics) + sizeof(Input) + sizeof(Memory);
  uint8_t *state = (uint8_t *) malloc(expectedLen * sizeof(uint8_t));

  // Load file
  char fn[1048];
  sprintf(fn, "%s_%d.bin", filename.c_str(), slot);
  std::ifstream file(fn, std::ios::binary);

  // Verify length
  file.seekg(0, file.end);
  size_t length = file.tellg();
  file.seekg(0, file.beg);
  if (length != expectedLen) {
	std::cerr << "Error: unexpected savestate length";
	return false;
  }

  // Read into state
  file.read((char *)state, expectedLen);

  // Save pointers before overwriting
  uint8_t *rom = memory.rom;
  auto stepMode = cpu.stepMode;

  // Save non-pointers by deep copy
  uint8_t *addrToSymbol = new uint8_t[sizeof(memory.addrToSymbol)];
  uint8_t *symbolToAddr = new uint8_t[sizeof(memory.symbolToAddr)];
  memcpy(addrToSymbol, (uint8_t *)(&memory.addrToSymbol), sizeof(memory.addrToSymbol));
  memcpy(symbolToAddr, (uint8_t *)(&memory.symbolToAddr), sizeof(memory.symbolToAddr));

  // Overwrite all state
  uint8_t *src = state;
  memcpy((uint8_t *)&audio, src, sizeof(Audio)); src += sizeof(Audio);
  memcpy((uint8_t *)&cpu, src, sizeof(CPU)); src += sizeof(CPU);
  memcpy((uint8_t *)&graphics, src, sizeof(Graphics)); src += sizeof(Graphics);
  memcpy((uint8_t *)&input, src, sizeof(Input)); src += sizeof(Input);
  memcpy((uint8_t *)&memory, src, sizeof(Memory)); src += sizeof(Memory);

  // Restore pointers
  memory.rom = rom;
  audio.emu = cpu.emu = graphics.emu = input.emu = memory.emu = this;
  cpu.stepMode = stepMode;

  // Restore non-pointers by deep copy
  memcpy((uint8_t *)(&memory.addrToSymbol), addrToSymbol, sizeof(memory.addrToSymbol));
  memcpy((uint8_t *)(&memory.symbolToAddr), symbolToAddr, sizeof(memory.symbolToAddr));

  return true;
}

void Application::run()
{
	// Instruction loop
	bool done = false;
	while (not done) {
		int oldTime = SDL_GetTicks();

		// Skip all logic if no ROM is loaded
		if (memory.romLoaded) {
			if (cpu.stepMode and cpu.stepInst) {
				// Perform one CPU instruction
				if (not cpu.executeInstruction()) {
					done = 1;
					break;
				}
				graphics.step();
				
				cpu.stepInst = false;
			} else if (not cpu.stepMode or cpu.stepFrame) {
				// Do a frame
				graphics.renderDebugTileset();
				
				// Step CPU
				unsigned long long frametime = cpu.c + CPU_CLOCKS_PER_FRAME;
				while (cpu.c < frametime) {
					if (not cpu.executeInstruction()) {
						done = 1;
						break;
					}

					graphics.step();
				}

				cpu.stepFrame = false;
			}
		}

		// Always render frames, for UI to work
		graphics.renderFrame();
		
		// SDL event loop
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);
			ImGuiIO& io = ImGui::GetIO();

			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_F1: // toggle: debugging on every instruction
						settings.debug = !settings.debug;
						break;
						
					case SDLK_F2: // debug Graphics
						graphics.printDebug();
						cpu.printRegisters();
						break;
							
					case SDLK_F3: // dump memory contents to file
						memory.dumpToFile("memdump.bin");
						break;
					
					case SDLK_r: // reset
						reset();
						break;

					case SDLK_SPACE:
						cpu.stepInst = true;
						break;

					case SDLK_f:
						cpu.stepFrame = true;
						break;
					
					default:
						if (not io.WantCaptureKeyboard) {
							input.handleGameInput(0, event.key.keysym.sym);
						}
						break;
				}
			}
			else if (event.type == SDL_KEYUP) {
				input.handleGameInput(1, event.key.keysym.sym);
			}
			else if (event.type == SDL_QUIT ||
				(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)) {
				done = true;
			}
		}
		
		uint32_t deltaTime = SDL_GetTicks() - oldTime;
		if (deltaTime > 0 and deltaTime < 16 and not cpu.fastForward) {
			SDL_Delay(16 - deltaTime);
		}
	}
}