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

////////////			�귽��Ƭ����G�ˬd�֨��O�_�����귽(�֨���)		///////////////////////
bool FindResInCatch(const std::string &filename) 
{

	std::map<std::string , Resmemory>::iterator iter ;

	iter =  GetCatch()->Find(filename )  ; 

	if (iter != GetCatch()->m_mempool.end() )
	{
		//�����
		return true; 
	}

	//�S���
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
//	//�u�������ʺAPool���귽
//	iter = GetResMgr().FindResForDyanmic(filename) ;
//
//	if ( iter == GetResMgr().m_resobjlist.end() )
//	{
//		//�S����쪺�ܡA�N���X
//		return ;
//	}
//
//	/////��쪺��
//	GetResMgr().ReleaseResource( iter ) ;
//}

//////////////			�귽�޲z		//////////////////////

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

	//��ܩҦ��귽�����p(�ɮפj�p�A�ɮצW�١A�귽Ū�����ơA�ɮת��A)
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
		//�S����쪺�ܡA�N���J
		m_resname.insert(filename) ;

	}
}

void CResMgr::LockRes(const char *filename)
{
//	std::vector<std::string>::iterator iter ;
	std::set<std::string>::iterator iter ; 

	std::string name(filename) ;

	iter = m_resname.find(name) ; 

	//�Q��std::equal_to����O�_�W�٬۵�
	//iter = find_if(m_resname.begin() , m_resname.end() , 
	//	   boost::bind(std::equal_to<std::string>() , _1 , name)) ;

	if ( iter == m_resname.end())
	{
		//�S����쪺�ܡA�N���J
		//m_resname.push_back(filename) ;

		m_resname.insert(filename) ;


		ResObjmap::iterator objiter ;
		ResObjmap::iterator stiter ;


		//�A�ѰʺA��_
		objiter = GetResMgr().FindResForDyanmic( filename) ;

		if(objiter != GetResMgr().m_resobjmap.end() )
		{
			//�b�ʺAPool����쪺�ܡA�n�N���귽��J�R�A�I
			//�w���L���ɭԡA�ʺA�귽�Q��Jcatch�A�A�qcatch�������A(�b�ʺAŪ�����|�o��)
			//�]���|�Q��Jcatch���귽�A�u���ʺA���귽�C
			//�ҥH�b�QLock���R�A���귽�A�n�ߧY�ܧ���R�APool���C

			//�N�귽��J�R�A�����e�A���ˬd�R�APool�s�b
			stiter = GetResMgr().FindResForStatic(filename) ; 

			if(stiter != GetResMgr().m_staticres.end() )
			{
				//����쪺�ܡA�q�ʺA�������C
				GetResMgr().ReleaseResource( objiter ) ;

				return ; 
			}
			else
			{
				//�S����쪺�ܡA��J���R�A


				//��s��O�G��s�귽�y�ʶq
				UpdatediaryTime( objiter->second ) ;
				//��s��O�G��s�귽�����A
				UpdatedirayStatic(objiter->second) ;
				//��J���R�A
				m_staticres.insert( make_pair(filename , objiter->second))  ;

				//�q�ʺA�������C
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

	//�Q��std::equal_to����O�_�W�٬۵�
//	iter = find_if(m_resname.begin() , m_resname.end() , 
//		   boost::bind(std::equal_to<std::string>() , _1 , name)) ;

	if ( iter != m_resname.end())
	{
		m_resname.erase(iter) ;
	}
}


void CResMgr::ClearDynamic( void )//�b�귽�޲z���A�M�ũҦ��ʺA�귽
{
	/////////////////��s��O
	//�]�w�X�ݾ��ʧ@�G��s�귽���ؿ��]���ܪ��A��Rel�^
	m_visitdiary.AddFun( &UpdatediraystateRel )  ;

	//��s��O
	ResObjmap::iterator iter(m_resobjmap.begin()) ;

	while( iter != m_resobjmap.end() )
	{
		boost::apply_visitor( m_visitdiary , (iter->second)) ;

		++iter ;
	}


	//�M�ŭp�ƾ�
	m_count.clear() ;

	//��s��������A�N�R���a��
	m_resobjmap.clear() ;
}
void CResMgr::ClearStatic( void )//�b�귽�޲z���A�M�ũҦ��R�A�귽
{
	/////////////////��s��O
	//�]�w�X�ݾ��ʧ@�G��s�귽���ؿ��]���ܪ��A��Rel�^
	m_visitdiary.AddFun( &UpdatediraystateRel )  ;
	//��s��O
	ResObjmap::iterator iter(m_staticres.begin()) ;

	while( iter != m_staticres.end() )
	{
		boost::apply_visitor( m_visitdiary , (iter->second)) ;

		++iter ;
	}

	//��s��������A�N�R���a��
	m_staticres.clear() ;
}

bool CResMgr::CheckResource(const char *filename) 		//�ˬd��T�O�_�b�R�A�ΰʺA�H�θ귽�^����
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


ResObjmap::iterator CResMgr::FindResForDyanmic(const std::string &filename)//�M��귽�G�q�ʺAPool
{


	ResObjmap::iterator iter ;

	iter = MapFind(m_resobjmap , filename) ; 

	return iter ;
}
ResObjmap::iterator CResMgr::FindResForStatic(const std::string &filename)//�M��귽�G�q�R�APool
{
	//cosiann
	//ResObjlist::iterator iter ;
	//CCatchvisitEqual<std::string> visit(filename) ;
	//iter = ArrayFindifWithVisitor( m_staticres , visit ) ;

	ResObjmap::iterator iter ;

	iter = MapFind(m_staticres , filename) ;


	return iter ;
}
void CResMgr::PushToPool(const Resobj &resobj , const std::string &filename)	//�N�귽��J�귽Pool��
{
	//�P�_�O�_���S�w�귽
	//std::vector<std::string>::iterator iter ;
	CCatchvisitEqual<std::string> visit(filename) ;

	std::set<std::string>::iterator iter ; 

	iter = m_resname.find(filename) ; 

	//iter = find_if(m_resname.begin() , m_resname.end() , visit ) ;

	/////				��J�R�Apool		////
	if ( iter != m_resname.end())
	{
		//��s��O�G��s�귽�y�ʶq
	//	UpdatediaryTime( resobj ) ;
		//��s��O�G��s�귽�����A
	//	UpdatedirayStatic(resobj) ;
		//��J�R�Alist 

		
		m_staticres.insert( make_pair(filename , resobj))  ;

	}
	else/////				��J�ʺApool		////
	{
		CResMgrvisitGetAllRes visit ;

		//���i����ɭp�ƾ����U
		//RegisterShare( boost::apply_visitor(visit , resobj)) ;


		//��s��O�G��s�귽�y�ʶq
		UpdatediaryTime( resobj ) ;

		m_resobjmap.insert( make_pair(filename , resobj))  ;
	}
}
void CResMgr::ReleaseResource( ResObjmap::iterator iter) //�����귽�A�B��s��O����
{
	//�b�q��O�W���귽�O��
	UpdatediraryRel( iter->second ) ;

	m_resobjmap.erase(iter) ;

}
void CResMgr::UpdatediaryTime(const Resobj &resobj)
{

	/////////////////��s��O
	//�]�w�X�ݾ��ʧ@�G��s�귽���ؿ�
	m_visitdiary.AddFun( &UpdatediarytimeForResMgr )  ;

	//��s��O
	boost::apply_visitor(m_visitdiary , resobj) ;
}
void CResMgr::UpdatedirayStatic(const Resobj &resobj)
{
	/////////////////��s��O
	//�]�w�X�ݾ��ʧ@�G��s�귽���ؿ�(���ܪ��A��static)
	m_visitdiary.AddFun( &UpdatediraystateStatic )  ;

	//��s��O
	boost::apply_visitor(m_visitdiary , resobj) ;
}
void CResMgr::UpdatediraryRel(const Resobj &resobj)
{
	/////////////////��s��O
	//�]�w�X�ݾ��ʧ@�G��s�귽���ؿ��]���ܪ��A��Rel�^
	m_visitdiary.AddFun( &UpdatediraystateRel )  ;

	//��s��O
	boost::apply_visitor(m_visitdiary , resobj) ;

}
void CResMgr::UpdateSize(int offset) 
{
	m_size += offset ;
}

//�o�O�Ψӧﵽ�^�Ǹ귽���СA�Ӥ��Osmart point�����D�C�����귽�ϥζq
void CResMgr::RegisterShare(void *res)
{
	Sharecount::iterator iter ;

	iter = MapFind( m_count , res) ;

	if ( iter != m_count.end() )
	{
		//����쪺�ܡA�����@��
		iter->second += 1 ;
	}
	else
	{
		//�S��쪺�ܡA�إߤ@��
		std::pair<void* , unsigned int>  pairres ;

		pairres.first = res ;
		pairres.second = 1 ;

		m_count.insert(pairres) ;
	}

}

//�^�ǡGtrue�A��ܨS����L�귽�b���ɤF�C 
//		false�G����٦���L�귽�A���ɡC�άO�S�����귽
bool CResMgr::UnRegisterShare(void *res) 
{
	Sharecount::iterator iter ;

	iter = MapFind( m_count , res) ;

	if ( iter != m_count.end() )
	{
		iter->second -= 1 ;

		//��ܨS����L�귽�A����
		if ( iter->second == 0)
		{
			//�åB�����o�����

			m_count.erase(iter) ;

			return true ;
		}
	}

	return false ;
}

