#include <gtest/gtest.h>

#include "../triangulation.h"
#include <iostream>

TEST(Triangulation, Init)
{
    using namespace Delaunay;
    TriMesh::Point p1(1, 2, 0);
    TriMesh::Point p2(100, 22, 0);
    TriMesh::Point p3(35, 27, 0);
    std::vector<TriMesh::Point> points;
    points.push_back(p1);
    points.push_back(p2);
    points.push_back(p3);
    Delaunay::Triangulation T(points);
    EXPECT_EQ(T.mesh.n_vertices(), 6);
}
