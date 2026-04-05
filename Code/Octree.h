#ifndef OCTREE_H
#define OCTREE_H

#include "Octant.h"

namespace Azul
{
	// There's data that we want to be globally accessible in our octree structure
	// And redefining those variables for each octant would be wasteful, so we'll just
	// store a backpointer to here, have this be the head, and have it hold that data
	class Octree : public Octant
	{
	public:

		// The Big Four
		Octree() = delete;
		Octree(const Octree & _other) = delete;
		Octree & operator = (const Octree & _other) = delete;

		~Octree();

		Octree(const AABB & _bounds, const unsigned int _maxDataPerOctant, const unsigned int _maxOctantDepth);

		inline const unsigned int GetMaxDepth() const
		{
			return this->mMaxOctantDepth;
		}
		inline const unsigned int GetMaxData() const
		{
			return this->mMaxDataPerOctant;
		}



		// Debug functions
		inline void AddToIterationCount(const unsigned int _delta)
		{
			this->mDebugIterationCount += _delta;
		}
		inline unsigned int GetIterationCount() const
		{
			return this->mDebugIterationCount;
		}
		inline void ResetIterationCount()
		{
			this->mDebugIterationCount = 0;
		}

	private:

		const unsigned int mMaxDataPerOctant;
		const unsigned int mMaxOctantDepth;

		// Useful to see how many iterations we actually went through during a raycast
		unsigned int mDebugIterationCount;
	};
}

#endif