#pragma once

class CGraphics : public IGraphics
{
public:
	CGraphics(HWND window);
	~CGraphics(void);

	void		AddDrawable(IDrawable* drawable);
	void		DrawScene(void);

private:
	std::list<IDrawable*>		m_lAllDrawables;

	LPDIRECT3DDEVICE9			m_pDxDevice;

	HWND						m_hWnd;
};