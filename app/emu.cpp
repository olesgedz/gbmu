#include "emu.h"
#include "ppu.h"
#include "memory.h"
#include "cpu.h"
#include "bus.h"
#include "io.h"

bool DEBUG = false;

namespace dsemu {

void run() {
    io::init();
    cpu::init();
    ppu::init();

    while(true) {
        if (cpu::paused) {
            sleepMs(1); //500);
            continue;
        }

        cpu::tick();
        ppu::tick();
    }
}

}
