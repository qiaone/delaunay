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
typedef std::vector<HHandle> HHandleVec;

class Delaunay
{
protected:
    OpenMesh::FPropHandleT<VHandleVec> FaceToVertices;
    OpenMesh::VPropHandleT<FHandle> VertexToFace;
	OpenMesh::VPropHandleT<HHandle> VertexToHEdge;

public:
    Delaunay();
    void perform(PointVec& all_points);
    void drawMesh();
    TriMesh mesh;

private:
    void drawTriangle(FHandle fh);
    void addVertices(PointVec& points);
    void init();
    bool isInCircle(HHandle hh, VHandle vh, VHandle vh_oppo);
    bool isLeft(Point& p, Point& a, Point& b);
	bool isOverlap(VHandle vh1, VHandle vh2);
	bool isOnEdge(Point& pt, HHandle hh);
    bool isInTriangle(Point& pt, FHandle fh);
	bool isInfinite(VHandle vh);
    void rebucket(VHandle vh, VHandleVec& vhvec);
    void rebucket(EHandle eh, VHandleVec& vhvec);
	void saveVhs(FHandle fh, VHandleVec &vhs_buffer);
	void saveVhs(HHandle hh, VHandleVec &vhs_buffer);
    void legalize(HHandle hh, VHandle vh);
    void deleteVertices(int n);
};

