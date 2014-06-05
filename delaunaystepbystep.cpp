#pragma once
#include "delaunaystepbystep.h"
#include <Windows.h>
#include <QDebug>

const float INF = 1.0e5f;
const float ESP = 1.0e-6f;

DelaunayStepByStep::DelaunayStepByStep() :
    isStepDemo(false),
    current_point_num(0)
{
}

DelaunayStepByStep::DelaunayStepByStep(bool isStepDemo_) :
    isStepDemo(isStepDemo_),
    current_point_num(0)
{
}

void DelaunayStepByStep::performStepByStep()
{
    current_vh = mesh.vertex_handle((unsigned int)current_point_num);
    current_fh = mesh.property(VertexToFace, current_vh);
    current_hh = mesh.property(VertexToHEdge, current_vh);

    if (current_hh.is_valid())
    {
        // the incrementing vertex is mapped to an (half)edge
        // save the vertices mapped to the two faces incident to the edge
        saveVhsMappedToEdge();

        // split edge
        mesh.split(mesh.edge_handle(current_hh), current_vh);
    }
    else if (current_fh.is_valid())
    {
        // save vertices mapped to this face
        // coz properties will be destroyed after split
        saveVhsMappedToFace();

        emit signalBeforeSplit(current_fh);

        // split face
        mesh.split(current_fh, current_vh);
    }

    // rebucket (caused by face_split)
    rebucket(current_vh, vhs_buffer);

    // legalize accept two params:
    //       /\ <-hh
    // vh->*/__\
    // legalize each triangle
    for(auto& hh : mesh.voh_range(current_vh))
    {
        legalizing_hh = mesh.next_halfedge_handle(hh);
        legalize();
    }
    //    }

    current_point_num++;


    // delete infinite vertices
    //deleteVertices(all_points.size());
}

//void DelaunayStepByStep::perform(PointVec& all_points)
//{
//    // add all vertices into the mesh
//    init(all_points);

//    // add the big triangle and link vertices and faces
//    //init();

//    // will be error if as follows, don't know why:
//    //for(auto& vh : mesh.vertices())
//    //for(auto vit = mesh.vertices_begin(); vit != mesh.vertices_end(); vit++)

//    // start triangulation
//    for(size_t i = 0; i < all_points.size(); i++)
//    {

//        if ( i == 4)
//        {
//            break;
//        }

//        VHandle vh = mesh.vertex_handle((unsigned int)i);
//        FHandle fh = mesh.property(VertexToFace, vh);
//        HHandle hh = mesh.property(VertexToHEdge, vh);

//        VHandleVec vhs_buffer;

//        if (hh.is_valid())
//        {
//            // the incrementing vertex is mapped to an (half)edge
//            // save the vertices mapped to the two faces incident to the edge
//            saveVhsMappedToEdge();

//            // split edge
//            mesh.split(mesh.edge_handle(hh), vh);
//        }
//        else if (fh.is_valid())
//        {
//            // save vertices mapped to this face
//            // coz properties will be destroyed after split
//            saveVhsMappedToFace();

//            // split face
//            mesh.split(fh, vh);
//        }
//        else
//        {
//            continue;
//        }

//        // rebucket (caused by face_split)
//        rebucket(vh, vhs_buffer);

//        // legalize accept two params:
//        //       /\ <-hh
//        // vh->*/__\
//        // legalize each triangle
//        for(auto& hh : mesh.voh_range(vh))
//        {
//            legalize(mesh.next_halfedge_handle(hh), vh);
//        }
//    }

//    // delete infinite vertices
//    //deleteVertices(all_points.size());
//}

void DelaunayStepByStep::drawMesh()
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

void DelaunayStepByStep::drawTriangle(FHandle fh)
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

void DelaunayStepByStep::addVertices(PointVec& points)
{
    //mesh.clear();
    //for(auto& p : points)
    //    mesh.add_vertex(p);
    mesh.clear();
    for(int i = 0; i < points.size(); i++)
    {
        mesh.add_vertex(points[i]);
    }
}

