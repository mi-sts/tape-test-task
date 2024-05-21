#pragma once
#include "../IRAM.h"

class RAMUtils
{
public:
    static bool writeInt(IRAM& RAM, int value, size_t firstByteIndex);
    static int readInt(IRAM& RAM, size_t firstByteIndex);
};
