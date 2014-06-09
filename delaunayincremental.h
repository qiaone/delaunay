#ifndef DELAUNAYINCREMENTAL_H
#define DELAUNAYINCREMENTAL_H

#include <vector>
#include <queue>
#include <QObject>
#include "def.h"
#include <array>

class DelaunayIncremental : public QObject
{
    Q_OBJECT

protected:
    OpenMesh::FPropHandleT<VHandleVec> FaceToVertices;
    OpenMesh::VPropHandleT<FHandle> VertexToFace;
    OpenMesh::VPropHandleT<HHandle> VertexToHEdge;

public:
    DelaunayIncremental();
    void performIncremental(Point new_point);
    TriMesh mesh;
    std::queue<HHandle> legalize_queue;
    VHandle new_vh;
    bool isInCircle(HHandle hh, VHandle vh, VHandle vh_oppo);
    std::vector<std::array<Point, 4>> flip_records;
    bool isFlipped;
    bool hasInfinitePoint(const FHandle &fh);
    bool hasInfinitePoint(const std::array<Point, 4>& points);

private:
    bool isLeft(Point& p, Point& a, Point& b);
    bool isOverlap(VHandle vh1, VHandle vh2);
    bool isOnEdge(Point& pt, HHandle hh);
    bool isInTriangle(Point& pt, FHandle fh);
    bool isInfinite(Point& pt);
    void legalize(HHandle hh);
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
