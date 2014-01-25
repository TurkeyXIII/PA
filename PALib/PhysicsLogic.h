#pragma once

class CPhysicsLogic : public IPhysicsLogic
{
public:
	CPhysicsLogic(void);
	~CPhysicsLogic(void);

	void	DetectCollisions(void);

	void	AddBody(IBody* pBody);

private:
	std::list<Ptr<IBody>> m_lpBody;
};