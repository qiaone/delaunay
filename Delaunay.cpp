#include "Delaunay.h"


void Delaunay::delaunay_tri(Vec_Point & points)
{
	// add vertex
    add_vertex(points);

	// initialize
	init();

	// start triangulation
    int i, n = points.size();
	for (i = 0; i < n; i++)
	{
		// get vertex_handle
		TriMesh::VHandle vh = m_mesh.vertex_handle(i);

		// this vertex is covered by face(fhandle)
        TriMesh::FaceHandle fhandle = m_mesh.property(VertexToFace, vh);

		// save point_handle covered by this face
        Vec_VHandle &fpts = m_mesh.property(FaceToVertices, fhandle);
        Vec_VHandle buffer(fpts.begin(), fpts.end());

		// split face 
		// TO DO: degeneration may happen! 
		m_mesh.split(fhandle, vh);

		// rebucket (caused by face_split)
		// TO DO: degeneration may happen! 
		rebucket(vh, buffer);

		// flip edge and rebucket
		int n = 0;
		TriMesh::HalfedgeHandle hEH1, hEH0, hEH[4];
		hEH1 = hEH0 = m_mesh.halfedge_handle(vh);
		do
		{
			hEH1 = m_mesh.next_halfedge_handle(hEH1);
			hEH[n++] = hEH1;
			hEH1 = m_mesh.next_halfedge_handle(hEH1);
			hEH1 = m_mesh.opposite_halfedge_handle(hEH1);
		} while (hEH1 != hEH0);

		for (int i = 0; i < n; i++)
			// TO DO: degeneration may happen! 
			legalize_edge(hEH[i], vh);
	}

	// delete infinit vertex
	delete_vertex(n);
}

void Delaunay::draw_mesh()
{
	glColor3f(0, 0, 1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_TRIANGLES);
	TriMesh::FaceIter f_it, f_end(m_mesh.faces_end());
	for (f_it = m_mesh.faces_begin(); f_it != f_end; ++f_it)
	{
		TriMesh::FaceVertexIter fv_it = m_mesh.fv_iter(*f_it);
		while (fv_it.is_valid())
		{
			TriMesh::Point p = m_mesh.point(*fv_it);
			glVertex3f(p[0], p[1], p[2]);
			++fv_it;
		}
	}
	glEnd();
}

void Delaunay::draw_triangle(TriMesh::FaceHandle _fh)
{
	glColor3f(0, 1, 0);
	glBegin(GL_TRIANGLES);
	TriMesh::FaceVertexIter fv_it = m_mesh.fv_iter(_fh);
	while (fv_it.is_valid())
	{
		TriMesh::Point p = m_mesh.point(*fv_it);
		glVertex3f(p[0], p[1], p[2]);
		++fv_it;
	}
	glEnd();
}

void Delaunay::add_vertex(Vec_Point &_vecPt)
{
	// clear
	m_mesh.clear();

	// add vertex
	int n = _vecPt.size();
	for (int i = 0; i < n; i++)
	{
		m_mesh.add_vertex(_vecPt[i]);
	}
}

void Delaunay::init()
{
	// add property
    m_mesh.add_property(FaceToVertices);
    m_mesh.add_property(VertexToFace);

	// add infinite vertex
	TriMesh::VertexHandle vhandle[3];
	vhandle[0] = m_mesh.add_vertex(TriMesh::Point(-INF, -INF, 0));
	vhandle[1] = m_mesh.add_vertex(TriMesh::Point(INF, -INF, 0));
	vhandle[2] = m_mesh.add_vertex(TriMesh::Point(0, INF, 0));

	// add the initial triangle
	TriMesh::FaceHandle fhandle = m_mesh.add_face(vhandle, 3);

	// assign all vertex_handle to the initial triangle
	TriMesh::VertexIter vIt;
	for (vIt = m_mesh.vertices_begin(); *vIt != vhandle[0]; vIt++)
	{
        m_mesh.property(FaceToVertices, fhandle).push_back(*vIt);
        m_mesh.property(VertexToFace, *vIt) = fhandle;
	}
}


bool Delaunay::in_circle(TriMesh::HalfedgeHandle _hEh, TriMesh::VertexHandle _vhp,
	TriMesh::VertexHandle _vhx)
{
	// boundary edge
	if (m_mesh.is_boundary(_hEh) ||
		m_mesh.is_boundary(m_mesh.opposite_halfedge_handle(_hEh)))
		return false;

	TriMesh::Point pt[4];
	TriMesh::VertexHandle vh;
	vh = m_mesh.from_vertex_handle(_hEh);
	pt[0] = m_mesh.point(vh);
	vh = m_mesh.to_vertex_handle(_hEh);
	pt[1] = m_mesh.point(vh);
	pt[2] = m_mesh.point(_vhp);
	pt[3] = m_mesh.point(_vhx);

	for (int i = 0; i < 3; i++)
	{
		pt[i] -= pt[3];
		pt[i][2] = pt[i][0] * pt[i][0] + pt[i][1] * pt[i][1];
	}

	float rst = dot(cross(pt[0], pt[1]), pt[2]);
	return rst > 0;
}

bool Delaunay::to_left(TriMesh::Point &_p, TriMesh::Point &_a,
	TriMesh::Point &_b)
{
	float rst = _p[0] * _a[1] - _p[1] * _a[0] +
		_a[0] * _b[1] - _a[1] * _b[0] +
		_b[0] * _p[1] - _b[1] * _p[0];

	return rst > 0;
}

