#include "FileTape.h"

#include <iostream>

#include "TapeConfigLoader.h"
#include "utils/FileUtils.h"
#include "utils/InputUtils.h"

const char ValuesDelimiterSymbol = '|';
const char EmptyValueSymbol = ' ';

FileTape::FileTape(const std::string& tapeFilePath, const std::string& configFilePath) :
    tapeFilePath(tapeFilePath), tapeFilePointer(0), cellIndex(0), executionTime(0)
{
    initializeTapeData(tapeFilePath);
    initializeTapeConfigData(configFilePath);
}

bool FileTape::moveLeft()
{
    if (cellIndex == 0)
    {
        std::cerr << "Cannot move tape to the left!" << std::endl;
        return false;
    }
        
    const bool isMoved = moveTapePointerLeft();
    if (isMoved)
    {
        executionTime += tapeConfigData.moveNextDelay;
        cellIndex -= 1;
    }

    return isMoved;
}

bool FileTape::moveRight()
{
    const bool isMoved = moveTapePointerRight();
    if (isMoved)
    {
        executionTime += tapeConfigData.moveNextDelay;
        cellIndex += 1;
    }
    
    return isMoved;
}

int FileTape::read()
{
    if (!tapeFile.is_open() || tapeFile.fail())
    {
        std::cerr << "Cannot read the tape data file!" << std::endl;
        return 0;
    }

    tapeFile.seekg(tapeFilePointer, std::ios::beg);
    int number = 0;
    char numberSymbol;
    while (tapeFile.get(numberSymbol) && std::isdigit(numberSymbol))
    {
        number = number * 10 + (numberSymbol - '0');
    }

    executionTime += tapeConfigData.readDelay;
    return number;
}

bool FileTape::write(int value)
{
    if (!tapeFile.is_open() || tapeFile.fail())
    {
        std::cerr << "Cannot read the tape data file!" << std::endl;
        return false;
    }

    std::fstream tempFile;
    std::string tempFilePath = FileUtils::createTempFile(tempFile);
    if (tempFilePath.empty())
    {
        std::cerr << "Cannot create the temp file!" << std::endl;
        return false;
    }

    std::streamoff tapeFileSize = FileUtils::getFileSize(tapeFile);
    
    std::vector<char> leftPartBuffer(tapeFilePointer, 0);
    tapeFile.seekg(0, std::ios::beg);
    tapeFile.read(leftPartBuffer.data(), leftPartBuffer.size());
    tempFile.write(leftPartBuffer.data(), leftPartBuffer.size());

    const std::string valueString = std::to_string(value);
    tempFile.write(valueString.c_str(), valueString.size());

    const std::streamoff previousValueLastDigitPointer = getValueLastDigitPointer(tapeFilePointer);
    if (previousValueLastDigitPointer != tapeFileSize - 1)
    {
        std::vector<char> rightPartBuffer(tapeFileSize - (previousValueLastDigitPointer + 1), 0);
        tapeFile.seekg(previousValueLastDigitPointer + 1, std::ios::beg);
        tapeFile.read(rightPartBuffer.data(), rightPartBuffer.size());
        tempFile.write(rightPartBuffer.data(), rightPartBuffer.size());
    }

    tapeFile.close();
    tapeFile.open(tapeFilePath, std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);

    std::streamoff tempFileSize = FileUtils::getFileSize(tempFile);
    std::vector<char> tempFileBuffer(tempFileSize, 0);
    tempFile.seekg(0);
    tempFile.read(tempFileBuffer.data(), tempFileBuffer.size());
    tempFile.close();
    if (!FileUtils::deleteFile(tempFilePath))
    {
        std::cerr << "Cannot delete the temp file!" << std::endl;
    }
    
    tapeFile.write(tempFileBuffer.data(), tempFileBuffer.size());
    executionTime += tapeConfigData.writeDelay;

    return true;
}

bool FileTape::rewindLeft(int cellsNumber)
{
    if (cellIndex < cellsNumber)
    {
        return false;
    }

    int movementsNumber = 0;
    for (movementsNumber = 0; movementsNumber < cellsNumber; ++movementsNumber)
    {
        const bool isMoved = moveTapePointerLeft();
        if (!isMoved)
            break;
    }

    cellIndex -= movementsNumber;
    executionTime += tapeConfigData.rewindPerElementDelay * movementsNumber;
    return movementsNumber == cellsNumber;
}

bool FileTape::rewindRight(int cellsNumber)
{
    int movementsNumber = 0;
    for (movementsNumber = 0; movementsNumber < cellsNumber; ++movementsNumber)
    {
        const bool isMoved = moveTapePointerRight();
        if (!isMoved)
            break;
    }
    
    cellIndex += movementsNumber;
    executionTime += tapeConfigData.rewindPerElementDelay * movementsNumber;
    return movementsNumber == cellsNumber;
}

void FileTape::resetExecutionTime()
{
    executionTime = 0;
}

void FileTape::initializeTapeData(const std::string& tapeFilePath)
{
    tapeFile.open(tapeFilePath, std::ios::binary | std::ios::in | std::ios::out);
    if (!tapeFile.is_open())
    {
        std::cerr << "Cannot open the tape data file!" << std::endl;
    }
}

