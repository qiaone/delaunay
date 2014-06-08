#pragma once
#include "delaunayincremental.h"
#include <QDebug>
#include <QTime>
#include <QThread>
#include <QApplication>

const float INF = 1.0e5f;
const float ESP = 1.0e-6f;

DelaunayIncremental::DelaunayIncremental() :
    delay_seconds(0),
    total_points_count(0)
{
    mesh.clear();

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
    FHandle big_triangle_fh = mesh.add_face(vhs, 3);
}

void DelaunayIncremental::pointLocation(VHandle& vh)
{
    for(auto& fh : mesh.faces())
    {
        if (isInTriangle(mesh.point(vh), fh))
        {
            mesh.property(FaceToVertices, fh).push_back(vh);
            mesh.property(VertexToFace, vh) = fh;
            break;
        }
        // if (isOnTriangleEdges)
    }
}

void DelaunayIncremental::performIncremental(Point new_point)
{
    new_vh = mesh.add_vertex(new_point);

    pointLocation(new_vh);

    FHandle fh = mesh.property(VertexToFace, new_vh);
    HHandle hh = mesh.property(VertexToHEdge, new_vh);

    if (hh.is_valid())
    {
        emit signalBeforeSplit(hh);

        // split edge
        mesh.split(mesh.edge_handle(hh), new_vh);

    }
    else if (fh.is_valid())
    {
        emit signalBeforeSplit(fh);

        // split face
        mesh.split(fh, new_vh);
    }

    //// legalize each triangle
    //for(auto& hh : mesh.voh_range(new_vh))
    //{
    //    legalize_queue.push(mesh.next_halfedge_handle(hh));
    //}

    //while (!legalize_queue.empty())
    //{
    //    legalize(legalize_queue.front());
    //    legalize_queue.pop();
    //}
    // delete infinite vertices
    //deleteVertices(total_points_count);

    total_points_count++;

}

bool DelaunayIncremental::isInCircle(HHandle hh, VHandle vh1, VHandle vh2)
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

bool DelaunayIncremental::isLeft(Point& p, Point& a, Point& b)
{
    float rst = p[0] * a[1] - p[1] * a[0] +
            a[0] * b[1] - a[1] * b[0] +
            b[0] * p[1] - b[1] * p[0];

    return rst > ESP;
}

bool DelaunayIncremental::isOverlap(VHandle vh1, VHandle vh2)
{
    Point &p = mesh.point(vh1);
    Point &q = mesh.point(vh2);
    Point dp = p - q;

    return dp.l1_norm() < ESP;
}

bool DelaunayIncremental::isOnEdge(Point& p, HHandle hh)
{
    Point a = mesh.point(mesh.to_vertex_handle(hh));
    Point b = mesh.point(mesh.from_vertex_handle(hh));

    float rst = p[0] * a[1] - p[1] * a[0] +
            a[0] * b[1] - a[1] * b[0] +
            b[0] * p[1] - b[1] * p[0];

    return abs(rst) <= ESP;
}

bool DelaunayIncremental::isInTriangle(Point& point, FHandle fh)
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

bool DelaunayIncremental::isInfinite(Point& pt)
{
    // the second coordinate is INF
    return INF - abs(pt[1]) < ESP;
}

void DelaunayIncremental::legalize(HHandle hh)
{
    /*           ________ vh_oppo
     *          /\      /
     *         /  \hh  /
     *        /    \  /
     *    vh /___>__\/
     */

    VHandle vh_oppo = mesh.opposite_he_opposite_vh(hh);
    if (isInCircle(hh, new_vh, vh_oppo))
    {
        emit signalBeforeFlip(hh, new_vh, vh_oppo);

        // flip edge
        EHandle eh = mesh.edge_handle(hh);
        if (mesh.is_flip_ok(eh))
        {
            mesh.flip(eh);
            emit signalAfterFlip();

            // iterative
            HHandle heh1, heh2;
            heh1 = mesh.halfedge_handle(eh, 0);
            heh2 = mesh.halfedge_handle(eh, 1);
            if (mesh.to_vertex_handle(heh1) == new_vh)
            {
                heh1 = mesh.prev_halfedge_handle(heh1);
                heh2 = mesh.next_halfedge_handle(heh2);
            }
            else
            {
                heh1 = mesh.next_halfedge_handle(heh1);
                heh2 = mesh.prev_halfedge_handle(heh2);
            }

            legalize_queue.push(heh1);
            legalize_queue.push(heh2);
        }
    }
}

void DelaunayIncremental::deleteVertices(int n)
{
    for (int i = n; i < n + 3; i++)
    {
        VHandle vh = mesh.vertex_handle(i);
        mesh.delete_vertex(vh);
    }

    mesh.garbage_collection();
}
