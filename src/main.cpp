#include <igl/readOBJ.h>

Eigen::MatrixXd V;
Eigen::MatrixXi F;

int main(int argc, char *argv[])
{

    Eigen::MatrixXd V;
    Eigen::MatrixXd F;

    // Load a mesh in OFF format
    igl::readOBJ("./models/3holes_quad.obj", V, F);

    std::cout << F << std::endl;

    return 0;
}
