#pragma once
#include "Photon.h"
#include <vector>
#include <memory>
#include <algorithm>

struct KDTreeNode {
    Photon photon;
    std::unique_ptr<KDTreeNode> left;
    std::unique_ptr<KDTreeNode> right;
    int axis;  // 0 for x, 1 for y, 2 for z

    KDTreeNode(const Photon& ph, int ax) : photon(ph), axis(ax) {}
};

class KDTree {
public:
    std::unique_ptr<KDTreeNode> root;

    void build(std::vector<Photon>& photons) {
        root = buildRecursive(photons, 0, photons.size(), 0);
    }
    void addPhoton(const Photon& photon);
    std::unique_ptr<KDTreeNode> buildRecursive(std::vector<Photon>& photons, size_t start, size_t end, int depth);
    void query(const Eigen::Vector3f& point, float radius, std::vector<Photon>& result) const;
private:
    std::vector<Photon> photons;
    void balance();
    void queryRecursive(const KDTreeNode* node, const Eigen::Vector3f& point, float radius, std::vector<Photon>& result) const;
};

