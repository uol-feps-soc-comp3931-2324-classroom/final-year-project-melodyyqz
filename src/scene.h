#pragma once

#include "photon.h"
#include <embree3/rtcore.h>
#include <Eigen/Dense>

class Scene {
    RTCDevice device;
    RTCScene scene;

public:
    Scene() {
        device = rtcNewDevice(nullptr);
        scene = rtcNewScene(device);
    }

    ~Scene() {
        rtcReleaseScene(scene);
        rtcReleaseDevice(device);
    }

    // Add the object to the scene
    void addMesh(const SimpleMeshData& mesh) {
		RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
		rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, mesh.positions.data(), 0, sizeof(Eigen::Vector3f), mesh.positions.size());
		rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, mesh.indices.data(), 0, sizeof(unsigned int), mesh.indices.size()/3);
		rtcCommitGeometry(geom);
		rtcAttachGeometry(scene, geom);
		rtcReleaseGeometry(geom);
        rtcCommitScene(scene);
	}

    bool trace(const Photon& photon, Eigen::Vector3f& hitPoint) {
        RTCIntersectContext context;
        rtcInitIntersectContext(&context);

        // Ray structure
        RTCRayHit rayHit;
        // Initialize ray origin and direction
        rayHit.ray.org_x = photon.position.x();
        rayHit.ray.org_y = photon.position.y();
        rayHit.ray.org_z = photon.position.z();
        rayHit.ray.dir_x = photon.direction.x();
        rayHit.ray.dir_y = photon.direction.y();
        rayHit.ray.dir_z = photon.direction.z();
        // Start of ray
        rayHit.ray.tnear = 0.0f;
        // Maximum distance of ray
        rayHit.ray.tfar = FLT_MAX;
        rayHit.ray.mask = -1;
        rayHit.ray.flags = 0;
        rayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        rayHit.hit.primID = RTC_INVALID_GEOMETRY_ID;

        // Intersect ray with the scene
        rtcIntersect1(scene, &context, &rayHit);

        // Check if the ray hit any geometry
        if (rayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
            hitPoint = photon.position + photon.direction * rayHit.ray.tfar;
            return true;
        }
        // Ray did not hit any geometry
        return false;
    }
};

