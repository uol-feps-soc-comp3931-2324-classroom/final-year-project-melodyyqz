#define TASKING_TBB
#define __SSE2__

#include <glad/glad.h>
#include "rapidobj/rapidobj.hpp"
#include <embree3/rtcore.h>
#include <embree3/rtcore_common.h>
#include <iostream>
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
#include "photon_emitter.h"
#include "scene.h"
#include "light.h"
#include "photon.h"

// Window dimensions
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
    Scene scene;

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

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Create shader program
    Shader shaderProgram("default.vert", "default.frag");
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Initialise model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

    VAO VAO1;
    VAO1.Bind();

    // Load object
    SimpleMeshData meshData = load_wavefront_obj("glass-obj.obj");
    scene.addMesh(meshData);
    scene.commitScene();

    // Create a contiguous array of GLfloat for positions
    std::vector<GLfloat> positionData;
    positionData.reserve(meshData.positions.size() * 3);
    for (const auto& pos : meshData.positions) {
        positionData.push_back(pos.x);
        positionData.push_back(pos.y);
        positionData.push_back(pos.z);
    }
    std::vector<GLfloat> normalData;  // You need to ensure this is filled similarly to positionData
    for (const auto& normal : meshData.normals) {
        normalData.push_back(normal.x);
        normalData.push_back(normal.y);
        normalData.push_back(normal.z);
    }

    // Create VBO and EBO with the arrays
    VBO VBO_Positions(&positionData[0], positionData.size() * sizeof(GLfloat)); // Assuming positionData is already populated
    VAO1.LinkVBO(VBO_Positions, 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    VBO VBO_Normals(&normalData[0], normalData.size() * sizeof(GLfloat)); // Assuming normalData is already populated
    VAO1.LinkVBO(VBO_Normals, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    EBO EBO1(&meshData.indices[0], meshData.indices.size() * sizeof(unsigned int)); // Assuming indices is already populated
    EBO1.Bind();

    VAO1.Unbind();

    VBO_Positions.Unbind();
    VBO_Normals.Unbind();
    EBO1.Unbind();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Camera and light
    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));
    Light light(Eigen::Vector3f(0.0f, 5.0f, 5.0f), Eigen::Vector3f(0.0f, -1.0f, -1.0f), Eigen::Vector3f(1.0f, 1.0f, 1.0f));

    // Photon Emitter testing code here
    std::vector<Light> lights;
    lights.emplace_back(Eigen::Vector3f(0, 5, 0), Eigen::Vector3f(0, -1, 0), Eigen::Vector3f(1, 1, 1));  // Example to add a light
    PhotonEmitter emitter(scene, lights);
    std::vector<Photon> photonMap;
    emitter.emitPhotons(100, photonMap);  // Emit photons

    // Output results
    std::cout << "Emitted " << photonMap.size() << " photons." << std::endl;
    /*for (const auto& photon : photonMap) {
        std::cout << "Photon at position: " << photon.position.transpose()
            << " with energy: " << photon.energy.transpose() << std::endl;
    }*/

    // Don't close window instantly
    while (!glfwWindowShouldClose(window)) {
        // Background colour
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shaderProgram.Activate();
        GLuint modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        camera.Inputs(window);
        camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");

        glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), light.position.x(), light.position.y(), light.position.z());
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightColor"), light.intensity.x(), light.intensity.y(), light.intensity.z());
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);

        VAO1.Bind();
        glDrawElements(GL_TRIANGLES, meshData.indices.size(), GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
	}

    // Cleanup
    VAO1.Delete();
    VBO_Positions.Delete();
    VBO_Normals.Delete();
    EBO1.Delete();
    shaderProgram.Delete();
    embreeManager.release();
    // Cleanup window
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;

}
// object https://free3d.com/3d-model/glass-6488.html
