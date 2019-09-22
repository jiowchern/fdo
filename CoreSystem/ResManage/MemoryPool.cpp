#include "stdafx.h"
#include "memorypool.h"

boost::shared_ptr<CMemorydiary> GetMem(void)
{
	static boost::shared_ptr<CMemorydiary> mem(new 
		//�o�̪����إߪ���귽��O�ɦW
		CMemorydiary(CObjMgrslot( &GetObjDiary("objdiary.txt") ))) ;

	return mem ;
}

CObjectdiary& GetObjDiary(const char *filename)
{
	static CObjectdiary obj(filename)  ;

	return obj ;
}


////////////////			�����ϥΰO���骺�C��

CMemorydiary::CMemorydiary(SlotType solt) :m_objs()
{
	m_signal.connect(solt) ;
}

CMemorydiary::~CMemorydiary(void)
{
	////�p�G�٦�����S��Checkout���ܡA�o�̷|��ܥX�{
	//for_each(m_objs.begin() , m_objs.end() , 
	//	boost::bind( CStreamOut() , "MemoryLeak�G " , _1 )) ;
}

void CMemorydiary::CheckIn(const std::string &objname , unsigned int objsize , void* const id)
{
	if (m_objs.find(id) != m_objs.end())
	{
		//�|�X�{�o���ΡA�O�~�Ӫ����Y�C�N���L�����U�ӡC
		std::cout << "Double-constructed memory: " << id << std::endl;
	}
	//�O���O�����}
	m_objs.insert(id) ;
	//�I�s���ѡG�s�W��O
	m_signal(true , objname , objsize) ;
}

void CMemorydiary::CheckOut(const std::string &objname , unsigned int objsize ,void* const id)
{
	if (m_objs.find(id) == m_objs.end())
	{
		//�|�X�{�o�ر��άO�������~�Ӫ����O
		std::cout << "Destroyed non-constructed memory: " << id << std::endl;

		return ; 
	}
	//�����O�����}
	m_objs.erase(id) ;
	//�I�s���ѡG������O
	m_signal(false ,objname , objsize) ;
}

bool CMemorydiary::Ok(void)
{
	return m_objs.empty() ;
}


//////////////////				���󪺤�O			///////////
CObjectdiary::CObjectdiary(void) : m_table() , m_name()
{
}

CObjectdiary::CObjectdiary(const char *filename) : m_table() , m_name(filename)
{
}

CObjectdiary::~CObjectdiary(void)
{
	//�n�}�ҰO�����ܡA����Ѯ����C
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
		//��s���󪺦��ƩM�������󪺤j�p
		boost::tuples::get<0>(iter->second) += 1 ;
		boost::tuples::get<1>(iter->second) += objsize ;
	}
	else
	{
		std::pair<std::string , Objdate> p ;
		//�إߤ@�����
		Objdate date( 1 , objsize) ;

		p.first = objname ; 
		p.second = date ;

		//�åB�s�W
		m_table.insert(p) ;
	}

}
void CObjectdiary::Erase(const std::string &objname , unsigned int objsize)
{
	Objmapiter iter ;

	iter = MapFind(m_table , objname) ;
	
	if (iter != m_table.end())
	{
		//�ˬd�O�_���ߤ@�F
		if (1 == boost::tuples::get<0>(iter->second))
		{
			//�ߤ@���ܡA�����R���a�I
			m_table.erase(iter) ;
		}
		else
		{
			//��s���󪺦��ƩM�������󪺤j�p
			boost::tuples::get<0>(iter->second) -= 1 ;
			boost::tuples::get<1>(iter->second) -= objsize ;
		}
	}
}
//�j�M�Y���󪺥X�{����
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
//�j�M�Y���󪺦Y�O���骺�`�q
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
/////////////////			����޲z��slot	/////////////////////
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
