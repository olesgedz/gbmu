#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include "dromaius.h"

uint8_t Memory::bios[] = {
        0x31, 0xFE, 0xFF, 0xAF, 0x21, 0xFF, 0x9F, 0x32, 0xCB, 0x7C, 0x20, 0xFB, 0x21, 0x26, 0xFF, 0x0E,
        0x11, 0x3E, 0x80, 0x32, 0xE2, 0x0C, 0x3E, 0xF3, 0xE2, 0x32, 0x3E, 0x77, 0x77, 0x3E, 0xFC, 0xE0,
        0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1A, 0xCD, 0x95, 0x00, 0xCD, 0x96, 0x00, 0x13, 0x7B,
        0xFE, 0x34, 0x20, 0xF3, 0x11, 0xD8, 0x00, 0x06, 0x08, 0x1A, 0x13, 0x22, 0x23, 0x05, 0x20, 0xF9,
        0x3E, 0x19, 0xEA, 0x10, 0x99, 0x21, 0x2F, 0x99, 0x0E, 0x0C, 0x3D, 0x28, 0x08, 0x32, 0x0D, 0x20,
        0xF9, 0x2E, 0x0F, 0x18, 0xF3, 0x67, 0x3E, 0x64, 0x57, 0xE0, 0x42, 0x3E, 0x91, 0xE0, 0x40, 0x04,
        0x1E, 0x02, 0x0E, 0x0C, 0xF0, 0x44, 0xFE, 0x90, 0x20, 0xFA, 0x0D, 0x20, 0xF7, 0x1D, 0x20, 0xF2,
        0x0E, 0x13, 0x24, 0x7C, 0x1E, 0x83, 0xFE, 0x62, 0x28, 0x06, 0x1E, 0xC1, 0xFE, 0x64, 0x20, 0x06,
        0x7B, 0xE2, 0x0C, 0x3E, 0x87, 0xF2, 0xF0, 0x42, 0x90, 0xE0, 0x42, 0x15, 0x20, 0xD2, 0x05, 0x20,
        0x4F, 0x16, 0x20, 0x18, 0xCB, 0x4F, 0x06, 0x04, 0xC5, 0xCB, 0x11, 0x17, 0xC1, 0xCB, 0x11, 0x17,
        0x05, 0x20, 0xF5, 0x22, 0x23, 0x22, 0x23, 0xC9, 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
        0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
        0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
        0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E, 0x3c, 0x42, 0xB9, 0xA5, 0xB9, 0xA5, 0x42, 0x4C,
        0x21, 0x04, 0x01, 0x11, 0xA8, 0x00, 0x1A, 0x13, 0xBE, 0x20, 0xFE, 0x23, 0x7D, 0xFE, 0x34, 0x20,
        0xF5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xFB, 0x86, 0x20, 0xFE, 0x3E, 0x01, 0xE0, 0x50
};

void Graphics::initialize()
{
	// Initialize state
	mode = Mode::HBLANK;
	mclock = 0;
	r.line = 0;
	r.scx = 0;
	r.scy = 0;
	r.flags = 0;

	// Default screen scale
	// TODO: make setting
	screenScale = 3;

	// Initialize pixel buffers
	memset(screenPixels, 0x00, sizeof(screenPixels));
	memset(debugTilesetPixels, 0x00, sizeof(debugTilesetPixels));


	// Initialize OAM and VRAM
	memset(vram, 0x00, sizeof(vram));
	memset(oam, 0x00, sizeof(oam));
	
	// Initialize tileset
	for (int i = 0; i < 512; i++) {
		for (int j = 0; j < 8; j++) {
			for (int k = 0; k < 8; ++k) {
				tileset[i][j][k] = 0x00;
			}
		}
	}
	
	// Initialize sprite data	
	for (int i = 0; i < 40; i++) {
		spritedata[i].x = -8;
		spritedata[i].y = -16;
		spritedata[i].tile = 0;
		spritedata[i].flags = 0;
	}

	initDisplay();
	initialized = true;
}


void Graphics::initDisplay()
{
	// Create texture for game graphics
	glGenTextures(1, &emu->graphics.screenTexture);

	// Create texture for video mem debug
	glGenTextures(1, &emu->graphics.debugTexture);
}

