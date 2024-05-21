#pragma once
#include <iostream>

class FileUtils
{
public:
    static std::string createTempFile(std::fstream& fileStream);
    static bool deleteFile(std::string filePath);
    static std::streamoff getFileSize(std::fstream& fileStream);
};
