#pragma once
#include <vector>

#include "../IRAM.h"

class RAMUtils
{
public:
    static bool writeInt(IRAM& RAM, int value, size_t firstByteIndex);
    static int readInt(IRAM& RAM, size_t firstByteIndex);
    static std::vector<int> readIntVector(IRAM& RAM, size_t firstByteIndex, int count);
    static bool writeIntVector(IRAM& RAM, const std::vector<int>& intVector, size_t firstByteIndex);
};
