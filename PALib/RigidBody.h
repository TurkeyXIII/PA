#pragma once

class CRigidBody : public CBody
{
public:
	CRigidBody(void);
	~CRigidBody(void);

	bool			Contains(const PAVECTOR* pPoint);

	void			OnCollision(const PAVECTOR* pLocation, const PAVECTOR* pForce);
	void			Update(TimeMS now);

	// This makes the body 'take ownership' of the hitbox: the IHitbox* is made a Ptr<IHitbox>.
	void			AddHitbox(IHitbox* ph);

	void			SetMaterial(const CMaterial &mat);

	float			GetMomentOfInertia(const PAVECTOR* pAxis);

	float			GetDensity(void) const;
	float			GetYoungsModulus(void) const;

private:
	std::list<Ptr<IHitbox>>		m_lHitbox;

	CMaterial					m_mat;
};