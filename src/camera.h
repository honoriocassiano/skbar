#ifndef CAMERA_H_
#define CAMERA_H_

#include "projection.h"

namespace skbar {

class Camera {

public:
    Camera() {}

    virtual ~Camera() {}

    virtual void Resize(int width, int height) = 0;

    virtual void Rotate(float dtheta, float dphi) = 0;

    virtual void Drag(float dx, float dy) = 0;

    virtual void Zoom(float zoom) = 0;

    // TODO Remove float from here
    virtual const Projection<float>& GetProjection() const = 0;

    virtual void Render() const = 0;
};
}

#endif
