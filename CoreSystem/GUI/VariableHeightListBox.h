//
// VariableHeightListBox.h
//
#ifndef __VariableHeightListBox_h__
#define __VariableHeightListBox_h__

#include "CDXUTListBox.h"

class CLineObject;

class CVariableHeightListBox : public CDXUTListBox
{
private:
    int m_iOldRenderingTrackPos;
    int m_cyScrollUp;
    int m_cyScrolled;
    bool m_bSmoothScroll;
    bool m_bAddItem;
    int m_nNewItems;
    bool m_bAlignBottom;
    DWORD m_dwDefaultColor;
	const static bool sm_bSmoothScroll;
private:
    void RenderListBox(IDirect3DDevice9* pd3dDevice, float fElapsedTime);
    void UpdateRenderingTrackPos();
    void DrawLineObject(CLineObject* pLineObject, CDXUTElement* pElement, RECT* pDestRect);
    void InAddItemDecideIfSmoothScrolling(int nNewItems);
public:
    CVariableHeightListBox(CDXUTDialog *pDialog = NULL);
    ~CVariableHeightListBox();
    void SetDefaultColor(DWORD dwColor);
    // CDXUTControl overridings
    void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
    // ListBox item operations (polymorphism)
    //HRESULT AddItem( const string &szText, D3DXCOLOR FontColor=BLACK, void *pData=0 );
	HRESULT AddItem( const wstring &strText, D3DXCOLOR FontColor=FDO::BLACK, void *pData=0 );
    HRESULT AddItem( const WCHAR *wszText, void *pData, D3DXCOLOR FontColor=FDO::BLACK );
    CLineObject& GetLastLineObject();
    // Alignment (text and inline-image)
    void SetAlignBottom(bool bAlign);

	//static void SetSmoothScrolling(bool bTrue){ sm_bSmoothScroll = bTrue; }
	static bool IsSmoothScrolling(){ return sm_bSmoothScroll; }
	DXUTListBoxItem *GetItem( int nIndex );
};

#endif // __VariableHeightListBox_h__