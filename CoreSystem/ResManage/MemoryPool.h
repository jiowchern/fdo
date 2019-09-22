#ifndef _MEMORYPOOL_H_
#define _MEMORYPOOL_H_
#include "stdafx.h" 
#include <typeinfo>
#include "filestream.h"
#include "command.h"
#include <boost/pool/object_pool.hpp>
#include <boost\shared_ptr.hpp>

class CMemorydiary ;
class CObjectdiary; 
class CObjMgrslot ;

template<typename T> class CObjectMgr ;



//////////////				全域函數			/////////////////
//取得物件記憶體列表
boost::shared_ptr<CMemorydiary> GetMem(void) ;
//取得物件管理日記
CObjectdiary& GetObjDiary(const char *filename) ;


///////////////////			物件管理的Template全域函數		///////////////////

////			取得物件管理
template<typename T>
boost::shared_ptr<CObjectMgr<T> >& GetObjMgr(void)
{
	static boost::shared_ptr<CObjectMgr<T> > obj(new CObjectMgr<T>(GetMem())) ;

	return obj ;
}

/////			刪除物件：利用資源指標
template<typename T>
void DeleteObj(T *t)
{
	GetObjMgr<T>()->Delete(t) ;
}



////////			將建立出來的物件，納入物件管理作管理
template<typename T>
void CreateObj(T **t)
{
	(*t) = GetObjMgr<T>()->Create() ;
}


//提供記憶體的列表輸出用
class CStreamOut
{
public:
	typedef void result_type ;
public:
	template<typename T>
	void operator()(const std::string &title ,  T t)
	{
		cout << title << t << endl ;
	}
};


////////////////			紀錄使用記憶體的列表
class CMemorydiary
{
	typedef std::set<void*>::iterator						   setiter ;
	typedef boost::signal<void (bool , const std::string& , unsigned int)>   Signaltype ;
	typedef Signaltype::slot_type							   SlotType ;	
public:
	CMemorydiary(SlotType solt) ;
	~CMemorydiary(void);
public:
	//插入一筆記憶體table紀錄
	void CheckIn(const std::string &objname , unsigned int objsize , void* const id);
	//移除一筆記憶體table紀錄
	void CheckOut(const std::string &objname , unsigned int objsize ,void* const id);
	bool Ok(void);
private:
	std::set<void*>					m_objs ;
	Signaltype						m_signal ;
};


////////////////			物件管理		//////////////////


////這裡為什麼要分享CMemorydiary的原因：為了確保每一個物件在此物件管理中刪除時(系統關閉)，
////CMemorydiary是最後一個被刪除！所以這裡將它分享出來。


template<typename T>
class CObjectMgr
{
	typedef std::map<void* , T*>									Objmap ;
//	typedef std::map<void* , boost::object_pool<T> >				Objmap ;
public:
	CObjectMgr(boost::shared_ptr<CMemorydiary> diray ) : m_diray(diray) 
	{

	}
	~CObjectMgr(void)
	{
		Objmap::iterator iter = m_objs.begin() ;

		while( iter != m_objs.end())
		{
			delete (iter->second) ; 
			(iter->second) = NULL ;

			iter++ ;
		}

		m_objs.clear() ;
	}
public:
	T* Create( void )
	{

		T *t = NULL ;

		//建立物件
		try
		{

			
			t = new T ; 

	//		t = m_objpool.construct() ;


		}catch( std::logic_error &e )
		{
			//當建立失敗，接受例外
			Assert(0 , e.what() ) ;
		}

		m_objs.insert(make_pair(t , t)) ;


		//進行註冊
		m_diray->CheckIn(typeid(T).name() , sizeof(T) , this) ;

		return t ;


	}
	bool Delete(T *obj)
	{
		if (obj == NULL)
		{
			return false ;
		}

		Objmap::iterator iter ; 

		iter = MapFind(m_objs , obj) ;

		if ( iter != m_objs.end() )
		{
			
		//	m_objpool.destroy(iter->second) ;

			delete (iter->second) ; 
			(iter->second) = NULL ;

			m_objs.erase(iter) ;

			return true ;
		}

		return false  ;


		//	//進行反註冊：不作的原因是，想記錄某些物件共被呼叫幾次還有吃多少的記憶體。
		//	//作反註冊的話，就得不到我的需求。且要檢查memleak的話，也不能這樣做。
		//	//因為到最後，m_objpool會幫我清除
		//	//m_diray->CheckOut(typeid(T).name() , sizeof(T) , this) ;

		//	return true ;


		//return false ;
	}

private:
	Objmap												m_objs ; 
	boost::shared_ptr<CMemorydiary>						m_diray ;		//記憶體列表
	//Poolmap												m_objpool ;
//	boost::object_pool<T>								m_objpool ;		//物件記憶體pool   
};


//////////////////				物件的日記			///////////

class CObjectdiary
{
	//跟管理插槽作朋友
	friend class CObjMgrslot ;

	//物件資料屬性：數量，資料大小
	typedef boost::tuple<unsigned int , unsigned int>			Objdate ;
	//物件查表：物件名稱，物件資料屬性 
	typedef std::map<std::string , Objdate >					Objmap ;
	typedef Objmap::iterator									Objmapiter ;
public:
	CObjectdiary(void) ;
	CObjectdiary(const char *filename) ;
	~CObjectdiary(void);
private:
	//更新日記：插入一筆物件紀錄
	void Insert(const std::string &objname , unsigned int objsize);
	//更新日記：移除一筆物件記錄
	void Erase(const std::string &objname , unsigned int objsize);
	//紀錄資料到disk中
	void Save(void) ;
public:
	//搜尋某物件的出現次數
	unsigned int SerachTimes(const std::string &objid) ;
	//搜尋某物件的吃記憶體的總量
	unsigned int SerachSize(const std::string &objid) ;
private:
	Objmap										m_table ;
	std::ofstream 								m_file;
	std::string									m_name ;

};
/////////////////			物件管理的slot	/////////////////////
class CObjMgrslot
{
public:
	typedef void result_type ;
public:
	CObjMgrslot( CObjectdiary *diary) ;
	~CObjMgrslot(void);
public:
	void operator()(bool isenter , const std::string &objname , unsigned int objsize);
private:
	CObjectdiary									*m_diary ; 
};

#endif