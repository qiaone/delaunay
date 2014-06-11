﻿#ifndef DEF_H
#define DEF_H

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyConnectivity.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

struct MyTraits : public OpenMesh::DefaultTraits
{
    VertexAttributes(OpenMesh::Attributes::Status);
    FaceAttributes(OpenMesh::Attributes::Status);
    EdgeAttributes(OpenMesh::Attributes::Status);
};

enum FunType{ CIRCLE, ELLIPSE, NORM2};

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

const float INF = 1.0e5f;
const float ESP = 1.0e-6f;

#endif // DEF_H
