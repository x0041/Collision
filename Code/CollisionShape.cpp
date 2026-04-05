
#include "CollisionShape.h"
#include "Vec3.h"

#include "BSphere.h"
#include "AABB.h"
#include "OBB.h"
#include "Plane.h"
#include "Ray.h"
#include "Line.h"

namespace Azul
{
	// B Spheres

	bool CollisionShape::BSphereAABB (const BSphere & _sphere, const AABB  & _aabb )
	{
		// Using this, we can basically find the nearest point to the BSphere on the AABB
		const Vec3 nearestPoint = clamp(_sphere.GetOrigin(), _aabb.GetMinBounds(), _aabb.GetMaxBounds());

		// Avoids square root call
		const Vec3 offset = _sphere.GetOrigin() - nearestPoint;
		return (offset.dot(offset) < (_sphere.GetRadius() * _sphere.GetRadius()));
	}
	bool CollisionShape::BSphereOBB  (const BSphere & _sphere, const OBB   & _obb  )
	{
		// Convert BSphere world space to local
		const Mat4 invWorld			 = _obb.GetWorld().getInv();
		const Vec4 sphereLocal		 = Vec4(_sphere.GetOrigin(), 1.0f) * invWorld;
		const Vec4 nearestPointLocal = clamp(sphereLocal, -_obb.GetHalfBounds(), _obb.GetHalfBounds());
		const Vec3 nearestPointWorld(nearestPointLocal * _obb.GetWorld());

		// Avoids square root call
		const Vec3 offset = _sphere.GetOrigin() - nearestPointWorld;
		return (offset.dot(offset) < (_sphere.GetRadius() * _sphere.GetRadius()));
	}
	bool CollisionShape::BSpherePlane(const BSphere & _sphere, const Plane & _plane)
	{
		// Find the nearest point on the plane to the BSphere
		const float distance	 = (_sphere.GetOrigin() - _plane.GetOrigin()).dot(_plane.GetNormal());
		const Vec3	nearestPoint =  _sphere.GetOrigin() - (distance * _plane.GetNormal());

		// Avoids square root call
		const Vec3  offset =  _sphere.GetOrigin() - nearestPoint;
		return (offset.dot(offset) < (_sphere.GetRadius() * _sphere.GetRadius()));
	}
	bool CollisionShape::BSphereRay  (const BSphere & _sphere, const Ray   & _ray  )
	{
		// Rays use a normalized direction, so the projection can be simplified
		const float alpha		= std::max(0.0f, (_ray.GetDirection().dot(_sphere.GetOrigin() - _ray.GetOrigin())));
		const Vec3 nearestPoint = _ray.GetOrigin() + (_ray.GetDirection() * alpha);

		// Avoids square root call
		const Vec3 offset = _sphere.GetOrigin() - nearestPoint;
		return (offset.dot(offset) < (_sphere.GetRadius() * _sphere.GetRadius()));
	}
	bool CollisionShape::BSphereLine (const BSphere & _sphere, const Line  & _line )
	{
		const Vec3 nearestPoint = _line.GetStartPoint() + (((_line.GetOffset().dot(_sphere.GetOrigin() - _line.GetStartPoint())) / (_line.GetOffset().dot(_line.GetOffset()))) * _line.GetOffset());

		// Avoids square root call
		const Vec3 offset = _sphere.GetOrigin() - nearestPoint;
		return (offset.dot(offset) < (_sphere.GetRadius() * _sphere.GetRadius()));
	}
	
	// AABBs

