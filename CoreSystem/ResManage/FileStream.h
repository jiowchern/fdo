#ifndef _FILESTREAM_H_
#define _FILESTREAM_H_
#include <boost/tuple/tuple.hpp>
#include <fstream>
#include <string>
#include <set>

using namespace std ;

class CStreamWrite
{
public:
	CStreamWrite(ostream *o) 
	{
		m_o = o;
	}
public:
	typedef void result_type ;
public:
	template<typename T>
	void operator()(const T &iter)
	{
		if ( m_o == NULL)
		{
			return ;
		}
		
		(*m_o) << *iter  ;
		(*m_o) << "\n" ; 
	}
	template<>
	void operator()(const long &value)
	{
		if ( m_o == NULL)
		{
			return ;
		}

		(*m_o) << "剩餘實體記憶體大小： " << value  << "MB" << endl ;
	}
	template<>
	void operator()(const std::string &word)
	{
		if ( m_o == NULL)
		{
			return ;
		}
		(*m_o) << word ;
		(*m_o) << "\n" ; 
	}
	template<typename Key , typename T>	
	void operator()(const pair<Key , T> &p )
	{

	////物件資料屬性：數量，資料大小
	//typedef boost::tuple<unsigned int , unsigned int>			Objdate ;

	////物件查表：物件名稱，物件資料屬性 
	//typedef std::map<std::string , Objdate >					Objmap ;


		(*m_o) << "物件種類： " << p.first  << "		"  ; 

		(*m_o) << "呼叫次數： " << boost::tuples::get<0>(p.second) << "		" ; 

		(*m_o) << "記憶體消耗： " << boost::tuples::get<1>(p.second) <<  "Byte" << endl ;

	}
private:
	ostream						*m_o ;
};


//包含：快取，資源管理的Pool，遺失資源，物件管理記錄
class CFilestrem
{
public:
	CFilestrem(void) ;
	CFilestrem( const std::string &file );
	~CFilestrem( void );
public:
	void Wirte(const std::string &word);
public:
	std::ofstream 					m_file;	
private:
	std::set<std::string>			m_date ;
	std::string						m_filename ;

};

//記憶體計數器
class CMemoryCount
{
public:
	CMemoryCount(const std::string &filename) ;
	CMemoryCount(void) ;
	~CMemoryCount(void) ;
public:
	void Compute(float frametime , float time) ;
private:
	std::string						m_file ;
	std::list<long>					m_count ;
	MEMORYSTATUS					m_status ;
	std::ofstream 					m_o;	
	float							m_curtime ;
	long							m_phymem ;
};


CFilestrem& GetFileStream( const std::string &file  ) ;

CMemoryCount& GetMemCount(const std::string &filename) ;

#endif