void Graphics::updateTextures()
{
	// Bind texture and upload pixels
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GB_SCREEN_WIDTH, GB_SCREEN_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, screenPixels);

	// Bind texture and upload pixels
	glBindTexture(GL_TEXTURE_2D, debugTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DEBUG_WIDTH, DEBUG_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, debugTilesetPixels);

	// Restore state
	glBindTexture(GL_TEXTURE_2D, 0);
}

uint8_t Graphics::readByte(uint16_t addr)
{
	uint8_t res;
	
	switch (addr) {
		case 0x00:
			return r.flags;

		case 0x01: // LCD status
			//printf("  r.line = %d, r.lineComp = %d.\n", r.line, r.lineComp);
			return (mode & 0x03) +
				(r.line == r.lineComp ? 0x04 : 0x00) +
				(hBlankInt ? 0x08 : 0x00) +
				(vBlankInt ? 0x10 : 0x00) +
				(OAMInt ? 0x20 : 0x00) +
				(CoinInt ? 0x40 : 0x00);

		case 0x02:
			return r.scy;
			
		case 0x03:
			return r.scx;
			
		case 0x04:
			return r.line;

		case 0x05:
			return r.lineComp;
		
		case 0x08: // Object palette 0
			return objpalette[0][0] << 6 | objpalette[0][1] << 4 | objpalette[0][2] << 2 | objpalette[0][3];
			
		case 0x09: // Object palette 1
			return objpalette[1][0] << 6 | objpalette[1][1] << 4 | objpalette[1][2] << 2 | objpalette[1][3];

		case 0x0A:
			return r.winy;
			
		case 0x0B:
			return r.winx;

		default:
			printf("TODO! read from unimplemented 0x%02X\n", addr + 0xFF40);
			return 0x00; // TODO: Unhandled I/O, no idea what GB does here
	}
}

void Graphics::writeByte(uint8_t b, uint16_t addr)
{
	switch (addr) {
		case 0x0:
			r.flags = b;
			break;

		case 0x1: // LCD status, writing is only allowed on the interrupt enable flags
			hBlankInt = (b & 0x08 ? 1 : 0);
			vBlankInt = (b & 0x10 ? 1 : 0);	
			OAMInt = (b & 0x20 ? 1 : 0);	
			CoinInt = (b & 0x40 ? 1 : 0);
			printf("written 0x%02X to lcd STAT\n", b);
			break;
		
		case 0x2:
			r.scy = b;
			break;
			
		case 0x3:
			r.scx = b;
			break;

		case 0x4:
			// This resets LY, if I understand the specs correctly.
			r.line = 0;
			break;

		case 0x5:
			r.lineComp = b;
			break;
			
		case 0x6: // DMA from XX00-XX9F to FE00-FE9F
			for (int i = 0; i <= 0x9F; i++) {
				emu->memory.writeByte(emu->memory.readByte((b << 8) + i), 0xFE00 + i);
			}
			break;
			
		case 0x7: // Background palette
			for (int i = 0; i < 4; i++) {
				bgpalette[i] = (b >> (i * 2)) & 3;
			}
			break;
			
		case 0x8: // Object palette 0
			for (int i = 0; i < 4; i++) {
				objpalette[0][i] = (b >> (i * 2)) & 3;
			}
			break;
			
		case 0x9: // Object palette 1
			for (int i = 0; i < 4; i++) {
				objpalette[1][i] = (b >> (i * 2)) & 3;
			}
			break;

		case 0xA:
			r.winy = b;
			break;
			
		case 0xB:
			r.winx = b;
			break;

		default:
			printf("TODO! write to unimplemented VRAM var 0x%02X\n", addr + 0xFF40);
			break;
	}
}

// Draw it in bright color: 0 = red, 1 = green, 2 = blue
inline void Graphics::setPixelColorDebug(int x, int y, uint8_t color)
{
	if (x >= GB_SCREEN_WIDTH || y >= GB_SCREEN_HEIGHT || x < 0 || y < 0) {
		return;
	}
	
	// rgba
	uint32_t pixel = 0xFF000000 | 0xFF << (8 * color);
	screenPixels[y * GB_SCREEN_WIDTH + x] = pixel;
}


