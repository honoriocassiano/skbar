#include "optrimesh.h"

#include "opquadmesh.h"
#include "opmeshrenderer.h"
#include "intersectionutils.h"
#include "vector.h"
#include "utils/openmesh.h"

#include <set>
#include <cmath>
#include <utils/debug.h>

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

int skbar::OPTriMesh::GetQuadFaceId(int triangleId) const {
    return mesh.data(mesh.face_handle(triangleId)).quadFaceData.id;
}

bool skbar::OPTriMesh::AddPointOnIntersection(
        const Intersection<int, Vec3f> &intersection) {

    bool added = false;

    if (intersection.Intersect()) {

        auto point = intersection.Position();

        mesh.split(
                OpenMesh::SmartFaceHandle(intersection.Pointer()),
                OpenMesh::Vec3f(point[0], point[1], point[2]));

        added = true;
    }

    return added;
}

std::tuple<bool, std::vector<skbar::Intersection<int, skbar::Vec3f>>> skbar::OPTriMesh::GetIntersectionsBetween(
        const skbar::Intersection<int, skbar::Vec3f> &p1, const skbar::Intersection<int, skbar::Vec3f> &p2,
        const skbar::Line<Vec3f> &viewDirection, const Projection<float> &projection) const {

    // Keep visited faces to ensure that is no infinity loop
    std::set<int> visitedFaces;
    std::vector<Intersection<int, Vec3f>> result;

    auto hasPath = false;

    // No intersections if on same face
    if (p1.Pointer() == p2.Pointer()) {
        return std::make_tuple(hasPath, result);
    }

    const Vec2f projectedP1 = projection.Project(p1.Position());
    const Vec2f projectedP2 = projection.Project(p2.Position());

//    Log("Segment: (%f, %f) - (%f, %f)", projectedP1[0], projectedP1[1],
//            projectedP2[0], projectedP2[1]);

    const Segment<Vec2f> projectedSegment(projectedP1, projectedP2);

    const OpenMesh::SmartFaceHandle firstFace(p1.Pointer(), &mesh);
    const OpenMesh::SmartFaceHandle lastFace(p2.Pointer(), &mesh);

    OpenMesh::SmartFaceHandle currentFace(p1.Pointer(), &mesh);

    OpenMesh::SmartHalfedgeHandle lastIntersectedHE(-1, &mesh);

    // Find first edge between the last point and the new
    {
        for (auto he : firstFace.halfedges()) {
            const auto v1 = he.from();
            const auto v2 = he.to();

            const Vec3f e1{mesh.point(v1)[0], mesh.point(v1)[1], mesh.point(v1)[2]};
            const Vec3f e2{mesh.point(v2)[0], mesh.point(v2)[1], mesh.point(v2)[2]};

            const auto projectedE1 = projection.Project(e1);
            const auto projectedE2 = projection.Project(e2);

            // TODO Review tis method
            // Maybe can use only one intersection calculation between the plane and the edge segment
            const Segment<Vec2f> projectedEdge(projectedE1, projectedE2);

//            Log("Edge Segment: (%f, %f) - (%f, %f)", projectedE1[0], projectedE1[1],
//                projectedE2[0], projectedE2[1]);

            Vec2f windowIntersection;

            if (utils::CheckIntersectionSegmentSegment(projectedSegment, projectedEdge, windowIntersection)) {

                const auto unprojectedWindowIntersection = projection.UnProject(windowIntersection);

                const auto triangleNormal = utils::ToStdVector(mesh.normal(he.face()));

                const Vec3f intersectionDirection = Normalize(Sub(unprojectedWindowIntersection, viewDirection.GetOrigin()));
                const Line<Vec3f> intersectionRay(viewDirection.GetOrigin(), intersectionDirection);

                const Plane<Vec3f> trianglePlane(e1, triangleNormal);

                Vec3f worldIntersection;

                utils::CheckIntersectionPlaneLine(trianglePlane, intersectionRay, worldIntersection);

                result.emplace_back(he.edge().idx(), worldIntersection);
                visitedFaces.insert(currentFace.idx());

                lastIntersectedHE = he.opp();
                currentFace = lastIntersectedHE.face();

                hasPath = true;

                break;
            }
        }
    }

    while ((currentFace != lastFace) && hasPath) {

//        // Do not visit already visited faces
//        if (visitedFaces.find(currentFace.idx()) != visitedFaces.end()) {
//            Log("No path found between %d (%f, %f, %f) and %d (%f, %f, %f)",
//                p1.Pointer(), p1.Position()[0], p1.Position()[1], p1.Position()[2],
//                p2.Pointer(), p2.Position()[0], p2.Position()[1], p2.Position()[2]);
//
//            hasPath = false;
//            result.clear();
//
//            break;
//        }

        auto edgeFound = false;

        for (auto he : currentFace.halfedges()) {
            if (he != lastIntersectedHE) {
                const auto v1 = he.from();
                const auto v2 = he.to();

                const Vec3f e1{mesh.point(v1)[0], mesh.point(v1)[1], mesh.point(v1)[2]};
                const Vec3f e2{mesh.point(v2)[0], mesh.point(v2)[1], mesh.point(v2)[2]};

                const auto projectedE1 = projection.Project(e1);
                const auto projectedE2 = projection.Project(e2);

                // TODO Review tis method
                // Maybe can use only one intersection calculation between the plane and the edge segment
                const Segment<Vec2f> projectedEdge(projectedE1, projectedE2);

//                Log("Edge Segment: (%f, %f) - (%f, %f)", projectedE1[0], projectedE1[1],
//                    projectedE2[0], projectedE2[1]);

                Vec2f windowIntersection;

                // TODO Change to line-segment intersection
                if (utils::CheckIntersectionSegmentSegment(projectedSegment, projectedEdge, windowIntersection)) {

                    const auto unprojectedWindowIntersection = projection.UnProject(windowIntersection);

                    const auto triangleNormal = utils::ToStdVector(mesh.normal(he.face()));

                    const Vec3f intersectionDirection = Normalize(Sub(unprojectedWindowIntersection, viewDirection.GetOrigin()));
                    const Line<Vec3f> intersectionRay(viewDirection.GetOrigin(), intersectionDirection);

                    const Plane<Vec3f> trianglePlane(e1, triangleNormal);

                    Vec3f worldIntersection;

                    utils::CheckIntersectionPlaneLine(trianglePlane, intersectionRay, worldIntersection);

                    result.emplace_back(he.edge().idx(), worldIntersection);
                    visitedFaces.insert(currentFace.idx());

                    lastIntersectedHE = he.opp();
                    currentFace = lastIntersectedHE.face();

                    edgeFound = true;

                    break;
                }
            }
        }

        if (!edgeFound || (visitedFaces.find(currentFace.idx()) != visitedFaces.end())) {

            hasPath = false;
            result.clear();

            break;
        }
    }

    if (!hasPath) {
        Log("No path found between %d (%f, %f, %f) and %d (%f, %f, %f)",
            p1.Pointer(), p1.Position()[0], p1.Position()[1], p1.Position()[2],
            p2.Pointer(), p2.Position()[0], p2.Position()[1], p2.Position()[2]);
    }

    return std::make_tuple(hasPath, result);
}

skbar::Intersection<int, skbar::Vec3f> skbar::OPTriMesh::GetClosestIntersection(
        const skbar::Line<skbar::Vec3f> &ray) const {

    bool intersected = false;

    Vec3f intersectionPoint;
    int intersectedFace = -1;

    float minDistance = INFINITY;

    for (auto fi : mesh.faces()) {
        auto vertices = fi.vertices().to_vector([this](auto vi) {
            auto point = mesh.point(vi);

            return Vec3f{point[0], point[1], point[2]};
        });

        Vec3f tempIntersection;

        if (utils::CheckIntersectionTriangleRay(
                ray, vertices[0], vertices[1], vertices[2], tempIntersection)) {

            auto distance = Norm(Sub(ray.GetOrigin(), tempIntersection));

            if (distance < minDistance) {
                minDistance = distance;
                intersectionPoint = tempIntersection;
                intersectedFace = fi.idx();
                intersected = true;
            }
        }
    }

    return Intersection(intersectedFace, intersectionPoint, intersected);
}
