#include "glcamera.h"

#include <GL/glew.h>
#include <GL/gl.h>

#include <cmath>
#include <algorithm>

skbar::GLCamera::GLCamera(int _width, int _height)
    : width(_width), height(_height), initialCenter{0, 0, 0}, initialPosition{0, 0, -1},
      initialUp{0, 1, 0}, left{-1, 0, 0}, bbox{std::array{-1.0f, 1.0f}, std::array{-1.0f, 1.0f}, std::array{-1.0f, 1.0f}} {

    position = initialPosition;
    center = initialCenter;
    up = initialUp;
}

skbar::GLCamera::~GLCamera() {
#warning "Implement this"
}

void skbar::GLCamera::Resize(int _width, int _height) {

    if ((width != _width) || (height != _height)) {
        width = _width;
        height = _height;
    }
}

void skbar::GLCamera::Rotate(float dtheta, float dphi) {
	/**
	 * Using formulas found <a href="http://mathworld.wolfram.com/SphericalCoordinates.html">here</a>
	 * changing coordinate system (here = on Wolfram):
	 *
	 * x = y
	 * y = z
	 * z = x
	 */
    using namespace std;

    // Direction vector from center to the camera
    Vec3f dir = Sub(position, center);

    // Invert rotation on x-window axis
    float radius = Norm(dir);

    // Calc new spherical coordinates
    float theta = atan2(dir[0], dir[2]) - dtheta;
    float phi = acos(dir[1] / radius) + dphi;

    // Calc new position
    float x = radius * sin(theta) * sin(phi);
    float y = radius * cos(phi);
    float z = radius * cos(theta) * sin(phi);

    Vec3f radial {x, y, z};
    Vec3f normalizedRadial = Normalize(radial);

    position = Sum(center, radial);

    left = Normalize(Vec3f{-normalizedRadial[2], 0, normalizedRadial[0]});

    up = Normalize(Cross(left, radial));
}

void skbar::GLCamera::Drag(float dx, float dy) {

    // Calc deltha
    Vec3f v {
        left[0]*dx - up[0]*dy,
        left[1]*dx - up[1]*dy,
        left[2]*dx - up[2]*dy
   };

    // Change position
    position[0] += v[0];
    position[1] += v[1];
    position[2] += v[2];

    // Change center
    center[0] += v[0];
    center[1] += v[1];
    center[2] += v[2];
}

void skbar::GLCamera::Zoom(float zoom) {

    Vec3f dir = Sub(center, position);
    float normDir = Norm(dir);
    Vec3f normalizedDir = Normalize(dir);

    // Amount of change
    Vec3f temp = Mul(normalizedDir, zoom * normDir * 0.05f);

    float distance = Norm(Sub(center, Sum(position, temp)));

    if (distance >= std::numeric_limits<float>::epsilon()) {
        position = Sum(position, temp);
    }
}

void skbar::GLCamera::SetBox(const skbar::BBox &_bbox) {
    bbox = _bbox;
}

void skbar::GLCamera::Render() const {
    Apply();
}

void skbar::GLCamera::Apply() const {

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(0, 0, width, height);

    gluPerspective(45, float(width) / float(height), 0.01, 100);

    // // TODO Check this
    // glFrustum(bbox[0][0], bbox[0][1],
    //           bbox[1][0], bbox[1][1],
    //           bbox[2][0], bbox[2][1]);

    gluLookAt(position[0], position[1], position[2],
              center[0], center[1], center[2],
              up[0], up[1], up[2]);

    glMatrixMode(GL_MODELVIEW);
}
