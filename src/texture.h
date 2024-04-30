#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include <Eigen/Dense>
#include <algorithm>    // For std::min and std::max_element

class Texture {
public:
    int width, height;
    std::vector<Eigen::Vector3f> pixels;

    // Constructor to initialize the texture size and pixel data
    Texture(int w, int h);

    // Method to add photon energy to the texture at the mapped 2D coordinates
    void addPhoton(const Eigen::Vector3f& position, const Eigen::Vector3f& energy);

    // Normalize the texture's energy values to make them suitable for display or further processing
    void normalize();

    GLuint uploadToGPU() const;
};

#endif // TEXTURE_H

