#pragma once
#include <vector>
#include <GL/glut.h>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyConnectivity.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

struct MyTraits : public OpenMesh::DefaultTraits
{
    VertexAttributes(OpenMesh::Attributes::Status);
    FaceAttributes(OpenMesh::Attributes::Status);
    EdgeAttributes(OpenMesh::Attributes::Status);
};

typedef OpenMesh::TriMesh_ArrayKernelT<MyTraits>  TriMesh;
typedef TriMesh::Point Point;
typedef std::vector<Point> PointVec;
typedef TriMesh::EHandle EHandle;
typedef TriMesh::VHandle VHandle;
typedef TriMesh::FHandle FHandle;
typedef TriMesh::HHandle HHandle;
typedef std::vector<VHandle> VHandleVec;
typedef std::vector<FHandle> FHandleVec;

class Delaunay
{
protected:
    TriMesh  mesh;
    OpenMesh::FPropHandleT<VHandleVec> FaceToVertices;
    OpenMesh::VPropHandleT<FHandle> VertexToFace;

public:
    void perform(PointVec&  all_points);

    void drawMesh();
    void drawMeshInQt(int w, int h);

private:

    void drawTriangle(FHandle fh);

    void addVertices(PointVec& points);

    void init();

    bool isInCircle(HHandle hh, VHandle vh, VHandle vh_oppo);

    bool isLeft(Point& _p, Point& _a, Point& _b);

    bool isInTriangle(Point& point, FHandle fh);

    void rebucket(VHandle vh, VHandleVec& vhvec);

    void rebucket(EHandle eh, VHandleVec& vhvec);

    void legalize(HHandle hh, VHandle vh);

    void deleteVertices(int n);
};

