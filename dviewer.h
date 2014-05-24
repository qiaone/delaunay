#ifndef DVIEWER_H
#define DVIEWER_H

#include <QGLViewer/qglviewer.h>
#include "../dcamera.h"
#include <memory>

namespace DT
{

class DViewer : public QGLViewer
{
public:
    DViewer(std::unique_ptr<DCamera> cam);
protected :
    virtual void init();
    virtual void draw();
    virtual QString helpString() const;
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent* e);
    void drawPoints();
    void test();
private:
    std::unique_ptr<qglviewer::Camera> camera;
};

}
#endif // VIEWER_H
