#include "pch.h"
#include "PAtoDX.h"

D3DXVECTOR3	DXVector(const PAVECTOR& v)
{
	D3DXVECTOR3 dxVec(v.x, v.y, v.z);
	return dxVec;
}

PAVECTOR	PAVector(const D3DXVECTOR3& v)
{
	PAVECTOR paVec(v.x, v.y, v.z);
	return paVec;
}

D3DXVECTOR3	operator-(const PAVECTOR& lhs, const D3DXVECTOR3& rhs)
{
	return D3DXVECTOR3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

D3DXVECTOR3	operator-(const D3DXVECTOR3& lhs, const PAVECTOR& rhs)
{
	return D3DXVECTOR3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

D3DXVECTOR3	operator+(const PAVECTOR& lhs, const D3DXVECTOR3& rhs)
{
	return D3DXVECTOR3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

D3DXVECTOR3	operator+(const D3DXVECTOR3& lhs, const PAVECTOR& rhs)
{
	return D3DXVECTOR3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}