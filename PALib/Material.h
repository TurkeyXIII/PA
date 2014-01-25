#pragma once

class CMaterial : public IObject
{
public:
	void	SetDensity(float density)				{m_density = density;}
	void	SetYoungsModulus(float youngsModulus)	{m_youngsModulus = youngsModulus;}

	float	GetDensity(void) const					{return m_density;}
	float	GetYoungsModulus(void) const			{return m_youngsModulus;}
private:
	float	m_density;
	float	m_youngsModulus;
};