#include "triangulation.h"
#include <limits>

namespace Delaunay
{

unsigned int Triangulation::id = 0;

Triangulation::Triangulation(std::vector<TriMesh::Point> & all_points)
{
    for(auto & point : all_points)
    {
        mesh.add_vertex(point);
    }

    init();
}

void Triangulation::perform()
{

}

void Triangulation::init()
{

    const float fmax = std::numeric_limits<float>::max();
    const float fmin = std::numeric_limits<float>::min();

    mesh.add_property(VertexId);
    mesh.add_property(FaceId);

    // add 3 infinite vertices
    TriMesh::VertexHandle vhandle[3];
    vhandle[0] = mesh.add_vertex(TriMesh::Point(fmin, fmin, 0));
    vhandle[1] = mesh.add_vertex(TriMesh::Point(fmax, fmin, 0));
    vhandle[2] = mesh.add_vertex(TriMesh::Point(0, fmax, 0));

    // add id to each vertex
    for (int i = 0; i < 3; i++)
    {
        mesh.property(VertexId, vhandle[i]) = generateId();
    }

    // add the initial triangle and id
    TriMesh::FaceHandle fhandle = mesh.add_face(vhandle, 3);
    mesh.property(FaceId, fhandle) = generateId();
}

inline unsigned int Triangulation::generateId()
{
    return id++;
}

}
