#ifndef DCAMERA_H
#define DCAMERA_H

#include <QGLViewer/camera.h>

class DCamera : public qglviewer::Camera
{
public :
    DCamera();

    virtual float zNear() const;
    virtual float zFar() const;
};

#endif // CAMERA_H
