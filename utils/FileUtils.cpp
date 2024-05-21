#include "FileUtils.h"

#include <chrono>
#include <fstream>
#include <sstream>

std::string FileUtils::createTempFile(std::fstream& fileStream)
{
    using namespace std::chrono;
    const auto nowMilliseconds = time_point_cast<milliseconds>(system_clock::now());
    const auto epochTime = nowMilliseconds.time_since_epoch();
    const auto sinceEpochMilliseconds = std::chrono::duration_cast<milliseconds>(epochTime).count();
    std::ostringstream millisecondsStringStream;
    millisecondsStringStream << sinceEpochMilliseconds;
    const std::string millisecondsString = millisecondsStringStream.str();
    
    std::string tempFilePath = "tmpfile" + millisecondsString + ".txt";
    fileStream.open(tempFilePath, std::ios::out | std::ios::in | std::ios::trunc);
    if (!fileStream.is_open())
    {
        std::cerr << "Cannot create a temporary file!" << std::endl;
        return "";
    }

    return tempFilePath;
}

bool FileUtils::deleteFile(std::string filePath)
{
    return std::remove(filePath.c_str()) == 0;
}

std::streamoff FileUtils::getFileSize(std::fstream& fileStream)
{
    if (!fileStream.is_open())
    {
        std::cerr << "Cannot open the file!" << std::endl;
        return 0;
    }

    std::streamoff initialTapeFilePointer = fileStream.tellg();
    fileStream.seekg(0, std::ios::end);
    std::streamoff fileSize = fileStream.tellg();
    fileStream.seekg(initialTapeFilePointer);

    return fileSize;
}
