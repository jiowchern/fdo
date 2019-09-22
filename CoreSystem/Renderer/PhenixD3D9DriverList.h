//---------------------------------------------------------------------------
#ifndef __PhenixD3D9DRIVERLIST_H__
#define __PhenixD3D9DRIVERLIST_H__

#include "PhenixD3D9Driver.h"
#include <d3d9.h>
#include <vector>

namespace FDO 
{
    typedef std::vector<D3D9Driver> D3D9DriverArray;

	class D3D9DriverList
	{
	private:
		D3D9DriverArray mDriverList;
		LPDIRECT3D9 mpD3D;

	public:
		D3D9DriverList( LPDIRECT3D9 pD3D );
		~D3D9DriverList();

		BOOL enumerate();
		size_t count() const;
		D3D9Driver* item( size_t index );

		D3D9Driver* item( const std::string &name );
	};
}
#endif