#pragma once

class IRAM
{
public:
    virtual bool write(unsigned char value, size_t index) = 0;
    virtual unsigned char read(size_t index) = 0;
    virtual size_t getSize() = 0;

    virtual ~IRAM() = default;
};
