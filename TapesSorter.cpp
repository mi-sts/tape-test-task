#include "TapesSorter.h"

#include <algorithm>

#include "TapeConfigLoader.h"
#include "utils/FileUtils.h"
#include "utils/RAMUtils.h"

const int MAX_TEMP_FILES_NUMBER = 128;

TapesSorter::TapesSorter(
    const std::string& inputTapeFilePath,
    const std::string& outputTapeFilePath,
    const std::string& configFilePath
) : inputTape(nullptr),
    outputTape(nullptr),
    mergeWaysNumber(2),
    sortedNumbersCount(0),
    mergeToSecondHalf(true),
    currentChunkSize(1),
    isSorted(false)
{
    TapeConfigLoader::loadConfig(configFilePath, tapeConfigData);
    tapesRAM = TapesRAM(1000);
    mergeWaysNumber = std::min<int>(tapeConfigData.RAMBytesSize / 4 / 2, MAX_TEMP_FILES_NUMBER);
    if (mergeWaysNumber % 2 != 0)
        --mergeWaysNumber;
    if (mergeWaysNumber < 2)
    {
        std::cerr << "Not enough RAM for the sorting algorithm!\n"
                     "Increase the parameter value in the config file." << std::endl;
    }
    inputTape = new FileTape(inputTapeFilePath, tapeConfigData);
    outputTape = new FileTape(outputTapeFilePath, tapeConfigData);
    initializeTempTapes();;
}

void TapesSorter::sort()
{
    initializeSortedNumbersCount();
    fillTempTapesWithSortedChunks();

    while (!isSorted)
        mergeChunksToAnotherHalf();

    saveSortedNumbers();
    printStatistics();
}

void TapesSorter::initializeTempTapes()
{
    for (int i = 0; i < mergeWaysNumber * 2; ++i)
    {
        std::fstream fileStream;
        std::string tempFilePath = FileUtils::createTempFile(fileStream);
        tempTapesFilePaths.push_back(tempFilePath);
        tempTapes.push_back(new FileTape(tempFilePath, tapeConfigData));
    }
}

void TapesSorter::freeTempTapes()
{
    for (std::string tempTapeFilePath : tempTapesFilePaths)
        FileUtils::deleteFile(tempTapeFilePath);
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
    if (sortedNumbersCount % mergeWaysNumber != 0)
        ++chunksNumber;

    std::vector<int> tempTapesOffsets(mergeWaysNumber, 0);
    for (int i = 0; i < chunksNumber; ++i)
    {
        int currentTempTapeIndex = i % mergeWaysNumber;
        FileTape* currentTempTape = tempTapes[currentTempTapeIndex];
        int chunkValuesNumber = 0;
        for (int j = 0; j < mergeWaysNumber; ++j)
        {
            int readValue = 0;
            if (!inputTape->read(readValue))
                continue;
            
            inputTape->moveRight();
            RAMUtils::writeInt(tapesRAM, readValue, j * 4);
            ++chunkValuesNumber;
        }

        std::vector<int> chunkValues = RAMUtils::readIntVector(tapesRAM, 0, chunkValuesNumber);
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
    size_t tapesCount = tempTapes.size();
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
    
    std::vector<int> fromTempTapesOffsets(mergeWaysNumber, 0);
    std::vector<int> toTempTapesOffsets(mergeWaysNumber, 0);
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
                ++fromTempTapesOffsets[j];
                ++mergingValuesCount;
                ++currentTempTapeIndex;
            }
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
            ++toTempTapesOffsets[toTapeIndex];
        }
    }

    for (int k = 0; k < mergeWaysNumber; ++k)
    {
        FileTape* fromTempTape = tempTapes[fromTapesIndexOffset + k];
        int tempTapeOffset = fromTempTapesOffsets[k];
        for (int l = 0; l < tempTapeOffset; ++l)
            fromTempTape->moveLeft();
    }

    for (int k = 0; k < mergeWaysNumber; ++k)
    {
        FileTape* toTempTape = tempTapes[toTapesIndexOffset + k];
        int tempTapeOffset = toTempTapesOffsets[k];
        for (int l = 0; l < tempTapeOffset; ++l)
            toTempTape->moveLeft();
    }

    mergeToSecondHalf = !mergeToSecondHalf;
    currentChunkSize *= mergeWaysNumber;

    int notEmptyTapesCount = 0;
    int lastNotEmptyTapeIndex = 0; 
    for (size_t i = 0; i < toTempTapesOffsets.size(); ++i)
    {
        if (toTempTapesOffsets[i] > 0)
        {
            ++notEmptyTapesCount;
            lastNotEmptyTapeIndex = toTapesIndexOffset + i;
        }
    }
    isSorted = notEmptyTapesCount == 1;
    if (isSorted)
        sortedNumbersTempTapeIndex = lastNotEmptyTapeIndex;
}

void TapesSorter::saveSortedNumbers()
{
    FileTape* sortedNumbersTempTape = tempTapes[sortedNumbersTempTapeIndex];
    int currentValue = 0;
    for (int i = 0; i < sortedNumbersCount; ++i)
    {
        if (!sortedNumbersTempTape->read(currentValue))
        {
            std::cerr << "Some sorted numbers are missing!" << std::endl;
            return;
        }

        outputTape->write(currentValue);
        outputTape->moveRight();
        sortedNumbersTempTape->moveRight();
    }
}

int TapesSorter::getTotalExecutionTime()
{
    int totalExecutionTime = 0;
    totalExecutionTime += inputTape->getExecutionTime();
    totalExecutionTime += outputTape->getExecutionTime();
    for (FileTape* tempTape : tempTapes)
        totalExecutionTime += tempTape->getExecutionTime();

    return totalExecutionTime;
}

void TapesSorter::printStatistics()
{
    int totalExecutionTime = getTotalExecutionTime();
    std::cout << "Total execution time: " << totalExecutionTime << "." << std::endl;
    std::cout << "Calculated using config parameters." << std::endl;
}

TapesSorter::~TapesSorter()
{
    delete inputTape;
    delete outputTape;
    for (FileTape* tempTape : tempTapes)
        delete tempTape;

    freeTempTapes();
}
