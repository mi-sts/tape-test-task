#pragma once
#include <string>

#include "TapeConfigData.h"

class TapeConfigLoader
{
public:
    static bool loadConfig(const std::string& filePath, TapeConfigData& configData);
};
