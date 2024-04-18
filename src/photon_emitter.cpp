#include "photon_emitter.h"
#include <random>

void PhotonEmitter::emitPhotons(int numPhotons, std::vector<Photon>& photonMap) {
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(0.0, 1.0);

    for (const auto& light : lights) {
        for (int i = 0; i < numPhotons; ++i) {
            // Random direction for simplicity
            Eigen::Vector3f randomDir(distribution(generator), distribution(generator), distribution(generator));
            randomDir.normalize();

            Photon photon(light.position, randomDir, light.intensity);
            Eigen::Vector3f hitPoint;
            if (scene.trace(photon, hitPoint)) {
                photonMap.push_back(photon);  // Store photon if it hits the scene
            }
        }
    }
}
