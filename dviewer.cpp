#include "dviewer.h"
#include "../Delaunay.h"
#include <math.h>
#include <QKeyEvent>

using namespace qglviewer;
using namespace std;

namespace DViewer
{

//Delaunay delaunay;
bool isDraw = true;

Viewer::Viewer(std::unique_ptr<Delaunay> delaunay) : mdelaunay(std::move(delaunay)) { }

void Viewer::drawPoints()
{
    glPointSize(4.0);
    
    glBegin(GL_POINTS);
    for (auto& point: points)
    {
        glColor3f(1.0,0.0,0.0);
        glVertex3f(point[0] / 1000, point[1] / 1000, point[2]);
    }
    glEnd();
}

void Viewer::init()
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
}

void Viewer::draw()
{
    // Here we are in the world coordinate system. Draw unit size axis.
    drawAxis();
    drawParaboloid();
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
        mdelaunay->drawMeshInQt();

    // Restore the original (world) coordinate system
    //glPopMatrix();
}



//QString Viewer::helpString() const
//{
//    QString text("<h2>M a n i p u l a t e d F r a m e</h2>");
//    text += "A <i>ManipulatedFrame</i> converts mouse gestures into <i>Frame</i> displacements. ";
//    text += "In this example, such an object defines the position of the spiral that can hence be manipulated.<br><br>";
//    text += "Adding two lines of code will then allow you to move the objects of ";
//    text += "your scene using the mouse. The button bindings of the <i>ManipulatedFrame</i> ";
//    text += "are the same than for the camera. Spinning is possible.<br><br>";
//    text += "Default key bindings have been changed in this example : press <b>Alt</b> ";
//    text += "while moving the mouse to move the camera instead of the ManipulatedFrame.";
//    text += "<img src=\"http://www.baidu.com/img/bdlogo.gif\" />";
//    return text;
//}

void Viewer::keyPressEvent(QKeyEvent *e)
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

#define GETX(u, v) (u * cos(v))
#define GETY(u, v) (u * sin(v))

void Viewer::drawParaboloid()
{
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    float step_v = M_PI / 50;
    float step_u = 0.01;
    glEnable(GL_BLEND);
    glEnable(GL_POLYGON_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.6f, 0.6f, 0.7f, 0.4f);
    glBegin(GL_TRIANGLES);
    for (float u = 0; u < 1; u += step_u)
    {
        for (float v = 0; v < 2 * M_PI; v += step_v)
        {
            glNormal3f(2 * GETX(u, v), 2 * GETY(u, v), -1);
            glVertex3f(GETX((u), (v)), GETY((u), (v)), u * u);
            glVertex3f(GETX((u), (v + step_v)), GETY((u), (v + step_v)), u * u);
            glVertex3f(GETX((u + step_u), (v + step_v)), GETY((u + step_u), (v + step_v)), (u + step_u) * (u + step_u));

            glVertex3f(GETX((u), (v)), GETY((u), (v)), u * u);
            glVertex3f(GETX((u + step_u), (v + step_v)), GETY((u + step_u), (v + step_v)), (u + step_u) * (u + step_u));
            glVertex3f(GETX((u + step_u), (v)), GETY((u + step_u), (v)), (u + step_u) * (u + step_u));

        }
    }
    glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
    glDisable(GL_BLEND);
    glDisable(GL_POLYGON_SMOOTH);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
    glEnd();
}
}