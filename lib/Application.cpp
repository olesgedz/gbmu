//
// Created by olesg on 12/19/2021.
//

#include "Application.hpp"
//#define HAVE_LIBC 1
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <GL/gl3w.h>
#include <SDL.h>


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

void Application::run() {
  const char* glsl_version;

  if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
	std::cerr << "Failed to initialize SDL.\n";
	exit(1);
  }

  // Setup window
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
#if __APPLE__
  // GL 3.2 Core + GLSL 150
	glsl_version = "#version 150";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
  // GL 3.0 + GLSL 130
  glsl_version = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif


  SDL_Renderer *sdlRenderer;
  SDL_Surface* screen = NULL;
  SDL_Window* sdlWindow = NULL;
  SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
							  &sdlWindow, &sdlRenderer);



  screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
								0x00FF0000,
								0x0000FF00,
								0x000000FF,
								0xFF000000);

  SDL_Texture *sdlTexture;
  sdlTexture = SDL_CreateTexture(sdlRenderer,
								 SDL_PIXELFORMAT_ARGB8888,
								 SDL_TEXTUREACCESS_STREAMING,
								 SCREEN_WIDTH, SCREEN_HEIGHT);


  gl3wInit();

  screen = SDL_GetWindowSurface(sdlWindow);
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImGui::StyleColorsDark();
  SDL_GLContext glcontext;
  // Setup ImGui Platform/Renderer bindings
  ImGui_ImplSDL2_InitForOpenGL(sdlWindow, glcontext);
  ImGui_ImplOpenGL3_Init(glsl_version);
  SDL_Event e;
  while (1) {
	SDL_PollEvent(&e);
	if (e.type == SDL_KEYDOWN) {
	  exit(0);
	}
	ImGui::Begin("Info", nullptr);


	  ImGui::Text("No ROM loaded");
	  ImGui::Button("Load ROM...");

	ImGui::End();
	SDL_UpdateTexture(sdlTexture, NULL, screen->pixels, screen->pitch);
	SDL_RenderClear(sdlRenderer);
	SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
	SDL_RenderPresent(sdlRenderer);
  }

  SDL_DestroyWindow(sdlWindow);

  SDL_Quit();

};