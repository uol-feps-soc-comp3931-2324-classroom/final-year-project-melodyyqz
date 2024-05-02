#include "texture_generator.h"
#include <iostream>
#include <cstring>  // For memset

// Constructor
TextureGenerator::TextureGenerator(int width, int height)
    : textureWidth(width), textureHeight(height), textureData(nullptr) {
    initializeTexture();
}

// Destructor
TextureGenerator::~TextureGenerator() {
    if (textureData) {
        delete[] textureData;
    }
    glDeleteTextures(1, &textureID);
}

void TextureGenerator::initializeTexture() {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, textureWidth, textureHeight, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    textureData = new float[textureWidth * textureHeight * 3]();
}

void TextureGenerator::updateTextureWithPhotons(const std::vector<Photon>& photons) {
    memset(textureData, 0, textureWidth * textureHeight * 3 * sizeof(float));

    for (const auto& photon : photons) {
        int x = static_cast<int>((photon.position.x() + 1) * textureWidth * 0.5f);
        int y = static_cast<int>((photon.position.y() + 1) * textureHeight * 0.5f);

        // Clamp the position to the texture size
        x = std::max(0, std::min(x, textureWidth - 1));
        y = std::max(0, std::min(y, textureHeight - 1));

        int index = (y * textureWidth + x) * 3;
        textureData[index] += photon.energy.x();    // R
        textureData[index + 1] += photon.energy.y(); // G
        textureData[index + 2] += photon.energy.z(); // B
    }

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textureWidth, textureHeight, GL_RGB, GL_FLOAT, textureData);
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint TextureGenerator::getTextureID() const {
    return textureID;
}
