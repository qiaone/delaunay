#ifndef DVIEWER_H
#define DVIEWER_H

#include <QGLViewer/qglviewer.h>
#include <memory>

namespace DT
{

class DViewer : public QGLViewer
{
public:
    DViewer();
protected :
    virtual void init();
    virtual void draw();
    virtual QString helpString() const;
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent* e);
    void drawPoints();
    void test();
};

}
#endif // VIEWER_H
