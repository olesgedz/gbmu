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
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_DisplayMode current;
  SDL_GetCurrentDisplayMode(0, &current);
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

 sdlWindow =  SDL_CreateWindow("ImguiDemoCLion", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
							  1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);


  screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
								0x00FF0000,
								0x0000FF00,
								0x000000FF,
								0xFF000000);

  sdlRenderer = SDL_CreateRenderer(sdlWindow, -1,  SDL_RENDERER_ACCELERATED);
  SDL_Texture *sdlTexture;
  sdlTexture = SDL_CreateTexture(sdlRenderer,
								 SDL_PIXELFORMAT_ARGB8888,
								 SDL_TEXTUREACCESS_STREAMING,
								 SCREEN_WIDTH, SCREEN_HEIGHT);
  SDL_GLContext gl_context = SDL_GL_CreateContext(sdlWindow); //needed for init
  if (gl3wInit()) {
	fprintf(stderr, "failed to initialize OpenGL\n");

  }

  //
//
//
//
  screen = SDL_GetWindowSurface(sdlWindow);
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImGui::StyleColorsDark();
  SDL_GLContext glcontext = SDL_GL_CreateContext(sdlWindow);
  SDL_GL_MakeCurrent(sdlWindow, glcontext);

  // Setup ImGui Platform/Renderer bindings
  ImGui_ImplSDL2_InitForOpenGL(sdlWindow, glcontext);
  ImGui_ImplOpenGL3_Init(glsl_version);

  SDL_Event e;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  while (1) {
	SDL_PollEvent(&e);
	if (e.type == SDL_KEYDOWN) {
	  exit(0);
	}
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(sdlWindow);
	ImGui::NewFrame();
	ImGui::Begin("Console", nullptr);
	ImGui::Text("NOTHING.");
//	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
//	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;
//	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	ImGui::End();
	glViewport(0, 0, (int) ImGui::GetIO().DisplaySize.x, (int) ImGui::GetIO().DisplaySize.y);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(sdlWindow);
//	SDL_UpdateTexture(sdlTexture, NULL, screen->pixels, screen->pitch);
//	SDL_RenderClear(sdlRenderer);
//	SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
//	SDL_RenderPresent(sdlRenderer);
  }

  SDL_DestroyWindow(sdlWindow);

  SDL_Quit();

};