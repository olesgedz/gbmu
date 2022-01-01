#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <unistd.h>
#include <chrono>
#include <thread>

using std::string;
using std::vector;
using std::cout;
using std::endl;

typedef uint8_t byte; 
typedef uint16_t ushort;

inline bool getBit(byte &b, byte n) {
    return (b & (1 << n)) ? 1 : 0;
}

inline void setBit(byte &b, byte n, bool on) {
    if (on) {
        b |= (1 << n);
    } else {
        b &= ~(1 << n);
    }
}

ushort toShort(byte a, byte b);

struct Byte {
    Byte(byte _b) : b(_b) {}
    byte b;
};

struct Short {
    Short(ushort _s) : s(_s) {}
    ushort s;
};

struct Int64 {
    Int64(uint64_t _s) : s(_s) {}
    uint64_t s;
};

inline std::ostream &operator <<(std::ostream &os, Byte b) {
    os << std::hex << std::setfill('0') << std::setw(2) << (int)b.b << std::dec;
    return os;
}

inline std::ostream &operator <<(std::ostream &os, Short s) {
    os << std::hex << std::setfill('0') << std::setw(4) << s.s << std::dec;
    return os;
}

inline std::ostream &operator <<(std::ostream &os, Int64 s) {
    os << std::hex << std::setfill('0') << std::setw(16) << s.s << std::dec;
    return os;
}

inline void sleepMs(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));

}

extern bool DEBUG;
