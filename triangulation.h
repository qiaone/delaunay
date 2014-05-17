#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include <unordered_map>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyConnectivity.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

namespace Delaunay
{

struct MyTraits : public OpenMesh::DefaultTraits
{
    //typedef OpenMesh::Vec3d Point; // use double-values points
};

typedef OpenMesh::TriMesh_ArrayKernelT<MyTraits> TriMesh;

class Triangulation
{
public:
    Triangulation(std::vector<TriMesh::Point> & all_points);
    void perform();
    TriMesh mesh;
protected:
    OpenMesh::FPropHandleT<unsigned int> FaceId;
    OpenMesh::VPropHandleT<unsigned int> VertexId;
    unsigned int generateId();
private:
    static unsigned int id;
    std::unordered_map<unsigned int, unsigned int> VertexToFace;
    std::unordered_multimap<unsigned int, unsigned int> FaceToVertices;
};

}


#endif // TRIANGULATION_H
