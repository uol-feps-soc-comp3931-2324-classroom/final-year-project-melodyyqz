// KDTree.cpp
#include "KDTree.h"

std::unique_ptr<KDTreeNode> KDTree::buildRecursive(const std::vector<Photon>& photons, size_t start, size_t end, int depth) {
    if (start >= end) return nullptr;
    size_t length = end - start;
    size_t median = start + length / 2;
    int axis = depth % 3;

    std::nth_element(photons.begin() + start, photons.begin() + median, photons.begin() + end,
        [axis](const Photon& a, const Photon& b) {
            return a.position[axis] < b.position[axis];
        });

    std::unique_ptr<KDTreeNode> node = std::make_unique<KDTreeNode>(photons[median], axis);
    node->left = buildRecursive(photons, start, median, depth + 1);
    node->right = buildRecursive(photons, median + 1, end, depth + 1);

    return node;
}
