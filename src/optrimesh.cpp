#include "optrimesh.h"

#include "opquadmesh.h"
#include "opmeshrenderer.h"
#include "intersectionutils.h"
#include "vector.h"

#include <cmath>

skbar::OPTriMesh::OPTriMesh() {

}

skbar::OPTriMesh::~OPTriMesh() {

}

bool OPLoad(skbar::OPTriMesh::TriMeshImpl &mesh, const skbar::OPQuadMesh &baseMesh) {

    auto tempTriMesh = baseMesh.GetTriangulated().Get();

    mesh.assign(tempTriMesh);

    // Copy data from quad mesh
    for (auto fi : mesh.faces()) {
        mesh.data(fi).quadFaceData.id = tempTriMesh.data(fi).quadFaceData.id;
    }

    mesh.update_normals();

    return true;
}

bool skbar::OPTriMesh::Load(const skbar::QuadMesh &qmesh) {

    return OPLoad(mesh, dynamic_cast<const OPQuadMesh &>(qmesh));
}

std::size_t skbar::OPTriMesh::GetQuadFaceId(std::size_t triangleId) const {

}

bool skbar::OPTriMesh::AddPointOnIntersection(
    const Intersection<int, Vec3f> &intersection) {

    bool added = false;

    if (intersection.Intersect()) {

        auto point = intersection.Position();

        mesh.split(
                OpenMesh::SmartFaceHandle(intersection.Face()),
                OpenMesh::Vec3f(point[0], point[1], point[2]));

        added = true;
    }

    // Vec3f intersection;
    // OpenMesh::SmartFaceHandle intersectedFace(-1);

    // float minDistance = INFINITY;

    // for (auto fi : mesh.faces()) {
    //     auto vertices = fi.vertices().to_vector([this] (auto vi) {
    //         auto point = mesh.point(vi);

    //         return Vec3f{ point[0], point[1], point[2] };
    //     });

    //     Vec3f tempIntersection;

    //     if (utils::CheckIntersectionTriangleRay(
    //             ray, vertices[0], vertices[1], vertices[2], tempIntersection)) {

    //         auto distance = Norm(Sub(ray.GetOrigin(), intersection));

    //         if (distance < minDistance) {
    //             minDistance = distance;
    //             intersection = tempIntersection;
    //             intersectedFace = fi;
    //         }
    //     }

    // }

    // if (minDistance < INFINITY) {

    //     mesh.split(intersectedFace, OpenMesh::Vec3f(intersection[0], intersection[1], intersection[2]));

    //     added = true;
    // }

    return added;
}

bool skbar::OPTriMesh::CheckIntersection(const skbar::Line<skbar::Vec3f> &ray) const {
    bool hasIntersection = false;

    for (auto fi : mesh.faces()) {
        auto vertices = fi.vertices().to_vector([this] (auto vi) {
            auto point = mesh.point(vi);

            return Vec3f{ point[0], point[1], point[2] };
        });

        Vec3f tempIntersection;

        if (utils::CheckIntersectionTriangleRay(
                ray, vertices[0], vertices[1], vertices[2], tempIntersection)) {

            hasIntersection = true;
            break;
        }

    }

    return hasIntersection;
}

skbar::Intersection<int, skbar::Vec3f> skbar::OPTriMesh::GetClosestIntersection(
    const skbar::Line<skbar::Vec3f> &ray) const {

    bool intersected = false;

    Vec3f intersection;
    // OpenMesh::SmartFaceHandle intersectedFace(-1);
    int intersectedFace = -1;

    float minDistance = INFINITY;

    for (auto fi : mesh.faces()) {
        auto vertices = fi.vertices().to_vector([this] (auto vi) {
            auto point = mesh.point(vi);

            return Vec3f{ point[0], point[1], point[2] };
        });

        Vec3f tempIntersection;

        if (utils::CheckIntersectionTriangleRay(
                ray, vertices[0], vertices[1], vertices[2], tempIntersection)) {

            auto distance = Norm(Sub(ray.GetOrigin(), intersection));

            if (distance < minDistance) {
                minDistance = distance;
                intersection = tempIntersection;
                intersectedFace = fi.idx();
            }
        }

    }

    return Intersection(intersectedFace, intersection);
}

void skbar::OPTriMesh::Render() const {
    OPMeshRenderer::Render(mesh);
}