inline void Graphics::setPixelColor(int x, int y, uint8_t color)
{
	uint8_t palettecols[4] = {255, 192, 96, 0};
	
	if (x >= GB_SCREEN_WIDTH || y >= GB_SCREEN_HEIGHT || x < 0 || y < 0) {
		return;
	}
	
	// rgba
	uint32_t pixel = 0xFF000000 | palettecols[color] << 16 | palettecols[color] << 8 | palettecols[color];
	screenPixels[y * GB_SCREEN_WIDTH + x] = pixel;
}

inline void Graphics::setDebugPixelColor(int x, int y, uint8_t color)
{
	uint8_t palettecols[4] = {255, 192, 96, 0};
	
	if (x >= DEBUG_WIDTH || y >= DEBUG_HEIGHT || x < 0 || y < 0) {
		return;
	}
	
	// rgba
	uint32_t pixel = 0xFF000000 | palettecols[color] << 16 | palettecols[color] << 8 | palettecols[color];
	debugTilesetPixels[y * DEBUG_WIDTH + x] = pixel;
}

void Graphics::printDebug()
{
	printf("bgtoggle=%d,spritetoggle=%d,lcdtoggle=%d,bgmap=%d,tileset=%d,scx=%d,scy=%d\n",
		(r.flags & Flag::BG) ? 1 : 0, 
		(r.flags & Flag::SPRITES) ? 1 : 0, 
		(r.flags & Flag::LCD) ? 1 : 0,
		(r.flags & Flag::TILEMAP) ? 1 : 0,
		(r.flags & Flag::TILESET) ? 1 : 0,
		r.scx, r.scy);
		
	printf("spritedata[0].x = 0x%02X.\n", spritedata[0].x);
	printf("0xC000: %02X %02X %02X %02X.\n", readByte(0xC000), readByte(0xC001), readByte(0xC002), readByte(0xC003));
	printf("0xFE00: %02X %02X %02X %02X.\n", readByte(0xFE00), readByte(0xFE01), readByte(0xFE02), readByte(0xFE03));
	/*
	int base = 0x1800;
	int i;
	
	for(i=base; i<=base+0x3FF; i++) {
		if (i%32 == 0) printf("\n");
		printf("%02X ", vram[i]);
	}
	printf("\n");
	*/
}

void Graphics::renderDebugTileset()
{
	int color;

	// lines of tiles
	for (int y = 0; y < 24; ++y)
	{
		// columns of tiles
		for (int x = 0; x < 16; ++x)
		{
			for (int i = 0; i < 8; ++i)
			{
				for (int j = 0; j < 8; ++j)
				{
					color = bgpalette[tileset[y*16 + x][i][j]];
					setDebugPixelColor(x * 8 + j, y * 8 + i, color);
				}
			}
		}
	}
}

