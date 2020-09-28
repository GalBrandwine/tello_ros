#pragma once
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
