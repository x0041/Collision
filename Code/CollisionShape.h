#ifndef COLLISION_SHAPE_H
#define COLLISION_SHAPE_H

#include "HitResult.h"

namespace Azul
{
	// Forward Declaration
	class BSphere;
	class AABB;
	class OBB;
	class Plane;
	class Ray;
	class Line;

	class CollisionShape
	{

	protected:
		
		// The Big Four

		// This class is abstract so it can't be instantiated on its own anyway, but may as well make these protected
		CollisionShape()										= default;
		CollisionShape(const CollisionShape & _other)				= default;
		CollisionShape & operator = (const CollisionShape & _other) = default;

	public:
		
		virtual ~CollisionShape() = default;

		// Use double dispatch visitor pattern to handle different intersections
		virtual bool Intersect(const CollisionShape & _other) const = 0;

		// Only rays can raycast, so
		virtual HitResult Raycast(const Ray & _raycast) const = 0;

	protected:

		// I want these functions protected so they don't cloud the autofill... there's technically 
		// nothing wrong with calling them. But, to make these functions protected, the child classes need
		// to be friends so they can class the functions in the others; it doesn't break encapsulation, so
		// it should be fine, just need to remember to add it as a friend when adding a new intersect
		// function

		friend class BSphere;
		friend class AABB;
		friend class OBB;
		friend class Plane;
		friend class Ray;
		friend class Line;

		virtual bool protIntersectBSphere(const BSphere	& _other) const = 0;
		virtual bool protIntersectAABB	 (const AABB	& _other) const = 0;
		virtual bool protIntersectOBB	 (const OBB		& _other) const = 0;
		virtual bool protIntersectPlane	 (const Plane	& _other) const = 0;
		virtual bool protIntersectRay	 (const Ray		& _other) const = 0;
		virtual bool protIntersectLine	 (const Line	& _other) const = 0;

	protected:

		// Helper functions to avoid repeated code in child classes
		// 
		// Child class on same child class functions can go in the child class themselves
		// because it's not as if there's going to be two definitions of those kinds of functions
		//
		// Because we're in a parent class, these are static since the instance data is inaccessible from here anyway
		// You could make the argument for making the reverse of each function to make the order not matter in the child
		// classes, but for the sake of this project, it doesn't matter

		static bool BSphereAABB (const BSphere & _sphere, const AABB  & _aabb );
		static bool BSphereOBB  (const BSphere & _sphere, const OBB	  & _obb  );
		static bool BSpherePlane(const BSphere & _sphere, const Plane & _plane);
		static bool BSphereRay  (const BSphere & _sphere, const Ray	  & _ray  );
		static bool BSphereLine (const BSphere & _sphere, const Line  & _line );

		static bool AABBOBB	 (const AABB & _aabb, const OBB	  & _obb  );
		static bool AABBPlane(const AABB & _aabb, const Plane & _plane); 
		static bool AABBRay	 (const AABB & _aabb, const Ray   & _ray  );
		static bool AABBLine (const AABB & _aabb, const Line  & _line );

		static bool OBBPlane(const OBB & _obb, const Plane & _plane); 
		static bool OBBRay	(const OBB & _obb, const Ray   & _ray  );
		static bool OBBLine	(const OBB & _obb, const Line  & _line );

		static bool PlaneRay (const Plane & _plane, const Ray  & _ray  );
		static bool PlaneLine(const Plane & _plane, const Line & _line );
															    
		static bool RayLine(const Ray & _ray, const Line & _line );

		static bool	 protOBBGetSAT(const Vec4 & _axis, const OBB & _a, const OBB & _b);
		static float protOBBGetProj(const Vec4 & _axis, const OBB & _this, const float _mag);

	protected:

		static constexpr float epsilon = 0.000001f;

	};
}

#endif