#ifndef VERTEX_H
#define VERTEX_H

#include "point2d.h"
#include "halfedge.h"
#include <list>
#include <memory>

class HalfEdge;

class Vertex
{
public:
    Vertex(Point2D point_);
    Point2D point;
    std::list<std::shared_ptr<HalfEdge>> out_halfedges;
};

#endif // VERTEX_H
