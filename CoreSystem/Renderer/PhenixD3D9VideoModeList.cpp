#include "stdafx.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#include "PhenixD3D9VideoModeList.h"

namespace FDO
{
	D3D9VideoModeList::D3D9VideoModeList( D3D9Driver* pDriver )
	{      
		mpDriver = pDriver;
		enumerate();
	}

	D3D9VideoModeList::~D3D9VideoModeList()
	{
		mpDriver = NULL;
		mModeList.clear();
	}

	BOOL D3D9VideoModeList::enumerate()
	{
        //可列舉的模式
        const D3DFORMAT allowedFormat[] =                
        {
            D3DFMT_R5G6B5,
            D3DFMT_X8R8G8B8,
            /*
            D3DFMT_A8R8G8B8,
            D3DFMT_A2R10G10B10,
            D3DFMT_A1R5G5B5,
            D3DFMT_X1R5G5B5
            */
        };
        const UINT iFormatCount = sizeof(allowedFormat) / sizeof(allowedFormat[0]); 

		LPDIRECT3D9 pD3D = mpDriver->getD3D();
		UINT adapter = mpDriver->getAdapterNumber();

        for( UINT iFormat=0; iFormat<iFormatCount; ++iFormat )
        {
		    for( UINT iMode=0; iMode<pD3D->GetAdapterModeCount(adapter, allowedFormat[iFormat]); ++iMode )
		    {
			    D3DDISPLAYMODE displayMode;
			    pD3D->EnumAdapterModes( adapter, allowedFormat[iFormat], iMode, &displayMode );

			    // Filter out low & up -resolutions
			    if( displayMode.Width < 640 || displayMode.Height < 480 )  
					continue;

			    // Check to see if it is already in the list (to filter out refresh rates)
			    BOOL found = false;
			    D3D9VideoModeArray::iterator end( mModeList.end() );
			    for( D3D9VideoModeArray::iterator it = mModeList.begin(); it != end; ++it )
			    {
				    D3DDISPLAYMODE oldDisp = it->getDisplayMode();
				    if( oldDisp.Width == displayMode.Width &&
					    oldDisp.Height == displayMode.Height &&
					    oldDisp.Format == displayMode.Format )
				    {
					    // Check refresh rate and favour higher if poss
					    if (oldDisp.RefreshRate < displayMode.RefreshRate)
						    it->increaseRefreshRate(displayMode.RefreshRate);
					    found = true;
					    break;
				    }
			    }

			    if( !found )
				    mModeList.push_back( D3D9VideoMode( displayMode ) );
		    }
        }
		return TRUE;
	}

	size_t D3D9VideoModeList::count()
	{
		return mModeList.size();
	}

	D3D9VideoMode* D3D9VideoModeList::item( size_t index )
	{
		return &mModeList.at( index );
	}

	D3D9VideoMode* D3D9VideoModeList::item( const std::string &name )
	{
		D3D9VideoModeArray::iterator end( mModeList.end() );

		for(D3D9VideoModeArray::iterator it = mModeList.begin(); it != end; ++it)
		{
			if (it->getDescription() == name)
				return &(*it);
		}

		return NULL;
	}
}
