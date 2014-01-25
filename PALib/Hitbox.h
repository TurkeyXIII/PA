#pragma once

class CHitbox : public IHitbox
{
public:
	CHitbox(void);
	~CHitbox(void);

	// gets the locations of the center of mass if this hitbox is divided by the plane described by v1 and v2.
	// the values returned are for the part on the plane's normal side
	// args:
	// pvCOM (out)	location of the COM of the split relative to the COM of the whole hitbox (origin)
	// pv1 (in)		first vector in plane
	// pv2 (in)		second vector in plane
	// returns:
	// percentage of total hitbox contained within split section
	//float GetSplitSection(const D3DXVECTOR3* pvCOM, const D3DXVECTOR3* pv1, const D3DXVECTOR3* pv2);

	// Gets the moment of inertia about the described axis of rotation.
	// This is indep't of mass; IE for a sphere it would return (2/5)R^2, not (2/5)mR^2
	float	GetMomentOfInertia(const PAVECTOR* pAxis);

	bool	Contains(const PAVECTOR* pPoint);

	void	AddVert(const D3DXVECTOR3& pv);

//	int		GetNFaces();

private:
	class Face
	{
	public:
		Face() : m_valid(true) {}

		void	CalculateNormal();
		void	ReplaceAdjFace(std::list<Face>::iterator current, std::list<Face>::iterator replacement);
		bool	IsAdjTo(std::list<Face>::iterator test);

		D3DXVECTOR3*				m_apVert[3];
		std::list<Face>::iterator	m_aitAdjFace[3]; //stored as iterators for easy removal from m_lFace
		D3DXVECTOR3					m_normal;

		bool						m_valid;
	};

	bool	Contains(const D3DXVECTOR3* pPoint);
	void	FixFace(std::list<Face>::iterator faceIt);

	std::list<D3DXVECTOR3>	m_lBoundingVert;
	std::list<Face>			m_lFace;
};