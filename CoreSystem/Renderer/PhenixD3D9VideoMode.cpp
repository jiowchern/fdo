#include "stdafx.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#include "PhenixD3D9VideoMode.h"
#include "stdio.h"

namespace FDO 
{
    static unsigned int modeCount = 0;

    D3D9VideoMode::D3D9VideoMode()
    {
        modeNumber = ++modeCount;
        ZeroMemory( &mDisplayMode, sizeof(D3DDISPLAYMODE) );
    }

    D3D9VideoMode::D3D9VideoMode( const D3D9VideoMode &ob )
    {
        modeNumber = ++modeCount;
        mDisplayMode = ob.mDisplayMode;
    }

    D3D9VideoMode::D3D9VideoMode( D3DDISPLAYMODE d3ddm )
    {
        modeNumber = ++modeCount;
        mDisplayMode = d3ddm;
    }

    D3D9VideoMode::~D3D9VideoMode()
    {
        --modeCount;
    }

	std::string D3D9VideoMode::getDescription() const
	{
		char tmp[128];
		unsigned int colourDepth = 16;
		if( mDisplayMode.Format == D3DFMT_X8R8G8B8 ||
			mDisplayMode.Format == D3DFMT_A8R8G8B8 ||
			mDisplayMode.Format == D3DFMT_R8G8B8 )
			colourDepth = 32;

		sprintf( tmp, "%d x %d x %d", mDisplayMode.Width, mDisplayMode.Height, colourDepth );
		return std::string(tmp);
	}

	unsigned int D3D9VideoMode::getColourDepth() const
	{
		unsigned int colourDepth = 16;
		if( mDisplayMode.Format == D3DFMT_X8R8G8B8 ||
			mDisplayMode.Format == D3DFMT_A8R8G8B8 ||
			mDisplayMode.Format == D3DFMT_R8G8B8 )
			colourDepth = 32;

		return colourDepth;
	}
}