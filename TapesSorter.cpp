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
    mergeWaysNumber(4u),
    sortedNumbersCount(0),
    mergeToSecondHalf(true),
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
    for (int i = 0; i < mergeWaysNumber * 2; ++i)
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
    long long chunksNumber = sortedNumbersCount / mergeWaysNumber;
    if (sortedNumbersCount % mergeWaysNumber)
        ++chunksNumber;

    std::vector<int> tempTapesOffsets(mergeWaysNumber, 0);
    for (int i = 0; i < chunksNumber; ++i)
    {
        int currentTempTapeIndex = i % mergeWaysNumber;
        FileTape* currentTempTape = tempTapes[currentTempTapeIndex];
        for (int j = 0; j < mergeWaysNumber; ++j)
        {
            int readValue = 0;
            if (!inputTape->read(readValue))
                continue;
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
            ++tempTapesOffsets[currentTempTapeIndex];
        }
    }

    for (int i = 0; i < mergeWaysNumber; ++i)
    {
        const int tempTapeOffset = tempTapesOffsets[i];
        for (int j = 0; j < tempTapeOffset; ++j)
            tempTapes[i]->moveLeft();
    }
    
    currentChunkSize = mergeWaysNumber;
}

void TapesSorter::mergeChunksToAnotherHalf()
{
    int tapesCount = tempTapes.size();
    int fromTapesIndexOffset = 0;
    int toTapesIndexOffset = tapesCount / 2;
    if (!mergeToSecondHalf)
    {
        fromTapesIndexOffset = tapesCount / 2;
        toTapesIndexOffset = 0;
    }

    long long chunksNumber = sortedNumbersCount / currentChunkSize;
    if (sortedNumbersCount % currentChunkSize != 0)
        ++chunksNumber;

    long long oneTapeChunksNumber = chunksNumber / mergeWaysNumber;
    if (chunksNumber % mergeWaysNumber != 0)
        ++oneTapeChunksNumber;
    
    std::vector<int> currentChunkTapesIndices(mergeWaysNumber, 0);
    
    for (int i = 0; i < oneTapeChunksNumber; ++i)
    {
        std::vector<std::vector<int>> mergingChunks(mergeWaysNumber, std::vector<int>());
        int mergingValuesCount = 0;
        for (int j = 0; j < mergeWaysNumber; ++j)
        {
            FileTape* currentTempTape = tempTapes[fromTapesIndexOffset + j];
            int currentValue = 0;
            int currentTempTapeIndex = 0;
            while (currentTempTapeIndex < currentChunkSize && currentTempTape->read(currentValue))
            {
                mergingChunks[j].push_back(currentValue);
                currentTempTape->clearValue();
                currentTempTape->moveRight();
                ++currentTempTapeIndex;
                ++mergingValuesCount;
            }

            for (int k = 0; k < currentTempTapeIndex; ++k)
                currentTempTape->moveLeft();
        }

        std::vector<int> mergedChunks;
        int mergedValues = 0;
        while (mergedValues < mergingValuesCount)
        {
            int minValue = INT_MAX;
            int minValueTapeIndex = 0;
            for (int j = 0; j < mergeWaysNumber; ++j)
            {
                if (mergingChunks[j].empty())
                    continue;
                
                int currentValue = mergingChunks[j][0];
                if (currentValue < minValue)
                {
                    minValue = currentValue;
                    minValueTapeIndex = j;
                }
            }

            mergingChunks[minValueTapeIndex].erase(mergingChunks[minValueTapeIndex].begin());
            mergedChunks.push_back(minValue);
            ++mergedValues;
        }

        int toTapeIndex = i % mergeWaysNumber;
        FileTape* toTempTape = tempTapes[toTapesIndexOffset + toTapeIndex];
        for (int value : mergedChunks)
        {
            toTempTape->write(value);
            toTempTape->moveRight();
        }

        /*for (int j = 0; j < mergedChunks.size(); ++j)
            toTempTape->moveLeft();*/
    }
}
