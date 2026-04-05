
#include "BSphere.h"

#include "AABB.h"
#include "OBB.h"
#include "Plane.h"
#include "Ray.h"
#include "Line.h"

#include "MathEngine.h"

namespace Azul
{
	BSphere::BSphere(const Vec3 & _origin, const float _radius) : mOrigin(_origin), mRadius(_radius) {}
	BSphere::BSphere(const BSphere & _other) : mOrigin(_other.mOrigin), mRadius(_other.mRadius) {}

	BSphere::~BSphere() {}

	bool BSphere::Intersect(const CollisionShape & _other) const
	{
		return _other.protIntersectBSphere(* this);
	}
	
	bool BSphere::protIntersectBSphere(const BSphere & _other) const
	{
		// Avoids square root call thanks to proxy in comparison operator
		const Vec3 offset = this->mOrigin - _other.mOrigin;
		return (offset.dot(offset) < ((this->mRadius * this->mRadius) + (_other.mRadius * _other.mRadius)));
	}

	bool BSphere::protIntersectAABB	(const AABB	 & _other) const
	{
		return CollisionShape::BSphereAABB((*this), _other);
	}
	bool BSphere::protIntersectOBB  (const OBB   & _other) const
	{
		return CollisionShape::BSphereOBB((*this), _other);
	}
	bool BSphere::protIntersectPlane(const Plane & _other) const
	{
		return CollisionShape::BSpherePlane((*this), _other);
	}
	bool BSphere::protIntersectRay  (const Ray   & _other) const
	{
		return CollisionShape::BSphereRay((*this), _other);
	}
	bool BSphere::protIntersectLine (const Line  & _other) const
	{
		return CollisionShape::BSphereLine((* this), _other);
	}



	HitResult BSphere::Raycast(const Ray & _raycast) const
	{
		const Vec3 rayToSphere		= this->mOrigin - _raycast.GetOrigin();
		const float proj			= rayToSphere.dot(_raycast.GetDirection());
		const float distanceSquared = rayToSphere.dot(rayToSphere) - (proj * proj);
		const float radSquared		= this->mRadius * this->mRadius;

		// Break early (before sqrt, if we can)
		if (distanceSquared > radSquared)
		{
			return HitResult();
		}
		
		const float halfChord = Trig::sqrt(radSquared - distanceSquared);

		if ((proj - halfChord) > 0)
		{
			return HitResult(true, _raycast.GetOrigin() + (_raycast.GetDirection() * (proj - halfChord)), (proj - halfChord));
		}
		else if ((proj + halfChord) > 0)
		{
			return HitResult(true, _raycast.GetOrigin() + (_raycast.GetDirection() * (proj + halfChord)), (proj + halfChord));
		}
		else
		{
			return HitResult();
		}
	}
}