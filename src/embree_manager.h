#pragma once

#include "../thirdparty/embree/include/embree4/rtcore.h"

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

