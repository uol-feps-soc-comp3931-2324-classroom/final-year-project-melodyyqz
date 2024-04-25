// KDTree.cpp
#include "KDTree.h"
#include "photon.h"
#include <iostream>

std::unique_ptr<KDTreeNode> KDTree::buildRecursive(std::vector<Photon>& photons, size_t start, size_t end, int depth) {
    if (start >= end) return nullptr;
    size_t length = end - start;
    size_t median = start + length / 2;
    int axis = depth % 3;

    std::nth_element(photons.begin() + start, photons.begin() + median, photons.begin() + end,
        [axis](const Photon& a, const Photon& b) {
            return a.position[axis] < b.position[axis];
        });

    std::unique_ptr<KDTreeNode> node = std::make_unique<KDTreeNode>(photons[median], axis);
    
    //std::cout << "Building Node - Axis: " << axis << ", Photon Position: " << photons[median].position.transpose() << std::endl;
    
    node->left = buildRecursive(photons, start, median, depth + 1);
    node->right = buildRecursive(photons, median + 1, end, depth + 1);

    return node;
}

void KDTree::addPhoton(const Photon& photon) {
	photons.push_back(photon);
    balance();
}

void KDTree::query(const Eigen::Vector3f& point, float radius, std::vector<Photon>& result) const {
    queryRecursive(root.get(), point, radius, result);
}

void KDTree::queryRecursive(const KDTreeNode* node, const Eigen::Vector3f& point, float radius, std::vector<Photon>& result) const {
    if (!node) return;

    Eigen::Vector3f diff = point - node->photon.position;
    if (diff.norm() <= radius) {
        result.push_back(node->photon);
    }

    float distance = point[node->axis] - node->photon.position[node->axis];
    if (distance < 0) {
        queryRecursive(node->left.get(), point, radius, result);
        if (std::abs(distance) < radius) {
            queryRecursive(node->right.get(), point, radius, result);
        }
    }
    else {
        queryRecursive(node->right.get(), point, radius, result);
        if (std::abs(distance) < radius) {
            queryRecursive(node->left.get(), point, radius, result);
        }
    }
}

void KDTree::balance() {
    if (!photons.empty()) {
		root = buildRecursive(photons, 0, photons.size(), 0);
    }
}

/*std::vector<Photon> KDTree::extractRelevantPhotons(const Camera& camera) {
    std::vector<Photon> relevantPhotons;
    Eigen::Vector3f searchPoint = camera.getPosition();
    float searchRadius = calculateAppropriateRadius();  // Define this function based on your scene's scale
    this->query(searchPoint, searchRadius, relevantPhotons);
    return relevantPhotons;
}*/