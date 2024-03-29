#include "rapidobj/rapidobj.hpp"
#include <embree4/rtcore.h>
#include <embree4/rtcore_common.h>
#include <iostream>

int main() {

    // Load the Armadillo model
	rapidobj::Result armadillo = rapidobj::ParseFile("../../src/assets/Armadillo.obj");

    if (armadillo.error) {
        std::cout << armadillo.error.code.message() << '\n';
        return EXIT_FAILURE;
    }

    // Triangulate the model
    bool success = rapidobj::Triangulate(armadillo);

    if (!success) {
        std::cout << armadillo.error.code.message() << '\n';
        return EXIT_FAILURE;
    }

    // Print the number of shapes and triangles
    auto num_triangles = size_t();

    for (const auto& shape : armadillo.shapes) {
        num_triangles += shape.mesh.num_face_vertices.size();
    }

    std::cout << "Shapes:    " << armadillo.shapes.size() << '\n';
    std::cout << "Triangles: " << num_triangles << '\n';

    // Create an Embree device
    RTCDevice device = rtcNewDevice(nullptr);
    RTCScene scene = rtcNewScene(device);

    for (const auto& shape : armadillo.shapes) {
        for (size_t i = 0; i < shape.mesh.num_face_vertices.size(); ++i) {
			RTCGeometry geometry = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

			rtcSetSharedGeometryBuffer(geometry, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, shape.mesh.indices.data()[0], 0, sizeof(float) * 3, shape.mesh.indices.size());
			rtcSetSharedGeometryBuffer(geometry, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, shape.mesh.indices.data(), 0, sizeof(unsigned int) * 3, shape.mesh.indices.size());

			rtcCommitGeometry(geometry);
			rtcAttachGeometry(scene, geometry);
			rtcReleaseGeometry(geometry);
		}
	}
    // Commit the scene
    rtcCommitScene(scene);

    // Cleanup
    rtcReleaseScene(scene);
    rtcReleaseDevice(device);

    return EXIT_SUCCESS;

}