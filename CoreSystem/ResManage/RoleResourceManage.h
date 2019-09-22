#ifndef _ROLERESOURCEMANAGE_H_
#define _ROLERESOURCEMANAGE_H_

#pragma once
#include "stdafx.h"
#include <list>
#include <map>
#include <assert.h>
#include <iostream>
#include <string>
#include <boost\shared_ptr.hpp>
//#include "RoleResourceManage.h"
#include "FDO_RoleSkin.h"
#include "CRATFileStruct.h"
#include "CRPCFileStruct.h"
/*
struct stRPC
{
	std::string		filename ;
	int				*date ; //std::list<pframe>
};

struct stPAT
{
	std::string		filename ; 
	int				*date1 ; 
	int				*date2 ; 
};
*/

class CReleaseRoleRes
{
public:
	//一般物件的delete
	template<typename T>
	void operator( ) (T *res)
	{
		delete res ;
		res = NULL ;
	}
    //特化RPC
	/*template<>
	void operator()(FDO::sRoleActionList* pRoleActionList)
	{
        while( !pRoleActionList->empty() )
        {
            FDO::g_RoleMemoryMgr.FreeAction( pRoleActionList->front() );
            pRoleActionList->pop_front();
        }
	}*/
};

class CSearchRoleRes
{
public:
	CSearchRoleRes(const std::string &filename) : m_filename(filename)
	{

	}
	~CSearchRoleRes(void)
	{

	}
	template<typename T>
	bool operator()(const T &t)
	{
		if (t.name == m_filename)
		{
			return true ;
		}
		return false ;
	}
private:
	std::string					m_filename ;
};



template<typename T>
class CTimeCatch
{
public:
	typedef boost::shared_ptr<T>					ListResPtr ; 
	struct ResDate
	{
		ListResPtr			resPtr ;
		std::string			name ; 
	};

public:
	CTimeCatch(unsigned int relTime) :m_res() 
	{
		m_relTime = relTime ; 
	}
	~CTimeCatch(void)
	{
		m_res.clear() ; 
	}
public:
	void Update(unsigned int time) 
	{
		//判斷是否為空
		if(m_res.empty())
		{
			return ; 
		}


		m_curtime += time ; 

		//超過時間的話，進行刪除
		if(m_curtime >= m_relTime)
		{
			PopCatch() ; 

			m_curtime = 0 ; 
		}
	}
public:										
	bool Find(const std::string &filename , ListResPtr &resptr)
	{
		std::list<ResDate>::iterator iter ; 

		iter = std::find_if(m_res.begin() , m_res.end() , CSearchRoleRes(filename)) ;

		if( iter != m_res.end())
		{
			resptr = (*iter).resPtr ; 

			//從快取中移除！
			m_res.erase(iter) ;

			return true ;
		}

		return false ;

	}
	void PushCatch(const std::string &filename , ListResPtr &res)
	{
		if(!res.unique())
		{
			Assert(0 , "怎麼把非唯一的資源丟入快取呢？！") ;
		}


		ResDate date ; 

		date.name = filename ;		//記錄檔名
		date.resPtr = res ;			//記錄資源

		//直接丟進入
		m_res.push_back(date) ; 

	}
public:
	void PopCatch(void)
	{
		//因為在快取中，資源一定是唯一的。所以直接移除他就行了。
		m_res.pop_front() ; 

		//空的話，計數器歸0
		if(m_res.empty())
		{
			m_curtime = 0 ;
		}
	}

	void Clear(void)
	{
		m_res.clear() ; 
	}
private:
	std::list<ResDate>								m_res;				//快取中的資源
	unsigned int									m_curtime ;			//現在的計數時間
	unsigned int									m_relTime ;			//釋放時間

};


template<typename T>
class CRoleResourceManage
{
	typedef boost::shared_ptr<T>					ListResPtr ; 
	typedef std::map<std::string , ListResPtr>		ResMap ; 
public:
	CRoleResourceManage(void) : m_resmap()// , m_catch()
	{
		m_bCatch = false ;
	}
	~CRoleResourceManage(void)
	{
		//直接接給boost::shared_ptr幫忙刪除
		m_resmap.clear() ;
	}
public:
	void Init(bool useCatch , unsigned int relTime)
	{
		if(useCatch)
		{
			boost::shared_ptr<CTimeCatch<T> > catchPtr(new CTimeCatch<T>(relTime)) ; 
			
			m_catch = catchPtr ;
		}

		//記錄是否有用到快取
		m_bCatch = useCatch ; 
	}

	bool Get(const std::string &filename ,  boost::shared_ptr<T> &reslist) 
	{
		//先從map中尋找
		ResMap::iterator iter ; 

		iter = m_resmap.find(filename) ;

		if(iter != m_resmap.end())
		{
			reslist = iter->second ;

			return true ;
		}

		//再從快取中找尋
		if(m_bCatch)
		{
			if(m_catch->Find(filename , reslist))
			{
				//如果有抓到的話，重新丟入到map中
				Insert(filename , reslist) ;

				return true ;
			}
		}
		
		return false ;
	}
	void Insert(const std::string &filename , boost::shared_ptr< T > reslist) 
	{
		m_resmap.insert(ResMap::value_type(filename , reslist)) ; 
	}
	bool Release(const std::string &filename , boost::shared_ptr< T > &reslist)
	{
		ResMap::iterator iter ;

		iter = MapFind(m_resmap , filename) ;

		//判斷是否有資源在此pool
		if(iter != m_resmap.end())
		{
			if(m_bCatch)
			{
				//有快取的話...

				//先釋放此資源的所有權
				reslist.reset() ;

				
				
				if(!(iter->second).unique())
				{
					//表示還有其他資源再使用
					return false ;
				}

				//丟入到快取中。因為沒有人在使用了
				m_catch->PushCatch(filename , iter->second) ; 

				//從map中消失
				m_resmap.erase(iter) ;

			}
			else
			{
				//沒有使用快取的話，就一直保留。直到此類別被刪除掉！
			}		

			return true ;
		}
		else
		{
			//沒有找到的話，直接回傳
			return false ;
		}
	
	
	}
	void Update(unsigned int time)
	{
		if(m_bCatch)
		{	
			m_catch->Update(time) ;
		}
	}
    void Clear()
    {
        m_resmap.clear() ;


		if(m_bCatch)
		{
			//順便把快取的資源給釋放掉
			m_catch->Clear() ;
		}
    }
private:
	ResMap											m_resmap ; 
	boost::shared_ptr<CTimeCatch<T> >				m_catch ;			//快取物件
	bool											m_bCatch ;			//是否使用快取
};


boost::shared_ptr<CRoleResourceManage< FDO::SkinRAT > >& GetRATManage(void);
boost::shared_ptr<CRoleResourceManage< FDO::SkinRPC > >& GetRPCManage(void);


//////////////////////////////////////////////////////////////////////////
//新結構
//[10/29/2008 a-wi]
//////////////////////////////////////////////////////////////////////////
boost::shared_ptr<CRoleResourceManage< FDO::SRATFileRoot > >& GetRATFileManage(void);
boost::shared_ptr<CRoleResourceManage< FDO::SRPCFileRoot > >& GetRPCFileManage(void);

//boost::shared_ptr<CRoleResourceManage< FDO::sRoleActionList > >& GetRATManage(void);


#endif