bool Delaunay::in_triangle(TriMesh::Point &_p, TriMesh::FaceHandle _fh)
{
	std::vector<TriMesh::Point> vecPoint;
	TriMesh::FaceVertexIter fVIt = m_mesh.fv_iter(_fh);
	for (; fVIt.is_valid(); fVIt++)
	{
		vecPoint.push_back(m_mesh.point(*fVIt));
	}

	bool b1 = to_left(_p, vecPoint[0], vecPoint[1]);
	bool b2 = to_left(_p, vecPoint[1], vecPoint[2]);
	bool b3 = to_left(_p, vecPoint[2], vecPoint[0]);

	return (b1 == b2) && (b2 == b3);
}

void Delaunay::rebucket(TriMesh::VertexHandle _vH, Vec_VHandle &_vecVH)
{
	// get incident face handle
	std::vector<TriMesh::FaceHandle> vecFH;
	TriMesh::VertexFaceIter vfIt = m_mesh.vf_iter(_vH);
	while (vfIt.is_valid())
	{
		vecFH.push_back(*vfIt);
		vfIt++;
	}

	// rebucket points
	int i, j, m, n;
	m = vecFH.size();
	n = _vecVH.size();
	for (j = 0; j < m; j++)
        m_mesh.property(FaceToVertices, vecFH[j]).clear();

	for (i = 0; i < n; i++)
	{
		for (j = 0; j < m; j++)
		{
			if (in_triangle(m_mesh.point(_vecVH[i]), vecFH[j]))
			{
                m_mesh.property(FaceToVertices, vecFH[j]).push_back(_vecVH[i]);
                m_mesh.property(VertexToFace, _vecVH[i]) = vecFH[j];
				break;
			}
		}
	}
}

void Delaunay::rebucket(TriMesh::EdgeHandle _vHandle,
    Vec_VHandle &_vecVHandle)
{
	TriMesh::HalfedgeHandle hEH = m_mesh.halfedge_handle(_vHandle, 1);
	TriMesh::Point a = m_mesh.point(m_mesh.from_vertex_handle(hEH));
	TriMesh::Point b = m_mesh.point(m_mesh.to_vertex_handle(hEH));
	TriMesh::FaceHandle fH1 = m_mesh.face_handle(hEH);
	TriMesh::FaceHandle fH2 = m_mesh.opposite_face_handle(hEH);
    m_mesh.property(FaceToVertices, fH1).clear();
    m_mesh.property(FaceToVertices, fH2).clear();

	int i, n = _vecVHandle.size();
	for (i = 0; i < n; i++)
	{
		TriMesh::VertexHandle vH = _vecVHandle[i];
		if (to_left(m_mesh.point(vH), a, b))
		{
            m_mesh.property(FaceToVertices, fH1).push_back(vH);
            m_mesh.property(VertexToFace, vH) = fH1;
		}
		else
		{
            m_mesh.property(FaceToVertices, fH2).push_back(vH);
            m_mesh.property(VertexToFace, vH) = fH2;
		}
	}
}

void Delaunay::legalize_edge(TriMesh::HalfedgeHandle _hEH, TriMesh::VertexHandle _vH)
{
	TriMesh::VertexHandle vhx = m_mesh.opposite_he_opposite_vh(_hEH);
	if (in_circle(_hEH, _vH, vhx))
	{
		// save point_handle covered by this face
		TriMesh::FaceHandle fh1 = m_mesh.face_handle(_hEH);
		TriMesh::FaceHandle fh2 = m_mesh.opposite_face_handle(_hEH);
        Vec_VHandle& vecVh1 = m_mesh.property(FaceToVertices, fh1);
        Vec_VHandle& vecVh2 = m_mesh.property(FaceToVertices, fh2);
        Vec_VHandle buffer(vecVh1.size() + vecVh2.size());
        Vec_VHandle::iterator vecIt = copy(vecVh1.begin(), vecVh1.end(), buffer.begin());
		copy(vecVh2.begin(), vecVh2.end(), vecIt);

		// flip edge
		TriMesh::EdgeHandle eH = m_mesh.edge_handle(_hEH);
		if (m_mesh.is_flip_ok(eH))
		{
			m_mesh.flip(eH);
		}

		//rebucket
		rebucket(eH, buffer);

		// recursive
		TriMesh::HalfedgeHandle hEH1, hEH2;
		hEH1 = m_mesh.halfedge_handle(eH, 0);
		hEH2 = m_mesh.halfedge_handle(eH, 1);
		if (m_mesh.to_vertex_handle(hEH1) == _vH)
		{
			hEH1 = m_mesh.prev_halfedge_handle(hEH1);
			hEH2 = m_mesh.next_halfedge_handle(hEH2);
		}
		else{
			hEH1 = m_mesh.next_halfedge_handle(hEH1);
			hEH2 = m_mesh.prev_halfedge_handle(hEH2);
		}

		legalize_edge(hEH1, _vH);
		legalize_edge(hEH2, _vH);
	}
}

void Delaunay::delete_vertex(int n)
{
	for (int i = n; i < n + 3; i++)
	{
		TriMesh::VHandle vh = m_mesh.vertex_handle(i);
		m_mesh.delete_vertex(vh);
	}

	m_mesh.garbage_collection();
}
