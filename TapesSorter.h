#pragma once
#include <string>

#include "FileTape.h"
#include "TapeConfigData.h"
#include "TapesRAM.h"

class TapesSorter
{
public:
    TapesSorter(
        const std::string& inputTapeFilePath,
        const std::string& outputTapeFilePath,
        const std::string& configFilePath
    );

    void sort();

private:
    TapeConfigData tapeConfigData;
    TapesRAM tapesRAM;
    FileTape* inputTape;
    FileTape* outputTape;
    std::vector<FileTape*> tempTapes;
    unsigned char mergeWaysNumber;
    long long sortedNumbersCount;
    bool isFirstHalf;
    long long currentChunkSize;

    void initializeTempTapes();

    void initializeSortedNumbersCount();
    void fillTempTapesWithSortedChunks();
    void mergeChunksToAnotherHalf();
};
