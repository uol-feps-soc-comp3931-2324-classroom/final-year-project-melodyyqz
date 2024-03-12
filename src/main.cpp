#include "rapidobj/rapidobj.hpp"

#include <iostream>

int main() {
	rapidobj::Result armadillo = rapidobj::ParseFile("/assets/armadillo.obj");

    if (armadillo.error) {
        std::cout << armadillo.error.code.message() << '\n';
        return EXIT_FAILURE;
    }

    bool success = rapidobj::Triangulate(armadillo);

    if (!success) {
        std::cout << armadillo.error.code.message() << '\n';
        return EXIT_FAILURE;
    }

}