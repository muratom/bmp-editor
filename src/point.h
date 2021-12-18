#ifndef POINT_H
#define POINT_H

struct Point {
    Point(long int x, long int y) :
        x(x),
        y(y)
    {}

public:
    long int x;
    long int y;
};

#endif // POINT_H
