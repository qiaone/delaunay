#ifndef POINT2D_H
#define POINT2D_H

class Point2D
{
public:
    Point2D(double x, double y);
    double x();
    double y();
    void setx(double x);
    void sety(double y);
private:
    double _x, _y;
};

#endif // POINT2D_H
