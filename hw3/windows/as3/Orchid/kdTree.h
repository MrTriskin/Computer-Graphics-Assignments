#ifndef KDTREE_H
#define KDTREE_H

#include <vector>

#include "../Orchid/math/vector3d.h"
#include "../Orchid/ray/ray.h"
#include "../Orchid/objects/triangle.h"
#include "../Orchid/accel/aabbox.h"

class KDNode {
public:
	Orchid::AABBox box;
    KDNode* left;
    KDNode* right;
    std::vector<Orchid::Triangle*> triangles;
    bool leaf;

    KDNode(){};
    KDNode* build(std::vector<Orchid::Triangle*> &tris, int depth);
    bool hit (KDNode* node, const Orchid::Ray &ray, double &t, double &tmin, Orchid::Vector3d &normal, Orchid::Vector3d &c);
};

#endif // KDTREE_H