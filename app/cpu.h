#pragma once

#include "common.h"

namespace dsemu {
namespace cpu {

struct Register {
    byte lo;
    byte hi;
};

/*

The Zero Flag (Z)
This bit becomes set (1) if the result of an operation has been zero (0). Used for conditional jumps.

The Carry Flag (C, or Cy)
Becomes set when the result of an addition became bigger than FFh (8bit) or FFFFh (16bit). Or when the result of a subtraction or comparision became less than zero (much as for Z80 and 80x86 CPUs, but unlike as for 65XX and ARM CPUs). Also the flag becomes set when a rotate/shift operation has shifted-out a "1"-bit.
Used for conditional jumps, and for instructions such like ADC, SBC, RL, RLA, etc.

The BCD Flags (N, H)
These flags are (rarely) used for the DAA instruction only, N Indicates whether the previous instruction has been an addition or subtraction, and H indicates carry for lower 4bits of the result, also for DAA, the C flag must indicate carry for upper 8bits.
After adding/subtracting two BCD numbers, DAA is intended to convert the result into BCD format; BCD numbers are ranged from 00h to 99h rather than 00h to FFh.
Because C and H flags must contain carry-outs for each digit, DAA cannot be used for 16bit operations (which have 4 digits), or for INC/DEC operations (which do not affect C-flag).

*/

enum Flags {
    FlagZ = 7,
    FlagN = 6,
    FlagH = 5,
    FlagC = 4
};

enum Op {
    NOP,
    LD,
    LDI,
    LDD,
    INC,
    DEC,
    JR,
    DAA,
    ADD,
    SUB,
    RLA,
    RLCA,
    RRA,
    RRCA,
    STOP,
    SCF,
    CPL,
    CCF,
    SBC,
    XOR,
    AOR,
    CP,
    POP,
    PUSH,
    AND,
    OR,
    JP,
    RET,
    DI,
    EI,
    CALL,
    RST,
    CB,
    ADC,
    X,
    RETI,
    LDH,
    HALT
};

enum AddrType {
    ATypeNA,
    ATypeRR,
    ATypeIR,
    ATypeAR,
    ATypeRI,
    ATypeRA,
    ATypeJ,
    ATypeJ_NZ,
    ATypeJ_Z,
    ATypeJ_NC,
    ATypeJ_C,
    ATypeR,
    ATypeA,
    ATypeSP
};

enum ParamType {
    A,
    B,
    C,
    D,
    E,
    H,
    L,
    BC,
    DE,
    HL,
    SP,
    PC,
    N,
    NN,
    AF,
    x00, x10, x20, x30, x08, x18, x28, x38
};

enum Interrupts {
    IVBlank = 1
};

struct OpCode {
    byte value;
    const string &name;
    Op op;
    byte length;
    byte cycles;
    AddrType mode;
    ParamType params[4];
};

extern Register regAF;
extern Register regBC;
extern Register regDE;
extern Register regHL;
extern ushort regPC;
extern Register regSP;

extern OpCode opCodes[];

extern int extraCycles;
extern bool haltWaitingForInterrupt;

inline bool getFlag(Flags n) {
    return getBit(regAF.lo, n);
}

inline void setFlag(Flags n, bool val) {
    setBit(regAF.lo, n, val);
}

inline ushort *getReg16Pointer(const Register &reg) {
    return (ushort *)&reg;
}

inline ushort getReg16Value(const Register &reg) {
    return *getReg16Pointer(reg);
}

inline void setReg16Value(const Register &reg, ushort val) {
    *getReg16Pointer(reg) = val;
}

extern bool paused;

void run();
void handleInterrupt(byte flag, bool request, bool pcp1 = true);
void tick();
void init();
void interrupt(Interrupts i);

byte getInterruptsEnableFlag();
byte getInterruptsRequestsFlag();

void setInterruptsEnableFlag(byte f);
void setInterruptsRequestsFlag(byte f);

uint64_t getTickCount();
void changePC(ushort address);

byte pop();
ushort spop();
void push(byte);
void push(ushort);

}
}

