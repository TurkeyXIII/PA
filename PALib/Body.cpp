#include "pch.h"
#include "common.h"
#include "Body.h"

CBody::CBody(void) :
	m_position(D3DXVECTOR3(0,0,0)),
	m_velocity(D3DXVECTOR3(0,0,0)),

	m_orientation(D3DXQUATERNION(0,0,0,1)),
	m_angularVelocity(D3DXQUATERNION(0,0,0,1)),

	m_lastUpdate(0)
{
}

void	CBody::SetPosition(const PAVECTOR& pos)
{
	m_position = DXVector(pos);
}

void	CBody::AdjustRoll(float radianAngle)
{
	D3DXQUATERNION roll;
	D3DXQuaternionRotationAxis(&roll, &GetForward(), -radianAngle);

	//m_orientation *= roll;
	D3DXQUATERNION orientationTemp = m_orientation;
	D3DXQuaternionMultiply(&m_orientation, &roll, &orientationTemp);


	D3DXVECTOR3 fwd, up;
	fwd = GetForward();
	up = GetUp();
	debugf("fwd: (%.0f, %.0f, %.0f)\tup: (%.0f, %.0f, %.0f)\n", fwd.x, fwd.y, fwd.z, up.x, up.y, up.z);
}

void	CBody::AdjustPitch(float radianAngle)
{
	D3DXQUATERNION pitch;
	D3DXVECTOR3 right = RotateVector(D3DXVECTOR3(1.0f, 0.0f, 0.0f));
	debugf("right: (%.0f, %.0f, %.0f)\n", right.x, right.y, right.z);
	D3DXQuaternionRotationAxis(&pitch, &right, -radianAngle);

	//m_orientation *= pitch;
	D3DXQUATERNION orientationTemp = m_orientation;
	D3DXQuaternionMultiply(&m_orientation, &pitch, &orientationTemp);

	D3DXVECTOR3 fwd, up;
	fwd = GetForward();
	up = GetUp();
	debugf("fwd: (%.0f, %.0f, %.0f)\tup: (%.0f, %.0f, %.0f)\n", fwd.x, fwd.y, fwd.z, up.x, up.y, up.z);
}

void	CBody::AdjustYaw(float radianAngle)
{
	D3DXQUATERNION yaw;
	D3DXQuaternionRotationAxis(&yaw, &GetUp(), -radianAngle);

	//m_orientation *= yaw;
	D3DXQUATERNION orientationTemp = m_orientation;
	D3DXQuaternionMultiply(&m_orientation, &yaw, &orientationTemp);

	D3DXVECTOR3 fwd, up;
	fwd = GetForward();
	up = GetUp();
	debugf("fwd: (%.0f, %.0f, %.0f)\tup: (%.0f, %.0f, %.0f)\n", fwd.x, fwd.y, fwd.z, up.x, up.y, up.z);
}

D3DXVECTOR3	CBody::GetForward(void) const
{
	D3DXVECTOR3 fwd(0.0f, 1.0f, 0.0f);

	return RotateVector(fwd);
}

D3DXVECTOR3	CBody::GetUp(void) const
{
	D3DXVECTOR3 up(0.0f, 0.0f, 1.0f);

	D3DXVECTOR3 rotatedUp = RotateVector(up);

	debugf("up: (%.5f, %.5f, %.5f)\n", rotatedUp.x, rotatedUp.y, rotatedUp.z);
	float rotatedUpLength = D3DXVec3Length(&rotatedUp);

	assert(rotatedUpLength > 0.999 && rotatedUpLength < 1.001);

	return rotatedUp;
}

PAVECTOR	CBody::GetForwardPAVector(void) const
{
	return PAVector(GetForward());
}

PAVECTOR	CBody::GetUpPAVector(void) const
{
	return PAVector(GetUp());
}

// used for finding the location of a part of the body in real coordinates
// to find the location of a real point in body coordinates, set inverse to true
D3DXVECTOR3	CBody::RotateVector(const D3DXVECTOR3& vec, bool inverse) const
{
	D3DXVECTOR3 returnVec;

	D3DXQUATERNION qPoint(vec.x, vec.y, vec.z, 0.0f);
	D3DXQUATERNION orientationConjugate;
	D3DXQuaternionConjugate(&orientationConjugate, &m_orientation);

	D3DXQUATERNION qFinalPoint;
	if (inverse)
	{
		qFinalPoint = orientationConjugate * qPoint * m_orientation;
	}
	else
	{
		qFinalPoint = m_orientation * qPoint * orientationConjugate;
	}

//	debugf("qFinalPoint x: %f\ty:%f\tz:%f\tw:%f\n", qFinalPoint.x, qFinalPoint.y, qFinalPoint.z, qFinalPoint.w);
	assert(qFinalPoint.w < 0.0001 && qFinalPoint.w > -0.0001);
	
	returnVec.x = qFinalPoint.x;
	returnVec.y = qFinalPoint.y;
	returnVec.z = qFinalPoint.z;

	return returnVec;
}

void		CBody::SetVelocity(const PAVECTOR& v)
{
	m_velocity = DXVector(v);
}

void		CBody::AdjustVelocity(const PAVECTOR& v)
{
	m_velocity += DXVector(v);
}

PAVECTOR	CBody::GetVelocity(void) const
{
	return PAVector(m_velocity);
}

PAVECTOR	CBody::GetPosition(void) const
{
	return PAVector(m_position);
}

void		CBody::AdjustRollVelocity(float radianAngle)
{
	D3DXQUATERNION roll;
	D3DXQuaternionRotationAxis(&roll, &GetForward(), -radianAngle);

	D3DXQUATERNION angularVelocityTemp = m_angularVelocity;
	D3DXQuaternionMultiply(&m_angularVelocity, &roll, &angularVelocityTemp);
}

void		CBody::AdjustPitchVelocity(float radianAngle)
{
	D3DXQUATERNION pitch;
	D3DXVECTOR3 right = RotateVector(D3DXVECTOR3(1.0f, 0.0f, 0.0f));
	D3DXQuaternionRotationAxis(&pitch, &right, -radianAngle);

	D3DXQUATERNION angularVelocityTemp = m_angularVelocity;
	D3DXQuaternionMultiply(&m_angularVelocity, &pitch, &angularVelocityTemp);
}

void		CBody::AdjustYawVelocity(float radianAngle)
{
	D3DXQUATERNION yaw;
	D3DXQuaternionRotationAxis(&yaw, &GetUp(), -radianAngle);

	D3DXQUATERNION angularVelocityTemp = m_angularVelocity;
	D3DXQuaternionMultiply(&m_angularVelocity, &yaw, &angularVelocityTemp);
}

void		CBody::StopRotation(void)
{
	m_angularVelocity = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f);
}