#include "stdafx.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#include "PhenixD3D9Driver.h"
#include "PhenixD3D9VideoModeList.h"
#include "PhenixD3D9VideoMode.h"
//#include "PhenixAssert.h"

namespace FDO
{
    static unsigned int driverCount = 0;

	D3D9Driver::D3D9Driver()
	{
		tempNo = ++driverCount;
		mpD3D = NULL;
		ZeroMemory( &mAdapterIdentifier, sizeof(mAdapterIdentifier) );
		ZeroMemory( &mDesktopDisplayMode, sizeof(mDesktopDisplayMode) );
		mpVideoModeList = NULL;
	}

	D3D9Driver::D3D9Driver( const D3D9Driver &ob )
	{
		tempNo = ++driverCount;
		mpD3D = ob.mpD3D;
		mAdapterNumber = ob.mAdapterNumber;
		mAdapterIdentifier = ob.mAdapterIdentifier;
		mDesktopDisplayMode = ob.mDesktopDisplayMode;
		mpVideoModeList = NULL;
	}

	D3D9Driver::D3D9Driver( LPDIRECT3D9 pD3D, unsigned int adapterNumber, D3DADAPTER_IDENTIFIER9 adapterIdentifier, D3DDISPLAYMODE desktopDisplayMode )
	{
		tempNo = ++driverCount;
		mpD3D = pD3D;
		mAdapterNumber = adapterNumber;
		mAdapterIdentifier = adapterIdentifier;
		mDesktopDisplayMode = desktopDisplayMode;
		mpVideoModeList = NULL;
	}

	D3D9Driver::~D3D9Driver()
	{
        if(mpVideoModeList)
        {
            delete mpVideoModeList;
            mpVideoModeList = NULL;
        }
		--driverCount;
	}

	std::string D3D9Driver::DriverName() const
	{
		return std::string(mAdapterIdentifier.Driver);
	}

	std::string D3D9Driver::DriverDescription() const
	{
        std::string driverDescription(mAdapterIdentifier.Description);

        static const std::string delims = " \t\r";
        driverDescription.erase(driverDescription.find_last_not_of(delims)+1); // trim right
        driverDescription.erase(0, driverDescription.find_first_not_of(delims)); // trim left

        return  driverDescription;
	}

	D3D9VideoModeList* D3D9Driver::getVideoModeList()
	{
		if( !mpVideoModeList )
        {
			mpVideoModeList = new D3D9VideoModeList( this );
           // Assert( mpVideoModeList != 0, "Call new failed !!" );
        }

		return mpVideoModeList;
	}
}