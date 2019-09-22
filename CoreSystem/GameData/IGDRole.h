#ifndef _IGDRole_20081211am1119_
#define _IGDRole_20081211am1119_
#include "GameDatabase.h"
namespace FDO
{
	struct IGDRole : public IGameData
	{
		DECLARE_GD_EVENTLISTENER(IGDRole,IEventListener);

		enum
		{
			// param = const D3DXVECTOR3*
			EVENT_CHANGE_POSITION,
			EVENT_COUNT
		};
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	

}
#endif