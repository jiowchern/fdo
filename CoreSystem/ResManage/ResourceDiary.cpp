#include "stdafx.h" 
#include "resourceMgr.h"




//更新：紀錄資源的使用量
void Updatediarytime( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date)
{
	try
	{
		std::string id = boost::any_cast<std::string>(date[1]) ; //檔名

		//判斷是否有此履歷(利用檔名)
		if ( !(pdiray->Check(id)) )
		{
			//沒有的話，重新建立一個
			pdiray->Create( id , boost::any_cast<unsigned int>(date[0])  ) ;
		}

	}catch(boost::bad_any_cast &e)
	{
		Assert(0 , e.what()) ;
	}

}

//更新：紀錄資源的使用量屬於Catch
void UpdatediarytimeForCatch( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date)
{
	Updatediarytime( pdiray , date ) ;


	///////////			
	try
	{
		//更新快取記憶體size(增加)
		UpdateCatchSize( boost::numeric_cast<int>(
			boost::any_cast<unsigned int>(date[0]))) ;

	}catch(boost::bad_numeric_cast &e)
	{
		Assert(0 , e.what()) ;
	}
		 	
}

//更新：紀錄資源的使用量屬於資源管理
void UpdatediarytimeForResMgr( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date)
{
	Updatediarytime( pdiray , date ) ;


	///////////			
	try
	{
		//更新資源管理記憶體size(增加)
		UpdateResMgrSize( boost::numeric_cast<int>(
			boost::any_cast<unsigned int>(date[0]))) ;

	}catch(boost::bad_numeric_cast &e)
	{
		Assert(0 , e.what()) ;
	}
		 	
}

//更新：變更資源狀態：Release
void UpdatediraystateRel( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date)
{
	try
	{
		
		pdiray->SetState(boost::any_cast<std::string>(date[1]) , CDiary::REL) ;



	}catch(boost::bad_any_cast &e)
	{
		Assert(0 , e.what()) ;
	}
}

//更新：變更資源狀態：Static
void UpdatediraystateStatic( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date)
{
	try
	{
		
		pdiray->SetState(boost::any_cast<std::string>(date[1]) , CDiary::STATIC) ;



	}catch(boost::bad_any_cast &e)
	{
		Assert(0 , e.what()) ;
	}
}

//更新：變更資源狀態屬於Catch
void UpdatediraystateRelForCatch( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date)
{

	UpdatediraystateRel( pdiray , date ) ;

	/////////////////
	try
	{
		//更新快取記憶體size(扣掉)
		UpdateCatchSize( -(boost::numeric_cast<int>(
			boost::any_cast<unsigned int>(date[0])))) ;

	}catch(boost::bad_numeric_cast &e)
	{
		Assert(0 , e.what()) ;
	}

}

//更新：刪除日記屬於資源管理
void UpdatediraystateForResMgr( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date)
{
	try
	{
		//移除它		
		pdiray->Erase(boost::any_cast<std::string>(date[1])) ;


	}catch(boost::bad_any_cast &e)
	{
		Assert(0 , e.what()) ;
	}

	

	/////////////////
	try
	{
		//更新快取記憶體size(扣掉)
		UpdateResMgrSize( -(boost::numeric_cast<int>(
			boost::any_cast<unsigned int>(date[0])))) ;

	}catch(boost::bad_numeric_cast &e)
	{
		Assert(0 , e.what()) ;
	}

}
/////////////////////			資源日記		//////////////////////

CDiary::CDiary(void) : m_diary() , m_filename() 
{
}

CDiary::CDiary( const std::string &filename ) : m_diary() , m_filename(filename) 
{
}
CDiary::~CDiary( void )
{
	//需要記錄的話，把註解給拿開
	//Show() ;
	//m_o.close() ;

	m_diary.clear();

}
void CDiary::Show( void ) 
{
	Diarymap::iterator iter = m_diary.begin() ;

	m_o.open(m_filename.c_str()) ;

	if (m_o.fail())
	{
		return ;
	}

	while (iter != m_diary.end() )
	{
		m_o << "檔案大小：" << (iter->second).get<0>() << " Byte" << "		"  ; //unsigned int 
		m_o << "檔名：" << iter->first << "		"  ;				//std::string
		m_o << "次數：" << (iter->second).get<1>() << "		"  ;	//unsigned int 
		m_o << "狀態：" << (iter->second).get<2>() << "		"  ;	//ReState

		m_o << endl ;

		iter++; 
	}
}
bool CDiary::Check(const std::string &id )
{
	Diarymap::iterator iter ;

	iter = MapFind( m_diary , id ) ;

	//有的話，對此履歷增加次數
	if ( iter != m_diary.end() )
	{
		//unsigned int 
		(iter->second).get<1>() += 1 ;

		return true ;
	}

	return false ;
}
void CDiary::Create(const std::string &id , unsigned int filesize)
{
	Diarypair p ;

	//重新建立一個
	p.first = id ;
	//檔案大小，使用次數，狀態：USE
	p.second = boost::make_tuple(filesize , 1 , USE) ;

	m_diary.insert( p ) ;
}

void CDiary::SetState( const std::string &id , ResState state)
{
	Diarymap::iterator iter ;

	iter = MapFind( m_diary , id ) ;

	//有的話，對此資源改變狀態
	if ( iter != m_diary.end() )
	{
		//ResState
		(iter->second).get<2>() = state ;
	}
}

void CDiary::Erase(const std::string &id)
{
	Diarymap::iterator iter ;

	iter = MapFind( m_diary , id ) ;

	if ( iter != m_diary.end() )
	{
		//移除它
		m_diary.erase(iter) ;
	}


}
//將所有資源設定:REL。不過，需要嗎？
void CDiary::Delevery( void )
{	
	Diarymap::iterator iter = m_diary.begin() ;

	while (iter != m_diary.end() )
	{
		(iter->second).get<2>() = REL ;

		cout << "test： " << (iter->second).get<2>() << endl ;

		
		iter++; 
	}		
}
