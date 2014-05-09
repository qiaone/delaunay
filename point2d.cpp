#include "point2d.h"

Point2D::Point2D(double x, double y) : _x(x), _y(y)
{

}

double Point2D::x()
{
    return _x;
}

double Point2D::y()
{
    return _y;
}

void Point2D::setx(double x)
{
    _x(x);
}

void Point2D::sety(double y)
{
    _y(y);
}
