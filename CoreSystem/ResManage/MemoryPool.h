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



//////////////				������			/////////////////
//���o����O����C��
boost::shared_ptr<CMemorydiary> GetMem(void) ;
//���o����޲z��O
CObjectdiary& GetObjDiary(const char *filename) ;


///////////////////			����޲z��Template������		///////////////////

////			���o����޲z
template<typename T>
boost::shared_ptr<CObjectMgr<T> >& GetObjMgr(void)
{
	static boost::shared_ptr<CObjectMgr<T> > obj(new CObjectMgr<T>(GetMem())) ;

	return obj ;
}

/////			�R������G�Q�θ귽����
template<typename T>
void DeleteObj(T *t)
{
	GetObjMgr<T>()->Delete(t) ;
}



////////			�N�إߥX�Ӫ�����A�ǤJ����޲z�@�޲z
template<typename T>
void CreateObj(T **t)
{
	(*t) = GetObjMgr<T>()->Create() ;
}


//���ѰO���骺�C���X��
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


////////////////			�����ϥΰO���骺�C��
class CMemorydiary
{
	typedef std::set<void*>::iterator						   setiter ;
	typedef boost::signal<void (bool , const std::string& , unsigned int)>   Signaltype ;
	typedef Signaltype::slot_type							   SlotType ;	
public:
	CMemorydiary(SlotType solt) ;
	~CMemorydiary(void);
public:
	//���J�@���O����table����
	void CheckIn(const std::string &objname , unsigned int objsize , void* const id);
	//�����@���O����table����
	void CheckOut(const std::string &objname , unsigned int objsize ,void* const id);
	bool Ok(void);
private:
	std::set<void*>					m_objs ;
	Signaltype						m_signal ;
};


////////////////			����޲z		//////////////////


////�o�̬�����n����CMemorydiary����]�G���F�T�O�C�@�Ӫ���b������޲z���R����(�t������)�A
////CMemorydiary�O�̫�@�ӳQ�R���I�ҥH�o�̱N�����ɥX�ӡC


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

		//�إߪ���
		try
		{

			
			t = new T ; 

	//		t = m_objpool.construct() ;


		}catch( std::logic_error &e )
		{
			//��إߥ��ѡA�����ҥ~
			Assert(0 , e.what() ) ;
		}

		m_objs.insert(make_pair(t , t)) ;


		//�i����U
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


		//	//�i��ϵ��U�G���@����]�O�A�Q�O���Y�Ǫ���@�Q�I�s�X���٦��Y�h�֪��O����C
		//	//�@�ϵ��U���ܡA�N�o����ڪ��ݨD�C�B�n�ˬdmemleak���ܡA�]����o�˰��C
		//	//�]����̫�Am_objpool�|���ڲM��
		//	//m_diray->CheckOut(typeid(T).name() , sizeof(T) , this) ;

		//	return true ;


		//return false ;
	}

private:
	Objmap												m_objs ; 
	boost::shared_ptr<CMemorydiary>						m_diray ;		//�O����C��
	//Poolmap												m_objpool ;
//	boost::object_pool<T>								m_objpool ;		//����O����pool   
};


//////////////////				���󪺤�O			///////////

class CObjectdiary
{
	//��޲z���ѧ@�B��
	friend class CObjMgrslot ;

	//�������ݩʡG�ƶq�A��Ƥj�p
	typedef boost::tuple<unsigned int , unsigned int>			Objdate ;
	//����d��G����W�١A�������ݩ� 
	typedef std::map<std::string , Objdate >					Objmap ;
	typedef Objmap::iterator									Objmapiter ;
public:
	CObjectdiary(void) ;
	CObjectdiary(const char *filename) ;
	~CObjectdiary(void);
private:
	//��s��O�G���J�@���������
	void Insert(const std::string &objname , unsigned int objsize);
	//��s��O�G�����@������O��
	void Erase(const std::string &objname , unsigned int objsize);
	//������ƨ�disk��
	void Save(void) ;
public:
	//�j�M�Y���󪺥X�{����
	unsigned int SerachTimes(const std::string &objid) ;
	//�j�M�Y���󪺦Y�O���骺�`�q
	unsigned int SerachSize(const std::string &objid) ;
private:
	Objmap										m_table ;
	std::ofstream 								m_file;
	std::string									m_name ;

};
/////////////////			����޲z��slot	/////////////////////
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