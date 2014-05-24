#include "delaunay.h"


void Delaunay::perform(PointVec& all_points)
{
    // add all vertices into the mesh
    addVertices(all_points);

    // add the big triangle and link vertices and faces
    init();

    // will be error if as follows, don't know why:
    //for(auto& vh : mesh.vertices())
    //for(auto vit = mesh.vertices_begin(); vit != mesh.vertices_end(); vit++)

    // start triangulation
    for(size_t i = 0; i < all_points.size(); i++)
    {

        VHandle vh = mesh.vertex_handle((unsigned int)i);
        FHandle fh = mesh.property(VertexToFace, vh);

        // save vertices maped to this face
        // coz properties will be destroyed after split
        VHandleVec& vhs_buffer_ref = mesh.property(FaceToVertices, fh);
        VHandleVec vhs_buffer(vhs_buffer_ref.begin(), vhs_buffer_ref.end());

        // split face
        // TO DO: degeneration may happen!
        mesh.split(fh, vh);

        // rebucket (caused by face_split)
        // TO DO: degeneration may happen!
        rebucket(vh, vhs_buffer);

        // legalize each triangle
        for(auto& hh : mesh.voh_range(vh))
        {
            legalize(mesh.next_halfedge_handle(hh), vh);
        }
    }

    // delete infinite vertices
    deleteVertices(all_points.size());
}

void Delaunay::drawMesh()
{
    glColor3f(0, 0, 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_TRIANGLES);
    for (auto& fh : mesh.faces())
    {
        for(auto& vh : mesh.fv_range(fh))
        {
            Point p = mesh.point(vh);
            glVertex3f(p[0], p[1], p[2]);
        }
    }
    glEnd();
}

void Delaunay::drawMeshInQt()
{
    glColor3f(0, 0, 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_TRIANGLES);
    for (auto& fh : mesh.faces())
    {
        for(auto& vh : mesh.fv_range(fh))
        {
            Point p = mesh.point(vh);
            glVertex3f(p[0] / 1000, p[1] / 1000, p[2] / 1000);
        }
    }
    glEnd();
}

void Delaunay::drawTriangle(FHandle fh)
{
    glColor3f(0, 1, 0);
    glBegin(GL_TRIANGLES);
    for (auto& vh : mesh.fv_range(fh))
    {
        Point p = mesh.point(vh);
        glVertex3f(p[0], p[1], p[2]);
    }
    glEnd();
}

void Delaunay::addVertices(PointVec& points)
{
    mesh.clear();
    for(auto& p : points)
        mesh.add_vertex(p);
}

