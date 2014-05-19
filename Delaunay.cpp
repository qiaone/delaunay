#include "Delaunay.h"


void Delaunay::perform(PointVec& all_points)
{
    // add all vertices into the mesh
    add_vertices(all_points);

    // add the big triangle and link V/F
    init();

    // start triangulation
    int i, n = all_points.size();
    for (i = 0; i < n; i++)
    //for(auto& vh : m_mesh.vertices())
    {
        // get vertex_handle
        VHandle vh = mesh.vertex_handle(i);

        // this vertex is covered by face(fhandle)
        FHandle fhandle = mesh.property(VertexToFace, vh);

        // save point_handle covered by this face
        VHandleVec& fpts = mesh.property(FaceToVertices, fhandle);
        VHandleVec buffer(fpts.begin(), fpts.end());

        // split face
        // TO DO: degeneration may happen!
        mesh.split(fhandle, vh);

        // rebucket (caused by face_split)
        // TO DO: degeneration may happen!
        rebucket(vh, buffer);

        // flip edge and rebucket
        int n = 0;
        HHandle hEH1, hEH0, hEH[4];
        hEH1 = hEH0 = mesh.halfedge_handle(vh);
        do
        {
            hEH1 = mesh.next_halfedge_handle(hEH1);
            hEH[n++] = hEH1;
            hEH1 = mesh.next_halfedge_handle(hEH1);
            hEH1 = mesh.opposite_halfedge_handle(hEH1);
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
    TriMesh::FaceIter f_it, f_end(mesh.faces_end());
    for (f_it = mesh.faces_begin(); f_it != f_end; ++f_it)
    {
        TriMesh::FaceVertexIter fv_it = mesh.fv_iter(*f_it);
        while (fv_it.is_valid())
        {
            Point p = mesh.point(*fv_it);
            glVertex3f(p[0], p[1], p[2]);
            ++fv_it;
        }
    }
    glEnd();
}

void Delaunay::draw_triangle(FHandle _fh)
{
    glColor3f(0, 1, 0);
    glBegin(GL_TRIANGLES);
    TriMesh::FaceVertexIter fv_it = mesh.fv_iter(_fh);
    while (fv_it.is_valid())
    {
        Point p = mesh.point(*fv_it);
        glVertex3f(p[0], p[1], p[2]);
        ++fv_it;
    }
    glEnd();
}

void Delaunay::add_vertices(PointVec& points)
{
    mesh.clear();

    for(auto& p : points)
        mesh.add_vertex(p);
}

void Delaunay::init()
{
    // TO DO: deal with infinte point
    const float INF = 1.0e5;

    // add property
    mesh.add_property(FaceToVertices);
    mesh.add_property(VertexToFace);

    // add infinite vertex
    VHandle vhs[3];
    vhs[0] = mesh.add_vertex(Point(-INF, -INF, 0));
    vhs[1] = mesh.add_vertex(Point(INF, -INF, 0));
    vhs[2] = mesh.add_vertex(Point(0, INF, 0));

    // add the initial triangle
    FHandle fh = mesh.add_face(vhs, 3);

    // link vertex and face
    for(auto& vhi : mesh.vertices())
    {
        mesh.property(FaceToVertices, fh).push_back(vhi);
        mesh.property(VertexToFace, vhi) = fh;
    }
}


bool Delaunay::in_circle(HHandle _hEh, VHandle _vhp,
                         VHandle _vhx)
{
    // boundary edge
    if (mesh.is_boundary(_hEh) ||
            mesh.is_boundary(mesh.opposite_halfedge_handle(_hEh)))
        return false;

    Point pt[4];
    VHandle vh;
    vh = mesh.from_vertex_handle(_hEh);
    pt[0] = mesh.point(vh);
    vh = mesh.to_vertex_handle(_hEh);
    pt[1] = mesh.point(vh);
    pt[2] = mesh.point(_vhp);
    pt[3] = mesh.point(_vhx);

    for (int i = 0; i < 3; i++)
    {
        pt[i] -= pt[3];
        pt[i][2] = pt[i][0] * pt[i][0] + pt[i][1] * pt[i][1];
    }

    float rst = dot(cross(pt[0], pt[1]), pt[2]);
    return rst > 0;
}

bool Delaunay::to_left(Point& _p, Point& _a,
                       Point& _b)
{
    float rst = _p[0] * _a[1] - _p[1] * _a[0] +
            _a[0] * _b[1] - _a[1] * _b[0] +
            _b[0] * _p[1] - _b[1] * _p[0];

    return rst > 0;
}

bool Delaunay::in_triangle(Point& _p, FHandle _fh)
{
    std::vector<Point> vecPoint;
    TriMesh::FaceVertexIter fVIt = mesh.fv_iter(_fh);
    for (; fVIt.is_valid(); fVIt++)
    {
        vecPoint.push_back(mesh.point(*fVIt));
    }

    bool b1 = to_left(_p, vecPoint[0], vecPoint[1]);
    bool b2 = to_left(_p, vecPoint[1], vecPoint[2]);
    bool b3 = to_left(_p, vecPoint[2], vecPoint[0]);

    return (b1 == b2) && (b2 == b3);
}

void Delaunay::rebucket(VHandle _vH, VHandleVec& _vecVH)
{
    // get incident face handle
    std::vector<FHandle> vecFH;
    TriMesh::VertexFaceIter vfIt = mesh.vf_iter(_vH);
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
        mesh.property(FaceToVertices, vecFH[j]).clear();

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            if (in_triangle(mesh.point(_vecVH[i]), vecFH[j]))
            {
                mesh.property(FaceToVertices, vecFH[j]).push_back(_vecVH[i]);
                mesh.property(VertexToFace, _vecVH[i]) = vecFH[j];
                break;
            }
        }
    }
}

