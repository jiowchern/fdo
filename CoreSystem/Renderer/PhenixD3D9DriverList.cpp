#include "stdafx.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#include "PhenixD3D9DriverList.h"  

namespace FDO 
{
	D3D9DriverList::D3D9DriverList( LPDIRECT3D9 pD3D ) : mpD3D(pD3D)
	{
		enumerate();
	}

	D3D9DriverList::~D3D9DriverList(void)
	{
		mDriverList.clear();
	}

	BOOL D3D9DriverList::enumerate()
	{
		for( UINT iAdapter=0; iAdapter < mpD3D->GetAdapterCount(); ++iAdapter )
		{
			D3DADAPTER_IDENTIFIER9 adapterIdentifier;
			D3DDISPLAYMODE d3ddm;
			mpD3D->GetAdapterIdentifier( iAdapter, 0, &adapterIdentifier );
			mpD3D->GetAdapterDisplayMode( iAdapter, &d3ddm );

			mDriverList.push_back( D3D9Driver( mpD3D, iAdapter, adapterIdentifier, d3ddm ) );
		}       

		return true;
	}

	size_t D3D9DriverList::count() const 
	{
		return mDriverList.size();
	}

	D3D9Driver* D3D9DriverList::item( size_t index )
	{
		return &mDriverList.at( index );
	}

	D3D9Driver* D3D9DriverList::item( const std::string &name )
	{
		D3D9DriverArray::iterator end( mDriverList.end() );

		for(D3D9DriverArray::iterator it = mDriverList.begin(); it != end; ++it)
		{
			if (it->DriverDescription() == name)
				return &(*it);
		}

		return NULL;
	}
}
