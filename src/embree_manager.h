#pragma once

#include <embree3/rtcore.h>

class EmbreeManager {
public:
    EmbreeManager();
    ~EmbreeManager();

    void initialize();
    void release();

private:
    RTCDevice device;
    RTCScene scene;
};

