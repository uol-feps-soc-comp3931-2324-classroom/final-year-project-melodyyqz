#define TASKING_TBB
#define __SSE2__

#include <glad/glad.h>
#include "rapidobj/rapidobj.hpp"
#include <embree3/rtcore.h>
#include <embree3/rtcore_common.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
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
#include "KDTree.h"

// Window dimensions
const unsigned int width = 800;
const unsigned int height = 800;

glm::vec3 calculateMeshCentroid(const SimpleMeshData& mesh) {
    glm::vec3 centroid = glm::vec3(0.0f);
    int count = 0;
    for (const auto& vertex : mesh.positions) {
        centroid += glm::vec3(vertex.x, vertex.y, vertex.z);
        count++;
    }
    if (count > 0) {
        centroid /= float(count);
    }
    return centroid;
}

// found mesh centroid is at x: -15.7934, y: 76.1911, z: -0.0502528
void printMeshPosition(const SimpleMeshData& mesh) {
    glm::vec3 centroid = calculateMeshCentroid(mesh);
    std::cout << "Mesh centroid is at: "
        << "x: " << centroid.x << ", y: " << centroid.y << ", z: " << centroid.z << std::endl;
}

Eigen::Vector3f glmToEigen(const glm::vec3& v) {
    return Eigen::Vector3f(v.x, v.y, v.z);
}

GLfloat groundVertices[] = {
    // Positions           // Normals         // Colors (R, G, B)       // Texture Coords
    -50.0f, 0.0f, -50.0f,  0.0f, 1.0f, 0.0f,  0.2f, 0.2f, 0.2f,  0.0f, 0.0f,
     50.0f, 0.0f, -50.0f,  0.0f, 1.0f, 0.0f,  0.2f, 0.2f, 0.2f,  1.0f, 0.0f,
     50.0f, 0.0f,  50.0f,  0.0f, 1.0f, 0.0f,  0.2f, 0.2f, 0.2f,  1.0f, 1.0f,
    -50.0f, 0.0f,  50.0f,  0.0f, 1.0f, 0.0f,  0.2f, 0.2f, 0.2f,  0.0f, 1.0f
};

GLuint groundIndices[] = {
    0, 1, 2,  // First Triangle
    0, 2, 3   // Second Triangle
};

std::vector<Eigen::Vector3f> generateGroundPoints(int resolution = 10) {
    std::vector<Eigen::Vector3f> points;
    float step = 100.0f / resolution;  // ground extends from -50 to +50, so width is 100
    for (int i = 0; i <= resolution; ++i) {
        for (int j = 0; j <= resolution; ++j) {
            float x = -50 + i * step;
            float z = -50 + j * step;
            points.emplace_back(x, 0.0f, z);  // y is 0 as ground plane is at y = 0
        }
    }
    return points;
}

