#define TASKING_TBB
#define __SSE2__

#include <glad/glad.h>
#include "rapidobj/rapidobj.hpp"
#include <embree4/rtcore.h>
#include <embree4/rtcore_common.h>
#include <iostream>
#include "../embree/tutorials/common/tutorial/tutorial.h"
#include "simple_mesh.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_class.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "EBO.hpp"
#include "Camera.h"
#include "embree_manager.h"

const unsigned int width = 800;
const unsigned int height = 800;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Initialize Embree
    EmbreeManager embreeManager;
    embreeManager.initialize();

    // Create window
    GLFWwindow* window = glfwCreateWindow(width, height, "Caustics", NULL, NULL);
    if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
    glfwMakeContextCurrent(window);

    // Load GLAD
    gladLoadGL();
    glViewport(0, 0, width, height);

    // Create shader program
    Shader shaderProgram("default.vert", "default.frag");
    VAO VAO1;
    VAO1.Bind();

    // Load object
    SimpleMeshData armadilloMeshData = load_wavefront_obj("Armadillo.obj");

    // Create a contiguous array of GLfloat for positions
    std::vector<GLfloat> positionData;
    positionData.reserve(armadilloMeshData.positions.size() * 3);
    for (const auto& pos : armadilloMeshData.positions) {
        positionData.push_back(pos.x);
        positionData.push_back(pos.y);
        positionData.push_back(pos.z);
    }

    // Create VBO and EBO with the arrays
    VBO VBO1(&positionData[0], sizeof(GLfloat) * positionData.size());
    EBO EBO1(&armadilloMeshData.indices[0], sizeof(GLuint) * armadilloMeshData.indices.size());
    VAO1.LinkVBO(VBO1, 0);
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();

    glDisable(GL_CULL_FACE);

    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

    // Don't close window instantly
    while (!glfwWindowShouldClose(window)) {
        // Background colour
		glClearColor(1.f, 1.f, 1.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderProgram.Activate();

        camera.Inputs(window);
        camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");

        VAO1.Bind();
        glDrawElements(GL_TRIANGLES, armadilloMeshData.indices.size(), GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
	}


    // Create an Embree device
    /*
    //RTCDevice device = rtcNewDevice(nullptr);
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

    // Cleanup
    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    shaderProgram.Delete();
    embreeManager.release();
    // Cleanup window
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;

}
