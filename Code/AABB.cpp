
#include "AABB.h"

#include "BSphere.h"
#include "OBB.h"
#include "Plane.h"
#include "Ray.h"
#include "Line.h"

namespace Azul
{
	AABB::AABB(const Vec3 & _minBounds, const Vec3 & _maxBounds) : mMin(_minBounds), mMax(_maxBounds) {}
	AABB::AABB(const AABB & _other) : mMin(_other.mMin), mMax(_other.mMax) {}

	AABB::~AABB() {}

	bool AABB::Intersect(const CollisionShape & _other) const
	{
		return _other.protIntersectAABB(* this);
	}

	bool AABB::protIntersectAABB   (const AABB	  & _other) const
	{
		// Use axis-interval testing
		return (this->mMin.x() < _other.mMax.x() && _other.mMin.x() < this->mMax.x() &&
				this->mMin.y() < _other.mMax.y() && _other.mMin.y() < this->mMax.y() &&
				this->mMin.z() < _other.mMax.z() && _other.mMin.z() < this->mMax.z());
	}

	bool AABB::protIntersectBSphere(const BSphere & _other) const
	{
		return CollisionShape::BSphereAABB(_other, (* this));
	}
	bool AABB::protIntersectOBB    (const OBB     & _other) const
	{
		return CollisionShape::AABBOBB((* this), _other);
	}
	bool AABB::protIntersectPlane  (const Plane   & _other) const
	{
		return CollisionShape::AABBPlane((* this), _other);
	}
	bool AABB::protIntersectRay    (const Ray     & _other) const
	{
		return CollisionShape::AABBRay((* this), _other);
	}
	bool AABB::protIntersectLine   (const Line    & _other) const
	{
		return CollisionShape::AABBLine((* this), _other);
	}



	HitResult AABB::Raycast(const Ray & _raycast) const
	{
		// Use the Slab Method (by Tavian) to calculate the intersection

		// This could be pre-calculated when the ray is created to avoid doing this (in the case of many raycasts)
		const float invX = 1.0f / _raycast.GetDirection().x();
		const float invY = 1.0f / _raycast.GetDirection().y();
		const float invZ = 1.0f / _raycast.GetDirection().z();

		const float tx1 = (this->mMin.x() - _raycast.GetOrigin().x()) * invX;
		const float tx2 = (this->mMax.x() - _raycast.GetOrigin().x()) * invX;
		const float ty1 = (this->mMin.y() - _raycast.GetOrigin().y()) * invY;
		const float ty2 = (this->mMax.y() - _raycast.GetOrigin().y()) * invY;
		const float tz1 = (this->mMin.z() - _raycast.GetOrigin().z()) * invZ;
		const float tz2 = (this->mMax.z() - _raycast.GetOrigin().z()) * invZ;

		const float tmin = std::max(std::max(std::min(tx1, tx2), std::min(ty1, ty2)), std::min(tz1, tz2));
		const float tmax = std::min(std::min(std::max(tx1, tx2), std::max(ty1, ty2)), std::max(tz1, tz2));

		// If there is a collision tmin can be used to get the length along the ray it intersects
		if (tmax >= tmin)
		{
			return HitResult(true, (_raycast.GetOrigin() + (_raycast.GetDirection() * tmin)), tmin);
		}

		return HitResult();
	}
}