#ifndef FACE_H
#define FACE_H

#include "halfedge.h"
#include <memory>

class HalfEdge;

class Face
{
public:
    Face();
    std::unique_ptr<HalfEdge> adj_halfedge;
};

#endif // FACE_H
