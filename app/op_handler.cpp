#include "cpu.h"
#include "memory.h"
#include "bus.h"

#include <map>
#include <utility>
#include <unistd.h>

ushort toShort(byte a, byte b) {
    ushort s = b << 8;
    s |= a;

    return s;
}

namespace dsemu::cpu {

typedef int (*HANDLER)(const OpCode &op);

std::map<Op, HANDLER> handlerMap;

enum RegParamType {
    RPT16,
    RPTHi,
    RPTLo
};

typedef std::map<ParamType, std::pair<Register *, RegParamType>> PTM;

PTM paramTypeMap;
bool eiCalled = false;

bool interruptsEnabled;


extern int cpuSpeed;
std::map<byte, byte> jumpCycleMap;

void initParamTypeMap() {
    paramTypeMap[A] = std::make_pair(&regAF, RPTHi);
    paramTypeMap[B] = std::make_pair(&regBC, RPTHi);
    paramTypeMap[C] = std::make_pair(&regBC, RPTLo);
    paramTypeMap[D] = std::make_pair(&regDE, RPTHi);
    paramTypeMap[E] = std::make_pair(&regDE, RPTLo);
    paramTypeMap[H] = std::make_pair(&regHL, RPTHi);
    paramTypeMap[L] = std::make_pair(&regHL, RPTLo);
    paramTypeMap[BC] = std::make_pair(&regBC, RPT16);
    paramTypeMap[SP] = std::make_pair(&regSP, RPT16);
    paramTypeMap[HL] = std::make_pair(&regHL, RPT16);
    paramTypeMap[DE] = std::make_pair(&regDE, RPT16);
    paramTypeMap[AF] = std::make_pair(&regAF, RPT16);

    jumpCycleMap[0x20] = 4;
    jumpCycleMap[0x30] = 4;
    jumpCycleMap[0x28] = 4;
    jumpCycleMap[0x38] = 4;
    jumpCycleMap[0xC0] = 12;
    jumpCycleMap[0xD0] = 12;
    jumpCycleMap[0xC2] = 4;
    jumpCycleMap[0xD2] = 4;
    jumpCycleMap[0xC4] = 12;
    jumpCycleMap[0xD4] = 12;
    jumpCycleMap[0xC8] = 12;
    jumpCycleMap[0xD8] = 12;
    jumpCycleMap[0xCA] = 4;
    jumpCycleMap[0xDA] = 4;
    jumpCycleMap[0xCC] = 12;
    jumpCycleMap[0xDC] = 12;

}

byte *regFromBits(byte reg) {
    byte *pReg = nullptr;

    switch(reg) {
        case 0: pReg = &regBC.hi; break; //cout << "GOT B" << endl; break;
        case 1: pReg = &regBC.lo; break; // cout << "GOT C" << endl; break;
        case 2: pReg = &regDE.hi; break; // cout << "GOT D" << endl; break;
        case 3: pReg = &regDE.lo; break; // cout << "GOT E" << endl; break;
        case 4: pReg = &regHL.hi; break; // cout << "GOT H" << endl; break;
        case 5: pReg = &regHL.lo; break; // cout << "GOT L" << endl; break;
        case 6: pReg = &memory::ram[toShort(regHL.lo, regHL.hi)]; break;
        case 7: pReg = &regAF.hi; break;
            default:
                cout << "INVALID REG " << endl;
                exit(-1);
    }

    return pReg;
}

byte *getPointer(ParamType pt, bool hlAsRam = true) {
    switch(pt) {
        case A: return &regAF.hi;
        case B: return &regBC.hi; 
        case C: return &regBC.lo; 
        case D: return &regDE.hi; 
        case E: return &regDE.lo; 
        case H: return &regHL.hi; 
        case L: return &regHL.lo;
        case AF: return &regAF.lo;
        case BC: return &regBC.lo;
        case DE: return &regDE.lo;
        case SP: return &regSP.lo;
        case N: {
            //byte t = memory::ram[(regPC + 1)];
            //unsigned int i = 
            return &memory::ram[(regPC + 1)]; 
        } case HL: {
            return hlAsRam ? &memory::ram[getReg16Value(regHL)] : &regHL.lo;
        }
        default:
            break;
    }

    cout << "ERROR BAD POINTER" << endl;
    exit(-1);

    return nullptr;
}

int handleGoto(ushort address) {
    return 0;
}

int handleLDH(const OpCode &op) {
    bool srcIsA = op.params[1] == A;

    if (srcIsA) {
        byte *p = getPointer(op.params[0]);
        //cout << "READING B: " << Int64((uint64_t)p) << endl;
        //cout << "B: " << Byte(*p) << endl;
        
        bus::write(*p | 0xFF00, regAF.hi);
    } else {
        byte *p = getPointer(op.params[1]);
        regAF.hi = bus::read(*p | 0xFF00);
    }
    return 0;
}

ushort getAddrValue(ParamType op, short srcValue) {
    switch(op) {
        case HL: return getReg16Value(regHL);
        case AF: return getReg16Value(regAF);
        case BC: return getReg16Value(regBC);
        case DE: return getReg16Value(regDE);
        case NN: return srcValue;
        case N: return 0xFF00 | srcValue;
        default: 
            cout << "ERROR BAD ADDR VAL: " << endl;
            exit(-1);
            return 0;
    }
}

int handleLD(const OpCode &op) {
    RegParamType srcType = RegParamType::RPTHi;
    RegParamType dstType;

    Register *src;
    Register *dst;

    ushort srcValue = 0;
    ushort dstValue = 0;

    PTM::iterator dit = paramTypeMap.find(op.params[0]);
    PTM::iterator sit = paramTypeMap.find(op.params[1]);

    if (sit == paramTypeMap.end()) {
        if (op.params[1] == N) {
            srcValue = bus::read(regPC + 1);
        } else if (op.params[1] == NN) {
            srcValue = bus::read(regPC + 1);
            srcValue |= bus::read(regPC + 2) << 8;

            if (srcValue == 0x1200 || srcValue == 0x0012) {
                cout << "LDing ... " << Short(srcValue) << endl;
                //cpuSpeed = 500;
            }

        } else {
            cout << "Unknown LD Source Type " << endl;
            exit(-1);
        }
    } else {
        src = sit->second.first;
        srcType = sit->second.second;

        switch(srcType) {
            case RPT16: {
                ushort *p = (ushort *)src;
                srcValue = *p;
            } break;
            case RPTHi: {
                srcValue = src->hi;
            } break;
            case RPTLo: {
                srcValue = src->lo;
            } break;
        }
    }

    if (dit == paramTypeMap.end()) {
        dst = nullptr;
        dstType = RegParamType::RPT16;

        if (op.params[0] == N) {
            //
            //exit(-1);
            dstValue = bus::read(regPC + 1);
        } else if (op.params[0] == NN){
            //
            //exit(-1);
            dstValue = bus::read(regPC + 1);
            dstValue |= bus::read(regPC + 2) << 8;
            
        } else {
            cout << "Unknown LD Dest Type " << endl;
            exit(-1);
        }
    } else {
        dst = dit->second.first;
        dstType = dit->second.second;

        switch(dstType) {
            case RPT16: {
                ushort *p = (ushort *)dst;
                dstValue = *p;
            } break;
            case RPTHi: {
                dstValue = dst->hi;
            } break;
            case RPTLo: {
                dstValue = dst->lo;
            } break;
        }
    }

    switch(op.mode) {
        case ATypeRA: {

            ushort addr = getAddrValue(op.params[0], dstValue);

            if (srcType == RPT16) {
                bus::write(addr, srcValue);
            } else {
                bus::write(addr, (byte)srcValue);
            }
        }
            break;
        case ATypeAR: {
            ushort addr = getAddrValue(op.params[1], srcValue);

            if (dstType == RPTLo) {
                dst->lo = bus::read(addr); ;
            } else if (dstType == RPTHi) {
                dst->hi = bus::read(addr); ;
            } else {
                *((ushort *)dst) = bus::read(addr); ;
            }
            
            break;
        } break;
        case ATypeRR:
        case ATypeIR:
            {
                if (dstType == RPTLo) {
                    dst->lo = srcValue;
                } else if (dstType == RPTHi) {
                    dst->hi = srcValue;
                } else {
                    *((ushort *)dst) = srcValue;
                }
            } break;

        case ATypeSP:
        {
            if (op.params[0] == HL) {
                char i = (char)bus::read(regPC + 1);
                setReg16Value(regHL, getReg16Value(regSP) + i);

                setFlag(FlagN, 0);
                setFlag(FlagZ, 0);
                setFlag(FlagC, ((getReg16Value(regSP)+i)&0xFF) < (getReg16Value(regSP)&0xFF));
                setFlag(FlagH, ((getReg16Value(regSP)+i)&0xF) < (getReg16Value(regSP)&0xF));
                
            } else {
                setReg16Value(regSP, getReg16Value(regHL));
            }
        } break;

        default:
            cout << "INVALID OP FLAG: " << dstValue << endl;
            exit(-1);
            return 0;
            
    }
    return 0;

}

int handleNOP(const OpCode &op) {
    //cout << " NOP " << endl;
    //sleep(1);
    return 0;

}

int handleCB(const OpCode &op) {
    int code = bus::read(regPC + 1);
    byte reg = code & 7;
    byte bitOp = (code >> 6) & 3;
    byte bit = (code >> 3) & 7;
    byte *pReg = regFromBits(reg);

    if (bitOp) {
        switch(bitOp) {
            case 1:
                setFlag(FlagZ, !((*pReg) & (1 << bit)));
                setFlag(FlagN, false);
                setFlag(FlagH, true);
                break;
            case 2:
                (*pReg) &= ~(1 << bit);
                break;
            case 3:
                (*pReg) |= (1 << bit);
                break;
            default:
                cout << "INVALID BIT " << endl;
                exit(-1);
        }

        return 0;
    }

    bitOp = bit;
    int cBit = getFlag(FlagC);

    //cout << "BIT OP: " << Byte(bitOp) << endl;

    switch (bitOp) {
        case 0: //RLC
            //(*pReg) <<= (1 + getFlag(FlagC));
            {
                byte old = !!((*pReg) & 0x80);
                (*pReg) <<= 1;
                (*pReg) |= old;
                setFlag(FlagC, old);
                setFlag(FlagZ, !(*pReg));
            }
            break;
        case 1: //RRC
            //(*pReg) >>= (1 + getFlag(FlagC));
            {
                byte old = !!((*pReg) & 1);
                (*pReg) >>= 1;
                setFlag(FlagC, old);
                (*pReg) |= (old << 7);
                setFlag(FlagZ, !(*pReg));
            }
            break;
        case 2: //RL
            setFlag(FlagC, !!((*pReg) & 0x80));
            (*pReg) <<= 1;
            (*pReg) |= cBit;
            setFlag(FlagZ, !(*pReg));
            break;
        case 3: //RR
           // (*pReg) >>= 1;
            setFlag(FlagC, (*pReg) & 1);
            (*pReg) >>= 1;
            (*pReg) |= (cBit << 7);
            setFlag(FlagZ, !(*pReg));
            break;
        case 4: //SLA
            setFlag(FlagC, !!((*pReg) & 0x80));
            (*pReg) <<= 1;
            setFlag(FlagZ, !(*pReg));
            //(*pReg) |= cBit;
            break;
        case 5: //SRA 
            {
            setFlag(FlagC, (*pReg) & 1);
            byte old = (*pReg) & 0x80;
            (*pReg) >>= 1;
            (*pReg) |= old;
            setFlag(FlagZ, !(*pReg));
        }break;
        case 7: //SRL
            setFlag(FlagC, (*pReg) & 1);
            //cout << "SLR " << Byte(*pReg);
            (*pReg) >>= 1;
            setFlag(FlagZ, !(*pReg));
            //cout << " = " << Byte(*pReg) << endl;
            break;
        case 6: //SWAP
            //setFlag(FlagC, 0);
            (*pReg) = (((*pReg) & 0xF0) >> 4) | (((*pReg) & 0xF) << 4);
            regAF.lo = (!(*pReg)) << 7;
            break;
        default:
            cout << "INVALID BIT2 " << endl;
            exit(-1);
    }

    setFlag(FlagH, 0);
    setFlag(FlagN, 0);
    return 0;

}

int conditionalJump(ushort location, const OpCode &op, bool &didJump) {
    int diff = jumpCycleMap[op.value];

    if (op.mode == ATypeJ) {
        regPC = location;
        didJump = true;
        return 0;
    } else if (op.mode == ATypeJ_C && getFlag(FlagC)) {
        regPC = location;
        didJump = true;
        return diff;
    } else if (op.mode == ATypeJ_NC && !getFlag(FlagC)) {
        regPC = location;
        didJump = true;
        return diff;
    } else if (op.mode == ATypeJ_NZ && !getFlag(FlagZ)) {
        regPC = location;
        didJump = true;
        return diff;
    } else if (op.mode == ATypeJ_Z && getFlag(FlagZ)) {
        regPC = location;
        didJump = true;
        return diff;
    }

    return 0;
}

int handleJumpRelative(const OpCode &op) {
    char b = bus::read(regPC + 1);

    if ((byte)b == 0xfe) {
        cout << "INF LOOP? " << endl;
        sleep(5);
    }

    ushort location = regPC + b;
    bool didJump;

    return conditionalJump(location, op, didJump);
}

int handleJump(const OpCode &op) {
    ushort location = 0;
    bool didJump;

    switch(op.params[0]) {
        case NN:
            location = toShort(bus::read(regPC + 1), bus::read(regPC + 2));
            break;
        case HL:
            location = toShort(regHL.lo, regHL.hi);
            break;
        default:
            cout << "ERRO BAD JUMP" << endl;
            exit(-1);

    }

    return conditionalJump(location - op.length, op, didJump);

/*
    if (op.mode == ATypeJ) {
        regPC = location - op.length;
        return 0;
    } else if (op.mode == ATypeJ_C && getFlag(FlagC)) {
        regPC = location - op.length;
        return 4;
    } else if (op.mode == ATypeJ_NC && !getFlag(FlagC)) {
        regPC = location - op.length;
        return 4;
    } else if (op.mode == ATypeJ_NZ && !getFlag(FlagZ)) {
        regPC = location - op.length;
        return 4;
    } else if (op.mode == ATypeJ_Z && getFlag(FlagZ)) {
        regPC = location - op.length;
        return 4;
    }
    return 0;
*/
}

int handleDAA(const OpCode &op) {
    if (!getFlag(FlagN)) {
        ushort a = regAF.hi;
        byte nl = (regAF.hi & 0x0f);
        bool finalVal = false;
        
        if (getFlag(FlagH) || nl > 0x09) {
            a += 6;
        }

        if (getFlag(FlagC) || (a & 0xFFF0) > 0x90) {
            a += 0x60;
            finalVal = true;
        }

        regAF.hi = (byte)(a & 0xFF);
        setFlag(FlagC, finalVal);
    } else {
        if (getFlag(FlagH)) {
            regAF.hi -= 6;
        }

        if (getFlag(FlagC)) {
            regAF.hi -= 0x60;
        } else {
            setFlag(FlagC, false);
        }
    }

    setFlag(FlagZ, regAF.hi == 0);
    setFlag(FlagH, false);
    return 0;
}


ushort lastCallAddress = 0;

int handlePOP(const OpCode &op) {
    ushort *p = (ushort *)getPointer(op.params[0], false);
    
    ushort s = spop();
    cout << "POPPED VALUE: " << Short(s) << endl;

    if (p == (ushort *)&regAF) {
        *p = s & 0xFFF0;
    } else {
        *p = s;
    }

    if (op.value == 0xF1) {
        //sleep(2);
    }
    
    return 0;
}

int handlePUSH(const OpCode &op) {
    ushort *p = (ushort *)getPointer(op.params[0], false);

    push(*p);

    cout << "PUSHED: " << Short(*p) << endl;

    return 0;
}

int callSize = 0;

extern vector<byte> THESTACK;

int handleCALL(const OpCode &op) {
    ushort lca = regPC + op.length;
    bool didJump;
    ushort location = toShort(bus::read(regPC + 1), bus::read(regPC + 2)) - op.length;
    //lastCallAddress = regPC + op.length;

    //for (int i=0; i<callSize; i++) {
    //    cout << " ";
    //}

    //char sz[256];
    //sprintf("%%dc", callSize);

    callSize++;

    if (callSize > 30) {
        callSize = callSize + 0;
    }

    cout << std::setfill('-') << std::setw(callSize) << "-" << "HANDLING CALL: " << Short(regPC) << " CALLSIZE: " << callSize << " STACK: ";


    if (regPC == 0x2200) {
        regPC += 0;
    }

    int ret = conditionalJump(location, op, didJump);

    if (didJump) {
        cpu::push((ushort)(lca));
        lastCallAddress = lca;
    } else {
        cout << "NO" << endl;
    }

    for (size_t i=0; i<THESTACK.size(); i++) {
        cout << Byte(THESTACK[i]) << "-";
    }
    
    cout << endl;

    return ret;
}

bool cameFromI = false;

int handleRET(const OpCode &op) {
    bool didJump = false;
    ushort location = cpu::spop();

    //cout << "HANDLING RET: " << Short(lastCallAddress) << " - " << Short(location) << endl;
    int ret = conditionalJump(location - 1, op, didJump);

    
    if (regPC == 0x3017) {
        regPC = regPC + 0;
    }

    if (didJump) {
        cout << std::setfill('-') << std::setw(callSize) << "-" << "RET - AFTER RET: " << ret << " - " << Short(regPC) << " / " << Short(location) << " CALLSIZE: " << callSize << " STACK: ";

        if (!cameFromI) {
            //for (int i=0; i<callSize; i++) {
            //    cout << " ";
            //}

            callSize--;
        }

        if (callSize < 0) {
            cout << "OOPS" << endl;
        }

        for (size_t i=0; i<THESTACK.size(); i++) {
            cout << Byte(THESTACK[i]) << "-";
        }
        
        cout << endl;
    }

    if (!didJump) {
        cpu::push(location);
    }

    return ret;
}

int handleRETI(const OpCode &op) {
    interruptsEnabled = true;
    cameFromI = true;
    int n = handleRET(op);
    cameFromI = false;

    //cout << "RETI - AFTER RET: " << n << " - " << Short(regPC) << endl;

    return n;
}

void setFlags(byte first, byte second, bool add, bool withCarry) {
    if (add) {
        unsigned int a = first + second + (withCarry ? getFlag(FlagC) : 0);
        byte cf = getFlag(FlagC);
        //setFlag(FlagZ, (a & 0xFF) == 0);
        setFlag(FlagZ, !(a & 0xFF));
        //cout << "AF: 2: " << Byte(regAF.lo) << endl;
        //setFlag(FlagC, a > 0xFF);
        setFlag(FlagC, a >= 0x100);
        //cout << "AF: 3: " << Byte(regAF.lo) << endl;
        setFlag(FlagN, false);
        //cout << "AF: 4: " << Byte(regAF.lo) << endl;
        setFlag(FlagH, ((first&0xF) + (second&0xF) + (withCarry ? cf : 0)) >= 0x10);
        //cout << "AF: 5: " << Byte(regAF.lo) << " - " << Short(a) << endl;
        //setFlag(FlagH, (first & 0xF) + (second & 0xF) + (withCarry && getFlag(FlagC)) > 0xF);
    } else {
        int a = first - second - (withCarry && getFlag(FlagC));
        byte cf = getFlag(FlagC);
        setFlag(FlagZ, (a & 0xFF) == 0);
        setFlag(FlagC, a < 0);
        setFlag(FlagN, true);
        setFlag(FlagH, (first & 0xF) - (second & 0xF) - (withCarry ? cf : 0) < 0);
    }

    return;
}

int handleCP(const OpCode &op) { 
    byte *val = getPointer(op.params[0]);

    setFlags(regAF.hi, *val, false, false);
    return 0;
}

int handleADC(const OpCode &op) {
    byte *val = getPointer(op.params[1]);

    if (op.value == 0xCE) {
        byte t = bus::read(regPC + 1);
        unsigned int i = regAF.hi + t + getFlag(FlagC) >= 0x100;
        //setFlags(regAF.hi, t, true, true);
        setFlag(FlagN, false);
        setFlag(FlagH, ((regAF.hi&0xF) + (t&0xF) + getFlag(FlagC)) >= 0x10);
        regAF.hi = regAF.hi + t + getFlag(FlagC);
        setFlag(FlagC, i);
        setFlag(FlagZ, !regAF.hi);

        return  0;
    }

    unsigned int a = regAF.hi + *val + getFlag(FlagC);

    setFlags(regAF.hi, *val, true, true);

    regAF.hi = a & 0xFF;
    return 0;
}

int handleADD(const OpCode &op) {
    if (op.params[0] == A) {
        byte *p = getPointer(op.params[1]);
        ushort a = regAF.hi + *p;
        setFlags(regAF.hi, *p, true, false);
        regAF.hi = a & 0x00FF;

    } else if (op.params[0] == SP) {
        char e = bus::read(regPC + 1);
        setFlags(getReg16Value(regSP), e, true, false);
        setReg16Value(regSP, getReg16Value(regSP) + e);
        setFlag(FlagZ, false);
    } else {
        ushort *p = (ushort *)getPointer(op.params[1], false);
        ushort *pHL = (ushort *)&regHL;
        int n = *pHL + *p;

        setFlag(FlagC, n >= 0x10000);
        setFlag(FlagN, false);
        setFlag(FlagH, (n & 0xFFF) < (*pHL & 0xFFF));
        *pHL = n & 0xFFFF;
    }

    return 0;
}

int handleSUB(const OpCode &op) {
    byte *val = getPointer(op.params[0]);

    short a = regAF.hi - *val;
    setFlags(regAF.hi, *val, false, false);

    regAF.hi = a & 0x00FF;
    return 0;
}

int handleSBC(const OpCode &op) {
    byte *val = getPointer(op.params[0]);

    short a = regAF.hi - *val - getFlag(FlagC);
    setFlags(regAF.hi, *val, false, true);

    regAF.hi = a & 0x00FF;
    return 0;
}

int handleAND(const OpCode &op) {
    byte *val = getPointer(op.params[0]);

    regAF.hi &= *val;
    
    setFlag(FlagZ, regAF.hi == 0);
    setFlag(FlagN, false);
    setFlag(FlagH, true);
    setFlag(FlagC, 0);
    return 0;
}

int handleOR(const OpCode &op) {
    byte *val = getPointer(op.params[0]);

    regAF.hi |= *val;
    
    setFlag(FlagZ, regAF.hi == 0);
    setFlag(FlagN, false);
    setFlag(FlagH, false);
    setFlag(FlagC, 0);
    return 0;
}

int handleXOR(const OpCode &op) {
    byte *val = getPointer(op.params[0]);

    //cout << "XORING VAL: " << Byte(*val) << endl;

    regAF.hi ^= *val;
    
    setFlag(FlagZ, regAF.hi == 0);
    setFlag(FlagN, false);
    setFlag(FlagH, false);
    setFlag(FlagC, 0);
    return 0;
}

byte getVal(ParamType pt) {
    switch(pt) {
        case A: return regAF.hi; 
        case B: return regBC.hi; 
        case C: return regBC.lo; 
        case D: return regDE.hi; 
        case E: return regDE.lo; 
        case H: return regHL.hi; 
        case L: return regHL.lo; 
        case N: return bus::read(regPC + 1); 
        case HL: return bus::read(toShort(regHL.lo, regHL.hi)); break;
        default:
            cout << "BAD LDD" << endl;
            exit(-1);
            return 0;
    }
}

int handleLDD(const OpCode &op) {
    handleLD(op);
    ushort *p = (ushort *)&regHL;
    (*p)--;
    return 0;
}

int handleLDI(const OpCode &op) {
    handleLD(op);
    ushort *p = (ushort *)&regHL;
    (*p)++;
    return 0;
}

int handleINC(const OpCode &op) {
    ushort val = 0;
    ushort prev = 0;

    switch(op.params[0]) {
        case A: prev = regAF.hi++; val = regAF.hi; break;
        case B: prev = regBC.hi++; val = regBC.hi; break;
        case C: prev = regBC.lo++; val = regBC.lo; break;
        case D: prev = regDE.hi++; val = regDE.hi; break;
        case E: prev = regDE.lo++; val = regDE.lo; break;
        case H: prev = regHL.hi++; val = regHL.hi; break;
        case L: prev = regHL.lo++; val = regHL.lo; break;
        case BC: prev = (*getReg16Pointer(regBC))++; val = (getReg16Value(regBC));  return 0;
        case DE: prev = (*getReg16Pointer(regDE))++; val = (getReg16Value(regDE)); return 0;
        case SP: prev = (*getReg16Pointer(regSP))++; val = (getReg16Value(regSP)); return 0;
        case HL: {
            if (op.mode == ATypeA) {
                byte b = bus::read((getReg16Value(regHL)));
                prev = b++;
                bus::write((getReg16Value(regHL)), b);
                val = b;

            } else {
                prev = (*getReg16Pointer(regHL))++; 
                val = (getReg16Value(regHL)); 
                return 0;
            }
        } break;
        default:
            cout << "INVALID INC FLAG" << endl;
            exit(-1);
            return 0;
    }

    setFlag(FlagZ, val == 0);
    setFlag(FlagN, 0);
    setFlag(FlagH, (prev & 0xF) == 0xF);
    return 0;
}

int handleDEC(const OpCode &op) {
    ushort val = 0;

    switch(op.params[0]) {
        case A: regAF.hi--; val = regAF.hi; break;
        case B: regBC.hi--; val = regBC.hi; break;
        case C: regBC.lo--; val = regBC.lo; break;
        case D: regDE.hi--; val = regDE.hi; break;
        case E: regDE.lo--; val = regDE.lo; break;
        case H: regHL.hi--; val = regHL.hi; break;
        case L: regHL.lo--; val = regHL.lo; break;
        case BC: (*getReg16Pointer(regBC))--; val = (getReg16Value(regBC)); return 0;
        case DE: (*getReg16Pointer(regDE))--; val = (getReg16Value(regDE)); return 0;
        case SP: (*getReg16Pointer(regSP))--; val = (getReg16Value(regSP)); return 0;
        case HL: {
            if (op.mode == ATypeA) {
                byte b = bus::read((getReg16Value(regHL)));
                b--;
                bus::write((getReg16Value(regHL)), b);
                val = b;
            } else {
                (*getReg16Pointer(regHL))--; 
                val = (getReg16Value(regHL)); 
                return 0;
            }
        } break;
        default:
            cout << "INVALID DEC FLAG" << endl;
            exit(-1);
            return 0;
    }

    setFlag(FlagZ, val == 0);
    setFlag(FlagN, 1);
    setFlag(FlagH, (val & 0xF) == 0x0F);
    return 0;
}

int handleRLA(const OpCode &op) {
    byte cf = getFlag(FlagC);
    setFlag(FlagC, !!(regAF.hi & (1 << 7)));
    regAF.hi <<= 1;
    regAF.hi += cf;
    setFlag(FlagZ, false);
    setFlag(FlagH, false);
    setFlag(FlagN, false);
    return 0;
}

int handleRLCA(const OpCode &op) {
    byte b = !!(regAF.hi & 0x80);
    regAF.hi <<= 1;
    regAF.hi |= b;
    
    setFlag(FlagC, b);
    setFlag(FlagZ, false);
    setFlag(FlagH, false);
    setFlag(FlagN, false);

    return 0;
}

int handleRRA(const OpCode &op) {
    byte carry = getFlag(FlagC);

    setFlag(FlagC, regAF.hi & 1);
    regAF.hi >>= 1;
    regAF.hi |= (carry << 7);
    setFlag(FlagZ, false);
    setFlag(FlagH, false);
    setFlag(FlagN, false);

    return 0;
}

int handleCPL(const OpCode &op) {
    regAF.hi = ~regAF.hi;
    setFlag(FlagH, true);
    setFlag(FlagN, true);
    return 0;
}

int handleRRCA(const OpCode &op) {
    byte b = regAF.hi & 1;
    regAF.hi >>= 1;
    regAF.hi |= b << 7;
    
    if (b) {
        setFlag(FlagC, true);
    } else {
        setFlag(FlagC, false);
    }

    setFlag(FlagZ, false);
    setFlag(FlagH, false);
    setFlag(FlagN, false);

    return 0;
}

int handleDI(const OpCode &op) {
    interruptsEnabled = false;
    return 0;
}

int handleEI(const OpCode &op) {
    //interruptsEnabled = true;
    eiCalled = true;
    return 0;
}

int handleSCF(const OpCode &op) {
    setFlag(FlagH, false);
    setFlag(FlagN, false);
    setFlag(FlagC, true);
    return 0;
}

int handleCCF(const OpCode &op) {
    setFlag(FlagH, false);
    setFlag(FlagN, false);
    setFlag(FlagC, !getFlag(FlagC));
    return 0;
}

int handleRST(const OpCode &op) {
    push((ushort)(regPC + 1));

    switch(op.params[0]) {
        case x00: regPC = 0x00; break;
        case x08: regPC = 0x08; break;
        case x10: regPC = 0x10; break;
        case x18: regPC = 0x18; break;
        case x20: regPC = 0x20; break;
        case x28: regPC = 0x28; break;
        case x30: regPC = 0x30; break;
        case x38: regPC = 0x38; break;
        default: {
            cout << "UNKNOWN RST" << endl;
            exit(-1);
        }
    }

    regPC -= 1;
    return 0;
}

int handleHALT(const OpCode &opCode) {
    haltWaitingForInterrupt = true;
    return 0;
}

void init_handlers() {
    handlerMap[LD] = handleLD;
    handlerMap[LDI] = handleLDI;
    handlerMap[LDD] = handleLDD;
    handlerMap[NOP] = handleNOP;
    handlerMap[JP] = handleJump;
    handlerMap[JR] = handleJumpRelative;
    handlerMap[XOR] = handleXOR;
    handlerMap[OR] = handleOR;
    handlerMap[AND] = handleAND;
    handlerMap[INC] = handleINC;
    handlerMap[DEC] = handleDEC;
    handlerMap[RRCA] = handleRRCA;
    handlerMap[RRA] = handleRRA;
    handlerMap[RLCA] = handleRLCA;
    handlerMap[RLA] = handleRLA;

    handlerMap[ADD] = handleADD;
    handlerMap[ADC] = handleADC;
    handlerMap[SUB] = handleSUB;
    handlerMap[SBC] = handleSBC;
    handlerMap[CP] = handleCP;
    handlerMap[DI] = handleDI;
    handlerMap[EI] = handleEI;
    handlerMap[LDH] = handleLDH;
    handlerMap[RST] = handleRST;
    handlerMap[CALL] = handleCALL;
    handlerMap[RET] = handleRET;
    handlerMap[RETI] = handleRETI;
    handlerMap[DAA] = handleDAA;
    handlerMap[PUSH] = handlePUSH;
    handlerMap[POP] = handlePOP;
    handlerMap[CB] = handleCB;
    handlerMap[CPL] = handleCPL;
    handlerMap[HALT] = handleHALT;
    handlerMap[SCF] = handleSCF;
    handlerMap[CCF] = handleCCF;
    handlerMap[STOP] = handleNOP;

    initParamTypeMap();
}

int handle_op(OpCode &opCode) {
    std::map<Op, HANDLER>::iterator it = handlerMap.find(opCode.op);

    if (it == handlerMap.end()) {
        cout << "UNKNOWN OP CODE: " << Byte(opCode.value) << endl;
        exit(-1);
    }

    int ret = it->second(opCode);

    if (opCode.op != EI && eiCalled) {
        eiCalled = false;
        interruptsEnabled = true;
    }

    return ret;
}

}
