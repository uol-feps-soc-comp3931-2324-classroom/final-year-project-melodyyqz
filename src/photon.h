#pragma once

#include <Eigen/Dense>  // Using Eigen for vector math

class Photon {
public:
    Eigen::Vector3f position;
    Eigen::Vector3f direction;
    Eigen::Vector3f energy;  // Color stored as RGB
    bool touchGlass = false;

    Photon(const Eigen::Vector3f& pos, const Eigen::Vector3f& dir, const Eigen::Vector3f& energy)
        : position(pos), direction(dir), energy(energy) {}

    Photon(const Photon& other) = default;  // Copy constructor
    Photon& operator=(const Photon& other) = default;  // Copy assignment operator
    Photon(Photon&& other) noexcept = default;  // Move constructor
    Photon& operator=(Photon&& other) noexcept = default;  // Move assignment operator

    friend void swap(Photon& a, Photon& b) noexcept {
        using std::swap;
        swap(a.position, b.position);
        swap(a.direction, b.direction);
        swap(a.energy, b.energy);
    }
};
