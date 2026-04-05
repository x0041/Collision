#ifndef RAY_H
#define RAY_H

#include "CollisionShape.h"
#include "Vec3.h"

#include "HitResult.h"

namespace Azul
{
	class Ray : public CollisionShape
	{
	public:

		// The Big Four
		Ray() = delete;
		Ray & operator = (const Ray & _other) = delete;

		Ray(const Vec3 & _origin, const Vec3 & _direction);
		Ray(const Ray & _other);

		virtual ~Ray();

		inline const Vec3 & GetOrigin() const
		{
			return this->mOrigin;
		}
		inline const Vec3 & GetDirection() const
		{
			return this->mDirection;
		}

		// Use double dispatch visitor pattern to handle different intersections
		virtual bool Intersect(const CollisionShape & _other) const override;
		virtual HitResult Raycast(const Ray & _raycast) const override;
		HitResult Raycast(const CollisionShape & _other) const;

	protected:

		// Keep access to these hidden just to keep things streamlined in function calls
		virtual bool protIntersectBSphere(const BSphere & _other) const override;
		virtual bool protIntersectAABB	 (const AABB	& _other) const override;
		virtual bool protIntersectOBB	 (const OBB		& _other) const override;
		virtual bool protIntersectPlane	 (const Plane	& _other) const override;
		virtual bool protIntersectRay	 (const Ray		& _other) const override;
		virtual bool protIntersectLine	 (const Line	& _other) const override;

	private:

		const Vec3 mOrigin;
		const Vec3 mDirection;

	};
}

#endif