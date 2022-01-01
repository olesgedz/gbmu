//
// Created by olesg on 12/19/2021.
//
#include <GL/gl3w.h>
#if !defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)     \
 && !defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)     \
 && !defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)     \
 && !defined(IMGUI_IMPL_OPENGL_LOADER_CUSTOM)
#define IMGUI_IMPL_OPENGL_LOADER_GL3W
#endif
#include "Application.hpp"
////#define HAVE_LIBC 1
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <SDL.h>


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


void Application::run() {
 std::cout << "Hello world" << std::endl;
}


//void Application::run() {
//
//
//
////  int width = 640;
////  int height = 480;
////  const char* glsl_version;
////
////  if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
////	std::cerr << "Failed to initialize SDL.\n";
////	exit(1);
////  }
////
////  // Setup window
////  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
////  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
////  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
////  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
////  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
////  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
////  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
////  SDL_DisplayMode current;
////  SDL_GetCurrentDisplayMode(0, &current);
////#if __APPLE__
////  // GL 3.2 Core + GLSL 150
////	glsl_version = "#version 150";
////	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
////	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
////	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
////	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
////#else
////  // GL 3.0 + GLSL 130
////  glsl_version = "#version 130";
////  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
////  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
////  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
////  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
////#endif
////
////  // select opengl version
////  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
////  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
////  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
////
////  // create a window
////  SDL_Window *window;
////  if((window = SDL_CreateWindow("SDL2", 0, 0, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN)) == 0) {
////	std::cerr << "failed to open window" << std::endl;
////	SDL_Quit();
////	return ;
////  }
////
////  SDL_GLContext context = SDL_GL_CreateContext(window);
////
//////  if(gl3wInit()) {
//////	std::cerr << "failed to init GL3W" << std::endl;
//////	SDL_GL_DeleteContext(context);
//////	SDL_DestroyWindow(window);
//////	SDL_Quit();
//////	return ;
//////  }
////
////
////
////  SDL_Renderer *sdlRenderer;
////  SDL_Surface* screen = NULL;
////
////
////  screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
////								0x00FF0000,
////								0x0000FF00,
////								0x000000FF,
////								0xFF000000);
////
////  sdlRenderer = SDL_CreateRenderer(window, -1,  SDL_RENDERER_ACCELERATED);
////  SDL_Texture *sdlTexture;
////  sdlTexture = SDL_CreateTexture(sdlRenderer,
////								 SDL_PIXELFORMAT_ARGB8888,
////								 SDL_TEXTUREACCESS_STREAMING,
////								 SCREEN_WIDTH, SCREEN_HEIGHT);
////  if (gl3wInit()) {
////	fprintf(stderr, "failed to initialize OpenGL\n");
////
////  }
////
////
////
////
////
////  screen = SDL_GetWindowSurface(window);
////  IMGUI_CHECKVERSION();
////  ImGui::CreateContext();
////  ImGuiIO& io = ImGui::GetIO(); (void)io;
////  ImGui::StyleColorsDark();
////  SDL_GL_MakeCurrent(window, context);
////
////  // Setup ImGui Platform/Renderer bindings
////  ImGui_ImplSDL2_InitForOpenGL(window, context);
////  ImGui_ImplOpenGL3_Init(glsl_version);
////
////  SDL_Event e;
////  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
////  while (1) {
////	SDL_PollEvent(&e);
////	if (e.type == SDL_KEYDOWN) {
////	  exit(0);
////	}
////	ImGui_ImplOpenGL3_NewFrame();
////	ImGui_ImplSDL2_NewFrame(window);
////	ImGui::NewFrame();
////	ImGui::Begin("Console", nullptr);
////	ImGui::Text("NOTHING.");
//////	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
//////	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;
//////	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
////	ImGui::End();
////	glViewport(0, 0, (int) ImGui::GetIO().DisplaySize.x, (int) ImGui::GetIO().DisplaySize.y);
////	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
////	glClear(GL_COLOR_BUFFER_BIT);
////	ImGui::Render();
////	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
////	SDL_GL_SwapWindow(window);
//////	SDL_UpdateTexture(sdlTexture, NULL, screen->pixels, screen->pitch);
//////	SDL_RenderClear(sdlRenderer);
//////	SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
//////	SDL_RenderPresent(sdlRenderer);
////  }
////
////  SDL_DestroyWindow(window);
////
////  SDL_Quit();
//
//};



