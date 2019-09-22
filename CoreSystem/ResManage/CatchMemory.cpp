#include "stdafx.h"
#include "resourceMgr.h"




//�֨����O�����q�����G��s�֨��O����j�p
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


////////////			�귽�֨�		////////////////

CCatchRes::CCatchRes() :
 m_cursize(0) , m_mempool() , m_diray(new CDiary("MgrPool.txt")) , m_visitdiary(m_diray)
{

	//�HMB�����
	m_poolsize = MSIZE * 600; 
	
	//std::string table(FDO::GetConfig("CatchSize"));

	//try
	//{

	//	//�Q��config.ini��CatchSize�վ�֨��j�p�C
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
	//���귽��size + ���֨����귽���{��size
	aftersize = ressize + m_cursize ;
	
	//�O�_�W�L�֨��귽size
	if ( aftersize > m_poolsize)
	{
		//�S���Ŷ��F
		return false ;
	}
	
	return true ;
}
void CCatchRes::Push(const std::string &filename , const Resmemory &res)
{
	//��J�O�����
	m_mempool.insert(make_pair(filename , res)) ;

	/////////////////��s��O
	//�]�w�X�ݾ��ʧ@�G��s�귽�ϥζq
	m_visitdiary.AddFun( &UpdatediarytimeForCatch )  ;

	//��s��O
	boost::apply_visitor(m_visitdiary , res) ;

}
void CCatchRes::Pop( void )
{	
	assert(!m_mempool.empty() ) ;

	//���ƧǡG������ƧǪ���]�G�]�����a���[�F�A�֨��i��s��@���ܤj���귽�C�i��o�Ǹ귽
	//���a���ݭn�A�o�˷|�y�������n�����O�C
//	Sort( ) ;
	
	//�A�i��R���귽
	Clear( ) ;
}
void CCatchRes::Show( void )
{
	///////////////				���ձƧǥ�
	//CCatchtVisitOut visit ;  
	//
	////��ܩҦ��귽�����p(�ɮפj�p�A�W�١A�귽����)
	//for_each(m_mempool.begin( ) , m_mempool.end() , boost::apply_visitor(visit) ) ;



	//��ܩҦ��귽�����p(�ɮפj�p�A�ɮצW�١A�ө��֨����ơA�ɮת��A)
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
	/////////////////��s��O
	//�]�w�X�ݾ��ʧ@�G���ܸ귽���A��REL�A�P���ܧ�֨��O����Size
	m_visitdiary.AddFun( &UpdatediraystateRelForCatch )  ;

	Resmap::iterator iter(m_mempool.begin()) ;

	//��s��O
	boost::apply_visitor(m_visitdiary , iter->second );

	//�R���귽(�q�֨�������)
	m_mempool.erase(iter) ;
}

void CCatchRes::AllClear(void)
{
	size_t size = m_mempool.size() ;

	//�M�ũҦ��b�֨������귽
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

	//�ˬd�֨����A�O�_�֦����귽
	iter = MapFind(m_mempool , filename);

	return iter ;

}

//�R���Y���귽
void CCatchRes::Erase( std::map<std::string , Resmemory>::iterator iter )
{
	assert( iter != m_mempool.end() ) ;

	/////////////////��s��O
	//�]�w�X�ݾ��ʧ@�G���ܸ귽���A��REL�A�P���ܧ�֨��O����Size
	m_visitdiary.AddFun( &UpdatediraystateRelForCatch )  ;
	//��s��O
	boost::apply_visitor(m_visitdiary , iter->second );

	//�q�֨�������
	m_mempool.erase( iter ) ;


}
