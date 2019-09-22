//--------------------------------------------------------------------------------------
// File: CDXUTgui.h
//
// Desc: 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef DXUT_GUI_H
#define DXUT_GUI_H

/*
#include <usp10.h>
#include <dimm.h>
// ming add >>
#include <list>
#include <string>
#include <d3d9.h>
#include <d3dx9core.h>
*/
#include "CDXUTDialog.h"
#include "CDXUTControl.h"
#include "CDXUTListBox.h"
#include "CDXUTComboBox.h"
#include "CDXUTStatic.h"
#include "CDXUTButton.h"
#include "CDXUTEditBox.h"
#include "CDXUTDialogResourceManager.h"
#include "SmoothScrollingListBox.h"

using namespace std;

//-----------------------------------------------------------------------------
// Slider control
//-----------------------------------------------------------------------------
class CDXUTSlider : public CDXUTControl
{
public:
    CDXUTSlider( CDXUTDialog *pDialog = NULL );

    virtual BOOL ContainsPoint( POINT pt ); 
    virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    
    virtual void UpdateRects(); 

    virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );

    void SetValue( int nValue ) { SetValueInternal( nValue, false ); }
    int  GetValue() const { return m_nValue; };

    void GetRange( int &nMin, int &nMax ) const { nMin = m_nMin; nMax = m_nMax; }
    void SetRange( int nMin, int nMax );

protected:
    void SetValueInternal( int nValue, bool bFromInput );
    int  ValueFromPos( int x ); 

    int m_nValue;

    int m_nMin;
    int m_nMax;

    int m_nDragX;      // Mouse position at start of drag
    int m_nDragOffset; // Drag offset from the center of the button
    int m_nButtonX;

    bool m_bPressed;
    RECT m_rcButton;
};

//-----------------------------------------------------------------------------
// ProgressBar control 未完成
//-----------------------------------------------------------------------------
class CDXUTProgressBar : public CDXUTControl
{
public:
    CDXUTProgressBar( CDXUTDialog *pDialog = NULL );
    ~CDXUTProgressBar();
    
    virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );

    void SetMAX( const int &MAX ) { this->MAX = MAX; }
	int  GetMAX() { return this->MAX; }
	bool IsMAX() { return this->Value >= this->MAX ; }
	void AddValue( int rateValue ) { this->Value += rateValue; }
    void SetValue( const int &Value ) { this->Value = Value; }
	int  GetValue() { return this->Value; }

    // 設定背景材質
    void SetBackgroundTexture( const string &FileName, RECT* prcTexture=0 )
        { CDXUTControl::SetTexture( FileName, prcTexture ); }
    void SetForwardTexture( const string &FileName, RECT* prcTexture=0 )
        { CDXUTControl::SetTexture( FileName, prcTexture, 1 ); }

protected:

    int Value;
    int MAX;

    D3DCOLOR m_colorTopLeft;
    D3DCOLOR m_colorTopRight;
    D3DCOLOR m_colorBottomLeft;
    D3DCOLOR m_colorBottomRight;
};

//-----------------------------------------------------------------------------
class moCDXUTBtnListBox : public CDXUTListBox
{
    public:
    moCDXUTBtnListBox( CDXUTDialog *pDialog = NULL );
    virtual ~moCDXUTBtnListBox();

    CDXUTButton *pBtn[5];

    bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );

    virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
};

//-----------------------------------------------------------------------------
// CDXUTSpecialButton
//-----------------------------------------------------------------------------
class CDXUTSpecialButton : public CDXUTControl
{
public:
    CDXUTSpecialButton( CDXUTDialog *pDialog = NULL );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual BOOL ContainsPoint( POINT pt ) { return PtInRect( &m_rcBoundingBox, pt ); }
};
//-----------------------------------------------------------------------------

class CDXUTSlot : public CDXUTControl
{
	CDXUTControl*	mpControl;
public:
	CDXUTSlot(CDXUTDialog *pDialog = NULL);
	~CDXUTSlot();
	virtual HRESULT OnInit() ;
	virtual void Refresh();
	virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime ) ;
	virtual void RenderHint( IDirect3DDevice9* pd3dDevice, float fElapsedTime ) ;

	// Windows message handler
	virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam ) ;

	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam ) ;
	virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam ) ;

	virtual bool CanHaveFocus() ;
	virtual void OnFocusIn() ;
	virtual void OnFocusOut() ;
	virtual void OnMouseEnter() ;
	virtual void OnMouseLeave() ;
	virtual void OnHotkey() ;

	virtual BOOL ContainsPoint( POINT pt ) ;

	virtual void SetEnabled( bool bEnabled ) ;
	virtual bool GetEnabled() ;
	virtual void SetVisible( bool bVisible ) ;
	virtual bool GetVisible() ;

	virtual void SetHint_WordMaxWidth( int iWidth );
	virtual void SetHintFrameColor( D3DCOLOR color );
	virtual void SetHintTextColor( D3DCOLOR color );
	virtual void SetHintWordFormat( DWORD dwTextFormat );
	virtual void SetHintTurnRight( bool bTrue = true );
		
protected:
	virtual void UpdateRects();

};



#endif // DXUT_GUI_H