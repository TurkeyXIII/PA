#pragma once

class Mesh;

class CDrawable : public IDrawable
{

public:
	CDrawable(LPDIRECT3DDEVICE9 pDxDevice);
	virtual ~CDrawable(void);

	virtual void Draw(void);

private:
	Mesh*	m_mesh;
};