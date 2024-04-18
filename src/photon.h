#pragma once

#include <Eigen/Dense>  // Using Eigen for vector math

class Photon {
public:
    Eigen::Vector3f position;
    Eigen::Vector3f direction;
    Eigen::Vector3f energy;  // Color stored as RGB

    Photon(const Eigen::Vector3f& pos, const Eigen::Vector3f& dir, const Eigen::Vector3f& energy)
        : position(pos), direction(dir), energy(energy) {}
};
