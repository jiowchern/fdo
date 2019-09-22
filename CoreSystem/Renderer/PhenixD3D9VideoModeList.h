//---------------------------------------------------------------------------
#ifndef __PhenixD3D9VIDEOMODELIST_H__
#define __PhenixD3D9VIDEOMODELIST_H__

#include "PhenixD3D9Driver.h"
#include "PhenixD3D9VideoMode.h"
#include <vector>

namespace FDO 
{
    typedef std::vector<D3D9VideoMode> D3D9VideoModeArray;

	class D3D9VideoModeList
	{
	private:
		D3D9Driver* mpDriver;
		D3D9VideoModeArray mModeList;

	public:
		D3D9VideoModeList( D3D9Driver* pDriver );
		~D3D9VideoModeList();

		BOOL enumerate();

		D3D9VideoMode* item( size_t index );
		size_t count();

		D3D9VideoMode* item( const std::string &name );
	};
}
#endif