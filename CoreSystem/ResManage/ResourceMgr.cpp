#include "stdafx.h"
#include "resourceMgr.h"


CResMgr& GetResMgr( void ) 
{
	static CResMgr mgr(GetCatch()) ;

	return mgr ;

}

void UpdateResMgrSize( int offset)
{
	GetResMgr().UpdateSize( offset) ;
}

////////////			資源資料相關：檢查快取是否有此資源(快取中)		///////////////////////
bool FindResInCatch(const std::string &filename) 
{

	std::map<std::string , Resmemory>::iterator iter ;

	iter =  GetCatch()->Find(filename )  ; 

	if (iter != GetCatch()->m_mempool.end() )
	{
		//有找到
		return true; 
	}

	//沒找到
	return false ;

}

bool CheckResource(const char *filename)
{
	return GetResMgr().CheckResource(filename) ; 
}

//
//template<>
//void ReleaseResource(const char *filename)
//{
//	ResObjlist::iterator iter ;
//
//
//	//只有移除動態Pool的資源
//	iter = GetResMgr().FindResForDyanmic(filename) ;
//
//	if ( iter == GetResMgr().m_resobjlist.end() )
//	{
//		//沒有找到的話，就跳出
//		return ;
//	}
//
//	/////找到的話
//	GetResMgr().ReleaseResource( iter ) ;
//}

//////////////			資源管理		//////////////////////

CResMgr::CResMgr( boost::shared_ptr<CCatchRes> catchres ) 
	: m_resobjmap() , m_staticres() , m_diray(new CDiary("Catch.txt")) , m_visitdiary(m_diray) , 
	m_resname() , m_size(0) , m_catchres(catchres) , m_count()
{
}
CResMgr::~CResMgr( void )
{
	m_resobjmap.clear() ;
	m_staticres.clear() ;
	m_resname.clear();
	m_count.clear() ;
}
void CResMgr::Show( void )
{
	assert( m_diray != NULL ) ;

	//顯示所有資源的情況(檔案大小，檔案名稱，資源讀取次數，檔案狀態)
	//m_diray->Show( ) ;		

}
unsigned int CResMgr::Size( void ) const 
{
	return m_size ; 
}

void CResMgr::DirectLockRes(const char *filename)
{
	std::set<std::string>::iterator iter ; 

	std::string name(filename) ;

	iter = m_resname.find(name) ; 

	if ( iter == m_resname.end())
	{
		//沒有找到的話，就插入
		m_resname.insert(filename) ;

	}
}

void CResMgr::LockRes(const char *filename)
{
//	std::vector<std::string>::iterator iter ;
	std::set<std::string>::iterator iter ; 

	std::string name(filename) ;

	iter = m_resname.find(name) ; 

	//利用std::equal_to比較是否名稱相等
	//iter = find_if(m_resname.begin() , m_resname.end() , 
	//	   boost::bind(std::equal_to<std::string>() , _1 , name)) ;

	if ( iter == m_resname.end())
	{
		//沒有找到的話，就插入
		//m_resname.push_back(filename) ;

		m_resname.insert(filename) ;


		ResObjmap::iterator objiter ;
		ResObjmap::iterator stiter ;


		//再由動態找起
		objiter = GetResMgr().FindResForDyanmic( filename) ;

		if(objiter != GetResMgr().m_resobjmap.end() )
		{
			//在動態Pool有找到的話，要將此資源丟入靜態！
			//預防過景時候，動態資源被丟入catch，再從catch中移除，(在動態讀取中會發生)
			//因為會被丟入catch的資源，只有動態的資源。
			//所以在被Lock為靜態的資源，要立即變更到靜態Pool中。

			//將資源丟入靜態中之前，先檢查靜態Pool存在
			stiter = GetResMgr().FindResForStatic(filename) ; 

			if(stiter != GetResMgr().m_staticres.end() )
			{
				//有找到的話，從動態中移除。
				GetResMgr().ReleaseResource( objiter ) ;

				return ; 
			}
			else
			{
				//沒有找到的話，丟入到靜態


				//更新日記：更新資源流動量
				UpdatediaryTime( objiter->second ) ;
				//更新日記：更新資源的狀態
				UpdatedirayStatic(objiter->second) ;
				//丟入到靜態
				m_staticres.insert( make_pair(filename , objiter->second))  ;

				//從動態中移除。
				GetResMgr().ReleaseResource( objiter ) ;

			}



		}

	}




}

void CResMgr::UnLockRes(const char *filename) 
{
//	std::vector<std::string>::iterator iter ;
	std::set<std::string>::iterator iter ; 

	std::string name(filename) ;

	iter = m_resname.find(name) ; 

	//利用std::equal_to比較是否名稱相等
//	iter = find_if(m_resname.begin() , m_resname.end() , 
//		   boost::bind(std::equal_to<std::string>() , _1 , name)) ;

	if ( iter != m_resname.end())
	{
		m_resname.erase(iter) ;
	}
}