void DelaunayStepByStep::init(PointVec& points)
{
    mesh.clear();
    for(int i = 0; i < points.size(); i++)
    {
        mesh.add_vertex(points[i]);
    }

    // add properties
    mesh.add_property(FaceToVertices);
    mesh.add_property(VertexToFace);
    mesh.add_property(VertexToHEdge);

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

bool DelaunayStepByStep::isInCircle(HHandle hh, VHandle vh1, VHandle vh2)
{
    // boundary edge
    // an edge is boundary edge, when one of its halfedges
    // is boundary. Next line is equl functioanally.
    // if(mesh.is_boundary(mesh.edge_handle(hh)))
    if (mesh.is_boundary(hh) ||
            mesh.is_boundary(mesh.opposite_halfedge_handle(hh)))
        return false;

    Point pt[4];
    pt[0] = mesh.point(mesh.from_vertex_handle(hh));
    pt[1] = mesh.point(mesh.to_vertex_handle(hh));
    pt[2] = mesh.point(vh1);
    pt[3] = mesh.point(vh2);

    // deal with infinite point
    if (isInfinite(pt[3]))
    {
        if (!(isInfinite(pt[0])|| isInfinite(pt[1])))
            return false;
    }

    for (int i = 0; i < 3; i++)
    {
        pt[i] -= pt[3];
        pt[i][2] = pt[i][0] * pt[i][0] + pt[i][1] * pt[i][1];
    }

    float rst = dot(cross(pt[0], pt[1]), pt[2]);
    return rst > 0;
}

bool DelaunayStepByStep::isLeft(Point& p, Point& a, Point& b)
{
    float rst = p[0] * a[1] - p[1] * a[0] +
            a[0] * b[1] - a[1] * b[0] +
            b[0] * p[1] - b[1] * p[0];

    return rst > ESP;
}

bool DelaunayStepByStep::isOverlap(VHandle vh1, VHandle vh2)
{
    Point &p = mesh.point(vh1);
    Point &q = mesh.point(vh2);
    Point dp = p - q;

    return dp.l1_norm() < ESP;
}

bool DelaunayStepByStep::isOnEdge(Point& p, HHandle hh)
{
    Point a = mesh.point(mesh.to_vertex_handle(hh));
    Point b = mesh.point(mesh.from_vertex_handle(hh));

    float rst = p[0] * a[1] - p[1] * a[0] +
            a[0] * b[1] - a[1] * b[0] +
            b[0] * p[1] - b[1] * p[0];

    return abs(rst) <= ESP;
}

bool DelaunayStepByStep::isInTriangle(Point& point, FHandle fh)
{
    PointVec triangle_points;

    for (auto& vh : mesh.fv_range(fh))
    {
        triangle_points.push_back(mesh.point(vh));
    }

    bool b1 = isLeft(point, triangle_points[0], triangle_points[1]);
    bool b2 = isLeft(point, triangle_points[1], triangle_points[2]);
    bool b3 = isLeft(point, triangle_points[2], triangle_points[0]);

    return (b1 == b2) && (b2 == b3);
}

bool DelaunayStepByStep::isInfinite(Point& pt)
{
    // the second coordinate is INF
    return INF - abs(pt[1]) < ESP;
}

void DelaunayStepByStep::rebucket(VHandle vh, VHandleVec& vhvec)
{
    // get all outgoing half_edge around the vertex
    HHandleVec hhvec;
    //   for (auto vhit = mesh.voh_begin(vh); vhit != mesh.voh_end(vh); vhit++ )
    //{
    //	hhvec.push_back(*vhit);
    //}

    for(auto& hh : mesh.voh_range(vh))
    {
        hhvec.push_back(hh);
    }

    //for(auto& hh : mesh)

    // get all face handles around the vertex
    FHandleVec fhvec;
    //for(auto &hh : hhvec)
    //{
    //    fhvec.push_back(mesh.face_handle(hh));
    //}

    //for(auto& fh : mesh.vf_range(vh))
    //{
    //    fhvec.push_back(fh);
    //}

    for(auto fit = mesh.vf_begin(vh); fit != mesh.vf_end(vh); fit++)
    {
        fhvec.push_back(*fit);
    }


    // following code lead to compiler crash:
    //for(auto& fhi : mesh.vf_range(vh))
    //    fhvec.push_back(fhi);

    // clear the incident faces' property
    for(auto& fh : fhvec)
    {
        mesh.property(FaceToVertices, fh).clear();
    }

    // what's the diff up and down

    // reset vertex's property
    for (auto& vh : vhvec)
    {
        mesh.property(VertexToFace, vh).invalidate();
        mesh.property(VertexToHEdge, vh).invalidate();
    }

    // for every vertex influenced, find new Face/Edge it belongs to
    for(auto& vhi : vhvec)
    {
        // ENSURE(vh != vhi) and deal with points OVERLAP
        if (isOverlap(vhi, vh))
        {
            continue;
        }

        // find new face it belongs to
        for(auto& fh : fhvec)
        {
            if (isInTriangle(mesh.point(vhi), fh))
            {
                mesh.property(FaceToVertices, fh).push_back(vhi);
                mesh.property(VertexToFace, vhi) = fh;
                break;
            }

            // if (isOnTriangleEdges)

        }

        // the vertex is not IN any triangle
        // check whether lies ON an edge of the triangles
        if (!mesh.property(VertexToFace, vhi).is_valid())
        {
            for (auto &hhi : hhvec)
            {
                // find new edge it belongs to
                HHandle hh_on;
                HHandle hh_next =  mesh.next_halfedge_handle(hhi);
                if (isOnEdge(mesh.point(vhi), hhi))
                {
                    hh_on = hhi;
                }
                else if (isOnEdge(mesh.point(vhi),hh_next))
                {
                    hh_on = hh_next;
                }

                if (hh_on.is_valid())
                {
                    mesh.property(VertexToHEdge, vhi) = hh_on;
                    FHandle fh = mesh.face_handle(hh_on);
                    mesh.property(FaceToVertices, fh).push_back(vhi);
                    mesh.property(VertexToFace, vhi) = fh;
                    break;
                }
            }
        }
    }
}

void DelaunayStepByStep::rebucket(EHandle eh, VHandleVec& vhvec)
{
    HHandle hh = mesh.halfedge_handle(eh, 1);
    FHandle fh1 = mesh.face_handle(hh);
    FHandle fh2 = mesh.opposite_face_handle(hh);

    // reset face property
    mesh.property(FaceToVertices, fh1).clear();
    mesh.property(FaceToVertices, fh2).clear();

    // reset vertex's property
    for (auto &vhi : vhvec)
    {
        mesh.property(VertexToFace, vhi).invalidate();
        mesh.property(VertexToHEdge, vhi).invalidate();
    }

    // rebucket vertices
    Point start = mesh.point(mesh.from_vertex_handle(hh));
    Point end = mesh.point(mesh.to_vertex_handle(hh));
    for(auto& vhi : vhvec)
    {
        if (isLeft(mesh.point(vhi), start, end))
        {
            mesh.property(FaceToVertices, fh1).push_back(vhi);
            mesh.property(VertexToFace, vhi) = fh1;
        }
        else if (isLeft(mesh.point(vhi), end, start))
        {
            mesh.property(FaceToVertices, fh2).push_back(vhi);
            mesh.property(VertexToFace, vhi) = fh2;
        }
        else
        {
            // on edge
            mesh.property(VertexToHEdge, vhi) = hh;
            FHandle fh = mesh.face_handle(hh);
            mesh.property(FaceToVertices, fh).push_back(vhi);
            mesh.property(VertexToFace, vhi) = fh;
        }
    }
}

void DelaunayStepByStep::saveVhsMappedToFace()
{
    vhs_buffer.clear();
    VHandleVec& vhvec = mesh.property(FaceToVertices, current_fh);
    vhs_buffer.resize(vhvec.size());
    copy(vhvec.begin(), vhvec.end(), vhs_buffer.begin());
}

void DelaunayStepByStep::saveVhsMappedToEdge()
{
    vhs_buffer.clear();
    FHandle fh1 = mesh.face_handle(current_hh);
    FHandle fh2 = mesh.opposite_face_handle(current_hh);
    VHandleVec& vhvec1 = mesh.property(FaceToVertices, fh1);
    VHandleVec& vhvec2 = mesh.property(FaceToVertices, fh2);
    vhs_buffer.resize(vhvec1.size() + vhvec2.size());
    auto vit = copy(vhvec1.begin(), vhvec1.end(), vhs_buffer.begin());
    copy(vhvec2.begin(), vhvec2.end(), vit);
}

void DelaunayStepByStep::legalize()
{
    /*           ________ vh_oppo
     *          /\      /
     *         /  \hh  /
     *        /    \  /
     *    vh /___>__\/
     */

    VHandle vh_oppo = mesh.opposite_he_opposite_vh(legalizing_hh);
    if (isInCircle(legalizing_hh, current_vh, vh_oppo))
    {
        // save vertex handles mapped to the face
//        VHandleVec vhs_buffer;
//        saveVhs(legalizing_hh, vhs_buffer);
        saveVhsMappedToEdge();

        // flip edge
        EHandle eh = mesh.edge_handle(legalizing_hh);
        if (mesh.is_flip_ok(eh))
        {
            // save the state before flip
            fliprec.save(current_vh, vh_oppo, legalizing_hh, mesh);

            //qDebug() << "before emit";
            if (isStepDemo)
            {
                emit signalBeforeFlip();
                //Sleep(200);
                // TODO: step by step control: pause/next
                // send a signal to draw flip
                // wait a second?
            }
            mesh.flip(eh);
        }

        // rebucket
        rebucket(eh, vhs_buffer);

        // recursive
        HHandle heh1, heh2;
        heh1 = mesh.halfedge_handle(eh, 0);
        heh2 = mesh.halfedge_handle(eh, 1);
        if (mesh.to_vertex_handle(heh1) == current_vh)
        {
            heh1 = mesh.prev_halfedge_handle(heh1);
            heh2 = mesh.next_halfedge_handle(heh2);
        }
        else{
            heh1 = mesh.next_halfedge_handle(heh1);
            heh2 = mesh.prev_halfedge_handle(heh2);
        }

        legalize(heh1, current_vh);
        legalize(heh2, current_vh);
    }
}

void DelaunayStepByStep::deleteVertices(int n)
{
    for (int i = n; i < n + 3; i++)
    {
        VHandle vh = mesh.vertex_handle(i);
        mesh.delete_vertex(vh);
    }

    mesh.garbage_collection();
}

void DelaunayStepByStep::test_slot()
{
    qDebug() << "DelaunayStepByStep slot!";
}
