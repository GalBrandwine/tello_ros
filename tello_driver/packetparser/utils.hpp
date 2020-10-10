#pragma once
#include <chrono>

// struct TimeStruct
// {
//     int64_t microsec;
//     int64_t sec;
//     int64_t min;
//     int64_t hrs;
//     TimeStruct()
//     {
//         std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
//         auto duration = now.time_since_epoch();
//         microsec = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
//         sec = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
//         min = std::chrono::duration_cast<std::chrono::minutes>(duration).count();
//         hrs = std::chrono::duration_cast<std::chrono::hours>(duration).count();
//     }
// };

struct Byte
{
    // Left most
    unsigned char LeftNibble;
    // Right most
    unsigned char RightNibble;

    const Byte &operator=(Byte a)
    {
        LeftNibble = a.LeftNibble;
        RightNibble = a.RightNibble;
        return *this;
    }
};

static const Byte le16(int val)
{
    return Byte{val & 0xff, (val >> 8) & 0xff};
}

static int16_t int16(unsigned char val0, unsigned char val1)
{
    if (val1 & 0xff != 0)
        return ((val0 & 0xff) | ((val1 & 0xff) << 8)) - 0x10000;
    else
    {
        return (val0 & 0xff) | ((val1 & 0xff) << 8);
    }
}

static const uint16_t uint16(unsigned char val0, unsigned char val1)
{
    return (val0 & 0xff) | ((val1 & 0xff) << 8);
}