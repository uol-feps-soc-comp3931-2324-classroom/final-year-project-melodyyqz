#include "rapidobj/rapidobj.hpp"

#include <iostream>

int main() {
	rapidobj::Result armadillo = rapidobj::ParseFile("../../src/assets/Armadillo.obj");

    if (armadillo.error) {
        std::cout << armadillo.error.code.message() << '\n';
        return EXIT_FAILURE;
    }

    bool success = rapidobj::Triangulate(armadillo);

    if (!success) {
        std::cout << armadillo.error.code.message() << '\n';
        return EXIT_FAILURE;
    }

    auto num_triangles = size_t();

    for (const auto& shape : armadillo.shapes) {
        num_triangles += shape.mesh.num_face_vertices.size();
    }

    std::cout << "Shapes:    " << armadillo.shapes.size() << '\n';
    std::cout << "Triangles: " << num_triangles << '\n';

    return EXIT_SUCCESS;

}