#include <OpenMesh/Core/IO/MeshIO.hh>

#include "meshtypes.h"
#include "patchtracer.h"

using namespace skbar;

int main(int argc, char *argv[]) {

    // The Begginng of a New Era
    QuadMesh mesh;

    if (!OpenMesh::IO::read_mesh(mesh, "/home/cassiano/workspace-cpp/skbar/models/plane.obj"))
    {
        std::cerr << "read error\n";
        exit(1);
    }

    PatchTracer::Trace(mesh);

    return 0;
}
