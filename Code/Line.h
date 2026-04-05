#ifndef LINE_H
#define LINE_H

#include "CollisionShape.h"
#include "Vec3.h"

namespace Azul
{
	class Line : public CollisionShape
	{
	public:

		// The Big Four
		Line() = delete;
		Line & operator = (const Line & _other) = delete;

		Line(const Vec3 & _start, const Vec3 & _end);
		Line(const Line & _other);

		virtual ~Line();

		inline const Vec3 & GetStartPoint() const
		{
			return this->mOrigin;
		}
		inline const Vec3 GetEndPoint() const
		{
			return this->mOrigin + this->mOffset;
		}
		inline const Vec3 & GetOffset() const
		{
			return this->mOffset;
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

		const Vec3 mOrigin;
		const Vec3 mOffset;

	};
}

#endif