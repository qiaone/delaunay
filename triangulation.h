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
typedef TriMesh::Point Point;
typedef std::vector<Point> PointVec;
typedef TriMesh::VHandle VHandle;
typedef TriMesh::FHandle FHandle;
typedef std::vector<VHandle> VHandleVec;
typedef std::vector<FHandle> FHandleVec;

class Triangulation
{
public:
    Triangulation(PointVec & all_points);
    void perform();
    TriMesh mesh;
protected:
    OpenMesh::FPropHandleT<VHandleVec> FaceToVertices;
    OpenMesh::VPropHandleT<FHandle> VertexToFace;
    void rebucket(VHandle vh, VHandleVec vhvec);
    bool isInTriangle(Point& new_point, FHandle fh);


};

}


#endif // TRIANGULATION_H
