#pragma once

#include "common.h"

namespace dsemu::mappers {

class Mapper {
public:
    virtual byte read(ushort address) = 0;
    virtual void control(ushort address, byte b) = 0;

private:
};

class NROMMapper : public Mapper {
public:
    virtual byte read(ushort address);
    virtual void control(ushort address, byte b);

};

class MPC1 : public Mapper {
public:
    virtual byte read(ushort address);
    virtual void control(ushort address, byte b);

private:
    int bank = 0;
};

}
