#include "point2d.h"

Point2D::Point2D(double x_, double y_) : x(x_), y(y_)
{

}

Point2D Point2D::operator+(const Point2D & rhs)
{
    return Point2D(this->x + rhs.x, this->y + rhs.y);
}
