#include "triangulation.h"
#include "ensure_exception.h"
#include <limits>

namespace Delaunay
{

unsigned int Triangulation::id = 0;

Triangulation::Triangulation(std::vector<TriMesh::Point> & all_points)
{
    ENSURE(all_points.size() >= 3);

    const float fmax = std::numeric_limits<float>::max();
    const float fmin = std::numeric_limits<float>::min();

    mesh.add_property(VertexId);
    mesh.add_property(FaceId);

    // add 3 infinite vertices
    TriMesh::VertexHandle vhandle[3];
    vhandle[0] = mesh.add_vertex(TriMesh::Point(fmin, fmin, 0));
    vhandle[1] = mesh.add_vertex(TriMesh::Point(fmax, fmin, 0));
    vhandle[2] = mesh.add_vertex(TriMesh::Point(0, fmax, 0));

    // add the initial triangle and its face id
    auto fhandle = mesh.add_face(vhandle, 3);
    auto fid = generateId();
    mesh.property(FaceId, fhandle) = fid;

    // add id to each vertex, link them with the face by VtoF/FtoV
    for(auto & point : all_points)
    {
        auto vhandle = mesh.add_vertex(point);
        auto vid = generateId();
        mesh.property(VertexId, vhandle) = vid;
        VertexToFace(vid, fid);
        FaceToVertices(fid, vid);
    }
}

void Triangulation::perform()
{
    

    for (int i = 0; i < mesh.n_vertices(); i++)
    {

    }
}

inline unsigned int Triangulation::generateId()
{
    return id++;
}

}
