
#include "Octree.h"

Octree::Octree(const AABB & _bounds, const unsigned int _maxDataPerOctant, const unsigned int _maxOctantDepth) :
	Octant(_bounds, this), mMaxDataPerOctant(_maxDataPerOctant), mMaxOctantDepth(_maxOctantDepth) {}

Octree::~Octree() {}