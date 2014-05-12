#include <gtest/gtest.h>
#include "../Halfedge/point2d.h"

TEST(Point2DTest, Add)
{
    Point2D p1(3.0, 4.0);
    Point2D p2(5.0, 2.0);
    Point2D p3 = p1 + p2;
    EXPECT_EQ(p3.x, 8.0);
    EXPECT_EQ(p3.y, 7.0); // error
}
