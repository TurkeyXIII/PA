#pragma once

class CPhysicsLogic : public IPhysicsLogic
{
public:
	CPhysicsLogic(void);
	~CPhysicsLogic(void);

	void	DetectCollisions(void);

	void	AddBody(IBody* pBody);

	void	Update(TimeMS now);
	void	Increment(TimeMS difference);

private:
	std::list<Ptr<IBody>>	m_lpBody;

	TimeMS					m_lastUpdate;
};