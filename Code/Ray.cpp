
#include "Ray.h"

#include "BSphere.h"
#include "AABB.h"
#include "OBB.h"
#include "Plane.h"
#include "Line.h"

namespace Azul
{
	Ray::Ray(const Vec3 & _origin, const Vec3 & _direction) : mOrigin(_origin), mDirection(_direction.getNorm())
	{
		assert(!_direction.isZero());
	}
	Ray::Ray(const Ray & _other) : mOrigin(_other.mOrigin), mDirection(_other.mDirection) {}

	Ray::~Ray() {}

	bool Ray::Intersect(const CollisionShape & _other) const
	{
		return _other.protIntersectRay(* this);
	}
	
	bool Ray::protIntersectRay	  (const Ray	 & _other) const
	{
		// Given this information: https://palitri.com/vault/stuff/maths/Rays%20closest%20point.pdf
		// For the sake of the problem, "this" is "A / a->" and "_other" is "B / b->"

		// Solve for "c ->"
		const Vec3 originOffset = _other.GetOrigin() - this->mOrigin;

		// Solve for the denominator used to solve "D" and "E"
		const float temp = this->mDirection.dot(_other.GetDirection());
		const float denominator = 1.0f / ((this->mDirection.dot(this->mDirection) * _other.GetDirection().dot(_other.GetDirection())) - (temp * temp));

		// Solve for "D"
		const Vec3 pointA = this->mOrigin + this->mDirection *
							((-(this->mDirection.dot(_other.GetDirection()) * _other.GetDirection().dot(originOffset))			 +
							(this->mDirection.dot(originOffset)				* _other.GetDirection().dot(_other.GetDirection()))) * 
							denominator);

		// Solve for "E"
		const Vec3 pointB = _other.GetOrigin() + _other.GetDirection() * 
							(((	this->mDirection.dot(_other.GetDirection()) * this->mDirection.dot(originOffset))	   - 
							(	_other.GetDirection().dot(originOffset)		* this->mDirection.dot(this->mDirection))) *
							denominator);

		// The distance between "D" and "E" is the distance between the two vectors
		// Avoids square root call thanks to proxy in comparison operator
		return ((pointA - pointB).len() <= epsilon);
	}
	
	bool Ray::protIntersectBSphere(const BSphere & _other) const
	{
		return CollisionShape::BSphereRay(_other, (* this));
	}
	bool Ray::protIntersectAABB   (const AABB    & _other) const
	{
		return CollisionShape::AABBRay(_other, (* this));
	}
	bool Ray::protIntersectOBB    (const OBB     & _other) const
	{
		return CollisionShape::OBBRay(_other, (* this));;
	}
	bool Ray::protIntersectPlane  (const Plane   & _other) const
	{
		return CollisionShape::PlaneRay(_other, (* this));;
	}
	bool Ray::protIntersectLine   (const Line    & _other) const
	{
		return CollisionShape::RayLine((* this), _other);
	}



	HitResult Ray::Raycast(const Ray & _raycast) const
	{
		// Given this information: https://palitri.com/vault/stuff/maths/Rays%20closest%20point.pdf
		// For the sake of the problem, "this" is "A / a->" and "_other" is "B / b->"

		// Solve for "c ->"
		const Vec3 originOffset = _raycast.GetOrigin() - this->mOrigin;

		// Solve for the denominator used to solve "D" and "E"
		const float temp = this->mDirection.dot(_raycast.GetDirection());
		const float denominator = 1.0f / ((this->mDirection.dot(this->mDirection) * _raycast.GetDirection().dot(_raycast.GetDirection())) - (temp * temp));

		// Solve for "D"
		const Vec3 pointA = this->mOrigin + this->mDirection *
			((-(this->mDirection.dot(_raycast.GetDirection()) * _raycast.GetDirection().dot(originOffset)) +
			(   this->mDirection.dot(originOffset) * _raycast.GetDirection().dot(_raycast.GetDirection()))) *
			denominator);

		// Solve for "E"
		const Vec3 pointB = _raycast.GetOrigin() + _raycast.GetDirection() *
			((( this->mDirection.dot(_raycast.GetDirection()) * this->mDirection.dot(originOffset)) -
			(   _raycast.GetDirection().dot(originOffset) * this->mDirection.dot(this->mDirection))) *
			denominator);

		// The distance between "D" and "E" is the distance between the two vectors
		// Avoids square root call thanks to proxy in comparison operator
		if ((pointA - pointB).len() <= epsilon)
		{
			HitResult(true, pointB, (pointB - _raycast.GetOrigin()).len());
		}

		// We didn't hit it
		return HitResult();
	}
	HitResult Ray::Raycast(const CollisionShape & _other) const
	{
		return _other.Raycast(* this);
	}

}