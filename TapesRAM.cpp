#include "TapesRAM.h"

#include <iostream>


TapesRAM::TapesRAM(long sizeInBytes) : RAMData(sizeInBytes, 0u)
{
}

bool TapesRAM::write(unsigned char value, size_t index)
{
    if (!haveRAMDataIndex(index))
    {
        std::cerr << "Cannot write RAM value, the index is out of range!" << std::endl;
        return false;
    }

    RAMData[index] = value;
    return true;
}

unsigned char TapesRAM::read(size_t index)
{
    if (!haveRAMDataIndex(index))
    {
        std::cerr << "Cannot read RAM value, the index is out of range!" << std::endl;
        return 0u;
    }

    return RAMData[index];
}

size_t TapesRAM::getSize()
{
    return RAMData.size();
}

bool TapesRAM::haveRAMDataIndex(size_t index)
{
    return index < RAMData.size();
}
