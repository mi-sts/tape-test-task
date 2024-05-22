#pragma once
#include <vector>

#include "IRAM.h"

class TapesRAM : public IRAM
{
public:
    TapesRAM() = default;
    TapesRAM(long sizeInBytes);
    
    bool write(unsigned char value, size_t index) override;
    unsigned char read(size_t index) override;
    size_t getSize() override;

private:
    std::vector<unsigned char> RAMData;

    bool haveRAMDataIndex(size_t index);
};
