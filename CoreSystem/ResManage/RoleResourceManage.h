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
	//�@�몫��delete
	template<typename T>
	void operator( ) (T *res)
	{
		delete res ;
		res = NULL ;
	}
    //�S��RPC
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
		//�P�_�O�_����
		if(m_res.empty())
		{
			return ; 
		}


		m_curtime += time ; 

		//�W�L�ɶ����ܡA�i��R��
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

			//�q�֨��������I
			m_res.erase(iter) ;

			return true ;
		}

		return false ;

	}
	void PushCatch(const std::string &filename , ListResPtr &res)
	{
		if(!res.unique())
		{
			Assert(0 , "����D�ߤ@���귽��J�֨��O�H�I") ;
		}


		ResDate date ; 

		date.name = filename ;		//�O���ɦW
		date.resPtr = res ;			//�O���귽

		//������i�J
		m_res.push_back(date) ; 

	}
public:
	void PopCatch(void)
	{
		//�]���b�֨����A�귽�@�w�O�ߤ@���C�ҥH���������L�N��F�C
		m_res.pop_front() ; 

		//�Ū��ܡA�p�ƾ��k0
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
	std::list<ResDate>								m_res;				//�֨������귽
	unsigned int									m_curtime ;			//�{�b���p�Ʈɶ�
	unsigned int									m_relTime ;			//����ɶ�

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
		//��������boost::shared_ptr�����R��
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

		//�O���O�_���Ψ�֨�
		m_bCatch = useCatch ; 
	}

	bool Get(const std::string &filename ,  boost::shared_ptr<T> &reslist) 
	{
		//���qmap���M��
		ResMap::iterator iter ; 

		iter = m_resmap.find(filename) ;

		if(iter != m_resmap.end())
		{
			reslist = iter->second ;

			return true ;
		}

		//�A�q�֨�����M
		if(m_bCatch)
		{
			if(m_catch->Find(filename , reslist))
			{
				//�p�G����쪺�ܡA���s��J��map��
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

		//�P�_�O�_���귽�b��pool
		if(iter != m_resmap.end())
		{
			if(m_bCatch)
			{
				//���֨�����...

				//�����񦹸귽���Ҧ��v
				reslist.reset() ;

				
				
				if(!(iter->second).unique())
				{
					//����٦���L�귽�A�ϥ�
					return false ;
				}

				//��J��֨����C�]���S���H�b�ϥΤF
				m_catch->PushCatch(filename , iter->second) ; 

				//�qmap������
				m_resmap.erase(iter) ;

			}
			else
			{
				//�S���ϥΧ֨����ܡA�N�@���O�d�C���즹���O�Q�R�����I
			}		

			return true ;
		}
		else
		{
			//�S����쪺�ܡA�����^��
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
			//���K��֨����귽������
			m_catch->Clear() ;
		}
    }
private:
	ResMap											m_resmap ; 
	boost::shared_ptr<CTimeCatch<T> >				m_catch ;			//�֨�����
	bool											m_bCatch ;			//�O�_�ϥΧ֨�
};


boost::shared_ptr<CRoleResourceManage< FDO::SkinRAT > >& GetRATManage(void);
boost::shared_ptr<CRoleResourceManage< FDO::SkinRPC > >& GetRPCManage(void);


//////////////////////////////////////////////////////////////////////////
//�s���c
//[10/29/2008 a-wi]
//////////////////////////////////////////////////////////////////////////
boost::shared_ptr<CRoleResourceManage< FDO::SRATFileRoot > >& GetRATFileManage(void);
boost::shared_ptr<CRoleResourceManage< FDO::SRPCFileRoot > >& GetRPCFileManage(void);

//boost::shared_ptr<CRoleResourceManage< FDO::sRoleActionList > >& GetRATManage(void);


#endif