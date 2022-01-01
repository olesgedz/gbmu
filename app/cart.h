#pragma once

#include "common.h"

namespace dsemu::cart {

struct Header {
    byte entry[4];
    byte logo[0x30];

    char title[16];
    byte gbcFlag;
    byte licCode[2];
    byte sgbFlag;
    byte cartType;
    byte romSize;
    byte ramSize;
    byte japFlag;
    byte oldLicCode;
    byte maskRomVersion;
    byte headerChecksum;
    byte globalChecksum[2];
};

extern Header g_header;
extern byte *g_romData;
extern int g_romSize;

bool load(const string &romFile);
byte read(ushort address);
void control(ushort address, byte b);

}
