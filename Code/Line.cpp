
#include "Line.h"

#include "BSphere.h"
#include "AABB.h"
#include "OBB.h"
#include "Plane.h"
#include "Ray.h"

namespace Azul
{
	Line::Line(const Vec3 & _startPoint, const Vec3 & _endPoint) : mOrigin(_startPoint), mOffset(_endPoint - _startPoint) {}
	Line::Line(const Line & _other) : mOrigin(_other.mOrigin), mOffset(_other.mOffset) {}

	Line::~Line() {}

	bool Line::Intersect(const CollisionShape & _other) const
	{
		return _other.protIntersectLine(* this);
	}
	
	bool Line::protIntersectLine   (const Line    & _other) const
	{
		const float a = this->mOffset.dot(this->mOffset);
		const float b = this->mOffset.dot(_other.GetOffset());
		const float c = _other.GetOffset().dot(_other.GetOffset());

		const float d = (this->mOrigin - _other.GetStartPoint()).dot(this->mOffset);
		const float e = (this->mOrigin - _other.GetStartPoint()).dot(_other.GetOffset());

		float denominator = b * b - a * c;

		// Test if the lines are parallel
		if (fabs(denominator) <= epsilon)
		{
			// A vector to the nearest point
			const Vec3 nearby = _other.GetStartPoint() + ((e / c) * _other.GetOffset());

			return (fabs(this->mOrigin.dot(nearby)) <= epsilon);
		}

		denominator = 1.0f / denominator;

		// The lines are not parallel, so we need to do more math
		const Vec3 nearPointA = (this->mOrigin + this->mOffset) * (((c * d) - (b * e)) * denominator);
		const Vec3 nearPointB = (_other.GetEndPoint())			* (((d * b) - (a * e)) * denominator);

		// Avoids square root call
		const Vec3 offset = nearPointA - nearPointB;
		return (offset.dot(offset) <= epsilon);
	}

	bool Line::protIntersectBSphere(const BSphere & _other) const
	{
		return CollisionShape::BSphereLine(_other, (* this));
	}
	bool Line::protIntersectAABB   (const AABB    & _other) const
	{
		return CollisionShape::AABBLine(_other, (* this));
	}
	bool Line::protIntersectOBB    (const OBB     & _other) const
	{
		return CollisionShape::OBBLine(_other, (* this));;
	}
	bool Line::protIntersectPlane  (const Plane   & _other) const
	{
		return CollisionShape::PlaneLine(_other, (* this));;
	}
	bool Line::protIntersectRay	   (const Ray	  & _other) const
	{
		return CollisionShape::RayLine(_other, (* this));
	}



	HitResult Line::Raycast(const Ray & _raycast) const
	{
		// Given this information: https://palitri.com/vault/stuff/maths/Rays%20closest%20point.pdf
		// For the sake of the problem, "this" is "A / a->" and "_other" is "B / b->"

		// Solve for "c ->"
		const Vec3 originOffset = this->mOrigin - _raycast.GetOrigin();

		const Vec3 lineDirection = this->mOffset.getNorm();

		// Solve for the denominator used to solve "D" and "E"
		const float temp = _raycast.GetDirection().dot(lineDirection);
		const float denominator = 1.0f ;

		// Solve for "D"
		const Vec3 pointA = _raycast.GetOrigin() + _raycast.GetDirection()	   *
							((-(_raycast.GetDirection().dot(lineDirection) * lineDirection.dot(originOffset))   +
							(_raycast.GetDirection().dot(originOffset)	   * lineDirection.dot(lineDirection))) *
							denominator);

		// Find the closest point on the ray to the line, use that to find the closest point on the line
		const Vec3 pointB = this->mOrigin + (((this->mOffset.dot(pointA - this->mOrigin)) / (this->mOffset.dot(this->mOffset))) * this->mOffset);

		// The distance between "D" and "pointB" is the distance between the two vectors
		// Avoids square root call
		const Vec3 offset = pointA - pointB;
		if (offset.dot(offset) <= epsilon)
		{
			return HitResult(true, pointA, (pointA - _raycast.GetOrigin()).len());
		}

		// We didn't hit anything
		return HitResult();
	}
}