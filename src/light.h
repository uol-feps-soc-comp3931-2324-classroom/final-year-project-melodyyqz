#pragma once

#include <Eigen/Dense>

class Light {
public:
    Eigen::Vector3f position;
    Eigen::Vector3f direction;
    Eigen::Vector3f intensity;  // Color and strength

    Light(const Eigen::Vector3f& pos, const Eigen::Vector3f& dir, const Eigen::Vector3f& intensity)
        : position(pos), direction(dir), intensity(intensity) {}
};
