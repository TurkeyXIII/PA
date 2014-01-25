#include "pch.h"
#include "common.h"
#include "RigidBody.h"

CRigidBody::CRigidBody(void)
{
	m_mass = 1.0f;
}

CRigidBody::~CRigidBody(void)
{

}

bool	CRigidBody::Contains(const PAVECTOR* pPoint)
{
	D3DXVECTOR3 localPoint = (*pPoint) - m_position;
	
	localPoint = RotateVector(localPoint, true);
	
	for (std::list<Ptr<IHitbox>>::iterator it = m_lHitbox.begin();
		it != m_lHitbox.end();
		it++)
	{
		if ((*it)->Contains(&PAVector(localPoint))) return true;
	}

	return false;
}

//first attempt at an implementation - use 'force' as an impulse, and change velocities instantaneously
void	CRigidBody::OnCollision(const PAVECTOR* pLocation, const PAVECTOR* pForce)
{
	D3DXVECTOR3 dxLocation = DXVector(*pLocation);
	D3DXVECTOR3 dxForce = DXVector(*pForce);

	m_velocity += dxForce / m_mass;

	// find the vector from the origin to the point through which the force is exerted
	D3DXVECTOR3 radius;
	
	// first find some multiplier A for which radius = location + A * force
	float A;
	float forceMagnitude = D3DXVec3Length(&dxForce);
	A = - D3DXVec3Dot(&dxLocation, &dxForce) / (forceMagnitude * forceMagnitude);

	radius = dxLocation + A * dxForce;

	if (D3DXVec3Length(&radius) < 0.001)
	{
		//the force is effectively acting directly through the COM, and is therefore purely translational
		return;
	}

	// now consider two sections of the mass, divided by the force. One is attmpting to turn one way, the other section attempts to turn the other way.
	// the plane separating the two sections is described by two vectors.  One is the force, the other is perpendicular to the force and the radius.
	D3DXVECTOR3 rotationAxis;
	D3DXVec3Cross(&rotationAxis, &radius, &dxForce);

	// section one: the smaller section
	/*
	D3DXVECTOR3 sec1COM;
	float sec1Size = m_hitbox.GetSplitSection(&sec1COM, &force, &rotationAxis);

	if (sec1Size < 0.0001) 
	{
		// the force is right on the edge of the object, and is purely rotational
	*/
		D3DXQUATERNION	change;

		D3DXQuaternionRotationAxis(&change, &rotationAxis, D3DXVec3Length(&rotationAxis));
		
		m_angularVelocity += change / (m_mass * GetMomentOfInertia(&PAVector(rotationAxis)));
		
		return;
		/*
	}
	
	{
		// a combination of translation and rotation
		D3DXVECTOR3		sec1Radius;

	}
	*/
	
}

void	CRigidBody::Update(TimeMS now) 
{
	m_position += m_velocity * ((float)(now - m_lastUpdate) / 1000.0f);

	D3DXQUATERNION orientationTemp;
	while (now - m_lastUpdate > 1000)
	{
		orientationTemp = m_orientation;
		D3DXQuaternionMultiply(&m_orientation, &m_angularVelocity, &orientationTemp);

		m_lastUpdate += 1000;
	}

	orientationTemp = m_orientation;
	D3DXQUATERNION orientationFull;
	D3DXQuaternionMultiply(&orientationFull, &m_angularVelocity, &orientationTemp);

	D3DXQuaternionSlerp(&m_orientation, &orientationTemp, &orientationFull, (float)(now - m_lastUpdate) / 1000.0f);

	m_lastUpdate = now;
}

void	CRigidBody::AddHitbox(IHitbox* ph)
{
	Ptr<IHitbox> pHitbox = ph;
	m_lHitbox.push_back(pHitbox);
}

void	CRigidBody::SetMaterial(const CMaterial &mat)
{
	m_mat = mat;
}

float	CRigidBody::GetMomentOfInertia(const PAVECTOR* pAxis)
{
	return 1.0f;
}

float	CRigidBody::GetDensity(void) const
{
	return m_mat.GetDensity();
}

float	CRigidBody::GetYoungsModulus(void) const
{
	return m_mat.GetYoungsModulus();
}