//
//#include <iostream>
//#include <cstdio>
//#include <cstdlib>
//#include <string>
//#include <fstream>
//#include "Application.h"
//
//
//// Give all components an up-reference to us.
//Application::Application(settings_t settings)
//{
//  cpu.emu = this;
//  graphics.emu = this;
//  input.emu = this;
//  memory.emu = this;
//  audio.emu = this;
//  gui.emu = this;
//
//  // Save the settings
//  this->settings = settings;
//}
//
//bool Application::initializeWithRom(std::string const filename)
//{
//  // Save the ROM filename
//  this->filename = filename;
//
//  // Reset and re-initialize state
//  reset();
//
//  // Load the ROM from file into memory
//  return memory.loadRom(this->filename);
//}
//
//void Application::unloadRom() {
//  memory.unloadRom();
//}
//
//void Application::reset()
//{
//  // (re-)initialize GB components
//  cpu.initialize();
//  graphics.initialize();
//  input.initialize();
//  memory.initialize();
//  audio.initialize();
//}
//
//void Application::saveState(uint8_t slot)
//{
//  uint8_t state[sizeof(Audio) + sizeof(CPU) + sizeof(Graphics) + sizeof(Input) + sizeof(Memory)];
//
//  // Serialization, hardcore mode
//  uint8_t *dest = state;
//  memcpy(dest, (uint8_t *)&audio, sizeof(Audio)); dest += sizeof(Audio);
//  memcpy(dest, (uint8_t *)&cpu, sizeof(CPU)); dest += sizeof(CPU);
//  memcpy(dest, (uint8_t *)&graphics, sizeof(Graphics)); dest += sizeof(Graphics);
//  memcpy(dest, (uint8_t *)&input, sizeof(Input)); dest += sizeof(Input);
//  memcpy(dest, (uint8_t *)&memory, sizeof(Memory)); dest += sizeof(Memory);
//
//  // Write to file
//  char filename[255];
//  sprintf(filename, "savestate_%d.bin", slot);
//  std::ofstream file(filename, std::ios::binary);
//  file.write((const char *)state, sizeof(state));
//}
//
//bool Application::loadState(uint8_t slot)
//{
//  size_t expectedLen = sizeof(Audio) + sizeof(CPU) + sizeof(Graphics) + sizeof(Input) + sizeof(Memory);
//  uint8_t state[expectedLen];
//
//  // Load file
//  char filename[255];
//  sprintf(filename, "savestate_%d.bin", slot);
//  std::ifstream file(filename, std::ios::binary);
//
//  // Verify length
//  file.seekg(0, file.end);
//  size_t length = file.tellg();
//  file.seekg(0, file.beg);
//  if (length != expectedLen) {
//	std::cerr << "Error: unexpected savestate length";
//	return false;
//  }
//
//  // Read into state
//  file.read((char *)state, expectedLen);
//
//  // Save pointers before overwriting
//  uint8_t *rom = memory.rom;
//  auto stepMode = cpu.stepMode;
//
//  // Save non-pointers by deep copy
//  uint8_t *addrToSymbol = new uint8_t[sizeof(memory.addrToSymbol)];
//  uint8_t *symbolToAddr = new uint8_t[sizeof(memory.symbolToAddr)];
//  memcpy(addrToSymbol, (uint8_t *)(&memory.addrToSymbol), sizeof(memory.addrToSymbol));
//  memcpy(symbolToAddr, (uint8_t *)(&memory.symbolToAddr), sizeof(memory.symbolToAddr));
//
//  // Overwrite all state
//  uint8_t *src = state;
//  memcpy((uint8_t *)&audio, src, sizeof(Audio)); src += sizeof(Audio);
//  memcpy((uint8_t *)&cpu, src, sizeof(CPU)); src += sizeof(CPU);
//  memcpy((uint8_t *)&graphics, src, sizeof(Graphics)); src += sizeof(Graphics);
//  memcpy((uint8_t *)&input, src, sizeof(Input)); src += sizeof(Input);
//  memcpy((uint8_t *)&memory, src, sizeof(Memory)); src += sizeof(Memory);
//
//  // Restore pointers
//  memory.rom = rom;
//  audio.emu = cpu.emu = graphics.emu = input.emu = memory.emu = this;
//  cpu.stepMode = stepMode;
//
//  // Restore non-pointers by deep copy
//  memcpy((uint8_t *)(&memory.addrToSymbol), addrToSymbol, sizeof(memory.addrToSymbol));
//  memcpy((uint8_t *)(&memory.symbolToAddr), symbolToAddr, sizeof(memory.symbolToAddr));
//
//  return true;
//}
//
//void Application::run()
//{
//  // Instruction loop
//  bool done = false;
//  while (not done) {
//	int oldTime = SDL_GetTicks();
//
//	// Skip all logic if no ROM is loaded
//	if (memory.romLoaded) {
//	  if (cpu.stepMode and cpu.stepInst) {
//		// Perform one CPU instruction
//		if (not cpu.executeInstruction()) {
//		  done = 1;
//		  break;
//		}
//		graphics.step();
//
//		cpu.stepInst = false;
//	  } else if (not cpu.stepMode or cpu.stepFrame) {
//		// Do a frame
//		graphics.renderDebugTileset();
//
//		// Step CPU
//		unsigned long long frametime = cpu.c + CPU_CLOCKS_PER_FRAME;
//		while (cpu.c < frametime) {
//		  if (not cpu.executeInstruction()) {
//			done = 1;
//			break;
//		  }
//
//		  graphics.step();
//		}
//
//		cpu.stepFrame = false;
//	  }
//	}
//
//	// Always render frames, for UI to work
//	graphics.renderFrame();
//
//	// SDL event loop
//	SDL_Event event;
//	while (SDL_PollEvent(&event)) {
//	  ImGui_ImplSDL2_ProcessEvent(&event);
//	  ImGuiIO& io = ImGui::GetIO();
//
//	  if (event.type == SDL_KEYDOWN) {
//		switch (event.key.keysym.sym) {
//		  case SDLK_F1: // toggle: debugging on every instruction
//			settings.debug = !settings.debug;
//			break;
//
//		  case SDLK_F2: // debug Graphics
//			graphics.printDebug();
//			cpu.printRegisters();
//			break;
//
//		  case SDLK_F3: // dump memory contents to file
//			memory.dumpToFile("memdump.bin");
//			break;
//
//		  case SDLK_r: // reset
//			reset();
//			break;
//
//		  case SDLK_SPACE:
//			cpu.stepInst = true;
//			break;
//
//		  case SDLK_f:
//			cpu.stepFrame = true;
//			break;
//
//		  default:
//			if (not io.WantCaptureKeyboard) {
//			  input.handleGameInput(0, event.key.keysym.sym);
//			}
//			break;
//		}
//	  }
//	  else if (event.type == SDL_KEYUP) {
//		input.handleGameInput(1, event.key.keysym.sym);
//	  }
//	  else if (event.type == SDL_QUIT ||
//		  (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)) {
//		done = true;
//	  }
//	}
//
//	uint32_t deltaTime = SDL_GetTicks() - oldTime;
//	if (deltaTime > 0 and deltaTime < 16 and not cpu.fastForward) {
//	  SDL_Delay(16 - deltaTime);
//	}
//  }
//}