#include "pch.h"
#include "common.h"
#include "PhysicsLogic.h"

using namespace std;

CPhysicsLogic::CPhysicsLogic() :
	m_lastUpdate(0)
{
}

CPhysicsLogic::~CPhysicsLogic()
{
}

void	CPhysicsLogic::DetectCollisions()
{
	//not even sure if this is a thing
}

void	CPhysicsLogic::AddBody(IBody* pBody)
{
	m_lpBody.push_front(pBody);
}

void	CPhysicsLogic::Update(TimeMS now)
{
	for (list<Ptr<IBody>>::iterator it = m_lpBody.begin(); it != m_lpBody.end(); it++)
	{
		(*it)->Update(now);
	}

	m_lastUpdate = now;
}

void	CPhysicsLogic::Increment(TimeMS difference)
{
	Update(difference + m_lastUpdate);
}