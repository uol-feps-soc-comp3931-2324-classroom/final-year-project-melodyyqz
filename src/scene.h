#pragma once

#include "photon.h"
#include "simple_mesh.hpp"
#include <embree3/rtcore.h>
#include <Eigen/Dense>
#include <limits>
#include <Eigen/Geometry>
#include <iostream> // for debugging only


struct MaterialProperties {
    Eigen::Vector3f reflectance;  // Reflective property, affects color and intensity
    Eigen::Vector3f transmittance;  // Transmittance property, for transparent materials
    float indexOfRefraction;  // Index of refraction, important for refraction calculations

    MaterialProperties() : reflectance(0.0f, 0.0f, 0.0f), transmittance(0.0f, 0.0f, 0.0f), indexOfRefraction(1.0f) {}
};

class Scene {
    RTCDevice device;
    RTCScene scene;
    float energyThreshold = 0.01f;

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

        //// Log the scene bounds after committing the scene
        //RTCBounds bounds;
        //rtcGetSceneBounds(scene, &bounds);
        //std::cout << "Scene Bounds: ["
        //    << bounds.lower_x << ", " << bounds.lower_y << ", " << bounds.lower_z
        //    << "] to ["
        //    << bounds.upper_x << ", " << bounds.upper_y << ", " << bounds.upper_z
        //    << "]" << std::endl;
	}

    void commitScene() {
		rtcCommitScene(scene);
	}

    bool shouldReflect(const MaterialProperties& mat) {
        return mat.reflectance.norm() > 0.0f;
    }

    bool shouldTransmit(const MaterialProperties& mat) {
        return mat.transmittance.norm() > 0.0f;
    }

    MaterialProperties getMaterialProperties(const Eigen::Vector3f& hitPoint) {
        MaterialProperties props;
        props.reflectance = Eigen::Vector3f(0.8, 0.8, 0.8);
        props.transmittance = Eigen::Vector3f(0.1, 0.1, 0.1);
        props.indexOfRefraction = 1.5;
        return props;
    }

    Eigen::Vector3f reflect(const Eigen::Vector3f& incident, const Eigen::Vector3f& normal) {
        return incident - 2 * incident.dot(normal) * normal;
    }

    Eigen::Vector3f refract(const Eigen::Vector3f& incident, const Eigen::Vector3f& normal, float eta) {
        float cosi = -std::max(-1.0f, std::min(1.0f, incident.dot(normal)));
        float etai = 1, etat = eta;
        Eigen::Vector3f n = normal;
        if (cosi < 0) { cosi = -cosi; }
        else { std::swap(etai, etat); n = -normal; }
        eta = etai / etat;
        float k = 1 - eta * eta * (1 - cosi * cosi);
        return k < 0 ? Eigen::Vector3f(0, 0, 0) : eta * incident + (eta * cosi - sqrtf(k)) * n;
    }

    Eigen::Vector3f getNormalAt(const Eigen::Vector3f& hitPoint) {
        // Placeholder for normal retrieval
        return Eigen::Vector3f(0, 1, 0);  // Example normal pointing upwards
    }

    bool tracePhoton(Photon& photon) {
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
        //rayHit.ray.tfar = FLT_MAX;
        rayHit.ray.tfar = std::numeric_limits<float>::infinity();
        rayHit.ray.mask = -1;
        rayHit.ray.flags = 0;
        rayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        rayHit.hit.primID = RTC_INVALID_GEOMETRY_ID;

        // Intersect ray with the scene
        rtcIntersect1(scene, &context, &rayHit);

        // Check if the ray hit any geometry
        if (rayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
            Eigen::Vector3f hitPoint = photon.position + photon.direction * rayHit.ray.tfar;
            MaterialProperties mat = getMaterialProperties(hitPoint);
            std::cout << "Hit point: " << hitPoint.transpose() << std::endl;
            
            bool result = true;

            if (shouldReflect(mat)) {
                Eigen::Vector3f newDirection = reflect(photon.direction, getNormalAt(hitPoint));
                photon.direction = newDirection;  // Assumed direction is not const
                photon.position = hitPoint;
                photon.energy = (photon.energy.array() * mat.reflectance.array()).matrix();  // Assumed energy is not const
                if (photon.energy.norm() > energyThreshold) {
                    result =  tracePhoton(photon);
                }
            }
            else if (shouldTransmit(mat)) {
                Eigen::Vector3f newDirection = refract(photon.direction, getNormalAt(hitPoint), mat.indexOfRefraction);
                photon.direction = newDirection;
                photon.position = hitPoint;
                photon.energy = (photon.energy.array() * mat.transmittance.array()).matrix();
                if (photon.energy.norm() > energyThreshold) {
                    result = tracePhoton(photon);
                }
            }
            return result;
        }  
        return false;
    }
    

};

