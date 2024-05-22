#pragma once

class ITape
{
public:
    virtual bool moveLeft() = 0;
    virtual bool moveRight() = 0;
    virtual bool read(int& value) = 0;
    virtual bool write(int value) = 0;
    virtual bool isEmpty() = 0;
    virtual bool clearValue() = 0;
    virtual bool rewindLeft(int cellsNumber) = 0;
    virtual bool rewindRight(int cellsNumber) = 0;
    
    virtual ~ITape() = default;
};
