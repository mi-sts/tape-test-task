#include "RAMUtils.h"

#include <iostream>

bool RAMUtils::writeInt(IRAM& RAM, int value, size_t firstByteIndex)
{
    if (firstByteIndex + sizeof(int) - 1 >= RAM.getSize())
    {
        std::cerr << "Cannot write RAM int value, indices is out of range!" << std::endl;
        return false;
    }

    for (int i = 0; i < 4; ++i)
    {
        unsigned char intByte = (value >> ((3 - i) * 8)) & 0xFF;
        RAM.write(intByte, firstByteIndex + i);
    }
}

int RAMUtils::readInt(IRAM& RAM, size_t firstByteIndex)
{
    if (firstByteIndex + sizeof(int) - 1 >= RAM.getSize())
    {
        std::cerr << "Cannot read RAM int value, indices is out of range!" << std::endl;
        return 0;
    }

    int readValue = 0;
    for (int i = 0; i < 4; ++i)
    {
        unsigned char intByte = RAM.read(firstByteIndex + i);
        readValue |= intByte << ((3 - i) * 8);
    }

    return readValue;
}
