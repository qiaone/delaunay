#ifndef DELAUNAY_H
#define DELAUNAY_H

#include <array>
#include <vector>
#include <QObject>
#include <glm/glm.hpp>
#include <GL/glut.h>
#include "def.h"
#include "fliprecord.h"

class Delaunay : QObject
{
    Q_OBJECT

protected:
    OpenMesh::FPropHandleT<VHandleVec> FaceToVertices;
    OpenMesh::VPropHandleT<FHandle> VertexToFace;
	OpenMesh::VPropHandleT<HHandle> VertexToHEdge;

public:
    Delaunay();
    Delaunay(bool isStepDemo_);
    void perform(PointVec& all_points);
    void performStepByStep(PointVec& all_points);
    void drawMesh();
    TriMesh mesh;
    FlipRecord fliprec;
    bool isStepDemo;

private:
    void drawTriangle(FHandle fh);
    void addVertices(PointVec& points);
    void init();
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

signals:
    void drawBeforeFlip();

public slots:
    void test_slot();
};

#endif // DELAUNAY_H
