#include "TapesSorter.h"

#include <algorithm>

#include "TapeConfigLoader.h"
#include "utils/FileUtils.h"
#include "utils/RAMUtils.h"

TapesSorter::TapesSorter(
    const std::string& inputTapeFilePath,
    const std::string& outputTapeFilePath,
    const std::string& configFilePath
) : inputTape(nullptr),
    outputTape(nullptr),
    mergeWaysNumber(3u),
    sortedNumbersCount(0),
    isFirstHalf(true),
    currentChunkSize(1)
{
    TapeConfigLoader::loadConfig(configFilePath, tapeConfigData);
    tapesRAM = TapesRAM(tapeConfigData.RAMBytesSize);
    inputTape = new FileTape(inputTapeFilePath, tapeConfigData);
    outputTape = new FileTape(outputTapeFilePath, tapeConfigData);
    initializeTempTapes();

    sort();
}

void TapesSorter::sort()
{
    initializeSortedNumbersCount();
    fillTempTapesWithSortedChunks();
    mergeChunksToAnotherHalf();
}

void TapesSorter::initializeTempTapes()
{
    for (int i = 0; i < mergeWaysNumber; ++i)
    {
        std::fstream fileStream;
        std::string tempFilePath = FileUtils::createTempFile(fileStream);
        tempTapes.push_back(new FileTape(tempFilePath, tapeConfigData));
    }
}

void TapesSorter::initializeSortedNumbersCount()
{
    int currentValue = 0;
    while (inputTape->read(currentValue))
    {
        ++sortedNumbersCount;
        inputTape->moveRight();
    }

    for (int i = 0; i < sortedNumbersCount; ++i)
    {
        inputTape->moveLeft();
    }
}

void TapesSorter::fillTempTapesWithSortedChunks()
{
    long long fullChunksNumber = sortedNumbersCount / mergeWaysNumber;
    for (int i = 0; i < fullChunksNumber; ++i)
    {
        int currentTempTapeIndex = i % mergeWaysNumber;
        FileTape* currentTempTape = tempTapes[currentTempTapeIndex];
        for (int j = 0; j < mergeWaysNumber; ++j)
        {
            int readValue = 0;
            inputTape->read(readValue);
            inputTape->moveRight();
            RAMUtils::writeInt(tapesRAM, readValue, j * 4);
        }

        std::vector<int> chunkValues = RAMUtils::readIntVector(tapesRAM, 0, mergeWaysNumber);
        std::sort(chunkValues.begin(), chunkValues.end());
        RAMUtils::writeIntVector(tapesRAM, chunkValues, 0);

        for (int j = 0; j < mergeWaysNumber; ++j)
        {
            int sortedValue = RAMUtils::readInt(tapesRAM, j * 4);
            currentTempTape->write(sortedValue);
            currentTempTape->moveRight();
        }
    }
}

void TapesSorter::mergeChunksToAnotherHalf()
{
}
