#ifndef CAMERA_H_
#define CAMERA_H_

namespace skbar {

class Camera {

public:
    Camera() {}

    virtual ~Camera() {}

    virtual void Resize(int width, int height) = 0;

    virtual void Rotate(float dtheta, float dphi) = 0;

    virtual void Drag(float dx, float dy) = 0;

    virtual void Zoom(float zoom) = 0;
};
}

#endif
