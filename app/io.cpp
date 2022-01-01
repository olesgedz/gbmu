#include "io.h"
#include "ppu.h"
#include "cpu.h"
#include "memory.h"
#include "bus.h"

#include <map>
#include <unistd.h>

namespace dsemu::io {

typedef byte (*IO_READ_HANDLER)();
typedef void (*IO_WRITE_HANDLER)(byte b);

typedef std::map<ushort, std::pair<IO_READ_HANDLER, IO_WRITE_HANDLER>> HANDLER_MAP;

HANDLER_MAP handlerMap;

byte readScrollX() {
    return ppu::getXScroll();
}

void writeScrollX(byte b) {
    ppu::setXScroll(b);
}

byte readLCDStats() {
    return ppu::lcdStats;
}

void writeLCDStats(byte b) {
    ppu::lcdStats = b;
}

byte readLCDControl() {
    return ppu::lcdControl;
}

void writeLCDControl(byte b) {
    ppu::lcdControl = b;
}

void writeDMA(byte b) {
    for (int i=0; i<0xA0; i++) {
        byte d = bus::read((b * 0x100) + i);
        bus::write(0xFE00 + i, d);
    }

    cpu::extraCycles = 0;
}

byte readDMS() {
    return 0;
}

#define ADD_MEMORY_HANDLER(X) handlerMap[X] = std::make_pair([]() -> byte { if (X < 0x8000) { cout << "OOPS OOB" << endl; sleep(10); } return memory::read(X); }, [](byte b) -> void { memory::write(X, b); });

void init() {
    IO_WRITE_HANDLER noWrite = [](byte b) -> void { };

    handlerMap[0xFF43] = std::make_pair(readScrollX, writeScrollX);
    handlerMap[0xFF42] = std::make_pair(ppu::getYScroll, ppu::setYScroll);
    handlerMap[0xFF40] = std::make_pair(readLCDControl, writeLCDControl);
    handlerMap[0xFF41] = std::make_pair(readLCDStats, writeLCDStats);
    handlerMap[0xFF44] = std::make_pair(ppu::getCurrentLine, noWrite);
    handlerMap[0xFF46] = std::make_pair(readDMS, writeDMA);

    ADD_MEMORY_HANDLER(0xFF45);
    ADD_MEMORY_HANDLER(0xFF47);
    ADD_MEMORY_HANDLER(0xFF48);
    ADD_MEMORY_HANDLER(0xFF49);
}

bool startDown = false;
bool aDown = false;
bool selectDown = false;
bool rightDown = false;
bool leftDown = false;
bool upDown = false;
bool downDown = false;

byte selButtons = 0;
byte selDirs = 0;

byte read(ushort address) {

    HANDLER_MAP::iterator it = handlerMap.find(address);
    
    if (it != handlerMap.end()) {
        return it->second.first();
    }

    if (address == 0xFF00) {
        byte output = 0xCF;


        if (!selButtons) {
            if (startDown) {
                output &= ~(1 << 3);
            } else if (selectDown) {
                output &= ~(1 << 2);
            } else if (aDown) {
                output &= ~(1 << 0);
            }
        }

        if (!selDirs) {
            if (leftDown) {
                output &= ~(1 << 1);
            } else if (rightDown) {
                output &= ~(1 << 0);
            } else if (upDown) {
                output &= ~(1 << 2);
            } else if (downDown) {
                output &= ~(1 << 3);
            }
        }

        return output; //0xC0 | (0xF^moreBit) | (selDirs | selButtons);
    } else if (address == 0xFF01) {
        //cout << endl << "SERIAL READ: " << endl;
        //sleep(2);
        return 0;
    } else if (address == 0xFF02) {
        //cout << endl << "SERIAL IO: " << endl;
        //sleep(2);
        return 0;
    } else if (address == 0xFF03) {
        //cout << endl << "DIV IO: " << endl;
        //sleep(2);
        return 0;
    } else if (address == 0xFF04) {
        //cout << endl << "TIMA IO: " << endl;
        //sleep(2);
        return 0;
    } else if (address == 0xFF44) {
        return (byte)ppu::getCurrentLine();
        //cout << endl << "READ LCD: " << endl;
        //sleep(2);
    } else if (address == 0xFF0F) {
        return cpu::getInterruptsRequestsFlag();
    } else if (address == 0xFFFF) {
        return cpu::getInterruptsEnableFlag();
    }

    if (DEBUG) cout << "UNKNOWN IO READ: " << Short(address) << endl;
    //sleep(5);
    return 0;
}

void write(ushort address, byte b) {

    HANDLER_MAP::iterator it = handlerMap.find(address);
    
    if (it != handlerMap.end()) {
        it->second.second(b);
        memory::write(address, b);
        return;
    }

    if (DEBUG) cout << " NO WRITE MAP: " << Short(address) << endl;
    //sleep(1);

    if (address == 0xFF00) {
        //cout << endl << "JOYPAD WRITE: " << Byte(b) << endl;
        //memory::write(address, b & (0x10 | 0x20));
        selButtons = b & 0x20;
        selDirs = b & 0x10;
        memory::write(address, b);
       // sleep(2);
        return;
    } else if (address == 0xFF01) {
       // cout << endl << "SERIAL WRITE: " << endl;
        //sleep(2);
        memory::write(address, b);
        return;
    } else if (address == 0xFF02) {
        //cout << endl << "SERIAL WRITE: " << endl;
        //sleep(2);
        memory::write(address, b);
        return;
    } else if (address == 0xFF03) {
       // cout << endl << "DIV WRITE: " << endl;
       // sleep(2);
        memory::write(address, b);
        return;
    } else if (address == 0xFF04) {
       // cout << endl << "TIMA WRITE: " << endl;
        //sleep(2);
        memory::write(address, b);
        return;
    } else if (address == 0xFF0F) {
        cpu::setInterruptsRequestsFlag(b);
        memory::write(address, b);
        return;
    } else if (address == 0xFFFF) {
        cpu::setInterruptsEnableFlag(b);
        memory::write(address, b);
        return;
    }

    if (address > 0xFF00) {
        memory::write(address, b);
    }

    if (DEBUG) cout << "UNKNOWN IO WRITE: " << Short(address) << endl;
    //sleep(5);
}

}


