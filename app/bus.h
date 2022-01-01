#pragma once
#include "common.h"

namespace dsemu::bus {

byte read(ushort address);
void write(ushort address, byte b);
void write(ushort address, ushort s);

}
