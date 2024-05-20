#pragma once
#include "IRAM.h"

class TapeRAM : public IRAM
{
public:
    bool write(unsigned char value, int index) override;
    unsigned char read(int index) override;
    bool writeInt(int value, int firstByteIndex) override;
    int readInt(int firstByteIndex) override;
};
