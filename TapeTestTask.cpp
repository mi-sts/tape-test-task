
#include "TapesSorter.h"

int main(int argc, char* argv[])
{
    TapesSorter tapesSorter(
        "input_tape.txt",
        "output_tape.txt",
        "tape_config.txt"
    );
    tapesSorter.sort();
}
