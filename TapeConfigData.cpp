#include "TapeConfigData.h"

TapeConfigData::TapeConfigData(
    int readDelay,
    int writeDelay,
    int moveNextDelay,
    int rewindPerElementDelay,
    int RAMBytesSize
) : readDelay(readDelay), writeDelay(writeDelay), moveNextDelay(moveNextDelay),
    rewindPerElementDelay(rewindPerElementDelay), RAMBytesSize(RAMBytesSize)
{
}
