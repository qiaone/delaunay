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
typedef std::vector<TriMesh::Point> VecPt;
typedef std::vector<TriMesh::VHandle> VecVHandle;

// TO DO: deal with infinte point
const float INF = 1.0e5;

class Delaunay
{
protected:
	TriMesh  m_mesh;
	OpenMesh::FPropHandleT<VecVHandle> FPoints;
	OpenMesh::VPropHandleT<TriMesh::FHandle> PFace;

public:
	void delaunay_tri(VecPt & _vecPt);
	
	void draw_mesh();

private:

	void draw_triangle(TriMesh::FaceHandle _fh);

	void add_vertex(VecPt &_vecPt);

	void init();	

	bool in_circle(TriMesh::HalfedgeHandle _hEh, TriMesh::VertexHandle _vhp,
		TriMesh::VertexHandle _vhx);

	bool to_left(TriMesh::Point &_p, TriMesh::Point &_a, TriMesh::Point &_b);

	bool in_triangle(TriMesh::Point &_p, TriMesh::FaceHandle _fh);

	void rebucket(TriMesh::VertexHandle _vH, VecVHandle &_vecVH);
	
	void rebucket(TriMesh::EdgeHandle _vHandle,	VecVHandle &_vecVHandle);

	void legalize_edge(TriMesh::HalfedgeHandle _hEH, TriMesh::VertexHandle _vH);
	
	void delete_vertex(int n);
};

