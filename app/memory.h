#pragma once

#include "common.h"

namespace dsemu::memory {


    extern byte ram[0xFFFF + 1];

    byte read(ushort address);
    void write(ushort address, byte value);

}
