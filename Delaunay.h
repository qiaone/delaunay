#ifndef DELAUNAY_H
#define DELAUNAY_H

#include <array>
#include <vector>
#include <QObject>
#include "def.h"

class Delaunay : public QObject
{
    Q_OBJECT

protected:
    OpenMesh::FPropHandleT<VHandleVec> FaceToVertices;
    OpenMesh::VPropHandleT<FHandle> VertexToFace;
    OpenMesh::VPropHandleT<HHandle> VertexToHEdge;

public:
    Delaunay();
    void perform();
    void perform(PointVec& all_points);
    void drawMesh();
    void init(PointVec& points);
    TriMesh mesh;
    int total_points_count;

private:
    void drawTriangle(FHandle fh);
    void addVertices(PointVec& points);
    bool isInCircle(HHandle hh, VHandle vh, VHandle vh_oppo);
    bool isLeft(Point& p, Point& a, Point& b);
    bool isOverlap(VHandle vh1, VHandle vh2);
    bool isOnEdge(Point& pt, HHandle hh);
    bool isInTriangle(Point& pt, FHandle fh);
    bool isInfinite(Point& pt);
    void rebucket(VHandle vh, VHandleVec& vhvec);
    void rebucket(EHandle eh, VHandleVec& vhvec);
    void saveVhs(FHandle fh, VHandleVec &vhs_buffer);
    void saveVhs(HHandle hh, VHandleVec &vhs_buffer);
    void legalize(HHandle hh, VHandle vh);
    void deleteVertices(int n);
};

#endif // DELAUNAY_H
