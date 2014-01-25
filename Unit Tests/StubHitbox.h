#pragma once

class StubHitbox : public IHitbox
{
public:
	StubHitbox();
	~StubHitbox();

	float	GetMomentOfInertia(const PAVECTOR* pAxis);

	bool	Contains(const PAVECTOR* point);
};