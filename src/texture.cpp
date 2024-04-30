#include <glad/glad.h>
#include "Texture.h"

Texture::Texture(int w, int h) : width(w), height(h), pixels(w* h, Eigen::Vector3f(0, 0, 0)) {}

void Texture::addPhoton(const Eigen::Vector3f& position, const Eigen::Vector3f& energy) {
    int x = std::min(int(position.x() * width), width - 1);
    int y = std::min(int(position.y() * height), height - 1);
    int index = y * width + x;
    pixels[index] += energy;
}

void Texture::normalize() {
    Eigen::Vector3f maxEnergy = *std::max_element(pixels.begin(), pixels.end(),
        [](const Eigen::Vector3f& a, const Eigen::Vector3f& b) { return a.norm() < b.norm(); });
    for (auto& pixel : pixels) {
        pixel = pixel.cwiseQuotient(maxEnergy); // Ensure no division by zero
    }
}

// Convert texture data for shader use
GLuint Texture::uploadToGPU() const {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Flatten pixel data
    std::vector<float> flatData;
    flatData.reserve(width * height * 3);
    for (const auto& pixel : pixels) {
        flatData.push_back(pixel.x());
        flatData.push_back(pixel.y());
        flatData.push_back(pixel.z());
    }

    // Specify the texture image, including format and type
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, flatData.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

    return textureID; // Return the texture ID for use in rendering
}
