#include "stdafx.h"
#include "RoleBaseData.h"

namespace FDO
{
	RoleBaseData::RoleBaseData(const RoleID& rid ,PropertyPtr pBase ,PropertyPtr pStatus)
		: mpBase(pBase), mpStatus(pStatus) , mRoleId(rid)
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	RoleBaseData::~RoleBaseData()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	sint RoleBaseData::GetBaseProperty(uint nType)const 
	{
		if(mpBase)
			return mpBase->Get(nType);
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	const RoleID& RoleBaseData::GetClientID() const 
	{
		return mRoleId;
	}
};

