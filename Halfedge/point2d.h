#ifndef POINT2D_H
#define POINT2D_H

class Point2D
{
public:
    Point2D(double x_, double y_);
    double x, y;
    Point2D operator+(const Point2D & rhs);
};

#endif // POINT2D_H
