
#include "Plane.h"

#include "BSphere.h"
#include "AABB.h"
#include "OBB.h"
#include "Ray.h"
#include "Line.h"

namespace Azul
{
	Plane::Plane(const Vec3 & _origin, const Vec3 & _normal) : mOrigin(_origin), mNormal(_normal.getNorm()) {}
	Plane::Plane(const Plane & _other) : mOrigin(_other.mOrigin), mNormal(_other.mNormal) {}

	Plane::~Plane() {}

	bool Plane::Intersect(const CollisionShape & _other) const
	{
		return _other.protIntersectPlane(* this);
	}

	bool Plane::protIntersectPlane  (const Plane   & _other) const
	{
		const Vec3 cross = this->mNormal.cross(_other.mNormal);

		// Avoids square root call thanks to proxy in comparison operator
		return (cross.len() <= epsilon);
	}

	bool Plane::protIntersectBSphere(const BSphere & _other) const
	{
		return CollisionShape::BSpherePlane(_other, (* this));
	}
	bool Plane::protIntersectAABB   (const AABB    & _other) const
	{
		return CollisionShape::AABBPlane(_other, (* this));
	}
	bool Plane::protIntersectOBB    (const OBB     & _other) const
	{
		return CollisionShape::OBBPlane(_other, (* this));
	}
	bool Plane::protIntersectRay    (const Ray     & _other) const
	{
		return CollisionShape::PlaneRay((* this), _other);
	}
	bool Plane::protIntersectLine   (const Line    & _other) const
	{
		return CollisionShape::PlaneLine((* this), _other);
	}



	HitResult Plane::Raycast(const Ray & _raycast) const
	{
		const float denominator = this->mNormal.dot(_raycast.GetDirection());

		// Avoid working with intersections that are (almost) orthogonal
		if (fabs(denominator) > epsilon)
		{
			const Vec3 offset = this->mOrigin - _raycast.GetOrigin();
			float alpha = offset.dot(this->mNormal) / denominator;

			if (alpha > epsilon)
			{
				// There's is an intersection at some point (_ray.mOrigin + _ray.mDirection * alpha)
				return HitResult(true, (_raycast.GetOrigin() + (_raycast.GetDirection() * alpha)), alpha);
			}

			// There's no intersection
			return HitResult();
		}

		// Even if the vectors are orthogonal, it's possible the ray exists inside the plane
		// 
		// If the vector between the two origins's dot product with the plane's normal is zero, 
		// then it is orthogonal to it, meaning the ray exists within the plane
		if (fabs(this->mNormal.dot(this->mOrigin - _raycast.GetOrigin())) <= epsilon)
		{
			// There are infinite intersection points
			return HitResult(true, _raycast.GetOrigin(), 0);
		}

		// There's no intersection
		return HitResult();
	}
}