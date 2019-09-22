#include "stdafx.h"
#include "resourceMgr.h"




//快取跟日記的溝通介面：更新快取記憶體大小
void UpdateCatchSize(int offset)
{
	GetCatch()->UpdateSize( offset ) ;
}


//
boost::shared_ptr<CCatchRes> GetCatch( void )
{

	static boost::shared_ptr<CCatchRes> ccatch(new CCatchRes) ;

	return ccatch  ;
}


////////////			資源快取		////////////////

CCatchRes::CCatchRes() :
 m_cursize(0) , m_mempool() , m_diray(new CDiary("MgrPool.txt")) , m_visitdiary(m_diray)
{

	//以MB為單位
	m_poolsize = MSIZE * 600; 
	
	//std::string table(FDO::GetConfig("CatchSize"));

	//try
	//{

	//	//利用config.ini的CatchSize調整快取大小。
	//	m_poolsize *= boost::lexical_cast<unsigned int>(table) ; 

	//}catch(boost::bad_lexical_cast &e)
	//{
	//	Assert(0 , e.what()) ;
	//}


}

CCatchRes::~CCatchRes( void )
{
	m_mempool.clear( ) ;
}

bool CCatchRes::Check( unsigned int ressize)
{
	unsigned int aftersize = 0;
	//此資源的size + 此快取的資源的現有size
	aftersize = ressize + m_cursize ;
	
	//是否超過快取資源size
	if ( aftersize > m_poolsize)
	{
		//沒有空間了
		return false ;
	}
	
	return true ;
}
void CCatchRes::Push(const std::string &filename , const Resmemory &res)
{
	//丟入記憶體池
	m_mempool.insert(make_pair(filename , res)) ;

	/////////////////更新日記
	//設定訪問器動作：更新資源使用量
	m_visitdiary.AddFun( &UpdatediarytimeForCatch )  ;

	//更新日記
	boost::apply_visitor(m_visitdiary , res) ;

}
void CCatchRes::Pop( void )
{	
	assert(!m_mempool.empty() ) ;

	//先排序：不打算排序的原因：因為玩家玩久了，快取可能存放一推很大的資源。可能這些資源
	//玩家不需要，這樣會造成不必要的浪費。
//	Sort( ) ;
	
	//再進行刪除資源
	Clear( ) ;
}
void CCatchRes::Show( void )
{
	///////////////				測試排序用
	//CCatchtVisitOut visit ;  
	//
	////顯示所有資源的情況(檔案大小，名稱，資源種類)
	//for_each(m_mempool.begin( ) , m_mempool.end() , boost::apply_visitor(visit) ) ;



	//顯示所有資源的情況(檔案大小，檔案名稱，來往快取次數，檔案狀態)
//	m_diray->Show( ) ;

}
unsigned int CCatchRes::MaxSize( void ) const
{
	return m_poolsize ;
}

unsigned int CCatchRes::Size( void ) const
{
	return m_cursize ;
}
//
//void CCatchRes::Sort( void )
//{
//	CCatchVisitSort visit ;
//
//	m_mempool.sort(boost::apply_visitor(visit));
//
//}
void CCatchRes::Clear( void )
{
	/////////////////更新日記
	//設定訪問器動作：改變資源狀態為REL，同時變更快取記憶體Size
	m_visitdiary.AddFun( &UpdatediraystateRelForCatch )  ;

	Resmap::iterator iter(m_mempool.begin()) ;

	//更新日記
	boost::apply_visitor(m_visitdiary , iter->second );

	//刪除資源(從快取中移除)
	m_mempool.erase(iter) ;
}

void CCatchRes::AllClear(void)
{
	size_t size = m_mempool.size() ;

	//清空所有在快取中的資源
	for(unsigned int i = 0 ; i < size ; ++i)
	{
		Clear() ;
	}
}


void CCatchRes::UpdateSize(int offset)
{
	m_cursize += offset ;
}

std::map<std::string ,Resmemory>::iterator CCatchRes::Find(const std::string &filename )
{
//	CCatchvisitEqual<std::string> visit(filename);
	//iter = find_if(m_mempool.begin() , m_mempool.end() , boost::apply_visitor(visit)) ;

	Resmap::iterator iter ;

	//檢查快取中，是否擁有此資源
	iter = MapFind(m_mempool , filename);

	return iter ;

}

//刪除某項資源
void CCatchRes::Erase( std::map<std::string , Resmemory>::iterator iter )
{
	assert( iter != m_mempool.end() ) ;

	/////////////////更新日記
	//設定訪問器動作：改變資源狀態為REL，同時變更快取記憶體Size
	m_visitdiary.AddFun( &UpdatediraystateRelForCatch )  ;
	//更新日記
	boost::apply_visitor(m_visitdiary , iter->second );

	//從快取中移除
	m_mempool.erase( iter ) ;


}