void FileTape::initializeTapeConfigData(const std::string& tapeConfigPath)
{
    TapeConfigLoader::loadConfig(tapeConfigPath, tapeConfigData);
}

std::streamoff FileTape::getValueLastDigitPointer(std::streamoff fromPointer)
{
    if (!tapeFile.is_open())
    {
        std::cerr << "Cannot open the tape data file!" << std::endl;
        return 0;
    }

    std::streamoff initialTapeFilePointer = tapeFile.tellg();
    tapeFile.seekg(fromPointer);
    std::streamoff currentPointer = fromPointer;
    std::streamoff tapeFileSize = FileUtils::getFileSize(tapeFile);

    char currentSymbol;
    tapeFile.get(currentSymbol);
    if (currentPointer <= tapeFileSize - 1 && !isValueSymbol(currentSymbol))
    {
        std::cerr << "The pointer does not point to a digit!" << std::endl;
        tapeFile.seekg(initialTapeFilePointer);
        return 0;
    }
    
    ++currentPointer;
    while (currentPointer <= tapeFileSize - 1 && isValueSymbol(currentSymbol))
    {
        tapeFile.get(currentSymbol);
        ++currentPointer;
    }

    if (currentPointer != tapeFileSize && currentSymbol != ValuesDelimiterSymbol)
    {
        std::cerr << "Invalid values delimiter symbol!" << std::endl;
        return 0;
    }
    
    std::streamoff lastDigitPointer = 0;
    if (currentPointer != tapeFileSize)
    {
        lastDigitPointer = tapeFile.tellg();
        lastDigitPointer -= 2;
    }
    else
    {
        lastDigitPointer = tapeFileSize - 1;
    }

    tapeFile.seekg(initialTapeFilePointer);
    return lastDigitPointer;
}

std::streamoff FileTape::getValueFirstDigitPointer(std::streamoff fromPointer)
{
    if (!tapeFile.is_open())
    {
        std::cerr << "Cannot open the tape data file!" << std::endl;
        return 0;
    }

    std::streamoff initialTapeFilePointer = tapeFile.tellg();
    tapeFile.seekg(fromPointer);
    std::streamoff currentPointer = fromPointer;
    
    char currentSymbol;
    tapeFile.read(&currentSymbol, 1);
    if (currentPointer >= 0 && !isValueSymbol(currentSymbol))
    {
        std::cerr << "The pointer does not point to a digit!" << std::endl;
        tapeFile.seekg(initialTapeFilePointer);
        return 0;
    }
        
    while (currentPointer >= 0 && isValueSymbol(currentSymbol))
    {
        tapeFile.seekg(currentPointer);
        tapeFile.read(&currentSymbol, 1);
        --currentPointer;
    }

    if (currentPointer >= 0 && currentSymbol != ValuesDelimiterSymbol)
    {
        std::cerr << "Invalid values delimiter symbol!" << std::endl;
        return 0;
    }
    
    std::streamoff firstDigitPointer = 0;
    if (currentPointer >= 0)
    {
        firstDigitPointer = currentPointer + 2;
    }

    tapeFile.seekg(initialTapeFilePointer);
    return firstDigitPointer;
}

std::streamoff FileTape::getRightValuePointer(std::streamoff initialValuePointer)
{
    std::streamoff tapeFileSize = FileUtils::getFileSize(tapeFile);
    std::streamoff initialValueLastDigitPointer = getValueLastDigitPointer(initialValuePointer);
    
    if (initialValueLastDigitPointer > tapeFileSize - 3)
    {
        std::cerr << "Cannot get the right value!" << std::endl;
        return 0;
    }

    return initialValueLastDigitPointer + 2;
}

std::streamoff FileTape::getLeftValuePointer(std::streamoff initialValuePointer)
{
    if (initialValuePointer < 2)
    {
        std::cerr << "Cannot get the left value!" << std::endl;
        return 0;
    }

    std::streamoff leftValueLastDigitPointer = initialValuePointer - 2;
    std::streamoff leftValueFirstDigitPointer = getValueFirstDigitPointer(leftValueLastDigitPointer);

    return leftValueFirstDigitPointer;
}

bool FileTape::moveTapePointerRight()
{
    std::streamoff tapeFileSize = FileUtils::getFileSize(tapeFile);
    std::streamoff currentValueLastDigitPointer = getValueLastDigitPointer(tapeFilePointer);
    if (currentValueLastDigitPointer == tapeFileSize - 1)
    {
        std::string newValue { ValuesDelimiterSymbol,  EmptyValueSymbol };
        tapeFile.seekg(0, std::ios::end);
        tapeFile << newValue;
        tapeFilePointer = currentValueLastDigitPointer + 2;
    }
    else
    {
        tapeFilePointer = getRightValuePointer(tapeFilePointer);
    }

    return true;
}

bool FileTape::moveTapePointerLeft()
{
    if (tapeFilePointer < 2)
    {
        std::cerr << "Cannot get the right value!" << std::endl;
        return false;   
    }
    
    tapeFilePointer = getLeftValuePointer(tapeFilePointer);
    return true;
}

bool FileTape::isValueSymbol(char symbol)
{
    return std::isdigit(symbol) || symbol == ' ' || symbol == '-';
}
