#include "pch.h"
#include "common.h"
#include "PhysicsLogic.h"

CPhysicsLogic::CPhysicsLogic()
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