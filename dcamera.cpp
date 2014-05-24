#include "dcamera.h"

DCamera::DCamera()
{
}

float DCamera::zNear() const
{
    return 0.001f;
}

float DCamera::zFar() const
{
    return 1000.0;
}
