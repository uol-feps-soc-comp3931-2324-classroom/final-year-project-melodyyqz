#include "photon_emitter.h"
#include <vector>
#include <random>
#include <cmath>
#include <iostream> // For debugging outputs

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

PhotonEmitter::PhotonEmitter(const Light& lightSource, float angle)
    : light(lightSource), cutoffAngle(angle) {}

std::vector<Photon> PhotonEmitter::emitPhotons(int numPhotons) const {
    std::vector<Photon> photonMap;
    photonMap.reserve(numPhotons);

    // Setup random number generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    float cosCutoff = std::cos(cutoffAngle * M_PI / 180.0f); // Convert angle to radians and then to cosine

    for (int i = 0; i < numPhotons; ++i) {
        // Random value between -1 and 1
        float u = dist(gen) * 2.0f - 1.0f; 
        // Random value in the range [cosCutoff, 1]
        float v = dist(gen) * (1.0f - cosCutoff) + cosCutoff;
        float theta = std::acos(v); // Inverse cosine to get the angle
        float phi = 2.0f * M_PI * u; // Full circle

        Eigen::Vector3f localDir(
            std::sin(theta) * std::cos(phi),
            std::sin(theta) * std::sin(phi),
            std::cos(theta)
        );

        // Transform the local direction vector to align with the spotlight's direction
        Eigen::Vector3f worldDir = transformDirectionFromLocalToWorld(localDir, light.direction.normalized());

        photonMap.emplace_back(light.position, worldDir, light.intensity);
    }

    return photonMap;
}

Eigen::Vector3f PhotonEmitter::transformDirectionFromLocalToWorld(const Eigen::Vector3f& localDir, const Eigen::Vector3f& spotlightDir) const {
    Eigen::Quaternionf rotation = Eigen::Quaternionf::FromTwoVectors(Eigen::Vector3f::UnitZ(), spotlightDir);
    return rotation * localDir;
}
