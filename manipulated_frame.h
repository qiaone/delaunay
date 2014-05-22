#ifndef MANIPULATED_FRAME_H
#define MANIPULATED_FRAME_H

#include <QGLViewer/qglviewer.h>

class Viewer : public QGLViewer
{
protected :
    virtual void init();
    virtual void draw();
    virtual QString helpString() const;
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent* e);
};

#endif // MANIPULATED_FRAME_H
