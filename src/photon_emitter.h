#pragma once

#include "photon.h"
#include "light.h"
#include <vector>
#include <Eigen/Dense>

class PhotonEmitter {
public:
    PhotonEmitter(const Light& lightSource, float angle);
    std::vector<Photon> emitPhotons(int numPhotons) const;

private:
    Light light;
    float cutoffAngle;

    Eigen::Vector3f transformDirectionFromLocalToWorld(const Eigen::Vector3f& localDir, const Eigen::Vector3f& spotlightDir) const;
};
