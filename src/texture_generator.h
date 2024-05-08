#pragma once

#include <vector>
#include <glad/glad.h>
#include "photon.h"  // Ensure this includes your Photon class definition

class TextureGenerator {
public:
    TextureGenerator(int width, int height);
    ~TextureGenerator();

    void updateTextureWithPhotons(const std::vector<Photon>& photons);
    GLuint getTextureID() const;

private:
    GLuint textureID;
    int textureWidth;
    int textureHeight;
    float* textureData;

    void initializeTexture();
};
