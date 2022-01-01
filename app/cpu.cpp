#include "cpu.h"
#include "memory.h"
#include "ppu.h"
#include "bus.h"

#include <fstream>
#include <sstream>
#include <SDL2/SDL.h>

namespace dsemu {
namespace cpu {

extern bool interruptsEnabled;
bool haltWaitingForInterrupt = false;

void init_handlers();
int handle_op(OpCode &opCode);

std::ofstream olog;

Register regAF;
Register regBC;
Register regDE;
Register regHL;
int extraCycles;

Register regSP;
ushort regPC = 0;
int remainingTicks = 0;
bool vbRequested = false;
bool vbEnabled = false;
byte intEnableFlag = 0;
byte intRequestFlag = 0;
bool paused = false;

uint64_t totalTicks = 0;


vector<byte> THESTACK;

void push(ushort s) {
    setReg16Value(regSP, getReg16Value(regSP) - 2);
    bus::write(getReg16Value(regSP), s);
    THESTACK.push_back(s & 0xFF);
    THESTACK.push_back((s >> 8) & 0xFF);
}

void push(byte b) {
    setReg16Value(regSP, getReg16Value(regSP) - 1);
    bus::write(getReg16Value(regSP), b);
    THESTACK.push_back(b);
}

byte pop() {
    byte lo = bus::read(getReg16Value(regSP));
    setReg16Value(regSP, getReg16Value(regSP) + 1);
    THESTACK.pop_back();
    
    return lo;
}

ushort spop() {
    byte lo = pop();
    byte hi = pop();

    return toShort(lo, hi);
}

uint64_t getTickCount() {
    return totalTicks;
}

void init() {
    regPC = 0x100;
    *((short *)&regAF) = 0x01B0;
    *((short *)&regBC) = 0x0013;
    *((short *)&regDE) = 0x00D8;
    *((short *)&regHL) = 0x014D;
    *((short *)&regSP) = 0xFFFE;

    init_handlers();

    //olog.open("./emu.log");
}

int cpuSpeed = 0;
int n = 0;

void tick() {
    totalTicks++;

    if (remainingTicks) {
        remainingTicks--;
        return;
    }

    if (!haltWaitingForInterrupt) {
        byte b = bus::read(regPC);

        OpCode opCode = opCodes[b];
        n++;
        
        if ((n % 1000) == 0) {
            usleep(1);
        }

        if (n == 0xeae8a) {
            //paused = true;
        }

        std::stringstream ss;
        ss << Int64(n) << ": " << Short(regPC) << ": " << Byte(b) << " " << Byte(bus::read(regPC + 1)) << " " << Byte(bus::read(regPC + 2)) << " (" << std::left << std::setfill(' ') << std::setw(10) << opCode.name << ") "
                << std::right
                << " - AF: " << Short(toShort(regAF.lo, regAF.hi))
                << " - BC: " << Short(toShort(regBC.lo, regBC.hi))
                << " - DE: " << Short(toShort(regDE.lo, regDE.hi))
                << " - HL: " << Short(toShort(regHL.lo, regHL.hi))
                << " - SP: " << Short(toShort(regSP.lo, regSP.hi))
                << " - Cycles: " << (totalTicks - 1)
                << endl;

        if (DEBUG) cout << ss.str();

        int n = handle_op(opCode);

        if (opCode.value == 0xff) {
            cout << "HIT FF" << endl;
            //sleep(5);
        }

        regPC += opCode.length;

        remainingTicks = ((n + opCode.cycles) / 4) - 1;

        if (extraCycles) {
            remainingTicks += extraCycles;
            extraCycles = 0;
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(cpuSpeed));

    if (interruptsEnabled && bus::read(0xFF0F)) {
        handleInterrupt(bus::read(0xFF0F), true, false);
    }
}

void handleInterrupt(byte flag, bool request, bool pcp1) {
    if (flag & 1) {
        if (request) vbEnabled = true; else vbRequested = true;

        //cout << "V-Blank " << (request ? "requested" : "enabled") << endl;

        if (request) {
            intRequestFlag = flag;
        }

        if (request && interruptsEnabled && (intEnableFlag & 1)) {
            //cout << "CPU:> HANDLING VBLANK" << endl;
            intRequestFlag &= ~1;
            //cout << "Disabling VBlank: " << Byte(intRequestFlag) << endl;

            if (pcp1) {
                push((ushort)(regPC + 1));
            } else {
                push(regPC);
            }

            if (haltWaitingForInterrupt) {
                //cout << "RESUMING" << endl;
            }

            haltWaitingForInterrupt = false;
            regPC = 0x40;
            interruptsEnabled = false;
            //changePC(0x40);
            //cpuSpeed = 500;
        } else {
           // cout << "DENIED" << endl;
        }
    } else if (flag & 2) {
        intRequestFlag |= flag;

        if (request && interruptsEnabled && (intEnableFlag & 2)) {
            intRequestFlag &= ~2;

            if (pcp1) {
                push((ushort)(regPC + 1));
            } else {
                push(regPC);
            }

            if (haltWaitingForInterrupt) {
                //cout << "RESUMING" << endl;
            }

            haltWaitingForInterrupt = false;
            regPC = 0x48;
            interruptsEnabled = false;
        }
    }else if (flag & 4) {
        intRequestFlag |= flag;

        if (request && interruptsEnabled && (intEnableFlag & 4)) {
            intRequestFlag &= ~4;

            if (pcp1) {
                push((ushort)(regPC + 1));
            } else {
                push(regPC);
            }

            if (haltWaitingForInterrupt) {
                //cout << "RESUMING" << endl;
            }

            haltWaitingForInterrupt = false;
            regPC = 0x50;
            interruptsEnabled = false;
        }
    }
    else if (flag) {
        cout << "OK ANOTHER INTERRUPT: " << Byte(flag) << endl;
        sleep(10);
    } else {
        intRequestFlag = flag;
    }
}


byte getInterruptsEnableFlag() {
    return intEnableFlag;
}
byte getInterruptsRequestsFlag() {
    return intRequestFlag;
}

void setInterruptsEnableFlag(byte f) {
    intEnableFlag = f;
}

void setInterruptsRequestsFlag(byte f) {
    intRequestFlag = f;
}




}
}

