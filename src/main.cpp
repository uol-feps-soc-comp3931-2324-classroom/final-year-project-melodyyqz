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


// Mock object
GLfloat vertices[] = {
    -0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // lower left
    0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // lower right
    0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f, // upper
    -0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f, // inner lower left
    0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f, // inner lower right
    0.0f, -0.5f * float(sqrt(3)) / 3, 0.0f // inner down
};

//

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
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

        // BEGIN OF 3d STUFF HERE ------------------------------------------------

        // Initialise 3d view matrices
        //glm::mat4 model = glm::mat4(1.0f);
        //glm::mat4 view = glm::mat4(1.0f);
        //glm::mat4 proj = glm::mat4(1.0f);
        // Move camera back
        //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));
        //proj = glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.1f, 100.0f);

        // Get matrix's uniform location and set matrix
        //unsigned int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
        //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        //unsigned int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
        //glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        //unsigned int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
        //glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));


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

    // Delete objects created
    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    shaderProgram.Delete();
    // Cleanup window
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;

}
