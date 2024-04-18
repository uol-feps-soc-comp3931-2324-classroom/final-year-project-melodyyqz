#pragma once

#include "photon.h"
#include "light.h"
#include "scene.h"
#include <vector>

class PhotonEmitter {
public:
    Scene& scene;
    std::vector<Light> lights;

    PhotonEmitter(Scene& scene, const std::vector<Light>& lights)
        : scene(scene), lights(lights) {}

    void emitPhotons(int numPhotons, std::vector<Photon>& photonMap);
};

