#include "ui.h"
#include "memory.h"
#include "ppu.h"
#include "cpu.h"
#include "io.h"
#include "bus.h"

#include <SDL2/SDL.h>

namespace dsemu::ui {

SDL_Window *sdlWindow;
SDL_Renderer *sdlRenderer;
SDL_Texture *sdlTexture;
SDL_Surface *screen;


SDL_Window *sdlDebugWindow;
SDL_Renderer *sdlDebugRenderer;
SDL_Texture *sdlDebugTexture;
SDL_Surface *debugScreen;

static unsigned long colors[4] = {0xFFFFFF, 0xC0C0C0, 0x808080, 0x000000};

int scale = 5;

void init() {
    SDL_Init(SDL_INIT_VIDEO);


    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, &sdlWindow, &sdlRenderer);

    screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
                                            0x00FF0000,
                                            0x0000FF00,
                                            0x000000FF,
                                            0xFF000000);
    sdlTexture = SDL_CreateTexture(sdlRenderer,
                                                SDL_PIXELFORMAT_ARGB8888,
                                                SDL_TEXTUREACCESS_STREAMING,
                                                SCREEN_WIDTH, SCREEN_HEIGHT);

    int x, y;
    SDL_GetWindowPosition(sdlWindow, &x, &y);

    SDL_CreateWindowAndRenderer(16 * 8 * scale, 32 * 8 * scale, SDL_WINDOW_RESIZABLE, &sdlDebugWindow, &sdlDebugRenderer);

    debugScreen = SDL_CreateRGBSurface(0, 16 * 8 * scale, 32 * 8 * scale, 32,
                                            0x00FF0000,
                                            0x0000FF00,
                                            0x000000FF,
                                            0xFF000000);
    sdlDebugTexture = SDL_CreateTexture(sdlDebugRenderer,
                                                SDL_PIXELFORMAT_ARGB8888,
                                                SDL_TEXTUREACCESS_STREAMING,
                                                16 * 8 * scale, 32 * 8 * scale);

    SDL_SetWindowPosition(sdlDebugWindow, x + SCREEN_WIDTH + 10, y);
}

void displayTile(SDL_Surface *surface, ushort startLocation, ushort tileNum, int x, int y) {

    SDL_Rect rc;

    for (int tileY=0; tileY<16; tileY += 2) {
        byte b1 = bus::read(startLocation + (tileNum * 16) + tileY);
        byte b2 = bus::read(startLocation + (tileNum * 16) + 1 + tileY);
        
        for (int bit=7, n=0; bit >= 0; bit--, n++) {
            byte hi = !!(b1 & (1 << bit)) << 1;
            byte lo = !!(b2 & (1 << bit));

            byte color = hi|lo;

            rc.x = x + (n * scale);
            rc.y = y + (tileY / 2 * scale);
            rc.w = scale;
            rc.h = scale;

            SDL_FillRect(surface, &rc, colors[color]);
        }
    }

}

void updateDebugWindow() {
    int xDraw = 0;
    int yDraw = 0;
    int tileNum = 0;

    for (int y=0; y<32; y++) {
        for (int x=0; x<16; x++) {
            displayTile(debugScreen, 0x8000, tileNum, xDraw + (x * scale), yDraw + (y * scale));
            xDraw += (7 * scale);
            tileNum++;
        }
        yDraw += (7 * scale);
        xDraw = 0;
    }


	SDL_UpdateTexture(sdlDebugTexture, NULL, debugScreen->pixels, debugScreen->pitch);
	SDL_RenderClear(sdlDebugRenderer);
	SDL_RenderCopy(sdlDebugRenderer, sdlDebugTexture, NULL, NULL);
	SDL_RenderPresent(sdlDebugRenderer);
}

