#include "dviewer.h"
#include <math.h>
#include <QKeyEvent>
#include <QDebug>

using namespace qglviewer;
using namespace std;

DViewer::DViewer(QWidget *parent)
    : QGLViewer(parent), isDrawResult(false)
{

}

DViewer::DViewer(DelaunayIncremental* delaunay_inc_, int mainwindow_width_, int mainwindow_height_)
    : delaunay_inc(delaunay_inc_), mainwindow_width(mainwindow_width_), mainwindow_height(mainwindow_height_) { }

void DViewer::setParam(DelaunayIncremental* delaunay_inc_, int mainwindow_width_, int mainwindow_height_)
{
    delaunay_inc = delaunay_inc_;
    mainwindow_width = mainwindow_width_;
    mainwindow_height = mainwindow_height_;
}

void DViewer::showFlips3D( )
{
    // show all flips during a new point added
    for (auto& flip : delaunay_inc->flip_records)
    {
        if (delaunay_inc->hasInfinitePoint(flip))
        {
            continue;
        }
//        flipping_triangles.clear();
//        flipping_triangles
//            << QPoint(flip[0][0],flip[0][1])
//            << QPoint(flip[1][0],flip[1][1])
//            << QPoint(flip[2][0],flip[2][1])
//            << QPoint(flip[3][0],flip[3][1]);
//        showCircle2D();
//        isShowCircle = true;
//        isShowBeforeFlip = true;
//        update();

//        QTime t;
//        t.start();
//        while(t.elapsed() < 500)
//            QCoreApplication::processEvents();

//        isShowAfterFlip = true;
//        update();

//        t.start();
//        while(t.elapsed() < 500)
//            QCoreApplication::processEvents();

//        isShowAfterFlip = false;
    }
//    isShowCircle = false;
//    isShowBeforeFlip = false;
//    update();
}

    void DViewer::showResult3D()
{
    isDrawResult = true;
    update();
}

void DViewer::drawMesh()
{
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glPointSize(4.0);
    glColor3f(1, 0, 0);
    glBegin(GL_POINTS);
    for (auto& vh: delaunay_inc->mesh.vertices())
    {
        auto point = delaunay_inc->mesh.point(vh);
        glColor3f(1.0,0.0,0.0);
        glVertex3f((point[0] - 300) / 400, (300 - point[1]) / 400, 0);
        //glVertex3f(point[0] / 400, -point[1] / 400, 0);
    }
    glEnd();

    glColor3f(0, 0, 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_TRIANGLES);
    for (auto& fh : delaunay_inc->mesh.faces())
    {
        if (delaunay_inc->hasInfinitePoint(fh))
        {
            continue;
        }

        for(auto& vh : delaunay_inc->mesh.fv_range(fh))
        {
            auto point = delaunay_inc->mesh.point(vh);
            float x = (point[0] - 300) / 400;
            float y = (300 - point[1]) / 400;
            glVertex3f(x, y, 0);
            glNormal3f(1, 1, 0);
        }
    }
    glEnd();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(0, 1, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glBegin(GL_TRIANGLES);
    for (auto& fh : delaunay_inc->mesh.faces())
    {
        if (delaunay_inc->hasInfinitePoint(fh))
        {
            continue;
        }

        for(auto& vh : delaunay_inc->mesh.fv_range(fh))
        {
            auto point = delaunay_inc->mesh.point(vh);
            float x = (point[0] - 300) / 400;
            float y = (300 - point[1]) / 400;
            glVertex3f(x, y, x * x + y * y + 0.1);
            glNormal3f(2 * x, 2 * y, -1);
        }
    }
    glEnd();
    glDisable(GL_BLEND);
}

void DViewer::init()
{
#ifdef GL_RESCALE_NORMAL  // OpenGL 1.2 Only...
    glEnable(GL_RESCALE_NORMAL);
#endif

    setMouseBinding(Qt::NoModifier, Qt::LeftButton, CAMERA, TRANSLATE);
    setMouseBinding(Qt::NoModifier, Qt::RightButton, CAMERA, ROTATE);

    // Make sure the manipulatedFrame is not easily clipped by the zNear and zFar planes
    setSceneRadius(2);
    camera()->fitSphere(Vec(0, 0, 0), 1);

    //help();
    //restoreStateFromFile();

    // auto-normalize
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    // create display list
    paraboloidListId = glGenLists(1);
    glNewList(paraboloidListId, GL_COMPILE);
    drawParaboloid(Point(0.0f, 0.0f, 0.1f), 50.0f, 50.0f);
    glEndList();
}

void DViewer::draw()
{
    // Here we are in the world coordinate system. Draw unit size axis.
    // drawAxis();


    //// Save the current model view matrix (not needed here in fact)
    //glPushMatrix();
    //// Multiply matrix to get in the frame coordinate system.
    //glMultMatrixd(manipulatedFrame()->matrix());
    // Scale down the drawings
    //glScalef(5.0f, 5.0f, 5.0f);
    // Draw an axis using the QGLViewer static function
    //drawAxis();

    //drawPoints();
    if (isDrawResult)
        drawMesh();


    glDepthMask(GL_FALSE); //将深度缓存设置为只读状态
    // draw paraboloid
    glCallList(paraboloidListId);
    glDepthMask(GL_TRUE); //深度缓存设置为正常状态
    // Restore the original (world) coordinate system
    //glPopMatrix();


}

inline float getx(float u, float v)
{
    return u * cos(v);
}

inline float gety(float u, float v)
{
    return u * sin(v);
}

// bottom_point: the lowest point of the paraboloid
// slice , stack: control the number of triangles 
void DViewer::drawParaboloid(Point bottom_point, float slice, float stack)
{
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    const float step_v = (float)(M_PI / slice);
    const float step_u = 1.0 / stack;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.6f, 0.6f, 0.7f, 0.4f);

    glBegin(GL_TRIANGLE_STRIP);
    for (float u = 0; u < 1; u += step_u)
    {
        for (float v = 0; v < 2 * M_PI + step_v; v += step_v)
        {
            Point Tu, Tv, N;

            // calculate Normal
            Tu = Point(cos(v), sin(v), 2 * u);
            Tv = Point(-sin(v), cos(v), 0);
            N = cross(Tu, Tv);
            N.normalize();
            glNormal3f(N[0], N[1], N[2]);
            glVertex3f(getx(u, v) + bottom_point[0], gety(u, v) + bottom_point[1], u * u + bottom_point[2]);

            // calculate Normal
            Tu = Point(cos(v), sin(v), 2 * (u + step_u));
            N = cross(Tu, Tv);
            N.normalize();
            glNormal3f(N[0], N[1], N[2]);

            glVertex3f(getx((u + step_u), (v)) + bottom_point[0],
                    gety((u + step_u), (v)) + bottom_point[1],
                    (u + step_u) * (u + step_u) + bottom_point[2]);
        }
    }
    glEnd();

    glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
    glDisable(GL_BLEND);
    glDisable(GL_POLYGON_SMOOTH);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);

}

