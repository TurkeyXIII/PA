//header file for inclusion in other projects
//everything in here should be either purely virtual, or entirely implemented in this file
//there must not be any third-party headers included in this file

#pragma once
#include <cmath>

#ifndef NULL
#define NULL 0
#endif //ifndef NULL

typedef int TimeMS;

/////////////////////////////////////////////////
//
// PAVECTOR Arithmetic
//
/////////////////////////////////////////////////

struct PAVECTOR
{
	PAVECTOR() {}
	PAVECTOR(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	float x, y, z;
};

static inline PAVECTOR operator+(const PAVECTOR &lhs, const PAVECTOR &rhs)
{
	return PAVECTOR(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

static inline PAVECTOR operator-(const PAVECTOR &v)
{
	return PAVECTOR(-v.x, -v.y, -v.z);
}

static inline PAVECTOR operator-(const PAVECTOR &lhs, const PAVECTOR &rhs)
{
	return (lhs + -rhs);
}

static inline bool operator==(const PAVECTOR &lhs, const PAVECTOR &rhs)
{
	PAVECTOR diff = lhs - rhs;
	float diffmag = sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
	return diffmag < 0.0001f;
}

static inline bool operator!=(const PAVECTOR &lhs, const PAVECTOR &rhs)
{
	return !(lhs == rhs);
}

//standard libraries
#include <fstream>

/////////////////////////////////////////////////
//
// base class for reference counting
//
/////////////////////////////////////////////////
class IObject
{
private:
	unsigned long	m_count;
public:
	IObject(void) : m_count(0) {}
	virtual ~IObject() {}

	unsigned long	AddRef(void)
	{
		return ++m_count;
	}

	unsigned long	Release(void)
	{
		m_count--;
		if (!m_count) 
		{
			delete this;
			return 0;
		}
		return m_count;
	}

	unsigned long	GetCount(void) 
	{
		return m_count;
	}
};

template <class Type>
class Ptr
{
public:
	Ptr(void)
	{
		m_pt = NULL;
	}

	Ptr(Type* pt)
	{
		m_pt = pt;

		if (m_pt)
			m_pt->AddRef();
	}

	Ptr(const Ptr<Type>& pt)
	{
		m_pt = pt.m_pt;
		m_pt->AddRef();
	}

	~Ptr(void)
	{
		if (m_pt) m_pt->Release();
	}

	Type* operator->()
	{
		if (!m_pt)
			OutputDebugStringA("Warning: -> operator returning NULL\n");
		return m_pt;
	}

	Ptr<Type>& operator=(Ptr<Type> pt)
	{
		if (m_pt) m_pt->Release();
		m_pt = pt->m_pt;
		if (m_pt) m_pt->AddRef();
		return *this;
	}
	
	Ptr<Type>& operator=(Type* pt)
	{
		if (m_pt) m_pt->Release();
		m_pt = pt;
		if (m_pt) pt->AddRef();
		return *this;
	}
	
	operator Type*()
	{
		return m_pt;
	}

	operator bool()
	{
		if (m_pt) return true;
		return false;
	}
	
private:
	Type* m_pt;
};

/////////////////////////////////////////////////
//
// Physics Engine objects
//
/////////////////////////////////////////////////

// interface for anything that needs to be included in the physics engine's hit detection
class ICollidable : public IObject
{
public:

};

class IHitbox : public ICollidable
{
public:
	virtual float		GetMomentOfInertia(const PAVECTOR* pAxis) = 0;

	virtual bool		Contains(const PAVECTOR* pPoint) = 0;
};

class IBody : public IObject
{
public:
	virtual bool		Contains(const PAVECTOR* point) = 0;
	virtual float		GetDensity(void) const = 0;
	virtual float		GetYoungsModulus(void) const = 0;

	virtual void		SetPosition(const PAVECTOR& pos) = 0;
	virtual void		AdjustRoll(float radianAngle) = 0;
	virtual void		AdjustPitch(float radianAngle) = 0;
	virtual void		AdjustYaw(float radianAngle) = 0;

	virtual PAVECTOR	GetForwardPAVector(void) const = 0;
	virtual PAVECTOR	GetUpPAVector(void) const = 0;

	virtual void		SetVelocity(const PAVECTOR& v) = 0;
	virtual void		AdjustVelocity(const PAVECTOR& v) = 0;

	virtual PAVECTOR	GetVelocity(void) const = 0;
	virtual PAVECTOR	GetPosition(void) const = 0;

	virtual void		AdjustRollVelocity(float radianAngle) = 0;
	virtual void		AdjustPitchVelocity(float radianAngle) = 0;
	virtual void		AdjustYawVelocity(float radianAngle) = 0;
	virtual void		AdjustAxisVelocity(PAVECTOR& axis, float radianAngle) = 0;

	virtual void		StopRotation(void) = 0;

	virtual void		OnCollision(const PAVECTOR* location, const PAVECTOR* force) = 0; //maybe 'force' is wrong here as it could vary over the duration of the collision.  Maybe I need a 'collision' object?
	virtual void		Update(TimeMS now) = 0;

	virtual void		AddHitbox(IHitbox* ph) = 0;
};

class ISurface : public ICollidable
{

};

class IPhysicsLogic : public IObject
{
public:
	virtual void		DetectCollisions(void) = 0;
	virtual void		AddBody(IBody* pBody) = 0;

	virtual void		Update(TimeMS now) = 0;
	virtual void		Increment(TimeMS difference) = 0;
};


/////////////////////////////////////////////////
//
// Graphics engine objects
//
/////////////////////////////////////////////////
class IDrawable : public IObject
{

};

class IMesh : public IObject
{
};

class IGraphics : public IObject
{
public:
	virtual void		AddDrawable(IDrawable* drawable) = 0;
	virtual void		DrawScene(void) = 0;
};

/////////////////////////////////////////////////
//
// Factory interfaces.
// These need a concrete class visible to the user
//
/////////////////////////////////////////////////

class IDrawableFactory : public IObject
{
public:
	virtual IDrawable*	Create(void) = 0;
};

class IPhysicsLogicFactory : public IObject
{
public:
	virtual Ptr<IPhysicsLogic> Create(void) = 0;
};

class IBodyFactory : public IObject
{
public:
	virtual Ptr<IBody> Create(const std::string* filename) = 0;
};

#include "IndustrialPark.h"