void CResMgr::ClearDynamic( void )//在資源管理中，清空所有動態資源
{
	/////////////////更新日記
	//設定訪問器動作：更新資源的目錄（改變狀態為Rel）
	m_visitdiary.AddFun( &UpdatediraystateRel )  ;

	//更新日記
	ResObjmap::iterator iter(m_resobjmap.begin()) ;

	while( iter != m_resobjmap.end() )
	{
		boost::apply_visitor( m_visitdiary , (iter->second)) ;

		++iter ;
	}


	//清空計數器
	m_count.clear() ;

	//更新完畢之後，就刪除吧～
	m_resobjmap.clear() ;
}
void CResMgr::ClearStatic( void )//在資源管理中，清空所有靜態資源
{
	/////////////////更新日記
	//設定訪問器動作：更新資源的目錄（改變狀態為Rel）
	m_visitdiary.AddFun( &UpdatediraystateRel )  ;
	//更新日記
	ResObjmap::iterator iter(m_staticres.begin()) ;

	while( iter != m_staticres.end() )
	{
		boost::apply_visitor( m_visitdiary , (iter->second)) ;

		++iter ;
	}

	//更新完畢之後，就刪除吧～
	m_staticres.clear() ;
}

bool CResMgr::CheckResource(const char *filename) 		//檢查資訊是否在靜態或動態以及資源回收桶中
{
	ResObjmap::iterator iter ; 

	iter = FindResForStatic(filename) ; 

	if(iter != m_staticres.end())
	{
		return true ; 
	}
	
	iter = FindResForDyanmic(filename) ; 

	if(iter != m_resobjmap.end())
	{
		return true ; 
	}

	return FindResInCatch(filename) ; 
}


ResObjmap::iterator CResMgr::FindResForDyanmic(const std::string &filename)//尋找資源：從動態Pool
{


	ResObjmap::iterator iter ;

	iter = MapFind(m_resobjmap , filename) ; 

	return iter ;
}
ResObjmap::iterator CResMgr::FindResForStatic(const std::string &filename)//尋找資源：從靜態Pool
{
	//cosiann
	//ResObjlist::iterator iter ;
	//CCatchvisitEqual<std::string> visit(filename) ;
	//iter = ArrayFindifWithVisitor( m_staticres , visit ) ;

	ResObjmap::iterator iter ;

	iter = MapFind(m_staticres , filename) ;


	return iter ;
}
void CResMgr::PushToPool(const Resobj &resobj , const std::string &filename)	//將資源丟入資源Pool中
{
	//判斷是否為特定資源
	//std::vector<std::string>::iterator iter ;
	CCatchvisitEqual<std::string> visit(filename) ;

	std::set<std::string>::iterator iter ; 

	iter = m_resname.find(filename) ; 

	//iter = find_if(m_resname.begin() , m_resname.end() , visit ) ;

	/////				丟入靜態pool		////
	if ( iter != m_resname.end())
	{
		//更新日記：更新資源流動量
	//	UpdatediaryTime( resobj ) ;
		//更新日記：更新資源的狀態
	//	UpdatedirayStatic(resobj) ;
		//丟入靜態list 

		
		m_staticres.insert( make_pair(filename , resobj))  ;

	}
	else/////				丟入動態pool		////
	{
		CResMgrvisitGetAllRes visit ;

		//先進行分享計數器註冊
		//RegisterShare( boost::apply_visitor(visit , resobj)) ;


		//更新日記：更新資源流動量
		UpdatediaryTime( resobj ) ;

		m_resobjmap.insert( make_pair(filename , resobj))  ;
	}
}
void CResMgr::ReleaseResource( ResObjmap::iterator iter) //移除資源，且更新日記紀錄
{
	//在從日記上更改資源記錄
	UpdatediraryRel( iter->second ) ;

	m_resobjmap.erase(iter) ;

}
void CResMgr::UpdatediaryTime(const Resobj &resobj)
{

	/////////////////更新日記
	//設定訪問器動作：更新資源的目錄
	m_visitdiary.AddFun( &UpdatediarytimeForResMgr )  ;

	//更新日記
	boost::apply_visitor(m_visitdiary , resobj) ;
}
void CResMgr::UpdatedirayStatic(const Resobj &resobj)
{
	/////////////////更新日記
	//設定訪問器動作：更新資源的目錄(改變狀態為static)
	m_visitdiary.AddFun( &UpdatediraystateStatic )  ;

	//更新日記
	boost::apply_visitor(m_visitdiary , resobj) ;
}
void CResMgr::UpdatediraryRel(const Resobj &resobj)
{
	/////////////////更新日記
	//設定訪問器動作：更新資源的目錄（改變狀態為Rel）
	m_visitdiary.AddFun( &UpdatediraystateRel )  ;

	//更新日記
	boost::apply_visitor(m_visitdiary , resobj) ;

}
void CResMgr::UpdateSize(int offset) 
{
	m_size += offset ;
}

//這是用來改善回傳資源指標，而不是smart point的問題。紀錄資源使用量
void CResMgr::RegisterShare(void *res)
{
	Sharecount::iterator iter ;

	iter = MapFind( m_count , res) ;

	if ( iter != m_count.end() )
	{
		//有找到的話，紀錄一筆
		iter->second += 1 ;
	}
	else
	{
		//沒找到的話，建立一筆
		std::pair<void* , unsigned int>  pairres ;

		pairres.first = res ;
		pairres.second = 1 ;

		m_count.insert(pairres) ;
	}

}

//回傳：true，表示沒有其他資源在分享了。 
//		false：表示還有其他資源再分享。或是沒有找到資源
bool CResMgr::UnRegisterShare(void *res) 
{
	Sharecount::iterator iter ;

	iter = MapFind( m_count , res) ;

	if ( iter != m_count.end() )
	{
		iter->second -= 1 ;

		//表示沒有其他資源再分享
		if ( iter->second == 0)
		{
			//並且移除這筆資料

			m_count.erase(iter) ;

			return true ;
		}
	}

	return false ;
}