void Delaunay::rebucket(EHandle _vHandle, VHandleVec& _vecVHandle)
{
    HHandle hEH = mesh.halfedge_handle(_vHandle, 1);
    Point a = mesh.point(mesh.from_vertex_handle(hEH));
    Point b = mesh.point(mesh.to_vertex_handle(hEH));
    FHandle fH1 = mesh.face_handle(hEH);
    FHandle fH2 = mesh.opposite_face_handle(hEH);
    mesh.property(FaceToVertices, fH1).clear();
    mesh.property(FaceToVertices, fH2).clear();

    int i, n = _vecVHandle.size();
    for (i = 0; i < n; i++)
    {
        VHandle vH = _vecVHandle[i];
        if (to_left(mesh.point(vH), a, b))
        {
            mesh.property(FaceToVertices, fH1).push_back(vH);
            mesh.property(VertexToFace, vH) = fH1;
        }
        else
        {
            mesh.property(FaceToVertices, fH2).push_back(vH);
            mesh.property(VertexToFace, vH) = fH2;
        }
    }
}

void Delaunay::legalize_edge(HHandle _hEH, VHandle _vH)
{
    VHandle vhx = mesh.opposite_he_opposite_vh(_hEH);
    if (in_circle(_hEH, _vH, vhx))
    {
        // save point_handle covered by this face
        FHandle fh1 = mesh.face_handle(_hEH);
        FHandle fh2 = mesh.opposite_face_handle(_hEH);
        VHandleVec& vecVh1 = mesh.property(FaceToVertices, fh1);
        VHandleVec& vecVh2 = mesh.property(FaceToVertices, fh2);
        VHandleVec buffer(vecVh1.size() + vecVh2.size());
        VHandleVec::iterator vecIt = copy(vecVh1.begin(), vecVh1.end(), buffer.begin());
        copy(vecVh2.begin(), vecVh2.end(), vecIt);

        // flip edge
        EHandle eH = mesh.edge_handle(_hEH);
        if (mesh.is_flip_ok(eH))
        {
            mesh.flip(eH);
        }

        //rebucket
        rebucket(eH, buffer);

        // recursive
        HHandle hEH1, hEH2;
        hEH1 = mesh.halfedge_handle(eH, 0);
        hEH2 = mesh.halfedge_handle(eH, 1);
        if (mesh.to_vertex_handle(hEH1) == _vH)
        {
            hEH1 = mesh.prev_halfedge_handle(hEH1);
            hEH2 = mesh.next_halfedge_handle(hEH2);
        }
        else{
            hEH1 = mesh.next_halfedge_handle(hEH1);
            hEH2 = mesh.prev_halfedge_handle(hEH2);
        }

        legalize_edge(hEH1, _vH);
        legalize_edge(hEH2, _vH);
    }
}

void Delaunay::delete_vertex(int n)
{
    for (int i = n; i < n + 3; i++)
    {
        VHandle vh = mesh.vertex_handle(i);
        mesh.delete_vertex(vh);
    }

    mesh.garbage_collection();
}
