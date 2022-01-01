#include "ppu.h"
#include "cpu.h"
#include "memory.h"
#include "io.h"
#include "ui.h"
#include "bus.h"

#include <chrono>
#include <thread>
#include <unistd.h>
#include <cstring>
#include <SDL2/SDL.h>

namespace dsemu::ppu {


byte lcdControl;

byte lcdStats = 0;

ScrollInfo scrollInfo;
int currentFrame = 0;
byte currentLine = 0;
byte oamRAM[160];

unsigned long *videoBuffer;

void init() {
    currentFrame = 0;
    currentLine = 0;
    scrollInfo.x = 0;
    scrollInfo.y = 0;
    memset(oamRAM, 0, sizeof(oamRAM));

    videoBuffer = new unsigned long[256 * 256];
    memset(videoBuffer, 0, 256 * 256 * sizeof(unsigned long));

    cout << "VID BUFF: " << Int64((uint64_t)videoBuffer) << endl;
}

void drawFrame() {

    if (!cpu::haltWaitingForInterrupt) {
        /*
        cout << "SPRITES: " << endl;
        for (int i=0; i<160; i += 4) {
            OAMEntry *entry = (OAMEntry *)&oamRAM[i];
            
            cout << Byte(entry->x) << "," << Byte(entry->y) << "-" << Byte(entry->tile) << " ";

            if ((i % 40) == 0) {
                cout << endl;
            }
        }

        cout << endl;


        cout  << endl << "LCD Status: " << endl
            << "\tbgDisplay: " << Byte(bgDisplay()) << endl
            << "\tspriteDisplay: " << Byte(spriteDisplay()) << endl
            << "\tspriteSize8x16: " << Byte(spriteSize8x16()) << endl
            << "\tbgMapStart: " << Short(bgMapStart()) << endl
            << "\tbgTileStart: " << Short(bgTileStart()) << endl
            << "\twindowDisplay: " << Byte(windowDisplay()) << endl
            << "\twindowMapSelect: " << Short(windowMapSelect()) << endl
            << "\tlcdOn: " << Byte(lcdOn()) << endl << endl;
    */
        //if (!DEBUG) return;
    }
    
}

byte getCurrentLine() {
    return currentLine;
}
byte readOAM(ushort address) {


    //cout << "READ DUMPING OAM RAM: " << Short(address) << " - " << Byte(memory::ram[address]) << " - " << endl;

    for (int i=0; i<160; i++) {
        //if ((i % 32) == 0) cout << endl;
        //cout << " " << Byte(oamRAM[i]);
    }

    //cout << endl;

    //sleep(2);


    return oamRAM[address];
}

void writeOAM(ushort address, byte b) {
    oamRAM[address] = b;
}

long targetFrameTime = 1000 / 60;
long start = SDL_GetTicks();
int count = 0;
long prev = SDL_GetTicks();
static unsigned long colors[4] = {0xFFFFFF, 0xC0C0C0, 0x808080, 0x000000};

int normScroll = 3;


vector<OAMEntry *> getSpritesOnLine(int lineNum) {

    vector<OAMEntry *> entries;

    for (int i=0; i<160; i += 4) {
        OAMEntry *entry = (OAMEntry *)&oamRAM[i];
        
        if (entry->y >= lineNum + 8 && entry->y < lineNum + 16) {
            entries.push_back(entry);
        }
    }

    return entries;
}

OAMEntry *getSpriteOnX(vector<OAMEntry *> &sprites, int x) {
    for (auto entry : sprites) {
        if (entry->x >= x && entry->x < x + 8) {
            return entry;
        }
    }

    return nullptr;
}

void drawLine(int lineNum) {
    int mapy = (lineNum + getYScroll()) % 256;
    byte tileY = ((lineNum) % 8) * 2;

    auto sprites = getSpritesOnLine(lineNum);

    for (int x=0; x<XRES; x += 1) {
        auto sprite = getSpriteOnX(sprites, x);

        int mapx = (x + getXScroll()) % 256;

        ushort bgTileNum = bus::read(ppu::bgMapStart() + (mapx/8) + ((mapy/8) * 32));

        if (ppu::bgTileStart() == 0x8800) {
            bgTileNum += 128;
        }

        if (sprite != nullptr) {
            bgTileNum = sprite->tile;
        }

        byte b1 = bus::read(ppu::bgTileStart() + (bgTileNum * 16) + tileY);
        byte b2 = bus::read(ppu::bgTileStart() + (bgTileNum * 16) + 1 + tileY);

        int bit = 7 - (x % 8);
        byte hi = !!(b1 & (1 << bit)) << 1;
        byte lo = !!(b2 & (1 << bit));
        byte color = hi|lo;
        unsigned long c = colors[color];

        videoBuffer[x + (lineNum * XRES)] = c;
    }
}

void tick() {
    int f = cpu::getTickCount() % (TICKS_PER_FRAME);
    int l = f / 114;

    if (l != currentLine && l < 144) {
        if (DEBUG && !cpu::haltWaitingForInterrupt) cout << "PPU:> NEW LINE: " << l << " FRAME: " << currentFrame << endl;

        drawLine(currentLine);
    }

    if (lcdStats & 0x40 && bus::read(0xFF45) == l) {
        cpu::handleInterrupt(2, true, false);
    }
    
    if (l != currentLine && l == 144) {
        long end = SDL_GetTicks();
		long frameTime = end - prev;

		if (frameTime < targetFrameTime) 
		{
			SDL_Delay(targetFrameTime - frameTime);
		}

		if (end - start >= 1000) {
			if (!cpu::haltWaitingForInterrupt) cout << "FPS: " << count << endl;
			count = 0;
			start = end;
		}

		count++;

        currentFrame++;
        drawFrame();
        if (!cpu::haltWaitingForInterrupt) cout << endl << "PPU:> NEW FRAME: " << currentFrame << endl << endl;

        cpu::handleInterrupt(cpu::IVBlank, true, false);

        prev = SDL_GetTicks();
    }

    currentLine = l;


}

}



