#ifndef GL_CAMERA_H_
#define GL_CAMERA_H_

#include "camera.h"

#include "commontypes.h"

#include "vector.h"

#include <array>

namespace skbar {

class GLCamera : public Camera {

public:
    GLCamera(int width, int height);
    virtual ~GLCamera();

    void Resize(int width, int height) override;

    void Rotate(float dtheta, float dphi) override;

    void Drag(float dx, float dy) override;

    void Zoom(float zoom) override;

    void SetBox(const BBox &bbox);

    void Render() const override;

private:

    void Apply() const;

private:

    // Size of the window
    int width;
    int height;

    // Data used by OpenGL
    Vec3f center;
    Vec3f position;
    Vec3f up;

    // TODO Document this
    Vec3f left;

    // Initial configurations (in case of reset)
    Vec3f initialCenter;
    Vec3f initialPosition;
    Vec3f initialUp;

    BBox bbox;
};
}

#endif
