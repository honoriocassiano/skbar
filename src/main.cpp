// #include <OpenMesh/Core/IO/MeshIO.hh>
// #include <utils/debug.h>
// #include <Eigen/SparseCholesky>
//
// #include "meshtypes.h"
// #include "patchtracer.h"
// #include "patchquadrangulator.h"
//
// #include "example.h"

#include "sdlapplication.h"

#include "editablemesh.h"
#include "utils/debug.h"
#include "vector.h"
#include "quadmesh.h"
#include "trimesh.h"

#include "intersectionutils.h"

using namespace skbar;

int main(int argc, char *argv[]) {

//    // Plane p(Vec3f(), Vec3f{0, 1, 0});
//
//    // Line l(Vec3f{1, 1, 2}, Vec3f{1, 0, 0});
//
//    Segment<Vec2f> s1(Vec2f{1, 1}, Vec2f{0, 1});
//    Segment<Vec2f> s2(Vec2f(), Vec2f{1, 0});
//
//    // Vec3f intersection;
//    Vec2f intersection;
//
//    // bool hasIntersection = utils::CheckIntersectionPlaneLine(p, l, intersection);
//
//    bool hasIntersection = utils::CheckIntersectionSegmentSegment(s1, s2, intersection);
//
//    Log("%d: (%f, %f)", hasIntersection, intersection[0], intersection[1]);
//    // Log("%d: (%f, %f, %f)", hasIntersection, intersection[0], intersection[1], intersection[2]);

     SDLApplication app("SkBaR", 1024, 768);

     app.Initialize();

     app.LoadMesh("/home/cassiano/Documents/workspace-cpp/skbar/models/3holes_quad.obj");
     // app.LoadMesh("/home/cassiano/Documents/workspace-cpp/skbar/models/plane.obj");

     app.Run();

//     EditableMesh mesh;
//
//     mesh.Load("../models/3holes_quad.obj");
//
//     auto [min, max] = mesh.GetBoundingBox();
//
//     std::cout << min << std::endl;
//     std::cout << max << std::endl;

    return 0;
}
