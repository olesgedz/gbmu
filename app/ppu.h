#pragma once

#include "common.h"


namespace dsemu::ppu {

struct ScrollInfo {
    byte x;
    byte y;
};

extern unsigned long *videoBuffer;

extern byte lcdControl;
extern byte lcdStats;
extern ScrollInfo scrollInfo;
extern int currentFrame;

const int HZ = 1048576;
const int LINES_PER_FRAME = 154;
const int TICKS_PER_LINE = 114;
const int TICKS_PER_FRAME = LINES_PER_FRAME * TICKS_PER_LINE;
const int OAM_TICKS = 20;
const int PIXEL_TICKS = 43;
const int VBLANK_LINE = 144;

const int YRES = 144;
const int XRES = 160;

void tick();
void init();

byte getCurrentLine();

struct OAMEntry {
    byte y;
    byte x;
    byte tile;
    byte flags;
};

inline bool bgDisplay() { return getBit(lcdControl, 0); }
inline bool spriteDisplay() { return getBit(lcdControl, 1); }
inline bool spriteSize8x16() { return getBit(lcdControl, 2); }
inline ushort bgMapStart() { return getBit(lcdControl, 3) ? 0x9C00 : 0x9800; }
inline ushort bgTileStart() { return getBit(lcdControl, 4) ? 0x8000 : 0x8800; }
inline bool windowDisplay() { return getBit(lcdControl, 5); }
inline ushort windowMapSelect() { return getBit(lcdControl, 6) ? 0x9C00 : 0x9800; }
inline bool lcdOn() { return getBit(lcdControl, 7); }

/*
OAM Entry:
    PosY
    PosX
    Tile Num
    Priority bit
    FlipX bit
    FlipY bit
    Palette bits
*/

extern byte oamRAM[160];

byte readOAM(ushort address);
void writeOAM(ushort address, byte b);

inline byte getXScroll() { return scrollInfo.x; }
inline byte getYScroll() { return scrollInfo.y; }

inline void setXScroll(byte b) { scrollInfo.x = b; }
inline void setYScroll(byte b) { scrollInfo.y = b; }

}



