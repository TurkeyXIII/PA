// Contains concrete class declarations for all factories visible to the user
// This file must not include third party libraries

#pragma once

#include "palib.h"


class CPhysicsLogicFactory : public IPhysicsLogicFactory
{
public:
	CPhysicsLogicFactory(void);
	~CPhysicsLogicFactory(void);

	Ptr<IPhysicsLogic> Create(void);
};

class CBodyFactory : public IBodyFactory
{
public:
	CBodyFactory(void);
	~CBodyFactory(void);

	Ptr<IBody>	Create(const std::string* filename);
private:
	bool		Load3DS(IBody* pBody, const char* filename);
};