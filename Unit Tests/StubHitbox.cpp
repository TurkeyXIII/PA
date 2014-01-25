#include "stdafx.h"
#include "palib.h"

#include "StubHitbox.h"

StubHitbox::StubHitbox()
{
}

StubHitbox::~StubHitbox()
{
}

float	StubHitbox::GetMomentOfInertia(const PAVECTOR* axis)
{
	return 1.0f;
}

bool	StubHitbox::Contains(const PAVECTOR* pPoint)
{
	if (pPoint->x == 45.0f
		&& pPoint->y == 12.5f
		&& pPoint->z == 923.0f) return true;

	return false;
}