//
// Created by olesg on 12/19/2021.
//

#include "Application.hpp"
#define HAVE_LIBC 1
#include <SDL.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

void Application::run() {
  SDL_Init(SDL_INIT_VIDEO);


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




  screen = SDL_GetWindowSurface(sdlWindow);
  SDL_Event e;
  while (1) {
	SDL_PollEvent(&e);
	if (e.type == SDL_KEYDOWN) {
	  exit(0);
	}
	SDL_UpdateTexture(sdlTexture, NULL, screen->pixels, screen->pitch);
	SDL_RenderClear(sdlRenderer);
	SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
	SDL_RenderPresent(sdlRenderer);
  }

  SDL_DestroyWindow(sdlWindow);

  SDL_Quit();

};