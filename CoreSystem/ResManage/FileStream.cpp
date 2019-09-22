#include "stdafx.h" 
#include "filestream.h"


CFilestrem& GetFileStream( const std::string &file ) 
{
	static boost::shared_ptr<CFilestrem> stream(new CFilestrem(file) ) ;

	return *stream ;
}

CMemoryCount& GetMemCount(const std::string &filename)
{
	static boost::shared_ptr<CMemoryCount> mem(new CMemoryCount(filename)) ;

	return *mem ;
}

CFilestrem::CFilestrem( void ) :m_filename() ,  m_date()
{
}

CFilestrem::CFilestrem(const std::string &file ) : m_filename(file) , m_date()
{

}

CFilestrem::~CFilestrem( void )
{

	//�ݭn�O�����ܡA����Ѯ���

	//std::string table(FDO::GetConfig("OpenCount"));


	//if (table == "true")
	//{
	//	m_file.open( m_filename.c_str() ) ;

	//	for_each(m_date.begin() , m_date.end() , CStreamWrite(&m_file)) ;

	//	m_date.clear() ;

	//	m_file.close() ;
	//}
	//else
	{
		m_date.clear() ;

	}



}

void CFilestrem::Wirte(const std::string &word)
{
	m_date.insert(word) ;

}


CMemoryCount::CMemoryCount(void) : m_file() ,m_count() , m_curtime(0) , m_phymem(0)
{
}

CMemoryCount::CMemoryCount(const std::string &filename) : m_file(filename) , m_count() , m_curtime(0) , m_phymem(0)
{
}

CMemoryCount::~CMemoryCount(void)
{

	//�ݭn�O�����ܡA����Ѯ���

	//std::string table(FDO::GetConfig("OpenCount"));



	//if(table == "true")
	//{
	//	m_o.open( m_file.c_str() ) ;


	//	m_o << "����O�����`�j�p�F " << m_phymem << "MB" << endl ;

	//	//�C�X�Ѿl������O����j�p
	//	for_each(m_count.begin() , m_count.end() , CStreamWrite(&m_o)) ;

	//	m_count.clear() ;

	//	m_o.close() ;
	//}
	//else
	{
		m_count.clear() ;
	}



}

void CMemoryCount::Compute(float frametime , float maxtime)
{

	m_curtime += frametime ;

	if (m_curtime >= maxtime )
	{

		GlobalMemoryStatus (&m_status);

		//�O���Ѿl������O����C�ഫMB���
		m_count.push_back((m_status.dwAvailPhys >> 20)) ;


		m_phymem = (m_status.dwTotalPhys >> 20); 

		m_curtime = 0.0f ;

	}

}


