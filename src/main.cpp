#define TASKING_TBB
#define __SSE2__

#include "rapidobj/rapidobj.hpp"
#include <embree4/rtcore.h>
#include <embree4/rtcore_common.h>
#include <iostream>
#include "../embree/tutorials/common/tutorial/tutorial.h"
#include "simple_mesh.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Caustics", NULL, NULL);
    if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
    glfwMakeContextCurrent(window);

    SimpleMeshData armadilloMeshData = load_wavefront_obj("../../src/assets/Armadillo.obj");

    while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
    // Create an Embree device
    /*
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
    rtcCommitScene(scene);

    rtcReleaseScene(scene);
    rtcReleaseDevice(device);*/
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;

}