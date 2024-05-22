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

    ~TapesSorter();

private:
    TapeConfigData tapeConfigData;
    TapesRAM tapesRAM;
    FileTape* inputTape;
    FileTape* outputTape;
    std::vector<FileTape*> tempTapes;
    std::vector<std::string> tempTapesFilePaths;
    int mergeWaysNumber;
    long long sortedNumbersCount;
    bool mergeToSecondHalf;
    long long currentChunkSize;
    bool isSorted;
    int sortedNumbersTempTapeIndex = 0;
    std::vector<int> sortedNumbers;

    void initializeTempTapes();
    void freeTempTapes();

    void initializeSortedNumbersCount();
    void fillTempTapesWithSortedChunks();
    void mergeChunksToAnotherHalf();
    void saveSortedNumbers();
    int getTotalExecutionTime();
    void printStatistics();
};
