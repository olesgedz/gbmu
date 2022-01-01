#include "mappers.h"
#include "cart.h"
#include "memory.h"
#include <cstring>

using std::memcpy;

namespace dsemu::mappers {

bool init = false;
byte data[0xFFFF];
bool switched = false;

byte NROMMapper::read(ushort address) {

    return cart::g_romData[address];
}

void NROMMapper::control(ushort address, byte b) {
    //nothing to do for this one...
    cout << "CONTROL" << endl;
}

byte MPC1::read(ushort address) {
    if (!init) {
        cout << "INIT" << endl;
        memcpy(data, cart::g_romData, 0x8000);
        init = true;
    }
/*
    if (init) {
        //cout << "READ: " << Short(address) << endl;

        if (switched && address >= 0x4000 && bank > 1) {
            cout << "READ: " << Short(address) << " to " << std::hex << (int)(address + (bank * 0x4000))<< endl;
            sleepMs(500);
        } else if (bank > 1) {
            cout << "BANK0READ: " << Short(address) << endl;
        }

        //return memory::ram[address];
        //return data[address];
    }
*/
    if (address < 0x4000) {
        return cart::g_romData[address];
    }

    if (bank > 1) {
        //cout << "BANK READ: " << (int)bank << " - " << Short(address) << " to " << std::hex << (int)(address + (bank * 0x4000)) << endl;
        //sleepMs(200);
    }

    return cart::g_romData[address + (bank * 0x4000) - 0x4000];
}

void MPC1::control(ushort address, byte b) {
    cout << "CONTROL" << endl;

    if (!init) {
        memcpy(data, cart::g_romData, 0x8000);
        init = true;
    }
/*
    if (init) {
        if (address >= 0x2000 && address <= 0x3FFF) {
            bank = b & 0x1F;
            if(bank == 0 || bank == 0x20 || bank == 0x40 || bank == 0x60)
                bank++;

            switched = true;

            cout << "SWITCHING BANK: " << (int) bank << endl;
            sleepMs(200);

            memcpy(&data[0x4000], cart::g_romData + (bank * 0x4000), 0x4000);
        }
        return;
    }
*/
    if (address >= 0x6000) {
        //TODO: Memory model select...
    } else if (address >= 0x2000 && address <= 0x3FFF) {
        bank = b & 0x1F;

        if (bank == 0 || bank == 0x20 || bank == 0x40 || bank == 0x60) {
            bank++;
        }

        if (bank * 0x4000 > cart::g_romSize - 0x4000) {
            cout << "FAILURE IN MAPPER CONTROL" << endl;
            exit(-1);
        }
    }
}

}

/*
-            if (cart::g_header.cartType == 1 && address >= 0x6000) {
-                cout << "Memory Model Select: " << Byte(b & 1) << endl;
-                sleep(10);
-            } else if (cart::g_header.cartType == 1 && address >= 0x2000 && address <= 0x3FFF) {
-                //cout << "Rom Bank Select: " << Byte(b & 0x1F) << endl;
-
-                byte bank = b & 0x1F;
-                if(bank == 0 || bank == 0x20 || bank == 0x40 || bank == 0x60)
-                    bank++;
-
-                memcpy(&memory::ram[0x4000], cart::g_romData + (bank * 0x4000), 0x4000);
-
-                //cout << "Copied starting at: " << (int)(bank * 0x4000) << " - size: " << cart::g_romSize << endl;
-
-                if ((bank * 0x4000) > cart::g_romSize) {
-                    cout << "TOO BIG" << endl;
-                    sleep(10);
-                }
-
-                //sleep(1);
-            } else if (cart::g_header.cartType == 1 ) {
-                cout << "OTHER: " << Short(address) << endl;
-                sleep(10);
-            }
*/