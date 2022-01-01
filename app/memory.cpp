#include "memory.h"
#include "cpu.h"
#include "ppu.h"
#include <cstring>

namespace dsemu::memory {

    byte ram[0xFFFF + 1];

    void init() {
        ram[0xFF05] = 0x00;
        ram[0xFF06] = 0x00;
        ram[0xFF07] = 0x00;
        ram[0xFF10] = 0x80;
        ram[0xFF11] = 0xBF;
        ram[0xFF12] = 0xF3;
        ram[0xFF14] = 0xBF;
        ram[0xFF16] = 0x3F;
        ram[0xFF17] = 0x00;
        ram[0xFF19] = 0xBF;
        ram[0xFF1A] = 0x7F;
        ram[0xFF1B] = 0xFF;
        ram[0xFF1C] = 0x9F;
        ram[0xFF1E] = 0xBF;
        ram[0xFF20] = 0xFF;
        ram[0xFF21] = 0x00;
        ram[0xFF22] = 0x00;
        ram[0xFF23] = 0xBF;
        ram[0xFF24] = 0x77;
        ram[0xFF25] = 0xf3;
        ram[0xFF26] = 0xf1;
        ram[0xFF40] = 0x91;
        ram[0xFF42] = 0x00;
        ram[0xFF43] = 0x00;
        ram[0xFF45] = 0x00;
        ram[0xFF47] = 0xfc;
        ram[0xFF48] = 0xff;
        ram[0xFF49] = 0xff;
        ram[0xFF4A] = 0x00;
        ram[0xFF4B] = 0x00;
        ram[0xFFFF] = 0x00;

        std::memset(ram, 0, sizeof(ram));
    }

    byte read(ushort address) {

        if (address == 0xFF44) {
            return ppu::getCurrentLine();
        }

        return ram[address];
    }

    void write(ushort address, byte value) {
        ram[address] = value;

        if (address == 0xFFFF) {
            cout << "\t - IE EN REGISTER WRITE: " << Byte(value) << endl;
            cpu::handleInterrupt(value, false);
        } else if (address == 0xFF0F) {
            cout << "\t - IE FL REGISTER WRITE: " << Byte(value) << endl;
            
            //cpu::handleInterrupt(value, true);
        }
    }

}

