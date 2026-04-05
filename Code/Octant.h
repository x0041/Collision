#ifndef OCTANT_H
#define OCTANT_H

#include "Vec3.h"
#include "AABB.h"

#include "HitResult.h"

#include "GameObjectCollidable.h"
#include <vector>

namespace Azul
{
	// Forward declaration
	class Octree;
	class Octant
	{
	public:
		
		// The big four
		// Octant(const AABB & _bounds, const unsigned int _maxPerOctant, const unsigned int _maxOctantDepth) : poChildren(nullptr), poData(), mBounds(_bounds), mMaxDataPerOctant(_maxPerOctant), mMaxOctantDepth(_maxOctantDepth) {}
		
		Octant() = delete;
		Octant(const Octant & _other) = delete;
		Octant & operator = (const Octant & _other) = delete;
		
		~Octant();
		
		void Add(const GameObjectCollidable * _pAdd);
		void Remove(const GameObjectCollidable * _pRemove);

		HitResult Raycast(const Ray & _raycast) const;

		inline const AABB & GetBounds() const
		{
			return this->mBounds;
		}

	protected:

		// Octants are a base class of Octrees/OctreeBranches; we don't want them instantiated
		Octant(const AABB & _bounds, Octree * _pRoot);

		void protAdd(const GameObjectCollidable * _pAdd, const unsigned int _curDepth);
		void protRemove(const GameObjectCollidable * _pRemove);

		HitResult protRaycast(const Ray & _raycast) const;

	private:

		void privSplit();
		void privRemove(const GameObjectCollidable * _pRemove);

	private:

		Octree *  pRoot;
		Octant ** poChildren;
		std::vector<const GameObjectCollidable *> mData;

		const AABB mBounds;

	};
}

#endif