	bool CollisionShape::AABBOBB  (const AABB & _aabb, const OBB   & _obb  )
	{
		const OBB _convert(_aabb);

		// Perform SAT tests to see if OBB intersects along axis (only needs 9 axis tests because of world axes), break early when possible
		if (CollisionShape::protOBBGetSAT( _obb.GetWorld().v0(),									_obb, _convert)) { return false; }
		if (CollisionShape::protOBBGetSAT( _obb.GetWorld().v1(),									_obb, _convert)) { return false; }
		if (CollisionShape::protOBBGetSAT( _obb.GetWorld().v2(),									_obb, _convert)) { return false; }
		                                                   											
		if (CollisionShape::protOBBGetSAT(Vec4(1.0f, 0.0f, 0.0f, 0.0f),								_obb, _convert)) { return false; }
		if (CollisionShape::protOBBGetSAT(Vec4(0.0f, 1.0f, 0.0f, 0.0f),								_obb, _convert)) { return false; }
		if (CollisionShape::protOBBGetSAT(Vec4(0.0f, 0.0f, 1.0f, 0.0f),								_obb, _convert)) { return false; }
			   																			
		if (CollisionShape::protOBBGetSAT(Vec4(1.0f, 0.0f, 0.0f, 0.0f).cross(_obb.GetWorld().v0()), _obb, _convert)) { return false; }
		if (CollisionShape::protOBBGetSAT(Vec4(0.0f, 1.0f, 0.0f, 0.0f).cross(_obb.GetWorld().v1()), _obb, _convert)) { return false; }
		if (CollisionShape::protOBBGetSAT(Vec4(0.0f, 0.0f, 1.0f, 0.0f).cross(_obb.GetWorld().v2()), _obb, _convert)) { return false; }
		
		// Nothing else failed, so we're good
		return true;
	}
	bool CollisionShape::AABBPlane(const AABB & _aabb, const Plane & _plane)
	{
		// Get the center point of the AABB
		const Vec3 center	  = (_aabb.GetMinBounds() + _aabb.GetMaxBounds()) * 0.5f;
		const Vec3 halfBounds =  _aabb.GetMaxBounds() - center;

		const Vec3 absNorm = abs(_plane.GetNormal());

		const float radius	 = halfBounds.dot(absNorm);
		const float distance = _plane.GetNormal().dot(center - _plane.GetOrigin());

		return distance < radius;
	} 
	bool CollisionShape::AABBRay  (const AABB & _aabb, const Ray   & _ray  )
	{
		// Use the Slab Method (by Tavian) to calculate the intersection
		
		// This could be pre-calculated when the ray is created to avoid doing this (in the case of many raycasts)
		const float invX =  1.0f / _ray.GetDirection().x();
		const float invY =  1.0f / _ray.GetDirection().y();
		const float invZ =  1.0f / _ray.GetDirection().z();

		const float tx1 = (_aabb.GetMinBounds().x() - _ray.GetOrigin().x()) * invX;
		const float tx2 = (_aabb.GetMaxBounds().x() - _ray.GetOrigin().x()) * invX;
		const float ty1 = (_aabb.GetMinBounds().y() - _ray.GetOrigin().y()) * invY;
		const float ty2 = (_aabb.GetMaxBounds().y() - _ray.GetOrigin().y()) * invY;
		const float tz1 = (_aabb.GetMinBounds().z() - _ray.GetOrigin().z()) * invZ;
		const float tz2 = (_aabb.GetMaxBounds().z() - _ray.GetOrigin().z()) * invZ;

		const float tmin = std::max(std::max(std::min(tx1, tx2), std::min(ty1, ty2)), std::min(tz1, tz2));
		const float tmax = std::min(std::min(std::max(tx1, tx2), std::max(ty1, ty2)), std::max(tz1, tz2));

		// If there is a collision tmin can be used to get the length along the ray it intersects
		return (tmax >= tmin);
	}
	bool CollisionShape::AABBLine (const AABB & _aabb, const Line  & _line )
	{
		// According to Real Time Collision Detection by Christer Ericson (5.3.3)

		const Vec3 extent   = _aabb.GetMaxBounds() - _aabb.GetMinBounds();
		const Vec3 midPoint = _line.GetStartPoint() + _line.GetEndPoint() - _aabb.GetMinBounds() - _aabb.GetMaxBounds();

		// Try world coordinate axes as separating axes
		const float adx = fabs(_line.GetOffset().x()) + CollisionShape::epsilon;
		if (fabs(midPoint.x()) > extent.x() + adx) { return false; }

		const float ady = fabs(_line.GetOffset().y()) + CollisionShape::epsilon;
		if (fabs(midPoint.y()) > extent.y() + ady) { return false; }

		const float adz = fabs(_line.GetOffset().z()) + CollisionShape::epsilon;
		if (fabs(midPoint.z()) > extent.z() + adz) { return false; }

		// Try cross products of segment direction vector with coordinate axes
		if (fabs(midPoint.y() * _line.GetOffset().z() - midPoint.z() * _line.GetOffset().y()) > extent.y() * adz + extent.z() * ady) { return false; }
		if (fabs(midPoint.z() * _line.GetOffset().x() - midPoint.x() * _line.GetOffset().z()) > extent.x() * adz + extent.z() * adx) { return false; }
		if (fabs(midPoint.x() * _line.GetOffset().y() - midPoint.y() * _line.GetOffset().x()) > extent.x() * ady + extent.y() * adx) { return false; }
		
		// No separating axis found; segment must be overlapping AABB
		return true;
	}

	// OBBs

