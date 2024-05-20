#include "TapeConfigData.h"

TapeConfigData::TapeConfigData(
    int readDelay,
    int writeDelay,
    int moveNextDelay,
    int rewindPerElementDelay
) : readDelay(readDelay), writeDelay(writeDelay), moveNextDelay(moveNextDelay),
    rewindPerElementDelay(rewindPerElementDelay)
{
}
