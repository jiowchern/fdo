#include "StdAfx.h"
#include "CDXUTDialog.h"
#include "ImageControls.h"
#include "CDXUTButton.h"
#include "TalkBarImageControl.h"
//---------------------------------------------------------------------------
// CTalkBarImageControl
//---------------------------------------------------------------------------

CTalkBarImageControl::CTalkBarImageControl(CDXUTDialog *pDialog) : CDXUTImage(pDialog)
{
    m_bLocked = false;    
}

bool CTalkBarImageControl::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    return CDXUTButton::HandleMouse(uMsg, pt, wParam, lParam);
}

void CTalkBarImageControl::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    if (m_bLocked)
        CDXUTImage::Render(pd3dDevice, fElapsedTime);
}

