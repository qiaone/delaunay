#include "dviewer.h"
#include "../Delaunay.h"
#include <math.h>
#include <QKeyEvent>

using namespace qglviewer;
using namespace std;

bool isDraw = true;

DViewer::DViewer(QWidget *parent)
    : QGLViewer(parent)
{

}

DViewer::DViewer(std::unique_ptr<Delaunay> delaunay, int mainwindow_width, int mainwindow_height)
    : _delaunay(std::move(delaunay)), _mainwindow_width(mainwindow_width), _mainwindow_height(mainwindow_height) { }

void DViewer::setParam(std::unique_ptr<Delaunay> delaunay, int mainwindow_width, int mainwindow_height)
{
    _delaunay = std::move(delaunay);
    _mainwindow_width = mainwindow_width;
    _mainwindow_height = mainwindow_height;
}

void DViewer::drawFlip()
{
//    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
//    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

//    glEnable(GL_BLEND);
//    //    glEnable(GL_POLYGON_SMOOTH);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glColor4f(0.6f, 0.6f, 0.7f, 0.4f);

//    glBegin(GL_TRIANGLE_STRIP);
//    for (float u = 0; u < 1; u += step_u)
//    {
//        for (float v = 0; v < 2 * M_PI + step_v; v += step_v)
//        {
//            Point Tu, Tv, N;

//            // calculate Normal
//            Tu = Point(cos(v), sin(v), 2 * u);
//            Tv = Point(-sin(v), cos(v), 0);
//            N = cross(Tu, Tv);
//            N.normalize();
//            glNormal3f(N[0], N[1], N[2]);
//            glVertex3f(getx(u, v) + lPt[0], gety(u, v) + lPt[1], u * u + lPt[2]);

//            // calculate Normal
//            Tu = Point(cos(v), sin(v), 2 * (u + step_u));
//            N = cross(Tu, Tv);
//            N.normalize();
//            glNormal3f(N[0], N[1], N[2]);

//            glVertex3f(getx((u + step_u), (v)) + lPt[0],
//                    gety((u + step_u), (v)) + lPt[1],
//                    (u + step_u) * (u + step_u) + lPt[2]);
//        }
//    }
//    glEnd();

//    glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
//    glDisable(GL_BLEND);
//    //glDisable(GL_POLYGON_SMOOTH);
//    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
}

void DViewer::drawMesh()
{
    glPointSize(4.0);
    glColor3f(1, 0, 0);
    glBegin(GL_POINTS);

    for (auto& vh: _delaunay->mesh.vertices())
    {
        auto point = _delaunay->mesh.point(vh);
        glColor3f(1.0,0.0,0.0);
        glVertex3f((point[0] - _mainwindow_width / 2) / 400, (_mainwindow_height / 2 - point[1]) / 400, point[2]);
    }
    glEnd();

    glColor3f(0, 0, 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_TRIANGLES);
    for (auto& fh : _delaunay->mesh.faces())
    {
        for(auto& vh : _delaunay->mesh.fv_range(fh))
        {
            auto point = _delaunay->mesh.point(vh);
            glVertex3f((point[0] - _mainwindow_width / 2) / 400, (_mainwindow_height / 2 - point[1]) / 400, point[2]);
        }
    }
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}

void DViewer::init()
{
    setKeyDescription(Qt::Key_Space, "Perform Delaunay Triangulation");
    //setKeyDescription(Qt::Key_F, "Toggles flat shading display");

    //setMouseBindingDescription(Qt::NoModifier, Qt::MidButton, "draw points");


#ifdef GL_RESCALE_NORMAL  // OpenGL 1.2 Only...
    glEnable(GL_RESCALE_NORMAL);
#endif

    // Make sure the manipulatedFrame is not easily clipped by the zNear and zFar planes
    setSceneRadius(1);
    camera()->fitSphere(Vec(0,0,0), 1);

    //help();
    //restoreStateFromFile();

    // auto-normalize
    glEnable(GL_NORMALIZE);

    glShadeModel(GL_SMOOTH);

    // create display list
    listName = glGenLists(1);
    glNewList(listName, GL_COMPILE);
    drawParaboloid(Point(0, 0, 0.05), 50.0, 50.0);
    glEndList();
}

void DViewer::draw()
{
    // Here we are in the world coordinate system. Draw unit size axis.
    // drawAxis();
    // drawParaboloid();

    // draw paraboloid
    glCallList(listName);

    //// Save the current model view matrix (not needed here in fact)
    //glPushMatrix();
    //// Multiply matrix to get in the frame coordinate system.
    //glMultMatrixd(manipulatedFrame()->matrix());
    // Scale down the drawings
    //glScalef(5.0f, 5.0f, 5.0f);
    // Draw an axis using the QGLViewer static function
    //drawAxis();

    //drawPoints();
    if (isDraw)
        drawMesh();

    // Restore the original (world) coordinate system
    //glPopMatrix();
}

void DViewer::keyPressEvent(QKeyEvent *e)
{
    const Qt::KeyboardModifiers modifiers = e->modifiers();
    bool handled = false;
    if ((e->key()==Qt::Key_Space) && (modifiers==Qt::NoButton))
    {
        isDraw = true;
        //        delaunay.perform(points);
        updateGL();
    }
    else
        if ((e->key()==Qt::Key_F) && (modifiers==Qt::NoButton))
        {
            // do sth
            handled = true;
            updateGL();
        }

    if (!handled)
        QGLViewer::keyPressEvent(e);
}

//void Viewer::mousePressEvent(QMouseEvent* e)
//{
////    if ((e->button() == Qt::MidButton) && (e->modifiers() == Qt::NoButton))
////    {
////        int x = e->x();
////        int y = e->y();
////        Point pt(x - this->width() / 2, this->height() / 2 - y, 0);
////        points.push_back(pt);
////        updateGL();
////    }
////    else
////        QGLViewer::mousePressEvent(e);
////}
//

//#define getx(u, v) (u * cos(v))
//#define gety(u, v) (u * sin(v))

inline float getx(float u, float v)
{
    return u * cos(v);
}

inline float gety(float u, float v)
{
    return u * sin(v);
}

// lPt: the lowest point of the paraboloid
// slice , stack: control the number of triangles 
void DViewer::drawParaboloid(Point lPt, float slice, float stack )
{
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    const float step_v = (float)(M_PI / slice);
    const float step_u = 1.0 / stack;

    glEnable(GL_BLEND);
    //    glEnable(GL_POLYGON_SMOOTH);
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
            glVertex3f(getx(u, v) + lPt[0], gety(u, v) + lPt[1], u * u + lPt[2]);

            // calculate Normal
            Tu = Point(cos(v), sin(v), 2 * (u + step_u));
            N = cross(Tu, Tv);
            N.normalize();
            glNormal3f(N[0], N[1], N[2]);

            glVertex3f(getx((u + step_u), (v)) + lPt[0],
                    gety((u + step_u), (v)) + lPt[1],
                    (u + step_u) * (u + step_u) + lPt[2]);
        }
    }
    glEnd();

    glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
    glDisable(GL_BLEND);
    glDisable(GL_POLYGON_SMOOTH);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);

}