void Graphics::renderScanline()
{
	uint16_t yoff, xoff, tilenr;
	uint8_t row, col, px;
	uint8_t color;
	uint8_t bgScanline[160];
	int bit, offset;

	// Background
	if (r.flags & Flag::BG) {
		// Use tilemap 1 or tilemap 0?
		yoff = (r.flags & Flag::TILEMAP) ? TILEMAP_ADDR1 : TILEMAP_ADDR0;
	
		// divide y offset by 8 (to get whole tile)
		// then multiply by 32 (= amount of tiles in a row)
		yoff += (((r.line + r.scy) & 255) >> 3) << 5;
	
		// divide x scroll by 8 (to get whole tile)
		xoff = (r.scx >> 3) & 0x1F;
	
		// row number inside our tile, we only need 3 bits
		row = (r.line + r.scy) & 0x07;
	
		// same with column number
		col = r.scx & 0x07;
	
		// tile number from bgmap
		tilenr = vram[yoff + xoff];

		// TODO: tilemap signed stuff
		//printf("TEST: signed (int8_t)tilenr = %d\n", (int8_t)tilenr);
		if (not (r.flags & Flag::TILESET) and tilenr < 128) {
			tilenr = tilenr + 256;
		}
		
		
		for (int i = 0; i < 160; i++) {
			bgScanline[i] = tileset[tilenr][row][col];

			color = bgpalette[tileset[tilenr][row][col]];
		
			/*
			bit = 1 << (7 - col);
			offset = tilenr * 16 + row * 2;
			color = bgpalette[((vram[offset] & bit) ? 0x01 : 0x00)
								| ((vram[offset+1] & bit) ? 0x02 : 0x00)];
			*/
			
			setPixelColor(i, r.line, color);
		
			col++;
			if (col == 8) {
				col = 0;
				xoff = (xoff + 1) & 0x1F;
				tilenr = vram[yoff + xoff];

				if (not (r.flags & Flag::TILESET) and tilenr < 128) {
					tilenr = tilenr + 256;
				}
			}
		}
	}

	// Window, if enabled and on this line
	if (r.flags & Flag::WINDOW and r.winx >= 0 and r.winx < 167 and r.winy >= 0 and r.winy < 144 and r.line >= (r.winy - 7)) {
		// Use tilemap 1 or tilemap 0?
		yoff = (r.flags & Flag::WINDOWTILEMAP) ? TILEMAP_ADDR1 : TILEMAP_ADDR0;
	
		// divide adjusted winy by 8 (to get whole tile)
		// then multiply by 32 (= amount of tiles in a row)
		yoff += (((r.line - r.winy) & 255) >> 3) << 5;
	
		// divide winx by 8 (to get whole tile)
		xoff = ((r.winx - 7) >> 3) & 0x1F;
	
		// row number inside our tile, we only need 3 bits
		row = (r.line - r.winy) & 0x07;
	
		// same with column number
		col = (r.winx - 7) & 0x07;
	
		// tile number from bgmap
		tilenr = vram[yoff + xoff];

		// TODO: tilemap signed stuff
		//printf("TEST: signed (int8_t)tilenr = %d\n", (int8_t)tilenr);
		if (not (r.flags & Flag::TILESET) and tilenr < 128) {
			tilenr = tilenr + 256;
		}
	
		for (int i = 0; i < 160; i++) {
			bgScanline[i] = tileset[tilenr][row][col];

			color = bgpalette[tileset[tilenr][row][col]];
		
			/*
			bit = 1 << (7 - col);
			offset = tilenr * 16 + row * 2;
			color = bgpalette[((vram[offset] & bit) ? 0x01 : 0x00)
								| ((vram[offset+1] & bit) ? 0x02 : 0x00)];
			*/
			
			setPixelColor(i, r.line, color);
		
			col++;
			if (col == 8) {
				col = 0;
				xoff = (xoff + 1) & 0x1F;
				tilenr = vram[yoff + xoff];

				if (not (r.flags & Flag::TILESET) and tilenr < 128) {
					tilenr = tilenr + 256;
				}
			}
		}
	}

	
	// Sprites
	if (r.flags & Flag::SPRITES) {
		uint8_t spriteHeight = (r.flags & Flag::SPRITESIZE) ? 16 : 8;

		// the upper 8x8 tile is "NN AND FEh", and the lower 8x8 tile is "NN OR 01h".

		for (int i = 0; i < 40; i++) {
			// do we need to draw the sprite?
			//printf("sprite x: %d, sprite y: %d, line: %d\n", spritedata[i].x, spritedata[i].y, r.line);
			if (spritedata[i].y <= r.line and spritedata[i].y > r.line - spriteHeight) {
				// determine row, flip y if wanted
				if (spritedata[i].flags & SpriteFlag::YFLIP) {
					row = (spriteHeight - 1) - (r.line - spritedata[i].y);
				} else {
					row = r.line - spritedata[i].y;
				}
				
				// loop through the columns
				for (int col = 0; col < 8; col++) {
					if (spritedata[i].flags & SpriteFlag::XFLIP) {
						px = spritedata[i].x + (7 - col);
					} else {
						px = spritedata[i].x + col;
					}
					
					// only draw if this pixel's on the screen
					if (px >= 0 and px < 160) {
						uint8_t spriteTile = spritedata[i].tile;
						uint8_t spriteRow = row;

						if (spriteHeight == 16) {
							if (row < 8) {
								spriteTile = spritedata[i].tile & 0xFE;
							} else {
								spriteTile = spritedata[i].tile | 0x01;
								spriteRow = row % 8;
							}
						}



						color = objpalette[(spritedata[i].flags & SpriteFlag::PALETTE) ? 1 : 0]
								[tileset[spriteTile][spriteRow][col]];

						// only draw sprite pixel when color is not 0
						if (tileset[spriteTile][spriteRow][col] != 0) {

							// Always draw if sprite priority bit is zero
							// or if bg px is zero
							if (not (spritedata[i].flags & SpriteFlag::PRIORITY) or bgpalette[bgScanline[px]] == 0) {
								setPixelColor(px, r.line, color);
							}
						}
					}
				}
			}
		}
	}
}

