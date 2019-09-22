#include "StdAfx.h"
#include "RoleResourceManage.h"

/*
boost::shared_ptr<CRoleResourceManage<std::list<stRPC> > >& GetRPCManage(void)
{
	static boost::shared_ptr<CRoleResourceManage<std::list<stRPC> > >
		manage(new CRoleResourceManage<std::list<stRPC> >) ;

	return manage ;

}
*/

boost::shared_ptr<CRoleResourceManage< FDO::SkinRAT > >& GetRATManage(void)
{
	static boost::shared_ptr<CRoleResourceManage<FDO::SkinRAT > >
		manage(new CRoleResourceManage<FDO::SkinRAT> ) ;

	return manage ;

}

boost::shared_ptr<CRoleResourceManage< FDO::SkinRPC > >& GetRPCManage(void)
{
	static boost::shared_ptr<CRoleResourceManage<FDO::SkinRPC > >
		manage(new CRoleResourceManage<FDO::SkinRPC> ) ;

	return manage ;

}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
boost::shared_ptr<CRoleResourceManage< FDO::SRATFileRoot > >& GetRATFileManage(void)
{
	static boost::shared_ptr<CRoleResourceManage<FDO::SRATFileRoot > >
		manage(new CRoleResourceManage<FDO::SRATFileRoot> ) ;

	return manage ;

}

boost::shared_ptr<CRoleResourceManage< FDO::SRPCFileRoot > >& GetRPCFileManage(void)
{
	static boost::shared_ptr<CRoleResourceManage<FDO::SRPCFileRoot > >
		manage(new CRoleResourceManage<FDO::SRPCFileRoot> ) ;

	return manage ;

}

/*
boost::shared_ptr<CRoleResourceManage< FDO::sRoleActionList > >& GetRATManage(void)
{
	static boost::shared_ptr<CRoleResourceManage<FDO::sRoleActionList > >
		manage(new CRoleResourceManage<FDO::sRoleActionList >) ;

	return manage ;

}
*/