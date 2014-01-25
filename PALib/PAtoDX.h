#pragma once

D3DXVECTOR3	DXVector(const PAVECTOR& v);

PAVECTOR	PAVector(const D3DXVECTOR3& v);

D3DXVECTOR3	operator-(const PAVECTOR& lhs, const D3DXVECTOR3& rhs);
D3DXVECTOR3	operator-(const D3DXVECTOR3& lhs, const PAVECTOR& rhs);
D3DXVECTOR3	operator+(const PAVECTOR& lhs, const D3DXVECTOR3& rhs);
D3DXVECTOR3	operator+(const D3DXVECTOR3& lhs, const PAVECTOR& rhs);