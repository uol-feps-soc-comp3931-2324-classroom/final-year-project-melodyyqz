#pragma once

#include "photon.h"

class Scene {
public:
    // This function should be implemented to handle ray intersections.
    bool trace(const Photon& photon, Eigen::Vector3f& hitPoint) {
        // Implement your intersection logic here
        return false;  // Placeholder
    }
};

