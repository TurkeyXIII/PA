#pragma once

class CBody : public IBody
{
protected:
	D3DXVECTOR3			m_position;
	D3DXVECTOR3			m_velocity;

	D3DXQUATERNION		m_orientation;
	D3DXQUATERNION		m_angularVelocity;

	float				m_mass;

	TimeMS				m_lastUpdate;

	D3DXVECTOR3			RotateVector(const D3DXVECTOR3& vec, bool inverse = false) const;
public:
	CBody(void);
	virtual ~CBody(void) {}

	virtual bool		Contains(const PAVECTOR* point) = 0;

	virtual float		GetDensity(void) const = 0;
	virtual float		GetYoungsModulus(void) const = 0;

	virtual D3DXVECTOR3	GetForward(void) const;
	virtual D3DXVECTOR3 GetUp(void) const;
	virtual PAVECTOR	GetForwardPAVector(void) const;
	virtual PAVECTOR	GetUpPAVector(void) const;

	virtual void		SetPosition(const PAVECTOR& pos);
	virtual void		AdjustRoll(float radianAngle);
	virtual void		AdjustPitch(float radianAngle);
	virtual void		AdjustYaw(float radianAngle);
	virtual void		AdjustAxisAngle(PAVECTOR& axis, float radianAngle);
	virtual void		AdjustAxisAngle(D3DXVECTOR3& axis, float radianAngle);

	virtual void		SetVelocity(const PAVECTOR& v);
	virtual void		AdjustVelocity(const PAVECTOR& v);

	virtual PAVECTOR	GetVelocity(void) const;
	virtual PAVECTOR	GetPosition(void) const;

	virtual void		AdjustRollVelocity(float radianAngle);
	virtual void		AdjustPitchVelocity(float radianAngle);
	virtual void		AdjustYawVelocity(float radianAngle);
	virtual void		AdjustAxisVelocity(PAVECTOR& axis, float radianAngle);
	virtual void		AdjustAxisVelocity(D3DXVECTOR3& axis, float radianAngle);

	virtual void		StopRotation(void);

	virtual void		OnCollision(const PAVECTOR* location, const PAVECTOR* force) = 0; //maybe 'force' is wrong here as it could vary over the duration of the collision.  Maybe I need a 'collision' object?
	virtual void		Update(TimeMS now) = 0;

	virtual float		GetMomentOfInertia(const PAVECTOR* axis) = 0;
};