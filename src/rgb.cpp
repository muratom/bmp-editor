#include "rgb.h"

bool operator<(const RGB& p1, const RGB &p2)
{
    if (p1.r == p2.r) {
        if (p1.g == p2.g) {
            if (p1.b == p2.b) {
                return false;
            }
            return p1.b < p2.b;
        }
        return p1.g < p2.g;
    }
    return p1.r < p2.r;
}

bool operator==(const RGB &p1, const RGB &p2)
{
    return (p1.r == p2.r) && (p1.g == p2.g) && (p1.b == p2.b);
}
