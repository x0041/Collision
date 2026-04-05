#ifndef AABB_H
#define AABB_H

#include "CollisionShape.h"
#include "Vec3.h"

namespace Azul
{
	class AABB : public CollisionShape
	{
	public:

		// The Big Four
		AABB() = delete;
		AABB & operator = (const AABB & _other) = delete;

		AABB(const Vec3 & _minBounds, const Vec3 & _maxBounds);
		AABB(const AABB & _other);

		virtual ~AABB();

		inline const Vec3 & GetMinBounds() const
		{
			return this->mMin;
		}
		inline const Vec3 & GetMaxBounds() const
		{
			return this->mMax;
		}

		// Use double dispatch visitor pattern to handle different intersections
		virtual bool Intersect(const CollisionShape & _other) const override;
		virtual HitResult Raycast(const Ray & _raycast) const override;

	protected:

		// Keep access to these hidden just to keep things streamlined in function calls
		virtual bool protIntersectBSphere(const BSphere & _other) const override;
		virtual bool protIntersectAABB	 (const AABB	& _other) const override;
		virtual bool protIntersectOBB	 (const OBB		& _other) const override;
		virtual bool protIntersectPlane	 (const Plane	& _other) const override;
		virtual bool protIntersectRay	 (const Ray		& _other) const override;
		virtual bool protIntersectLine	 (const Line	& _other) const override;

	private:

		const Vec3 mMin;
		const Vec3 mMax;

	};
}

#endif