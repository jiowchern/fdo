#pragma once
#include "ImageControls.h"

class CTalkBarImageControl : public CDXUTImage
{
public:
    bool m_bLocked;
public:
    CTalkBarImageControl( CDXUTDialog *pDialog = NULL );
    // Overrides
    bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
};