void Graphics::updateTile(uint16_t addr)
{
	int tile = (addr >> 4) & 0x1FF;
	int row = (addr >> 1) & 0x07;
	
	//printf("updateTile! tile=%d, row=%d.\n", tile, row);
	
	for (int col = 0; col < 8; col++) {
		int bit = 1 << (7 - col);
		tileset[tile][row][col] = ((vram[addr] & bit) ? 1 : 0);
		tileset[tile][row][col] |= ((vram[addr+1] & bit) ? 2 : 0);
	}
}

void Graphics::buildSpriteData(uint8_t b, uint16_t addr)
{
	uint16_t spriteNum = addr >> 2;
	
	if (spriteNum < 40) { // Only 40 sprites
		switch (addr & 0x03) {
			case 0: // Y-coord
				spritedata[spriteNum].y = b - 16;
				//printf("Sprite #%d to Y of %d.\n", spriteNum, b-16);
				break;
				
			case 1: // X-coord
				spritedata[spriteNum].x = b - 8;
				//printf("Sprite #%d to X of %d.\n", spriteNum, b-8);
				break;
			
			case 2: // Tile
				spritedata[spriteNum].tile = b;
				break;
				
			case 3: // Flags
				spritedata[spriteNum].flags = b;
				break;
		}
	}
}


void Graphics::renderFrame()
{
	//int w, h;
	//int display_w, display_h;
	//SDL_GetWindowSize(mainWindow, &w, &h);
	//SDL_GL_GetDrawableSize(mainWindow, &display_w, &display_h);

	emu->gui.render();

	
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	ImVec4 clear_color = ImColor(128, 128, 128, 128);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(emu->gui.window);
}

const char *Graphics::modeToString(uint8_t mode) {
	switch (mode) {
		case Mode::HBLANK:
			return "HBLANK";
		case Mode::VBLANK:
			return "VBLANK";
		case Mode::OAM:
			return "OAM";
		case Mode::VRAM:
			return "VRAM";
		default:
			return "(invalid mode)";
	}
}

void Graphics::step()
{
	mclock += emu->cpu.dc;
	switch (mode) {
		case Mode::HBLANK:
			if (mclock >= 51) {
				mclock = 0;
				r.line++;

				//printf("coinInt = %d, line = %d, lineComp = %d\n", CoinInt, r.line, r.lineComp);
				if (CoinInt and r.line == r.lineComp) {
						emu->cpu.intFlags |= CPU::Int::LCDSTAT;
				}

				
				if (r.line == 144) { // last line
					mode = Mode::VBLANK;
					emu->cpu.intFlags |= CPU::Int::VBLANK;

					if (vBlankInt) {
						emu->cpu.intFlags |= CPU::Int::LCDSTAT;
					}

					updateTextures();
				}
				else {
					mode = Mode::OAM;

					if (OAMInt) {
						emu->cpu.intFlags |= CPU::Int::LCDSTAT;
					}
				}
			}
			break;
			
		case Mode::VBLANK:
			if (mclock >= 114) {
				mclock = 0;
				r.line++;
				
				if (r.line > 153) {
					mode = Mode::OAM;
					r.line = 0;

					if (OAMInt) {
						emu->cpu.intFlags |= CPU::Int::LCDSTAT;
					}
				}
			}
			break;
			
		case Mode::OAM:
			if (mclock >= 20) {
				mclock = 0;
				mode = Mode::VRAM;
			}
			break;
			
		case Mode::VRAM:
			if (mclock >= 43) {
				mclock = 0;
				mode = Mode::HBLANK;
				renderScanline();

				if (hBlankInt) {
					emu->cpu.intFlags |= CPU::Int::LCDSTAT;
				}
			}
	}
}
