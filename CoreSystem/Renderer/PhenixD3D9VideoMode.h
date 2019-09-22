//---------------------------------------------------------------------------
#ifndef __PhenixD3D9VIDEOMODE_H__
#define __PhenixD3D9VIDEOMODE_H__

#include <d3d9.h>
#include <string>

namespace FDO 
{
	class D3D9VideoMode
	{
	private:
		D3DDISPLAYMODE mDisplayMode;
		unsigned int modeNumber;

	public:
		D3D9VideoMode();
		D3D9VideoMode( const D3D9VideoMode &ob );
		D3D9VideoMode( D3DDISPLAYMODE d3ddm );
		~D3D9VideoMode();

		unsigned int getWidth() const { return mDisplayMode.Width; }
		unsigned int getHeight() const { return mDisplayMode.Height; }
		D3DFORMAT getFormat() const { return mDisplayMode.Format; }
		unsigned int getRefreshRate() const { return mDisplayMode.RefreshRate; }
		unsigned int getColourDepth() const;
		D3DDISPLAYMODE getDisplayMode() const { return mDisplayMode; }
		void increaseRefreshRate(unsigned int rr) { mDisplayMode.RefreshRate = rr; } 
		std::string getDescription() const;
	};
}
#endif