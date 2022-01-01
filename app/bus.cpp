#include "bus.h"
#include "cart.h"
#include "memory.h"
#include "ppu.h"
#include "io.h"
#include "cpu.h"

#include <unistd.h>
#include <cstring>

namespace dsemu::bus {

    byte bankNumber = 1;

    byte read(ushort address) {
        if (address < 0x8000) {
            return cart::read(address); // memory::read(address);
        } else if (address < 0xA000) {
            return memory::read(address);
        } else if (address < 0xFE00) {
            return memory::read(address);
        } else if (address < 0xFEA0) {
            return ppu::readOAM(address - 0xFE00);
        } else if (address < 0xFEFF) {
            return 0;
        } else if (address < 0xFF80) {
            return io::read(address);
        } else if (address < 0xFFFF) {
            return memory::read(address);
        } else {
            return cpu::getInterruptsEnableFlag();
        }
    }

    void write(ushort address, ushort s) {
        
        write(address, (byte)(s & 0xFF));
        write(address + 1, (byte)((s >> 8) & 0xFF));
    }

    void write(ushort address, byte b) {

        if (address < 0x8000) {
            cart::control(address, b);
        } else if (address < 0xA000) {
            if (address == 0xDD00 || address == 0xDD01) {
                cout << "WRITING TO " << Short(address) << " = " << Byte(b) << endl;
                sleep(2);
            }
            memory::write(address, b);
        } 
        else if (address < 0xFE00) {
            memory::write(address, b);
        } else if (address < 0xFEA0) {
            ppu::writeOAM(address - 0xFE00, b);

        } else if (address < 0xFEFF) {
        } else if (address < 0xFF80) {
            io::write(address, b);
        } else if (address < 0xFFFF) {
            memory::write(address, b);
        } else {
            cpu::setInterruptsEnableFlag(b);
        }
    }

}

