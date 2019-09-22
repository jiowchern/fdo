#ifndef _CDXUTDisplayRole_20081017am1123_
#define _CDXUTDisplayRole_20081017am1123_
#include "CDXUTControl.h"
namespace FDO
{
	/************************************************************************/
	/*  2008 10 17 AM 1140
		¥ý°±¤u...*/
	/************************************************************************/
	class CAvatarBase;
	class CDXUTDisplayRole : public CDXUTControl
	{
		//CAvatar	mRole;
	public:
					CDXUTDisplayRole	();
					~CDXUTDisplayRole	();
		
		virtual void Render				( IDirect3DDevice9* pd3dDevice, float fElapsedTime ) ;
	};
}
#endif