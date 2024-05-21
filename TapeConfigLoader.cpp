#include "TapeConfigLoader.h"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <unordered_map>

#include "utils/InputUtils.h"

bool TapeConfigLoader::loadConfig(const std::string& filePath, TapeConfigData& configData)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Cannot open the tape config file!" << std::endl;
        return false;
    }

    int readDelay = -1;
    int writeDelay = -1;
    int moveNextDelay = -1;
    int rewindPerElementDelay = -1;
    
    std::map<std::string, int*> paramNameToPointerMap = {
        {"READ_DELAY", &readDelay},
        {"WRITE_DELAY", &writeDelay},
        {"MOVE_NEXT_DELAY", &moveNextDelay},
        {"REWIND_PER_ELEMENT_DELAY", &rewindPerElementDelay}
    };

    int processedLinesNumber = 0;
    for (std::string line; std::getline(file, line); ++processedLinesNumber)
    {
        std::vector<std::string> splittedLine = InputUtils::split(line, '=');
        if (splittedLine.empty())
        {
            continue;
        }
        
        if (splittedLine.size() != 2)
        {
            std::cerr << "Incorrect config file format!" << std::endl;
            return false;
        }

        std::string paramName = splittedLine[0];
        std::string paramValueString = splittedLine[1];

        if (paramNameToPointerMap.find(splittedLine[0]) == paramNameToPointerMap.end())
        {
            std::cerr << "Unknown config parameter name!" << std::endl;
            return false;
        }
        int* correspondingParam = paramNameToPointerMap[paramName];
        
        try
        {
            int paramValue = std::stoi(paramValueString);
            if (paramValue < 0)
            {
                std::cerr << "The config parameter value should be a positive integer number!" << std::endl;
                return false;
            }
            *correspondingParam = paramValue;
        }
        catch (const std::invalid_argument& error)
        {
            std::cerr << "Incorrect config parameter value!" << std::endl;
            return false;
        }
    }

    if (processedLinesNumber < paramNameToPointerMap.size())
    {
        std::cerr << "The config file does not contain all parameters!" << std::endl;
        return false;
    }

    configData = TapeConfigData(readDelay, writeDelay, moveNextDelay, rewindPerElementDelay);
    return true;
}
