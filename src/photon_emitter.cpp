#include "photon_emitter.h"
#include <random>
#include <iostream> // just for debugging outputs

void PhotonEmitter::emitPhotons(int numPhotons, std::vector<Photon>& photonMap) {
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution_xz(-1.0, 1.0);
    std::uniform_real_distribution<float> distribution_y(-1.0, 0.0);

    for (const auto& light : lights) {
        for (int i = 0; i < numPhotons; ++i) {
            // Random direction for simplicity
            Eigen::Vector3f randomDir(distribution_xz(generator), distribution_y(generator), distribution_xz(generator));
            randomDir.normalize();

            std::cout << "Random Dir: " << randomDir.transpose() << std::endl;  // Debug print the direction

            Photon photon(light.position, randomDir, light.intensity);
            
            std::cout << "Photon Position: " << photon.position.transpose() << " Intensity: " << photon.energy.transpose() << std::endl;  // Print photon details
            
            Eigen::Vector3f hitPoint;
            if (scene.trace(photon, hitPoint)) {
                photonMap.push_back(photon);  // Store photon if it hits the scene
                std::cout << "Photon hit at: " << hitPoint.transpose() << std::endl;  // Debug successful hit
            }
        }
    }
}
