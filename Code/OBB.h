#ifndef OBB_H
#define OBB_H

#include "CollisionShape.h"
#include "Vec3.h"
#include "Rot.h"

namespace Azul
{
	class OBB : public CollisionShape
	{
	public:

		// The Big Four
		OBB() = delete;
		OBB & operator = (const OBB & _other) = delete;

		OBB(const Rot & _rotation, const Vec3 & _origin, const Vec3 & _halfBounds);
		OBB(const OBB & _other);

		OBB(const AABB & _other);

		virtual ~OBB();

		// Use double dispatch visitor pattern to handle different intersections
		virtual bool Intersect(const CollisionShape & _other) const override;
		virtual HitResult Raycast(const Ray & _raycast) const override;

		inline const Vec4 & GetHalfBounds() const
		{
			return this-> mHalfBounds;
		}
		inline const Mat4 & GetWorld() const
		{
			return this->mWorld;
		}
		inline const Vec4 & GetOrigin() const
		{
			return this->mOrigin;
		}

	protected:

		// Keep access to these hidden just to keep things streamlined in function calls
		virtual bool protIntersectBSphere(const BSphere & _other) const override;
		virtual bool protIntersectAABB	 (const AABB	& _other) const override;
		virtual bool protIntersectOBB	 (const OBB		& _other) const override;
		virtual bool protIntersectPlane	 (const Plane	& _other) const override;
		virtual bool protIntersectRay	 (const Ray		& _other) const override;
		virtual bool protIntersectLine	 (const Line	& _other) const override;

	private:

		// const Rot  mRotation;
		// const Vec3 mOrigin;
		// const Vec3 mHalfBounds;

		const Mat4 mWorld;
		const Vec4 mOrigin;
		const Vec4 mHalfBounds;

	};
}

#endif