#include "embree_manager.h"
#include <iostream>
#include "../thirdparty/embree/include/embree4/rtcore.h"


EmbreeManager::EmbreeManager() : device(nullptr), scene(nullptr) {}

EmbreeManager::~EmbreeManager() {
    release();
}

void EmbreeManager::initialize() {
    device = rtcNewDevice(nullptr);
    if (!device) {
        std::cerr << "Failed to create an Embree device\n";
        return;
    }

    scene = rtcNewScene(device);
    if (!scene) {
        std::cerr << "Failed to create an Embree scene\n";
        return;
    }

    // Set scene build quality and flags as required
    rtcSetSceneBuildQuality(scene, RTC_BUILD_QUALITY_HIGH);
}

void EmbreeManager::release() {
    if (scene) {
        rtcReleaseScene(scene);
        scene = nullptr;
    }
    if (device) {
        rtcReleaseDevice(device);
        device = nullptr;
    }
}
