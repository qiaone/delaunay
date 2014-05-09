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
    std::unique_ptr<Vertex> source, target;
    std::unique_ptr<Face> adj_face;
    std::unique_ptr<HalfEdge> pred, succ, twin;
};

#endif // HALFEDGE_H