void Delaunay::init()
{
    // TO DO: deal with infinte point
    const float INF = 1.0e5;

    // add properties
    mesh.add_property(FaceToVertices);
    mesh.add_property(VertexToFace);

    // add infinite vertices
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


bool Delaunay::isInCircle(HHandle hh, VHandle vh1, VHandle vh2)
{
    // boundary edge
    if (mesh.is_boundary(hh) ||
            mesh.is_boundary(mesh.opposite_halfedge_handle(hh))) // ?
        return false;

    Point pt[4];

    pt[0] = mesh.point(mesh.from_vertex_handle(hh));
    pt[1] = mesh.point(mesh.to_vertex_handle(hh));
    pt[2] = mesh.point(vh1);
    pt[3] = mesh.point(vh2);

    for (int i = 0; i < 3; i++)
    {
        pt[i] -= pt[3];
        pt[i][2] = pt[i][0] * pt[i][0] + pt[i][1] * pt[i][1];
    }

    float rst = dot(cross(pt[0], pt[1]), pt[2]);
    return rst > 0;
}

bool Delaunay::isLeft(Point& _p, Point& _a, Point& _b)
{
    float rst = _p[0] * _a[1] - _p[1] * _a[0] +
            _a[0] * _b[1] - _a[1] * _b[0] +
            _b[0] * _p[1] - _b[1] * _p[0];

    return rst > 0;
}

bool Delaunay::isInTriangle(Point& point, FHandle fh)
{
    std::vector<Point> triangle_points;

    for (auto& vh : mesh.fv_range(fh))
    {
        triangle_points.push_back(mesh.point(vh));
    }

    bool b1 = isLeft(point, triangle_points[0], triangle_points[1]);
    bool b2 = isLeft(point, triangle_points[1], triangle_points[2]);
    bool b3 = isLeft(point, triangle_points[2], triangle_points[0]);

    return (b1 == b2) && (b2 == b3);
}

void Delaunay::rebucket(VHandle vh, VHandleVec& vhvec)
{
    // get all face handles around the vertex
    FHandleVec fhvec;
    for(auto vfit = mesh.vf_begin(vh); vfit != mesh.vf_end(vh); vfit++)
    {
        fhvec.push_back(*vfit);
    }

    // following code lead to compiler crash:
    //for(auto& fhi : mesh.vf_range(vh))
    //    fhvec.push_back(fhi);

    // clear the incident faces' property
    for(auto& fh : fhvec)
    {
        mesh.property(FaceToVertices, fh).clear();
    }

    // for every vertex influenced, find new face it belongs to
    for(auto& vhi : vhvec)
    {
        if (vh == vhi)
        {
            continue;
        }
        //ENSURE(vh != vhi);
        for(auto& fh : fhvec)
        {
            if (isInTriangle(mesh.point(vhi), fh))
            {
                mesh.property(FaceToVertices, fh).push_back(vhi);
                mesh.property(VertexToFace, vhi) = fh;
                break;
            }
        }
    }
}

void Delaunay::rebucket(EHandle eh, VHandleVec& vhvec)
{
    HHandle heh = mesh.halfedge_handle(eh, 1);
    FHandle fh1 = mesh.face_handle(heh);
    FHandle fh2 = mesh.opposite_face_handle(heh);
    mesh.property(FaceToVertices, fh1).clear();
    mesh.property(FaceToVertices, fh2).clear();

    Point start = mesh.point(mesh.from_vertex_handle(heh));
    Point end = mesh.point(mesh.to_vertex_handle(heh));
    for(auto& vh : vhvec)
    {
        if (isLeft(mesh.point(vh), start, end))
        {
            mesh.property(FaceToVertices, fh1).push_back(vh);
            mesh.property(VertexToFace, vh) = fh1;
        }
        else
        {
            mesh.property(FaceToVertices, fh2).push_back(vh);
            mesh.property(VertexToFace, vh) = fh2;
        }
    }
}

void Delaunay::legalize(HHandle hh, VHandle vh)
{
    /*           ________ vh_oppo
     *          /\      /
     *         /  \hh  /
     *        /    \  /
     *    vh /___>__\/
     */
    VHandle vh_oppo = mesh.opposite_he_opposite_vh(hh);
    if (isInCircle(hh, vh, vh_oppo))
    {
        // save vertex handles mapped to the face
        FHandle fh1 = mesh.face_handle(hh);
        FHandle fh2 = mesh.opposite_face_handle(hh);
        VHandleVec& vhvec1 = mesh.property(FaceToVertices, fh1);
        VHandleVec& vhvec2 = mesh.property(FaceToVertices, fh2);
        VHandleVec vhs_buffer(vhvec1.size() + vhvec2.size());
        VHandleVec::iterator vit = copy(vhvec1.begin(), vhvec1.end(), vhs_buffer.begin());
        copy(vhvec2.begin(), vhvec2.end(), vit);

        // flip edge
        EHandle eh = mesh.edge_handle(hh);
        if (mesh.is_flip_ok(eh))
        {
            mesh.flip(eh);
        }

        // rebucket
        rebucket(eh, vhs_buffer);

        // recursive
        HHandle heh1, heh2;
        heh1 = mesh.halfedge_handle(eh, 0);
        heh2 = mesh.halfedge_handle(eh, 1);
        if (mesh.to_vertex_handle(heh1) == vh)
        {
            heh1 = mesh.prev_halfedge_handle(heh1);
            heh2 = mesh.next_halfedge_handle(heh2);
        }
        else{
            heh1 = mesh.next_halfedge_handle(heh1);
            heh2 = mesh.prev_halfedge_handle(heh2);
        }

        legalize(heh1, vh);
        legalize(heh2, vh);
    }
}

void Delaunay::deleteVertices(int n)
{
    for (int i = n; i < n + 3; i++)
    {
        VHandle vh = mesh.vertex_handle(i);
        mesh.delete_vertex(vh);
    }

    mesh.garbage_collection();
}
