#include "stdafx.h"
#include "memorypool.h"

boost::shared_ptr<CMemorydiary> GetMem(void)
{
	static boost::shared_ptr<CMemorydiary> mem(new 
		//這裡直接建立物件資源日記檔名
		CMemorydiary(CObjMgrslot( &GetObjDiary("objdiary.txt") ))) ;

	return mem ;
}

CObjectdiary& GetObjDiary(const char *filename)
{
	static CObjectdiary obj(filename)  ;

	return obj ;
}


////////////////			紀錄使用記憶體的列表

CMemorydiary::CMemorydiary(SlotType solt) :m_objs()
{
	m_signal.connect(solt) ;
}

CMemorydiary::~CMemorydiary(void)
{
	////如果還有物件沒有Checkout的話，這裡會顯示出現
	//for_each(m_objs.begin() , m_objs.end() , 
	//	boost::bind( CStreamOut() , "MemoryLeak： " , _1 )) ;
}

void CMemorydiary::CheckIn(const std::string &objname , unsigned int objsize , void* const id)
{
	if (m_objs.find(id) != m_objs.end())
	{
		//會出現這情形，是繼承的關係。就讓他紀錄下來。
		std::cout << "Double-constructed memory: " << id << std::endl;
	}
	//記錄記憶體位址
	m_objs.insert(id) ;
	//呼叫插槽：新增日記
	m_signal(true , objname , objsize) ;
}

void CMemorydiary::CheckOut(const std::string &objname , unsigned int objsize ,void* const id)
{
	if (m_objs.find(id) == m_objs.end())
	{
		//會出現這種情形是此物件有繼承的類別
		std::cout << "Destroyed non-constructed memory: " << id << std::endl;

		return ; 
	}
	//移除記憶體位址
	m_objs.erase(id) ;
	//呼叫插槽：移除日記
	m_signal(false ,objname , objsize) ;
}

bool CMemorydiary::Ok(void)
{
	return m_objs.empty() ;
}


//////////////////				物件的日記			///////////
CObjectdiary::CObjectdiary(void) : m_table() , m_name()
{
}

CObjectdiary::CObjectdiary(const char *filename) : m_table() , m_name(filename)
{
}

CObjectdiary::~CObjectdiary(void)
{
	//要開啟記錄的話，把註解拿掉。
	//m_file.open(m_name.c_str()) ;

	//for_each(m_table.begin() , m_table.end() , CStreamWrite(&m_file)) ;

	//m_file.close() ;

	//m_table.clear() ;

}
void CObjectdiary::Insert(const std::string &objname , unsigned int objsize)
{
	Objmapiter iter ;

	iter = MapFind(m_table , objname) ;

	if (iter != m_table.end())
	{
		//更新物件的次數和此類物件的大小
		boost::tuples::get<0>(iter->second) += 1 ;
		boost::tuples::get<1>(iter->second) += objsize ;
	}
	else
	{
		std::pair<std::string , Objdate> p ;
		//建立一筆資料
		Objdate date( 1 , objsize) ;

		p.first = objname ; 
		p.second = date ;

		//並且新增
		m_table.insert(p) ;
	}

}
void CObjectdiary::Erase(const std::string &objname , unsigned int objsize)
{
	Objmapiter iter ;

	iter = MapFind(m_table , objname) ;
	
	if (iter != m_table.end())
	{
		//檢查是否為唯一了
		if (1 == boost::tuples::get<0>(iter->second))
		{
			//唯一的話，直接刪除吧！
			m_table.erase(iter) ;
		}
		else
		{
			//更新物件的次數和此類物件的大小
			boost::tuples::get<0>(iter->second) -= 1 ;
			boost::tuples::get<1>(iter->second) -= objsize ;
		}
	}
}
//搜尋某物件的出現次數
unsigned int CObjectdiary::SerachTimes(const std::string &objid) 
{
	Objmapiter iter ;

	iter = MapFind(m_table , objid) ;
	
	if (iter != m_table.end())
	{
		return boost::tuples::get<0>(iter->second) ;
	}

	return 0 ;
}
//搜尋某物件的吃記憶體的總量
unsigned int CObjectdiary::SerachSize(const std::string &objid) 
{
	Objmapiter iter ;

	iter = MapFind(m_table , objid) ;
	
	if (iter != m_table.end())
	{
		return boost::tuples::get<1>(iter->second) ;
	}

	return 0 ;
}
/////////////////			物件管理的slot	/////////////////////
CObjMgrslot::CObjMgrslot( CObjectdiary *diary) : m_diary(diary) 
	{
	}
CObjMgrslot::~CObjMgrslot(void)
	{
	}
void CObjMgrslot::operator()(bool isenter , const std::string &objname , unsigned int objsize)
{
	if (isenter)
	{
		m_diary->Insert(objname , objsize) ;
	}
	else
	{
		m_diary->Erase(objname , objsize);
	}
}
