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
    bool write(int value) override;
    bool rewindLeft(int cellsNumber) override;
    bool rewindRight(int cellsNumber) override;
    
    void resetExecutionTime();
    void initializeTapeData(const std::string& tapeFilePath);
    void initializeTapeConfigData(const std::string& tapeConfigPath);
    std::streamoff getValueLastDigitPointer(std::streamoff fromPointer);
    std::streamoff getValueFirstDigitPointer(std::streamoff fromPointer);
    std::streamoff getRightValuePointer(std::streamoff initialValuePointer);
    std::streamoff getLeftValuePointer(std::streamoff initialValuePointer);
    bool moveTapePointerRight();
    bool moveTapePointerLeft();
    bool isValueSymbol(char symbol);
    
private:
    std::fstream tapeFile;
    std::string tapeFilePath;
    std::streamoff tapeFilePointer;
    int cellIndex;
    long executionTime;
    TapeConfigData tapeConfigData;
};
