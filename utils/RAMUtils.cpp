#include "RAMUtils.h"

#include <iostream>
#include <vector>

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
    if (firstByteIndex + 3 >= RAM.getSize())
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

std::vector<int> RAMUtils::readIntVector(IRAM& RAM, size_t firstByteIndex, int count)
{
    std::vector<int> intVector;

    if (firstByteIndex + count * 4 - 1 >= RAM.getSize())
    {
        std::cerr << "Cannot read RAM int vector, indices is out of range!" << std::endl;
        return intVector;
    }
    
    for (int i = 0; i < count; ++i)
    {
        intVector.push_back(readInt(RAM, firstByteIndex + i * 4));
    }

    return intVector;
}

bool RAMUtils::writeIntVector(IRAM& RAM, const std::vector<int>& intVector, size_t firstByteIndex)
{
    if (firstByteIndex + intVector.size() * 4 - 1 >= RAM.getSize())
    {
        std::cerr << "Cannot write RAM int vector, indices is out of range!" << std::endl;
        return false;
    }
    
    for (size_t i = 0; i < intVector.size(); ++i)
    {
        if (!writeInt(RAM, intVector[i], firstByteIndex + i * 4))
            return false;
    }

    return true;
}
