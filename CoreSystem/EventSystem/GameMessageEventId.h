#ifndef _GameMessageEventId_20090612pm0157_
#define _GameMessageEventId_20090612pm0157_
#include "GameMessageEventBase.h"
namespace GameEvent
{
	enum
	{
		DEF_SECTION0_BEGIN,
		
		// 再這裡添加新的事件定義...	
		ROLE_SHOW,	// GERole_Show_Hide
		ROLE_HIDE,	// GERole_Show_Hide
		NPC_ROLE_SHOW,	// GERole_Show_Hide
		NPC_ROLE_HIDE,	// GERole_Show_Hide
		ROLE_BOUNDDINGBOX_SHOW,
		ROLE_BOUNDDINGBOX_HIDE,
		DEF_SECTION0_END,
	};

	
	
};
namespace FDO
{
	def_gameevent_param(GEParamObj,GEEmpty);
	////////////////////////////////////////////////////////////////////
	struct _RoleInScreen : public GEParamObj
	{
		_RoleInScreen( RoleID rid, bool Show ) : roleId(rid) , bShow(Show)
		{

		}
		RoleID	roleId;
		bool	bShow;
	};
	GAMEEVENT_DEFPARAM(_RoleInScreen) GERoleInScreen;
	///////////////////////////////////////////////////////////////////
};
#endif
