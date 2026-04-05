
#include "HitResult.h"

namespace Azul
{
	HitResult::HitResult() : mHit(false), mHitPos(Vec3(0.0f, 0.0f, 0.0f)), mLength(-1.0f) {}
	HitResult::HitResult(const HitResult & _other) : mHit(_other.mHit), mHitPos(_other.mHitPos), mLength(_other.mLength) {}
	HitResult::HitResult(const bool _hit, const Vec3 _hitPos, const float _length) : mHit(_hit), mHitPos(_hitPos), mLength(_length) {}
	HitResult::~HitResult() {}

	HitResult & HitResult::operator = (const HitResult & _other)
	{
		this->mHit = _other.mHit;
		this->mHitPos.set(_other.mHitPos);
		this->mLength = _other.mLength;

		return (* this);
	}

	bool HitResult::operator == (const HitResult & _other) const
	{
		return (this->mHit == _other.mHit && this->mLength == _other.mLength);
	}
	bool HitResult::operator != (const HitResult & _other) const
	{
		return (this->mHit != _other.mHit || this->mLength != _other.mLength);
	}
	bool HitResult::operator <  (const HitResult & _other) const
	{
		// If we hit nothing, we can't be less than the other one
		// If we hit something, our length needs to be less than the other one
		//		But if we didn't hit anything, their length is -1, so ignore that
		return this->mHit && (!_other.mHit || (_other.mHit && (this->mLength < _other.mLength)));
	}
	bool HitResult::operator >  (const HitResult & _other) const
	{
		// If we hit nothing, we must be greater than the other one
		// If they hit nothing and we hit something, we must be lesser than them
		// If we both hit something, compare our lengths
		
		return !this->mHit || (_other.mHit && (this->mLength > _other.mLength));
	}
	bool HitResult::operator <= (const HitResult & _other) const
	{
		return this->mHit && (!_other.mHit || (_other.mHit && (this->mLength <= _other.mLength)));
	}
	bool HitResult::operator >= (const HitResult & _other) const
	{
		return !this->mHit || (_other.mHit && (this->mLength >= _other.mLength));
	}
}