	bool CollisionShape::OBBPlane(const OBB & _obb, const Plane & _plane)
	{
		const float d = _plane.GetNormal().dot(_plane.GetOrigin());
		const float centerDistance = _plane.GetNormal().dot(Vec3(_obb.GetOrigin())) - d;

		const float proj = _obb.GetHalfBounds().x() * fabs(_plane.GetNormal().dot(Vec3(_obb.GetWorld().v0().getNorm()))) +
						   _obb.GetHalfBounds().y() * fabs(_plane.GetNormal().dot(Vec3(_obb.GetWorld().v1().getNorm()))) +
						   _obb.GetHalfBounds().z() * fabs(_plane.GetNormal().dot(Vec3(_obb.GetWorld().v2().getNorm())));

		return (fabs(centerDistance) <= proj || centerDistance < 0.0f);
	}
	bool CollisionShape::OBBRay  (const OBB & _obb, const Ray   & _ray  )
	{
		// Use the Slab Method (by Tavian) to calculate the intersection

		const Mat4 worldInv = _obb.GetWorld().getInv();

		const Vec4 rayDir	 = (Vec4(_ray.GetDirection(), 0.0f) * worldInv).getNorm();
		const Vec4 rayOrigin = (Vec4(_ray.GetOrigin(),    1.0f) * worldInv);

		// We converted to local space... local bounds are (min(-0.5f, -0.5f, -0.5f) : max(0.5f, 0.5f, 0.5f))
		// So just use that
		const Vec4 halfWidth(0.5f, 0.5f, 0.5f, 1.0f);

		// This could be pre-calculated when the ray is created to avoid doing this (in the case of many raycasts)
		const float invX = 1.0f / rayDir.x();
		const float invY = 1.0f / rayDir.y();
		const float invZ = 1.0f / rayDir.z();

		const float tx1 = (-halfWidth.x() - rayOrigin.x()) * invX;
		const float tx2 = ( halfWidth.x() - rayOrigin.x()) * invX;
		const float ty1 = (-halfWidth.y() - rayOrigin.y()) * invY;
		const float ty2 = ( halfWidth.y() - rayOrigin.y()) * invY;
		const float tz1 = (-halfWidth.z() - rayOrigin.z()) * invZ;
		const float tz2 = ( halfWidth.z() - rayOrigin.z()) * invZ;

		const float tmin = std::max(std::max(std::min(tx1, tx2), std::min(ty1, ty2)), std::min(tz1, tz2));
		const float tmax = std::min(std::min(std::max(tx1, tx2), std::max(ty1, ty2)), std::max(tz1, tz2));

		// If there is a collision tmin can be used to get the length along the ray it intersects
		return (tmax >= tmin);
	}
	bool CollisionShape::OBBLine (const OBB & _obb, const Line  & _line )
	{
		// (Modified) According to Real Time Collision Detection by Christer Ericson (5.3.3)

		const Mat4 worldInv   = _obb.GetWorld().getInv();
		
		// Make everything in local space
		const Vec3 lineStart (Vec4(_line.GetStartPoint(), 1.0f) * worldInv);
		const Vec3 lineOffset(Vec4(_line.GetOffset(), 1.0f)		* worldInv);
		
		// We converted to local space... local bounds are (min(-0.5f, -0.5f, -0.5f) : max(0.5f, 0.5f, 0.5f))
		// So just use that (double that for the full extent)
		const Vec3 extent(1.0f, 1.0f, 1.0f);

		const Vec3 midPoint = lineStart + (lineStart + lineOffset); // - minBounds - maxBounds;

		// Try world coordinate axes as separating axes
		const float adx = fabs(lineOffset.x()) + CollisionShape::epsilon;
		if (fabs(midPoint.x()) > extent.x() + adx) { return false; }

		const float ady = fabs(lineOffset.y()) + CollisionShape::epsilon;
		if (fabs(midPoint.y()) > extent.y() + ady) { return false; }

		const float adz = fabs(lineOffset.z()) + CollisionShape::epsilon;
		if (fabs(midPoint.z()) > extent.z() + adz) { return false; }

		// Try cross products of segment direction vector with coordinate axes
		if (fabs(midPoint.y() * lineOffset.z() - midPoint.z() * lineOffset.y()) > extent.y() * adz + extent.z() * ady) { return false; }
		if (fabs(midPoint.z() * lineOffset.x() - midPoint.x() * lineOffset.z()) > extent.x() * adz + extent.z() * adx) { return false; }
		if (fabs(midPoint.x() * lineOffset.y() - midPoint.y() * lineOffset.x()) > extent.x() * ady + extent.y() * adx) { return false; }

		// No separating axis found; segment must be overlapping AABB
		return true;
	}
	
	// Planes

