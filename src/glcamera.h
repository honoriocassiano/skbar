#ifndef GL_CAMERA_H_
#define GL_CAMERA_H_

#include "camera.h"

#include "commontypes.h"
#include "glprojection.h"
#include "vector.h"

#include <array>

namespace skbar {

class GLCamera : public Camera {

public:
    GLCamera(int width, int height);
    virtual ~GLCamera() = default;

    void Resize(int width, int height) override;

    void Rotate(float dtheta, float dphi) override;

    void Drag(float dx, float dy) override;

    void Zoom(float zoom) override;

    void SetBox(const BBox &bbox);

    void Render() const override;

    const Vec3f &Position() const { return position; }

    const Vec3f &Center() const { return center; }

    Line<Vec3f> DirectionRay() const { return Line<Vec3f>(position, Sub(center, position)); }

    const Vec3f &Up() const { return up; }

    const Vec3f &Left() const { return left; }

    Line<Vec3f> PositionToRay(int x, int y) const;

    const Projection<float>& GetProjection() const override { return projection; }

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

    // Variable used only to owner an instance of the Projection object
    mutable GLProjection projection;
};
}

#endif
