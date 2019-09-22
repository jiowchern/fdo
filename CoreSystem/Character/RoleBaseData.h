#ifndef _RoleBaseData_20091230am1057_
#define _RoleBaseData_20091230am1057_

#include "GameMessageEvent.h"
#include "RoleProperty.h"
namespace FDO
{
	struct RoleID 
	{

		union
		{
			struct
			{
				uint32	nServerID;
				uint32	nType;
			};
			uint64	nID;	
		};
		RoleID	();

		RoleID	(uint t,uint i) ;

		RoleID	(uint t) ;

		RoleID	(const RoleID& k) ;

		virtual ~RoleID	() ;


		bool operator == (const RoleID& k)		const ;

		bool operator != (const RoleID& k)		const ;

		bool operator < (const RoleID& k)		const ;

		void operator =	(uint nId)	;

		void operator =	(const RoleID& nId)	;


	};

	struct RoleBaseData : public GameEvent::Target
	{
			RoleBaseData	(const RoleID& rid , PropertyPtr pBase ,PropertyPtr pStatus);
			~RoleBaseData	();

		sint			GetBaseProperty	(uint nType) const ; 
		const RoleID&	GetClientID		() const ;
	private:
		PropertyPtr	mpBase;
		PropertyPtr	mpStatus;
	protected:
		RoleID		mRoleId;
	};

}


#endif