
#include "Octant.h"
#include "Octree.h"

#include "GameObjectMan.h"
#include "GameObjectFactory.h"

namespace Azul
{
	Octant::Octant(const AABB & _bounds, Octree * _pRoot) : pRoot(_pRoot), poChildren(nullptr), mData(), mBounds(_bounds)                  
	{
		GameObjectMan::Add(GameObjectFactory::Create(ShaderObject::Name::FogWireframe, Mesh::Name::AABB, Vec4(1.0f, 0.0f, 0.0f, 1.0f), new AABB(_bounds)));
	}
	Octant::~Octant()
	{
		// Delete children if not a leaf node
		if (poChildren != nullptr)
		{
			for (unsigned int i = 0; i < 8; i++)
			{
				delete poChildren[i];
			}

			delete[] poChildren;
			poChildren = nullptr;
		}
	}

	void Octant::Add(const GameObjectCollidable * _pAdd)
	{
		// Check to see if the new data falls within our range
		if (_pAdd->Intersect(this->mBounds))
		{
			// Call recursive function
			protAdd(_pAdd, 0);
		}
	}
	void Octant::protAdd(const GameObjectCollidable * _pAdd, const unsigned int _curDepth)
	{
		// We don't need to check if we have collision with the data because either
		//     1. Our parent already did it
		//     2. We're the start of the recursive call and Add() already did it

		// Check to see if we're a leaf node
		if (this->poChildren == nullptr)
		{
			// Since we're a leaf node, we care about how much data we're holding
			// If we're holding the max amount of data and we're allowed to split further
			if (_curDepth <= this->pRoot->GetMaxDepth() && this->mData.size() >= this->pRoot->GetMaxData())
			{
				// We (will) have too much data once we add it, so split our data
				// Do not return here because we still need to add our data later
				this->privSplit();

				// We have children now, so let's see what we can get rid of

				// Go through the vector in reverse order, so it doesn't matter if the order changes or if things get re-added
				for (size_t i = this->mData.size(); i > 0; i--)
				{
					// Erase the data from this vector (if it still belongs to us, it'll be added back)
					const GameObjectCollidable * pCurrent = this->mData.at(i - 1);
					this->mData.erase(this->mData.begin() + (i - 1));

					// Add it (depth doesn't change because we're starting here, not in a child)
					this->protAdd(pCurrent, _curDepth);
				}
			}
			else
			{
				// We remain a leaf node, so just add the new data
				this->mData.push_back(_pAdd);
				return;
			}
		}
		
		// Go through the child to see where the data should be added to
		char lastIndex = 0xFF;
		for (unsigned int i = 0; i < 8; i++)
		{
			if (_pAdd->Intersect(poChildren[i]->GetBounds()))
			{
				// This is called if more than one child has a valid intersection
				if (lastIndex >= 0)
				{
					// Our data is not duplicated between octants; if data overlaps multiple octants,
					// it belongs to the parent which encompasses both octants

					// Add the data to self and leave; we don't need to check the other intersections
					this->mData.push_back(_pAdd);
					return;
				}

				// Store the child
				lastIndex = i;
			}
		}

		// Add it to the child
		poChildren[lastIndex]->protAdd(_pAdd, _curDepth + 1);
	}

	void Octant::Remove(const GameObjectCollidable * _pRemove)
	{
		// We should avoid checking the current octant's data list before checking to see if the
		// data falls to a child or not (data list could become long, but child count remains constant)
		if (_pRemove->Intersect(this->mBounds))
		{
			// Call recursive function
			protRemove(_pRemove);
		}
	}
	void Octant::protRemove(const GameObjectCollidable * _pRemove)
	{
		// Prioritize checking to see if the data exists in a child octant
		// There's always 8 children (if there are children), but there can
		// be N data instances
		
		// Remove it from self if only we can have it
		if (this->poChildren != nullptr)
		{
			char lastIndex = 0xFF;
			for (unsigned int i = 0; i < 8; i++)
			{
				if (_pRemove->Intersect(poChildren[i]->GetBounds()))
				{
					// This is called if more than one child has a valid intersection
					if (lastIndex >= 0)
					{
						// Our data is not duplicated between octants; if data overlaps multiple octants,
						// it belongs to the parent which encompasses both octants

						// Add the data to self and leave; we don't need to check the other intersections
						this->privRemove(_pRemove);
						return;
					}

					// Store the child
					lastIndex = i;
				}
			}

			// We don't have it, but a child must
			this->poChildren[lastIndex]->protRemove(_pRemove);
			return;
		}
		
		// I have no children, so I must have it
		this->privRemove(_pRemove);
		return;
	}
	void Octant::privRemove(const GameObjectCollidable * _pRemove)
	{
		for (size_t i = 0; i < this->mData.size(); i++)
		{
			if (this->mData.at(i) == _pRemove)
			{
				this->mData.erase(this->mData.begin() + i);
				return;
			}
		}
	}

