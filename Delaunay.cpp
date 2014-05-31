﻿#include "delaunay.h"

const float INF = 1.0e5;
const float ESP = 1.0e-6;

Delaunay::Delaunay()
{ }

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
		HHandle hh = mesh.property(VertexToHEdge, vh);

		VHandleVec vhs_buffer;
		if (fh.is_valid())
		{
			// save vertices maped to this face
			// coz properties will be destroyed after split
			saveVhs(fh, vhs_buffer);

			// split face
			mesh.split(fh, vh);	
		}
		else if (hh.is_valid())
		{
			// save vertices maped to the two faces 
			saveVhs(hh, vhs_buffer);
			
			// split edge
			mesh.split(mesh.edge_handle(hh), vh);
		}
		else
		{
			continue;
		}
		
		// rebucket (caused by face_split)
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
    //mesh.clear();
    //for(auto& p : points)
    //    mesh.add_vertex(p);
    mesh.clear();
    for(int i = 0; i < points.size(); i++)
    {
        mesh.add_vertex(points[i]);
    }
}

void Delaunay::init()
{
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

bool Delaunay::isInCircle(HHandle hh, VHandle vh1, VHandle vh2)
{
    // boundary edge
	// an edge is boundary edge, when one of its halfedges
	// is boundary. Next line is equl functioanally.
	// if(mesh.is_boundary(mesh.edge_handle(hh)))
    if (mesh.is_boundary(hh) ||
            mesh.is_boundary(mesh.opposite_halfedge_handle(hh))) 
        return false;
	
	// deal with infinite point
	if (isInfinite(vh2))
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

bool Delaunay::isLeft(Point& p, Point& a, Point& b)
{
    float rst = p[0] * a[1] - p[1] * a[0] +
            a[0] * b[1] - a[1] * b[0] +
            b[0] * p[1] - b[1] * p[0];

    return rst > ESP;
}

bool Delaunay::isOverlap(VHandle vh1, VHandle vh2)
{
	Point &p = mesh.point(vh1);
	Point &q = mesh.point(vh2);
	Point dp = p - q;

	return dp.l1_norm() < ESP;
}

bool Delaunay::isOnEdge(Point& p, HHandle hh)
{
	Point a = mesh.point(mesh.to_vertex_handle(hh));
	Point b = mesh.point(mesh.from_vertex_handle(hh));

	float rst = p[0] * a[1] - p[1] * a[0] +
		a[0] * b[1] - a[1] * b[0] +
		b[0] * p[1] - b[1] * p[0];
	
	return abs(rst) <= ESP;
}

bool Delaunay::isInTriangle(Point& point, FHandle fh)
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

bool Delaunay::isInfinite(VHandle vh)
{
	// the second coordinate is INF
	Point& pt = mesh.point(vh);
	return INF - abs(pt[1]) < ESP;
}

void Delaunay::rebucket(VHandle vh, VHandleVec& vhvec)
{
	// get all outgoing half_edge around the vertex
	HHandleVec hhvec;
	for (auto vhit = mesh.voh_begin(vh); vhit !=mesh.voh_end(vh); vhit++ )
	{
		hhvec.push_back(*vhit);
	}

    // get all face handles around the vertex
    FHandleVec fhvec;	
    for(auto &hh : hhvec)
    {
        fhvec.push_back(mesh.face_handle(hh));
    }

    // following code lead to compiler crash:
    //for(auto& fhi : mesh.vf_range(vh))
    //    fhvec.push_back(fhi);

    // clear the incident faces' property
    for(auto& fh : fhvec)
    {
        mesh.property(FaceToVertices, fh).clear();
    }

	// reset vertex's property
	for (auto& vh : vhvec)
	{
		mesh.property(VertexToFace, vh).invalidate();
		mesh.property(VertexToHEdge, vh).invalidate();  // this line may be redundant
	}

    // for every vertex influenced, find new Face/Edge it belongs to
    for(auto& vhi : vhvec)
    {
		//if (vh == vhi)
		//{
		// 	continue;
		//}

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
        }

		// this point is in none of the triangles, so need
		// to check whether this point is on any Edge
		if (!mesh.property(VertexToFace, vhi).is_valid())
		{
			for (auto &hh : hhvec)
			{
				// find new edge it belongs to. 
				if (isOnEdge(mesh.point(vhi), hh))
				{
					mesh.property(VertexToHEdge, vhi) = hh;
					break;
				}
			}
		}
    }
}

void Delaunay::rebucket(EHandle eh, VHandleVec& vhvec)
{
	HHandle hh = mesh.halfedge_handle(eh, 1);
    FHandle fh1 = mesh.face_handle(hh);
    FHandle fh2 = mesh.opposite_face_handle(hh);

	// reset face property
    mesh.property(FaceToVertices, fh1).clear();
    mesh.property(FaceToVertices, fh2).clear();

	// reset vertex's property
	for (auto &vh : vhvec)
	{
		mesh.property(VertexToFace, vh).invalidate();
		mesh.property(VertexToHEdge, vh).invalidate();  // this line may be redundant
	}

	// rebucket vertices
    Point start = mesh.point(mesh.from_vertex_handle(hh));
    Point end = mesh.point(mesh.to_vertex_handle(hh));
    for(auto& vh : vhvec)
    {
        if (isLeft(mesh.point(vh), start, end))
        {
            mesh.property(FaceToVertices, fh1).push_back(vh);
            mesh.property(VertexToFace, vh) = fh1;
        }
        else if (isLeft(mesh.point(vh), end, start))
        {
            mesh.property(FaceToVertices, fh2).push_back(vh);
            mesh.property(VertexToFace, vh) = fh2;
        }
		else 
		{
			// on edge
			mesh.property(VertexToHEdge, vh) = hh;
		}
    }
}

void Delaunay::saveVhs(FHandle fh, VHandleVec &vhs_buffer)
{
	VHandleVec& vhvec = mesh.property(FaceToVertices, fh);
	vhs_buffer.resize(vhvec.size());
	copy(vhvec.begin(), vhvec.end(), vhs_buffer.begin());
}

void Delaunay::saveVhs(HHandle hh, VHandleVec &vhs_buffer)
{
	FHandle fh1 = mesh.face_handle(hh);
	FHandle fh2 = mesh.opposite_face_handle(hh);
	VHandleVec& vhvec1 = mesh.property(FaceToVertices, fh1);
	VHandleVec& vhvec2 = mesh.property(FaceToVertices, fh2);
	vhs_buffer.resize(vhvec1.size() + vhvec2.size());
	auto vit = copy(vhvec1.begin(), vhvec1.end(), vhs_buffer.begin());
	copy(vhvec2.begin(), vhvec2.end(), vit);
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
		VHandleVec vhs_buffer;
		saveVhs(hh, vhs_buffer);

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
