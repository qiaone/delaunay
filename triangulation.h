#ifndef TRIANGULATION_H
#define TRIANGULATION_H

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
typedef std::vector<TriMesh::Point> VecPt;

class Triangulation
{
public:
    Triangulation(VecPt & all_points);
    void perform();
protected:
    TriMesh mesh;
};

}


#endif // TRIANGULATION_H
