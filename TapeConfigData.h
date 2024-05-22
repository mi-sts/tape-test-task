#pragma once

class TapeConfigData
{
public:
    TapeConfigData() = default;
    TapeConfigData(int readDelay, int writeDelay, int moveNextDelay, int rewindPerElementDelay, int RAMBytesSize);

    int readDelay;
    int writeDelay;
    int moveNextDelay;
    int rewindPerElementDelay;
    int RAMBytesSize;
};