	HitResult Octant::Raycast(const Ray & _raycast) const
	{
		// We'll need to check all children and ideally return the closest results
		// (If a raycast hits earlier, we want it to return that one rather than a
		// further one, but who knows what the order they will be checked)


		// Debug information
		this->pRoot->ResetIterationCount();


		// Check to see if ray falls within our range
		if (_raycast.Intersect(this->mBounds))
		{
			// Call recursive function
			return protRaycast(_raycast);
		}

		// There's nothing to be hit
		return HitResult(false, Vec3(), -1.0f);
	}
	HitResult Octant::protRaycast(const Ray & _raycast) const
	{
		HitResult result;

		// Unfortunately, we can't skip our own stuff because it could be literally anywhere in the volume
		// Well... at least within the inner seams of the octants within the bounds 
		//
		// But, we'll start with the data because if we find something really close, we can skip any Octants
		// further away the raycast hit result we got 
		for (size_t i = 0; i < this->mData.size(); i++)
		{
			HitResult other = _raycast.Raycast(*this->mData.at(i)->GetShape());

			// We could do < or <=, but we'll do first-come-first-serve
			if (other < result)
			{
				result = other;
			}
		}

		// Go through the child octants
		if (this->poChildren != nullptr)
		{
			for (unsigned int i = 0; i < 8; i++)
			{
				Octant * pCurrent = poChildren[i];
				if (_raycast.Intersect(pCurrent->GetBounds()))
				{
					// If the nearest point on the Octant is further than the current hit result we have,
					// we don't need to bother checking it or any of its children/data because they'll be
					// further than our current result so their results will be thrown out anyways (and it's
					// not as if we risk missing data because anything overlapping the edge will belong to
					// the parent Octant, which we just checked in the first for loop above)

					// Compare distance squared (if closest point on AABB is further than current hit result, no point checking any data)
					const Vec3 dist = (_raycast.GetOrigin() - clamp(_raycast.GetOrigin(), pCurrent->GetBounds().GetMinBounds(), pCurrent->GetBounds().GetMaxBounds()));
					if (result.GetHit() && (dist.dot(dist) >= result.GetLength() * result.GetLength()))
					{
						// It saves us from doing some checks... sometimes
						continue;
					}

					HitResult other = pCurrent->protRaycast(_raycast);

					// We could do < or <=, but we'll do first-come-first-serve
					if (other < result)
					{
						result = other;
					}
				}
			}
		}

		{
			// Debug information
			if (this->poChildren != nullptr)
			{
				// Include the 8 Octant intersection checks
				this->pRoot->AddToIterationCount(8);
			}
			// Include all the entities we did intersection checks on
			this->pRoot->AddToIterationCount(this->mData.size());
		}
		
		return result;
	}

	void Octant::privSplit()
	{
		assert(this->poChildren == nullptr);

		const Vec3 minB =  this->mBounds.GetMinBounds();
		const Vec3 maxB =  this->mBounds.GetMaxBounds();
		const Vec3 half = (this->mBounds.GetMaxBounds() - this->mBounds.GetMinBounds()) * 0.5f;

		// This could probably be a contiguous memory (better for performance (though this isn't called often))
		this->poChildren = new Octant * [8];

		this->poChildren[0] = new Octant(AABB(Vec3(minB.x(),			minB.y(),			 minB.z()			), Vec3(minB.x() + half.x(), minB.y() + half.y(), minB.z() + half.y())), this->pRoot);
		this->poChildren[1] = new Octant(AABB(Vec3(minB.x(),			minB.y(),			 minB.z() + half.z()), Vec3(minB.x() + half.x(), minB.y() + half.y(), maxB.z()			 )), this->pRoot);
		this->poChildren[2] = new Octant(AABB(Vec3(minB.x(),			minB.y() + half.y(), minB.z()			), Vec3(minB.x() + half.x(), maxB.y(),			  minB.z() + half.y())), this->pRoot);
		this->poChildren[3] = new Octant(AABB(Vec3(minB.x(),			minB.y() + half.y(), minB.z() + half.z()), Vec3(minB.x() + half.x(), maxB.y(),			  maxB.z()			 )), this->pRoot);
		this->poChildren[4] = new Octant(AABB(Vec3(minB.x() + half.x(), minB.y(),			 minB.z()			), Vec3(maxB.x(),			 minB.y() + half.y(), minB.z() + half.y())), this->pRoot);
		this->poChildren[5] = new Octant(AABB(Vec3(minB.x() + half.x(), minB.y(),			 minB.z() + half.z()), Vec3(maxB.x(),			 minB.y() + half.y(), maxB.z()			 )), this->pRoot);
		this->poChildren[6] = new Octant(AABB(Vec3(minB.x() + half.x(), minB.y() + half.y(), minB.z()			), Vec3(maxB.x(),			 maxB.y(),			  minB.z() + half.y())), this->pRoot);
		this->poChildren[7] = new Octant(AABB(Vec3(minB.x() + half.x(), minB.y() + half.y(), minB.z() + half.z()), Vec3(maxB.x(),			 maxB.y(),			  maxB.z()			 )), this->pRoot);
	}
}