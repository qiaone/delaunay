#include "triangulation.h"
#include "ensure_exception.h"
#include <limits>

namespace Delaunay
{

Triangulation::Triangulation(std::vector<TriMesh::Point> & all_points)
{
    ENSURE(all_points.size() >= 3);

    const float fmax = std::numeric_limits<float>::max();
    const float fmin = std::numeric_limits<float>::min();

    mesh.add_property(FaceToVertices);
    mesh.add_property(VertexToFace);

    // add 3 infinite vertices
    TriMesh::VertexHandle vhs[3];
    vhs[0] = mesh.add_vertex(TriMesh::Point(fmin, fmin, 0));
    vhs[1] = mesh.add_vertex(TriMesh::Point(fmax, fmin, 0));
    vhs[2] = mesh.add_vertex(TriMesh::Point(0, fmax, 0));

    // add the initial triangle and its face id
    auto fh = mesh.add_face(vhs, 3);

    // add id to each vertex, link them with the face by VtoF/FtoV
    for(auto & point : all_points)
    {
        auto vh = mesh.add_vertex(point);
        mesh.property(VertexToFace, vh);
        mesh.property(FaceToVertices, fh).push_back(vh);
    }
}

void Triangulation::perform()
{
    for (auto vit = mesh.vertices_begin(); vit != mesh.vertices_end(); vit++)
    {
        auto vh = vit.handle();
        auto fh = mesh.property(VertexToFace, vh);
        VHandleVec& temp_vhs_in_face = mesh.property(FaceToVertices, fh);
        //auto vid = mesh.property(VertexId, vh);
        //auto fid = VertexToFace[vid];

    }
}

}
