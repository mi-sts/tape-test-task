#pragma once
#include <fstream>
#include <string>
#include <vector>

#include "ITape.h"
#include "TapeConfigData.h"

class FileTape : ITape
{
public:
    FileTape(const std::string& tapeFilePath, const std::string& configFilePath);
    
    bool moveLeft() override;
    bool moveRight() override;
    int read() override;
    void write(int value) override;
    bool rewindLeft(int cellsNumber) override;
    bool rewindRight(int cellsNumber) override;
    
    void resetExecutionTime();
    void initializeTapeData(const std::string& tapeFilePath);
    void initializeTapeConfigData(const std::string& tapeConfigPath);
    
private:
    size_t cellIndex;
    long executionTime;
    std::vector<int> tapeData;
    TapeConfigData tapeConfigData;
};