	bool CollisionShape::PlaneRay (const Plane & _plane, const Ray   & _ray  )
	{
		const float denominator = _plane.GetNormal().dot(_ray.GetDirection());

		// Avoid working with intersections that are (almost) orthogonal
		if (fabs(denominator) > epsilon)
		{
			const Vec3 offset = _plane.GetOrigin() - _ray.GetOrigin();
			float alpha = offset.dot(_plane.GetNormal()) / denominator;

			if (alpha > epsilon)
			{
				// There's is an intersection at some point (_ray.mOrigin + _ray.mDirection * alpha)
				return true;
			}

			// There's no intersection
			return false;
		}

		// Even if the vectors are orthogonal, it's possible the ray exists inside the plane
		// 
		// If the vector between the two origins's dot product with the plane's normal is zero, 
		// then it is orthogonal to it, meaning the ray exists within the plane
		if (fabs(_plane.GetNormal().dot(_plane.GetOrigin() - _ray.GetOrigin())) <= epsilon)
		{
			// There are infinite intersection points
			return true;
		}

		// There's no intersection
		return false;
	}
	bool CollisionShape::PlaneLine(const Plane & _plane, const Line  & _line )
	{
		// According to Real Time Collision Detection by Christer Ericson (5.3.1)

		// Compute the alpha value
		const float d	  = _plane.GetNormal().dot(_plane.GetOrigin());
		const float alpha = (d - _plane.GetNormal().dot(_line.GetStartPoint())) / _plane.GetNormal().dot(_line.GetOffset());

		// If alpha is [0..1], there is an intersection
		if (alpha >= 0.0f && alpha <= 1.0f)
		{
			return true;
		}

		return false;
	}
	
	// Rays

	bool CollisionShape::RayLine(const Ray & _ray, const Line & _line)
	{
		// Given this information: https://palitri.com/vault/stuff/maths/Rays%20closest%20point.pdf
		// For the sake of the problem, "this" is "A / a->" and "_other" is "B / b->"

		// Solve for "c ->"
		const Vec3 originOffset = _line.GetStartPoint() - _ray.GetOrigin();

		const Vec3 lineDirection = _line.GetOffset().getNorm();

		// Solve for the denominator used to solve "D" and "E"
		const float temp = _ray.GetDirection().dot(lineDirection);
		const float denominator = 1.0f ;

		// Solve for "D"
		const Vec3 pointA = _ray.GetOrigin() + _ray.GetDirection()	   *
							((-(_ray.GetDirection().dot(lineDirection) * lineDirection.dot(originOffset))   +
							(_ray.GetDirection().dot(originOffset)	   * lineDirection.dot(lineDirection))) *
							denominator);

		// Find the closest point on the ray to the line, use that to find the closest point on the line
		const Vec3 pointB = _line.GetStartPoint() + (((_line.GetOffset().dot(pointA - _line.GetStartPoint())) / (_line.GetOffset().dot(_line.GetOffset()))) * _line.GetOffset());


		// The distance between "D" and "pointB" is the distance between the two vectors
		// Avoids square root call
		const Vec3 offset = pointA - pointB;
		return (offset.dot(offset) <= epsilon);
	}

	// Lines



	// Helper Functions

	bool  CollisionShape::protOBBGetSAT(const Vec4 & _axis, const OBB & _a, const OBB & _b)
    {
		if (_axis.isZero()) { return false; }

		// Formulas could be adjusted to use len squared instead of actual len (no sqrt() call)
		const float magnitude = 1.0f / _axis.len();

		const float CCV = (_b.GetOrigin() - _a.GetOrigin()).dot(_axis);
		const float D   = fabs(CCV) * magnitude;

		const float projAMax = CollisionShape::protOBBGetProj(_axis, _a, magnitude);
		const float projBMax = CollisionShape::protOBBGetProj(_axis, _b, magnitude);
        
        return D > (projAMax + projBMax);
    }
	float CollisionShape::protOBBGetProj(const Vec4 & _axis, const OBB & _this, const float _mag)
	{
		// ================================================================================
		// 
		// Don't use mHalfBounds here because scale was already applied to our world matrix
		// 
		// ================================================================================
		// Bounds are in model space (the math here is pointless atm because it uses half bounds)
		// const Vec4  dA = (_this.mHalfBounds - (-_this.mHalfBounds)) * 0.5f;
		const Vec4 dA(0.5f, 0.5f, 0.5f, 0.0f);

		const Vec4 vA = _axis * _this.GetWorld().getInv();
		const Vec4 absVA = abs(vA);

		const float projA    = absVA.dot(dA) * _mag;
		const float scaleA   = _this.GetWorld().v2().dot(_this.GetWorld().v2());

		return projA * scaleA;
	}
}