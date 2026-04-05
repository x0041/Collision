#ifndef HIT_RESULT_H
#define HIT_RESULT_H

#include "Vec3.h"

namespace Azul
{
	class HitResult
	{
	public:
		

		HitResult();
		HitResult(const HitResult & _other);
		HitResult & operator = (const HitResult & _other);

		HitResult(const bool _hit, const Vec3 _hitPos, const float _length);

		~HitResult();

		inline const bool GetHit() const
		{
			return this->mHit;
		}
		inline const Vec3 & GetHitPos() const
		{
			return this->mHitPos;
		}
		inline const float GetLength() const
		{
			return this->mLength;
		}

		bool operator == (const HitResult & _other) const;
		bool operator != (const HitResult & _other) const;
		bool operator <  (const HitResult & _other) const;
		bool operator >  (const HitResult & _other) const;
		bool operator <= (const HitResult & _other) const;
		bool operator >= (const HitResult & _other) const;

		explicit operator bool() const
		{
			return this->mHit;
		}

	private:

		bool  mHit;
		Vec3  mHitPos;
		float mLength;
	};
}

#endif