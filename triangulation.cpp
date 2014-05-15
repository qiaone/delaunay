#include "triangulation.h"
#include <limits>

namespace Delaunay
{

Triangulation::Triangulation(VecPt & all_points)
{
    for(auto & point : all_points)
        mesh.add_vertex(point);
}

void Triangulation::perform()
{

}

void Triangulation::init()
{

    const float fmax = std::numeric_limits<float>::max();
    const float fmin = std::numeric_limits<float>::min();

    // add infinite vertices
    TriMesh::VertexHandle vhandle[3];
    vhandle[0] = mesh.add_vertex(TriMesh::Point(fmin, fmin, 0));
    vhandle[1] = mesh.add_vertex(TriMesh::Point(fmax, fmin, 0));
    vhandle[2] = mesh.add_vertex(TriMesh::Point(0, fmax, 0));

    // add the initial triangle
    TriMesh::FaceHandle fhandle = mesh.add_face(vhandle, 3);

//    // assign all vertex_handle to the initial triangle
//    TriMesh::VertexIter vIt;
//    for (vIt = m_mesh.vertices_begin(); *vIt != vhandle[0]; vIt++)
//    {
//        m_mesh.property(FPoints, fhandle).push_back(*vIt);
//        m_mesh.property(PFace, *vIt) = fhandle;
//    }
}

}
