#include "viewer.h"
#include "../Delaunay.h"
#include <math.h>
#include <QKeyEvent>
#include <QGLViewer/manipulatedFrame.h>

using namespace qglviewer;
using namespace std;

namespace DViewer
{

Delaunay delaunay;
bool isDraw = false;
PointVec points;

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
    setShortcut(CAMERA_MODE, 0);

    setMouseBinding(Qt::AltModifier, Qt::LeftButton, QGLViewer::CAMERA, QGLViewer::TRANSLATE);
    setMouseBinding(Qt::AltModifier, Qt::RightButton, QGLViewer::CAMERA, QGLViewer::ROTATE);
    //setMouseBinding(Qt::AltModifier, Qt::MidButton, QGLViewer::CAMERA, QGLViewer::ZOOM);
    setWheelBinding(Qt::AltModifier, QGLViewer::CAMERA, QGLViewer::ZOOM);

    setMouseBinding(Qt::NoModifier, Qt::LeftButton, QGLViewer::FRAME, QGLViewer::TRANSLATE);
    setMouseBinding(Qt::NoModifier, Qt::RightButton, QGLViewer::FRAME, QGLViewer::ROTATE);
    //setMouseBinding(Qt::NoModifier, Qt::MidButton, QGLViewer::FRAME, QGLViewer::ZOOM);
    setWheelBinding(Qt::NoModifier, QGLViewer::FRAME, QGLViewer::ZOOM);

    setKeyDescription(Qt::Key_Space, "Display something");
    setKeyDescription(Qt::Key_F, "Toggles flat shading display");

    setMouseBindingDescription(Qt::NoModifier, Qt::MidButton, "draw points");


#ifdef GL_RESCALE_NORMAL  // OpenGL 1.2 Only...
    glEnable(GL_RESCALE_NORMAL);
#endif

    // Make sure the manipulatedFrame is not easily clipped by the zNear and zFar planes
    setSceneRadius(30);
    camera()->fitSphere(Vec(0,0,0), 1);

    // Add a manipulated frame to the viewer.
    // If you are not "using namespace qglqglviewer", you need
    // to specify: new qglviewer::ManipulatedFrame().
    setManipulatedFrame(new ManipulatedFrame());

    help();
    restoreStateFromFile();
}

void Viewer::draw()
{
    // Here we are in the world coordinate system. Draw unit size axis.
    drawAxis();
    // Save the current model view matrix (not needed here in fact)
    glPushMatrix();
    // Multiply matrix to get in the frame coordinate system.
    glMultMatrixd(manipulatedFrame()->matrix());
    // Scale down the drawings
    glScalef(0.3f, 0.3f, 0.3f);
    // Draw an axis using the QGLViewer static function
    drawAxis();

    drawPoints();
    if (isDraw)
        delaunay.drawMeshInQt();

    // Restore the original (world) coordinate system
    glPopMatrix();
}



QString Viewer::helpString() const
{
    QString text("<h2>M a n i p u l a t e d F r a m e</h2>");
    text += "A <i>ManipulatedFrame</i> converts mouse gestures into <i>Frame</i> displacements. ";
    text += "In this example, such an object defines the position of the spiral that can hence be manipulated.<br><br>";
    text += "Adding two lines of code will then allow you to move the objects of ";
    text += "your scene using the mouse. The button bindings of the <i>ManipulatedFrame</i> ";
    text += "are the same than for the camera. Spinning is possible.<br><br>";
    text += "Default key bindings have been changed in this example : press <b>Alt</b> ";
    text += "while moving the mouse to move the camera instead of the ManipulatedFrame.";
    text += "<img src=\"http://www.baidu.com/img/bdlogo.gif\" />";
    return text;
}

void Viewer::keyPressEvent(QKeyEvent *e)
{
    const Qt::KeyboardModifiers modifiers = e->modifiers();
    bool handled = false;
    if ((e->key()==Qt::Key_Space) && (modifiers==Qt::NoButton))
    {
        isDraw = true;
        delaunay.perform(points);
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

void Viewer::mousePressEvent(QMouseEvent* e)
{
    if ((e->button() == Qt::MidButton) && (e->modifiers() == Qt::NoButton))
    {
        int x = e->x();
        int y = e->y();
        Point pt(x, y, 0);
        points.push_back(pt);
        updateGL();
    }
    else
        QGLViewer::mousePressEvent(e);
}

}