int main() {
    // Initialize GLFW
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Create shader program
    Shader shaderProgram("default.vert", "default.frag");
    //glEnable(GL_DEPTH_TEST);

    scene.addGroundPlane();
    scene.commitScene();
    // Initialise model
    //glm::mat4 model = glm::translate(glm::mat4(1.0f), objectPosition);
    glm::mat4 model = glm::mat4(1.0f);
    // Scale it down
    float scale = 0.1f;
    model = glm::scale(model, glm::vec3(scale, scale, scale));

    VAO VAO1;
    VAO1.Bind();

    // Load object
    SimpleMeshData meshData = load_wavefront_obj("glass-obj.obj");
    scene.addMesh(meshData);
    scene.commitScene();

    glm::vec3 glassCupCentroid = calculateMeshCentroid(meshData);
    Eigen::Vector3f glassCupCentroidEigen = glmToEigen(glassCupCentroid);

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

    VAO VAO_Ground;
    VAO_Ground.Bind();

    VBO VBO_Ground(groundVertices, sizeof(groundVertices));
    EBO EBO_Ground(groundIndices, sizeof(groundIndices));
    VAO_Ground.LinkVBO(VBO_Ground, 0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)0);  // Positions
    VAO_Ground.LinkVBO(VBO_Ground, 1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));  // Normals
    VAO_Ground.LinkVBO(VBO_Ground, 2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));  // Colors
    VAO_Ground.LinkVBO(VBO_Ground, 3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(9 * sizeof(GLfloat)));  // Texture Coordinates

    VAO_Ground.Unbind();
    VBO_Ground.Unbind();
    EBO_Ground.Unbind();

    //glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Camera, light, and photon emitter setup
    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));
    //camera.setLookAt(glm::vec3(sceneCenter.x(), sceneCenter.y(), sceneCenter.z()));
    //Eigen::Vector3f lightDirection = (sceneCenter - lightPosition).normalized();
    //Light light(lightPosition, lightDirection, Eigen::Vector3f(1.0f, 1.0f, 1.0f));
    //Light light(Eigen::Vector3f(0.0f, 5.0f, 5.0f), Eigen::Vector3f(0.0f, -1.0f, -1.0f), Eigen::Vector3f(1.0f, 1.0f, 1.0f));
    Light light(
        Eigen::Vector3f(glassCupCentroid.x + 10.0f, glassCupCentroid.y + 20.0f, glassCupCentroid.z),
        Eigen::Vector3f(-0.1f, -1.0f, -0.1f),
        Eigen::Vector3f(2.0f,2.0f, 2.0f)
    );
    PhotonEmitter emitter(light, 90.0f);

    // Emit photons
    std::vector<Photon> photons = emitter.emitPhotons(1000);  // Emit 1000 photons

    std::cout << "Emitted " << photons.size() << " photons." << std::endl;
    for (const auto& photon : photons) {
        Eigen::Vector3f normalizedDirection = photon.direction.normalized();
        std::cout << "Photon Position: " << photon.position.transpose()
            << ", Direction: " << normalizedDirection.transpose()
            << ", Energy: " << photon.energy.transpose() << std::endl;
    }

    // Trace photons
    int hitCount = 0;
    std::vector<Photon> storedPhotons;
    for (auto& photon : photons) {
        std::cout << "Tracing photon from position: " << photon.position.transpose()
            << " with direction: " << photon.direction.transpose() << std::endl;
        if (scene.tracePhoton(photon)) { // Make sure to use 'tracePhoton' if that's what you implemented
            hitCount++;
            storedPhotons.push_back(photon);
        }
    }
    std::cout << "Hit count: " << hitCount << std::endl;
    std::cout << "Ground hit count: " << scene.getGroundPhotonCount() << std::endl;
    KDTree photonMap;
    photonMap.build(storedPhotons);

    Eigen::Vector3f normal = Eigen::Vector3f(0, 1, 0); // Assuming the normal is upward
    Eigen::Vector3f causticsEffect = scene.computeCaustics(glassCupCentroidEigen, normal);

    // Don't close window instantly
    while (!glfwWindowShouldClose(window)) {
        // Background colour
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shaderProgram.Activate();

        // Caustics effect to shader
        GLint causticsLoc = glGetUniformLocation(shaderProgram.ID, "causticsColor");
        glUniform3f(causticsLoc, causticsEffect.x(), causticsEffect.y(), causticsEffect.z());

        GLuint modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        camera.Inputs(window);
        camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");

        glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), light.position.x(), light.position.y(), light.position.z());
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightColor"), light.intensity.x(), light.intensity.y(), light.intensity.z());
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);

        // Draw glass
        VAO1.Bind();
        glDrawElements(GL_TRIANGLES, meshData.indices.size(), GL_UNSIGNED_INT, 0);
        
        // Draw the ground plane
        glm::mat4 groundModel = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(groundModel));
        // Calculate caustics for multiple points on the ground and pass them to the shader
        Eigen::Vector3f groundNormal(0, 1, 0); // Assuming the ground plane is aligned with the XY plane
        std::vector<Eigen::Vector3f> groundPoints = generateGroundPoints(); // Implement this based on your ground geometry
        for (const auto& point : groundPoints) {
            Eigen::Vector3f causticsEffect = scene.computeCaustics(point, groundNormal);
            // Pass this caustic effect to the shader
            GLint causticsLoc = glGetUniformLocation(shaderProgram.ID, "causticsColor");
            glUniform3f(causticsLoc, causticsEffect.x(), causticsEffect.y(), causticsEffect.z());
        }
        
        VAO_Ground.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
	}

    // Cleanup
    VAO1.Delete();
    VBO_Positions.Delete();
    VBO_Normals.Delete();
    EBO1.Delete();
    VAO_Ground.Delete();
    VBO_Ground.Delete();
    EBO_Ground.Delete();
    shaderProgram.Delete();
    embreeManager.release();
    // Cleanup window
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;

}
// object https://free3d.com/3d-model/glass-6488.html


