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
typedef std::vector<TriMesh::VHandle> VHandleVec;

class Triangulation
{
public:
    Triangulation(std::vector<TriMesh::Point> & all_points);
    void perform();
    TriMesh mesh;
protected:
    OpenMesh::FPropHandleT<VHandleVec> FaceToVertices;
    OpenMesh::VPropHandleT<TriMesh::FHandle> VertexToFace;
};

}


#endif // TRIANGULATION_H
