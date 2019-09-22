#include "stdafx.h" 
#include "resourceMgr.h"




//��s�G�����귽���ϥζq
void Updatediarytime( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date)
{
	try
	{
		std::string id = boost::any_cast<std::string>(date[1]) ; //�ɦW

		//�P�_�O�_�����i��(�Q���ɦW)
		if ( !(pdiray->Check(id)) )
		{
			//�S�����ܡA���s�إߤ@��
			pdiray->Create( id , boost::any_cast<unsigned int>(date[0])  ) ;
		}

	}catch(boost::bad_any_cast &e)
	{
		Assert(0 , e.what()) ;
	}

}

//��s�G�����귽���ϥζq�ݩ�Catch
void UpdatediarytimeForCatch( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date)
{
	Updatediarytime( pdiray , date ) ;


	///////////			
	try
	{
		//��s�֨��O����size(�W�[)
		UpdateCatchSize( boost::numeric_cast<int>(
			boost::any_cast<unsigned int>(date[0]))) ;

	}catch(boost::bad_numeric_cast &e)
	{
		Assert(0 , e.what()) ;
	}
		 	
}

//��s�G�����귽���ϥζq�ݩ�귽�޲z
void UpdatediarytimeForResMgr( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date)
{
	Updatediarytime( pdiray , date ) ;


	///////////			
	try
	{
		//��s�귽�޲z�O����size(�W�[)
		UpdateResMgrSize( boost::numeric_cast<int>(
			boost::any_cast<unsigned int>(date[0]))) ;

	}catch(boost::bad_numeric_cast &e)
	{
		Assert(0 , e.what()) ;
	}
		 	
}

//��s�G�ܧ�귽���A�GRelease
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

//��s�G�ܧ�귽���A�GStatic
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

//��s�G�ܧ�귽���A�ݩ�Catch
void UpdatediraystateRelForCatch( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date)
{

	UpdatediraystateRel( pdiray , date ) ;

	/////////////////
	try
	{
		//��s�֨��O����size(����)
		UpdateCatchSize( -(boost::numeric_cast<int>(
			boost::any_cast<unsigned int>(date[0])))) ;

	}catch(boost::bad_numeric_cast &e)
	{
		Assert(0 , e.what()) ;
	}

}

//��s�G�R����O�ݩ�귽�޲z
void UpdatediraystateForResMgr( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date)
{
	try
	{
		//������		
		pdiray->Erase(boost::any_cast<std::string>(date[1])) ;


	}catch(boost::bad_any_cast &e)
	{
		Assert(0 , e.what()) ;
	}

	

	/////////////////
	try
	{
		//��s�֨��O����size(����)
		UpdateResMgrSize( -(boost::numeric_cast<int>(
			boost::any_cast<unsigned int>(date[0])))) ;

	}catch(boost::bad_numeric_cast &e)
	{
		Assert(0 , e.what()) ;
	}

}
/////////////////////			�귽��O		//////////////////////

CDiary::CDiary(void) : m_diary() , m_filename() 
{
}

CDiary::CDiary( const std::string &filename ) : m_diary() , m_filename(filename) 
{
}
CDiary::~CDiary( void )
{
	//�ݭn�O�����ܡA����ѵ����}
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
		m_o << "�ɮפj�p�G" << (iter->second).get<0>() << " Byte" << "		"  ; //unsigned int 
		m_o << "�ɦW�G" << iter->first << "		"  ;				//std::string
		m_o << "���ơG" << (iter->second).get<1>() << "		"  ;	//unsigned int 
		m_o << "���A�G" << (iter->second).get<2>() << "		"  ;	//ReState

		m_o << endl ;

		iter++; 
	}
}
bool CDiary::Check(const std::string &id )
{
	Diarymap::iterator iter ;

	iter = MapFind( m_diary , id ) ;

	//�����ܡA�惡�i���W�[����
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

	//���s�إߤ@��
	p.first = id ;
	//�ɮפj�p�A�ϥΦ��ơA���A�GUSE
	p.second = boost::make_tuple(filesize , 1 , USE) ;

	m_diary.insert( p ) ;
}

void CDiary::SetState( const std::string &id , ResState state)
{
	Diarymap::iterator iter ;

	iter = MapFind( m_diary , id ) ;

	//�����ܡA�惡�귽���ܪ��A
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
		//������
		m_diary.erase(iter) ;
	}


}
//�N�Ҧ��귽�]�w:REL�C���L�A�ݭn�ܡH
void CDiary::Delevery( void )
{	
	Diarymap::iterator iter = m_diary.begin() ;

	while (iter != m_diary.end() )
	{
		(iter->second).get<2>() = REL ;

		cout << "test�G " << (iter->second).get<2>() << endl ;

		
		iter++; 
	}		
}
