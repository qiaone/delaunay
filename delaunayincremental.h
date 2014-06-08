#ifndef DELAUNAYINCREMENTAL_H
#define DELAUNAYINCREMENTAL_H

#include <array>
#include <vector>
#include <QObject>
#include <glm/glm.hpp>
#include <GL/glut.h>
#include "def.h"

class DelaunayIncremental : public QObject
{
    Q_OBJECT

protected:
    OpenMesh::FPropHandleT<VHandleVec> FaceToVertices;
    OpenMesh::VPropHandleT<FHandle> VertexToFace;
    OpenMesh::VPropHandleT<HHandle> VertexToHEdge;

public:
    DelaunayIncremental();
    void performIncremental(Point point);
    TriMesh mesh;

private:
    FHandle big_triangle_fh;
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
    int current_point_num;
    int total_points_count;
    int delay_seconds;
    void pointLocation(VHandle& vh);

signals:
    void signalBeforeSplit(FHandle);
    void signalAfterSplit(FHandle);
    void signalBeforeSplit(HHandle);
    void signalAfterSplit(HHandle);
    void signalBeforeFlip(HHandle, VHandle, VHandle);
    void signalAfterFlip();
};

#endif // DELAUNAYINCREMENTAL_H
