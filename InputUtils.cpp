#include "InputUtils.h"

std::vector<std::string> InputUtils::split(std::string& string, char delimeter)
{
    std::stringstream stringStream(string);
    std::vector<std::string> splittedSubstrings;
    std::string currentSubstring;
    
    while(std::getline(stringStream, currentSubstring, delimeter))
    {
        splittedSubstrings.push_back(currentSubstring);
    }

    return splittedSubstrings;
}
