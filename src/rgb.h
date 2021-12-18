#ifndef PIXEL_H
#define PIXEL_H

#include <cstdint>

#pragma pack (push, 1)

struct RGB
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
};

#pragma pack (pop)

bool operator<(const RGB &p1, const RGB &p2);
bool operator==(const RGB &p1, const RGB &p2);

#endif // PIXEL_H
