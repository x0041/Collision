#ifndef BSPHERE_H
#define BSPHERE_H

#include "CollisionShape.h"
#include "Vec3.h"

namespace Azul
{
	class BSphere : public CollisionShape
	{
	public:

		// The Big Four
		BSphere() = delete;
		BSphere & operator = (const BSphere & _other) = delete;
		
		BSphere(const Vec3 & _origin, const float _radius);
		BSphere(const BSphere & _other);

		virtual ~BSphere();

		inline const Vec3 & GetOrigin() const
		{
			return this->mOrigin;
		}
		inline const float GetRadius() const
		{
			return this->mRadius;
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

		const Vec3  mOrigin;
		const float mRadius;

	};
}

#endif