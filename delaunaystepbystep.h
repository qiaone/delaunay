#ifndef DELAUNAYSTEPBYSTEP_H
#define DELAUNAYSTEPBYSTEP_H

#include <array>
#include <vector>
#include <QObject>
#include <glm/glm.hpp>
#include <GL/glut.h>
#include "def.h"
#include "fliprecord.h"

class DelaunayStepByStep : QObject
{
    Q_OBJECT

protected:
    OpenMesh::FPropHandleT<VHandleVec> FaceToVertices;
    OpenMesh::VPropHandleT<FHandle> VertexToFace;
    OpenMesh::VPropHandleT<HHandle> VertexToHEdge;

public:
    DelaunayStepByStep();
    DelaunayStepByStep(bool isStepDemo_);
//    void perform(PointVec& all_points);
    void performStepByStep();
    void drawMesh();
    void init(PointVec& points);
    TriMesh mesh;
    FlipRecord fliprec;
    bool isStepDemo;

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
    void saveVhsMappedToFace();
    void saveVhsMappedToEdge();
    void legalize();
    void deleteVertices(int n);
    int current_point_num;

    VHandle current_vh;
    FHandle current_fh;
    HHandle current_hh, legalizing_hh;
    VHandleVec vhs_buffer;

signals:
    void signalBeforeSplit(FHandle);
    void signalAfterSplit(FHandle);
    void signalBeforeFlip();
    void signalAfterFlip();

public slots:
    void test_slot();
};

#endif // DELAUNAYSTEPBYSTEP_H
