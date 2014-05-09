#ifndef HALFEDGE_H
#define HALFEDGE_H

#include "vertex.h"
#include "face.h"
#include <memory>

class Vertex;
class Face;

class HalfEdge
{
public:
    HalfEdge();
    std::shared_ptr<Vertex> source, target;
    std::shared_ptr<Face> adj_face;
    std::shared_ptr<HalfEdge> pred, succ, twin;
};

#endif // HALFEDGE_H
