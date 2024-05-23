#pragma once

class ITape
{
public:
    virtual bool moveTapeRight() = 0;
    virtual bool moveTapeLeft() = 0;
    virtual bool read(int& value) = 0;
    virtual bool write(int value) = 0;
    virtual bool isEmpty() = 0;
    virtual bool clearValue() = 0;
    virtual bool rewindTapeRight(int cellsNumber) = 0;
    virtual bool rewindTapeLeft(int cellsNumber) = 0;
    
    virtual ~ITape() = default;
};