void update() {
    cpu::paused = true;
    SDL_Rect rc;

    for (int lineNum=0; lineNum<ppu::YRES; lineNum++) {

        for (int x=0; x<ppu::XRES; x++) {
            rc.x = x * scale;
            rc.y = lineNum * scale;
            rc.w = scale;
            rc.h = scale;

            SDL_FillRect(screen, &rc, ppu::videoBuffer[x + (lineNum * ppu::XRES)]); // ppu::videoBuffer[lineNum][x]);
        }
    }
/*

    cout << "TILES: " << (int)ppu::scrollInfo.x << ", " << (int)ppu::scrollInfo.y << endl;

    for (int y=0; y<32; y++) {
        for (int x=0; x<32; x++) {
            ushort tileNum = memory::read(ppu::bgMapStart() + x + (y * 32));

            if (ppu::bgTileStart() == 0x8800) {
                tileNum += 128;
            }

            cout << Byte(tileNum) << "-";
        }

        cout << endl;

    }


    yDraw = 0;
    xDraw = 0;

    for (int i=0; i<160; i += 4) {
        ppu::OAMEntry *entry = (ppu::OAMEntry *)&ppu::oamRAM[i];
        if (entry->tile != 0) {
            for (int tileY=0; tileY<16; tileY += 2) {
                byte b1 = memory::read(ppu::bgTileStart() + (entry->tile * 16) + tileY);
                byte b2 = memory::read(ppu::bgTileStart() + (entry->tile * 16) + 1 + tileY);
                
                for (int bit=7, n=0; bit >= 0; bit--, n++) {
                    byte hi = !!(b1 & (1 << bit)) << 1;
                    byte lo = !!(b2 & (1 << bit));

                    byte color = hi|lo;

                    rc.x = xDraw + (entry->x * scale) + (n * scale) - (8 * scale);
                    rc.y = yDraw + (entry->y * scale) + (tileY / 2 * scale) - (16 * scale);
                    rc.w = scale;
                    rc.h = scale;

                    SDL_FillRect(screen, &rc, colors[color]);
                }

                //xDraw = 0;
                //xDraw += (8 * scale);
            }

        }
    }
*/
    updateDebugWindow();

	SDL_UpdateTexture(sdlTexture, NULL, screen->pixels, screen->pitch);
	SDL_RenderClear(sdlRenderer);
	SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
	SDL_RenderPresent(sdlRenderer);

   // cout << "UNP" << endl;
    cpu::paused = false;
}

void handleEvents() {

    //io::startDown = true;

    SDL_Event e;
    if (SDL_PollEvent(&e) > 0)
    {
        SDL_UpdateWindowSurface(sdlWindow);

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_p) {
            cpu::paused = !cpu::paused;
            cout << "Paused: " << cpu::paused << endl;
            //sleepMs(1000);
        }

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
            //cout << "KEYDOWN" << endl;
            io::startDown = true;
            //sleepMs(1000);
        }

        if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_RETURN) {
            //cout << "KEYUP" << endl;
            io::startDown = false;
            //sleepMs(1000);
        }

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_CAPSLOCK) {
            cout << "KEYDOWN" << endl;
            io::selectDown = true;
            //sleepMs(1000);
        }

        if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_CAPSLOCK) {
            cout << "KEYUP" << endl;
            io::selectDown = false;
            //sleepMs(1000);
        }

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_a) {
            cout << "KEYDOWN a" << endl;
            io::aDown = true;
            //sleepMs(1000);
        }

        if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_a) {
            cout << "KEYUP a" << endl;
            io::aDown = false;
            //sleepMs(1000);
        }

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_LEFT) {
            cout << "KEYDOWN Left" << endl;
            io::leftDown = true;
            //sleepMs(1000);
        }

        if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_LEFT) {
            cout << "KEYUP Left" << endl;
            io::leftDown = false;
            //sleepMs(1000);
        }

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RIGHT) {
            cout << "KEYDOWN rightDown" << endl;
            io::rightDown = true;
            //sleepMs(1000);
        }

        if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_RIGHT) {
            cout << "KEYUP rightDown" << endl;
            io::rightDown = false;
            //sleepMs(1000);
        }

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_UP) {
            cout << "KEYDOWN upDown" << endl;
            io::upDown = true;
            //sleepMs(1000);
        }

        if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_UP) {
            cout << "KEYUP upDown" << endl;
            io::upDown = false;
            //sleepMs(1000);
        }

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN) {
            cout << "KEYDOWN downDown" << endl;
            io::downDown = true;
            //sleepMs(1000);
        }

        if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_DOWN) {
            cout << "KEYUP downDown" << endl;
            io::downDown = false;
            //sleepMs(1000);
        }

        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
            exit(0);
        }
    }
}

}

