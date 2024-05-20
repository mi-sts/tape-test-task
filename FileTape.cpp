#include "FileTape.h"

#include <iostream>

#include "InputUtils.h"
#include "TapeConfigLoader.h"

FileTape::FileTape(const std::string& tapeFilePath, const std::string& configFilePath) : cellIndex(0), executionTime(0)
{
    initializeTapeData(tapeFilePath);
    initializeTapeConfigData(configFilePath);
}

bool FileTape::moveLeft()
{
    if (cellIndex <= 0)
        return false;
    
    --cellIndex;
    executionTime += tapeConfigData.moveNextDelay;
    return true;
}

bool FileTape::moveRight()
{
    if (cellIndex >= tapeData.size() - 1)
        return false;
    
    ++cellIndex;
    executionTime += tapeConfigData.moveNextDelay;
    return true;
}

int FileTape::read()
{
    executionTime += tapeConfigData.readDelay;
    return tapeData[cellIndex];
}

void FileTape::write(int value)
{
    executionTime += tapeConfigData.writeDelay;
    tapeData[cellIndex] = value;
}

bool FileTape::rewindLeft(int cellsNumber)
{
    if (cellIndex < cellsNumber)
        return false;

    cellIndex -= cellsNumber;
    executionTime += tapeConfigData.rewindPerElementDelay * cellsNumber;
    return true;
}

bool FileTape::rewindRight(int cellsNumber)
{
    if (cellIndex + cellsNumber <= tapeData.size() - 1)
        return false;

    cellIndex += cellsNumber;
    executionTime += tapeConfigData.rewindPerElementDelay * cellsNumber;
    return true;
}

void FileTape::resetExecutionTime()
{
    executionTime = 0;
}

void FileTape::initializeTapeData(const std::string& tapeFilePath)
{
    std::ifstream file(tapeFilePath);
    if (!file.is_open())
    {
        std::cerr << "Cannot open the tape data file!" << std::endl;
        return;
    }

    std::string dataString;
    std::getline(file, dataString);
    std::vector<std::string> tapeStringValues = InputUtils::split(dataString, ' ');
    std::vector<int> tapeValues;
    
    try
    {
        for (const std::string& stringValue : tapeStringValues)
        {
            int value = std::stoi(stringValue);
            tapeValues.push_back(value);
        }
    }
    catch (const std::invalid_argument& error)
    {
        std::cerr << "Invalid tape data element!" << std::endl;
        return;
    }

    tapeData = tapeValues;
}

void FileTape::initializeTapeConfigData(const std::string& tapeConfigPath)
{
    TapeConfigLoader::loadConfig(tapeConfigPath, tapeConfigData);
}
