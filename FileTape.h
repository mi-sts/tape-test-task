#pragma once
#include <fstream>
#include <string>

#include "ITape.h"
#include "TapeConfigData.h"

class FileTape : ITape
{
public:
    FileTape(const std::string& tapeFilePath, const TapeConfigData& tapeConfigData);
    
    bool moveTapeRight() override;
    bool moveTapeLeft() override;
    bool read(int& value) override;
    bool write(int value) override;
    bool isEmpty() override;
    bool clearValue() override;
    bool rewindTapeRight(int cellsNumber) override;
    bool rewindTapeLeft(int cellsNumber) override;
    long long getExecutionTime();

    ~FileTape();
    
private:
    std::fstream tapeFile;
    std::string tapeFilePath;
    std::streamoff tapeFilePointer;
    int cellIndex;
    long long executionTime;
    const TapeConfigData& tapeConfigData;

    bool writeString(std::string str);
    void resetExecutionTime();
    void initializeTapeData(const std::string& tapeFilePath);
    std::streamoff getValueLastDigitPointer(std::streamoff fromPointer);
    std::streamoff getValueFirstDigitPointer(std::streamoff fromPointer);
    std::streamoff getRightValuePointer(std::streamoff initialValuePointer);
    std::streamoff getLeftValuePointer(std::streamoff initialValuePointer);
    bool moveTapePointerRight();
    bool moveTapePointerLeft();
    bool isValueSymbol(char symbol);
};
