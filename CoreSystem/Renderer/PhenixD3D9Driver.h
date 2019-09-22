//---------------------------------------------------------------------------
#ifndef __PhenixD3D9DRIVER_H__
#define __PhenixD3D9DRIVER_H__                                   

#include <d3dx9.h>
#include <string>

namespace FDO 
{          
	class D3D9VideoModeList;
	class D3D9VideoMode;

	class D3D9Driver
	{
	private:
		LPDIRECT3D9 mpD3D;
		unsigned int mAdapterNumber;
		D3DADAPTER_IDENTIFIER9 mAdapterIdentifier;
		D3DDISPLAYMODE mDesktopDisplayMode;
		D3D9VideoModeList* mpVideoModeList;
		unsigned int tempNo;

	public:
		// Constructors
		D3D9Driver();						// Default
		D3D9Driver( const D3D9Driver &ob );	// Copy
		D3D9Driver( LPDIRECT3D9 pD3D, unsigned int adapterNumber, D3DADAPTER_IDENTIFIER9 adapterIdentifer, D3DDISPLAYMODE desktopDisplayMode );
		~D3D9Driver();

		// Information accessors
		std::string DriverName() const;
		std::string DriverDescription() const;

		LPDIRECT3D9 getD3D() { return mpD3D; }
		unsigned int getAdapterNumber() const { return mAdapterNumber; }
		D3DADAPTER_IDENTIFIER9 getAdapterIdentifier() const { return mAdapterIdentifier; }
		D3DDISPLAYMODE getDesktopMode() const { return mDesktopDisplayMode; }

		D3D9VideoModeList* getVideoModeList();
	};
}
#endif