
#include "OBB.h"

#include "BSphere.h"
#include "AABB.h"
#include "Plane.h"
#include "Ray.h"
#include "Line.h"

#include "MathEngine.h"

namespace Azul
{
    OBB::OBB(const Rot & _rotation, const Vec3 & _origin, const Vec3 & _halfBounds) : mWorld(Scale(Vec3(_halfBounds * 2.0f)) * Rot(_rotation) * Trans(_origin)), mOrigin(Vec4(_origin, 1.0f)), mHalfBounds(Vec4(_halfBounds, 1.0f)) {}
    OBB::OBB(const OBB & _other) : mWorld(_other.mWorld), mOrigin(_other.mOrigin), mHalfBounds(_other.mHalfBounds) {}
	OBB::OBB(const AABB & _other) : 
		mWorld(Scale(Vec3(_other.GetMaxBounds() - _other.GetMinBounds())) * Trans((_other.GetMinBounds() + _other.GetMaxBounds()) * 0.5f)), 
		mOrigin(Vec4((_other.GetMinBounds() + _other.GetMaxBounds()) * 0.5f, 1.0f)), 
		mHalfBounds(Vec4(_other.GetMaxBounds() - _other.GetMinBounds(), 1.0f)) {}
	OBB::~OBB() {}

	bool OBB::Intersect(const CollisionShape & _other) const
	{
		return _other.protIntersectOBB(* this);
	}

	bool OBB::protIntersectOBB	  (const OBB	 & _other) const
	{	
		// Perform SAT tests to see if OBB intersects along axis (needs 15 axis tests), break early when possible
		if (CollisionShape::protOBBGetSAT( this->mWorld.v0(),                           (* this), _other)) { return false; }
		if (CollisionShape::protOBBGetSAT( this->mWorld.v1(),                           (* this), _other)) { return false; }
		if (CollisionShape::protOBBGetSAT( this->mWorld.v2(),                           (* this), _other)) { return false; }
		                                                   								
		if (CollisionShape::protOBBGetSAT(_other.mWorld.v0(),                           (* this), _other)) { return false; }
		if (CollisionShape::protOBBGetSAT(_other.mWorld.v1(),                           (* this), _other)) { return false; }
		if (CollisionShape::protOBBGetSAT(_other.mWorld.v2(),                           (* this), _other)) { return false; }
			   																			
		if (CollisionShape::protOBBGetSAT( this->mWorld.v0().cross(_other.mWorld.v0()), (* this), _other)) { return false; }
		if (CollisionShape::protOBBGetSAT( this->mWorld.v0().cross(_other.mWorld.v1()), (* this), _other)) { return false; }
		if (CollisionShape::protOBBGetSAT( this->mWorld.v0().cross(_other.mWorld.v2()), (* this), _other)) { return false; }
			   																			
		if (CollisionShape::protOBBGetSAT( this->mWorld.v1().cross(_other.mWorld.v0()), (* this), _other)) { return false; }
		if (CollisionShape::protOBBGetSAT( this->mWorld.v1().cross(_other.mWorld.v1()), (* this), _other)) { return false; }
		if (CollisionShape::protOBBGetSAT( this->mWorld.v1().cross(_other.mWorld.v2()), (* this), _other)) { return false; }
			   																			
		if (CollisionShape::protOBBGetSAT( this->mWorld.v2().cross(_other.mWorld.v0()), (* this), _other)) { return false; }
		if (CollisionShape::protOBBGetSAT( this->mWorld.v2().cross(_other.mWorld.v1()), (* this), _other)) { return false; }
		if (CollisionShape::protOBBGetSAT( this->mWorld.v2().cross(_other.mWorld.v2()), (* this), _other)) { return false; }
		
		// Nothing else failed, so we're good
		return true;
	}

	bool OBB::protIntersectBSphere(const BSphere & _other) const
	{
		return CollisionShape::BSphereOBB(_other, (* this));
	}
	bool OBB::protIntersectAABB   (const AABB    & _other) const
	{
		return CollisionShape::AABBOBB(_other, (* this));
	}
	bool OBB::protIntersectPlane  (const Plane   & _other) const
	{
		return CollisionShape::OBBPlane((* this), _other);
	}
	bool OBB::protIntersectRay    (const Ray     & _other) const
	{
		return CollisionShape::OBBRay((* this), _other);
	}
	bool OBB::protIntersectLine   (const Line    & _other) const
	{
		return CollisionShape::OBBLine((* this), _other);
	}

	HitResult OBB::Raycast(const Ray & _raycast) const
	{
		// Use the Slab Method (by Tavian) to calculate the intersection

		const Mat4 worldInv = this->mWorld.getInv();

		const Vec4 rayDir = (Vec4(_raycast.GetDirection(), 0.0f) * worldInv).getNorm();
		const Vec4 rayOrigin = (Vec4(_raycast.GetOrigin(), 1.0f) * worldInv);

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
		if (tmax >= tmin)
		{
			// The ray is in local, convert it back to world
			const Vec3 location = Vec3((rayOrigin + (rayDir * tmin)) * this->mWorld);

			return HitResult(true, location, (location - _raycast.GetOrigin()).len());
		}

		return HitResult();
	}
}