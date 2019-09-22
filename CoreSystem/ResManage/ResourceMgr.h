#ifndef _RESOURCEMGR_H_
#define _RESOURCEMGR_H_

#include <boost\shared_ptr.hpp>
#include <boost\any.hpp>
#include <boost\lexical_cast.hpp>
#include <boost\cast.hpp>
#include <boost\variant.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost\scoped_ptr.hpp>
#include <boost\function.hpp>
#include <boost/utility.hpp>
#include <boost\bind.hpp>
#include <boost\signal.hpp>
#include <boost/pool/pool_alloc.hpp>

#include "stdafx.h" 
//////////////			�Q�޲z���귽*.h		/////////////////
#include "packetreader.h"     //�����Y�ʥ]��
#include "texture.h"		  //FDO::Textrue��
#include "FDO_RoleMemoryMgr.h" //sImage��
#include "JLSkinMesh.h" //JLSkinMesh��


/////////////////////////////////////////////
#include "command.h"

#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <set>

using namespace std ;

class CCatchRes ;
class CDiary ;
class CResMgr ;

template<typename T> class CD3DRes ;

void TesResource( void ) ;

void TesResMgr( void ) ;

//////////////////		�����ܼ�		////////////////
const unsigned int MSIZE = 1024 * 1024 ; //(1MB)


//////////////////		������		////////////////
//��s�֨��O����j�p
void UpdateCatchSize(int offset) ;
//��s�귽�޲z�O����j�p
void UpdateResMgrSize( int offset);
//���o�֨��O����
boost::shared_ptr<CCatchRes> GetCatch( void );
//���o�귽�޲z
CResMgr& GetResMgr( void ) ;


//////								�����ơG��s��O����			/////////

//��s�G�����귽���ϥζq
void Updatediarytime( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date);

//��s�G�����귽���ϥζq�ݩ�Catch
void UpdatediarytimeForCatch( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date);

//��s�G�����귽���ϥζq�ݩ�귽�޲z
void UpdatediarytimeForResMgr( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date);

//��s�G�ܧ�귽���A�ݩ�Catch�GRel
void UpdatediraystateRelForCatch( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date) ;

//��s�G�ܧ�귽���A�ݩ�귽�޲z
void UpdatediraystateForResMgr( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date);

//��s�G�ܧ�귽���A�GRel
void UpdatediraystateRel( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date);

//��s�G�ܧ�귽���A�GSTATIC
void UpdatediraystateStatic( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date);



/////////////////				�����ơG�귽�޲z����		//////

//�إ߸귽����(CD3DRes)
template<typename T> boost::shared_ptr<CD3DRes<T> > CreateResObj( const std::string &filename) ;

//�M��귽����(CD3DRes)
template<typename T> boost::shared_ptr<CD3DRes<T> > FindResObj( const std::string &filename , bool &isFind) ;

//�����GRenderTarget��
template<typename T> boost::shared_ptr<CD3DRes<T> >CreateResObj( const std::string &filename , 
														  unsigned int width , unsigned int height) ;

//Ū���귽��귽�޲z�Gtemplate<�귽���� , �귽�������>
template<typename Type , typename TypeObj> Type* LoadResource(const char *filename );

//template�����G�W�[��J����e�A�إߤ@�Ӫť�Texture�CRenderTarget��
template<typename Type , typename TypeObj> Type* LoadResource(const char *filename ,
														unsigned int width , unsigned int height);

//template<�귽���� , �귽�������>  �|�ǤJ��thread�ҫإߪ�Memory Packet
template<typename Type , typename TypeObj>Type* LoadResource(const char *filename , FDO::memPack *pMem) ;

//Ū���귽��귽�޲z�Gtemplate<�귽���� , �귽�������>
template<typename Type , typename TypeObj> Type* FindResource(const char *filename );


//�q�귽�޲z���R���귽�]�Q�θ귽���С^
template<typename RES> void ReleaseResource(RES res);

//�S�ơG�q�귽�޲z���R���귽�]�Q���ɦW�^
//template<> void ReleaseResource(const char *filename);

//�q�귽Pool���A�M��귽�A�åB�^�Ǹ귽����(T*)(�p�Gstd::string*)
template<typename T> T* FindResFromResPool(const std::string &filename) ;



bool CheckResource(const char *filename) ;
/////////////////				�����ơGŪ�����Y�ʥ]����		//////

boost::shared_ptr<FDO::memPack> GetMemForPacket(const std::string &name ,unsigned int &resize);

/////////////////				�����ơG�q�O����϶����Texture		//////

//�ĤT�ӰѼƩM�ĥ|�ӰѼƬ�out
void GetTexFormMem(unsigned char* data , unsigned int DataSize , 
				   unsigned int &resize , IDirect3DTexture9 **tex);

//�ĤT�ӰѼƩM�ĥ|�ӰѼƬ�out
void GetTexFormMemEx(unsigned char* data , unsigned int DataSize , 
				   unsigned int &resize , IDirect3DTexture9 **tex);

//�إߤ@�Ӫť�Texture(RenderTarget��)
void GetTexture(unsigned int &resize , unsigned int width , unsigned int height , 
				IDirect3DTexture9 **tex);

/////////////////				��L�p�u��B��


//�q�֨��O���餤�A�M��귽
template<typename T>
bool FindResInCatch(const std::string &filename , CD3DRes<T> *res) ;

//�����G�q�֨��O���餤�A�M��귽
bool FindResInCatch(const std::string &filename) ;

//���otexture���귽�O����j�p
void GetTextrueResSize(unsigned int &resize , IDirect3DTexture9 **tex) ;

////////////////////////�R�O�ܰʳ����G�귽����		 ///////////////  

//�ۭq�귽�����Glogo��Ū��texture�귽�ΡC�إߦ��O���F�F���R�A�h�����γB
struct stLogtexture
{
	stLogtexture(void)
	{
		tex = NULL ;
	}
	~stLogtexture(void)
	{
		if (tex != NULL)
		{
			tex->Release() ;
			tex = NULL ;
		}
	}
	IDirect3DTexture9				*tex ;
};

//�ۭq�귽�����Gskybox�귽���
typedef boost::tuple<LPD3DXMESH , std::vector<D3DMATERIAL9> , std::vector<LPDIRECT3DTEXTURE9> , 
					 DWORD>																	SkyboxData ;



//�귽���j�p�B�귽�ɦW�B�귽����
//sImage����
typedef boost::tuple<unsigned int , std::string , boost::shared_ptr<FDO::sImage> >			ResImage ;

//Texture
typedef boost::tuple<unsigned int , std::string , boost::shared_ptr<IDirect3DTexture9> >	ResTextrue ;

//Texture�Glogo�ϥ�
typedef boost::tuple<unsigned int , std::string , boost::shared_ptr<stLogtexture> >			ResLogTextrue ;

//Texture�GRenderTarget��
typedef boost::tuple<unsigned int , std::string , boost::shared_ptr<IDirect3DTexture9> >	ResTextarget ;


//X file�]��¥u�����^
typedef boost::tuple<unsigned int , std::string , boost::shared_ptr<ID3DXMesh> >			ResXfile ;

//JLSkinMesh(Ū�a��Mesh��)
typedef boost::tuple<unsigned int , std::string , boost::shared_ptr<FDO::JLSkinMesh> >		ResSkinMesh ;

//JLSkyBox�]Ū���ѪŲ��Ϊ���ơ^
typedef boost::tuple<unsigned int , std::string , boost::shared_ptr<SkyboxData> >			ResSkybox ;


//�@�몺�O����϶����
typedef boost::tuple<unsigned int , std::string , boost::shared_ptr<FDO::memPack> >			ResMemdata ;

//����FDO::sRoleActionList������귽����
//typedef boost::tuple<unsigned int , std::string , boost::shared_ptr<CShareAction> >			ResActionlist ;

//�귽�����G
//	1.�O����϶���� 
//	2.sImage 
//	3.X file�]��¥u������^
//	4.Texture
//  5.Ū��Log�ϥΪ�Texture
//	6.���SkinMesh�귽
//	7.�ѪŲ�
//  8.�إߪť�Texture(�ݩ�RenderTarget)  (no use)
//	9.FDO::sRoleActionList������귽����

typedef boost::variant<ResMemdata , ResImage , ResXfile ,
					ResTextrue , ResLogTextrue , ResSkinMesh , 
					ResSkybox , ResTextarget>								Resmemory; 

/////////////				�귽�޲z���R�O�ܰʳ����G�귽����		////////////////////////

//sImage����
typedef boost::shared_ptr<CD3DRes<FDO::sImage> >			ResImageObj ;	

//X file�]��¥u�����^
typedef boost::shared_ptr<CD3DRes<ID3DXMesh> >				ResXfileObj ;

//JLSkinMesh(Ū�a��Mesh��)
typedef boost::shared_ptr<CD3DRes<FDO::JLSkinMesh> >		ResSkinMeshObj ;

//Texture
typedef boost::shared_ptr<CD3DRes<IDirect3DTexture9> >		ResTextrueObj ;

//Texture�GRenderTarget��
typedef boost::shared_ptr<CD3DRes<IDirect3DTexture9> >		ResTextargetObj ;

//�ѪŲ�
typedef boost::shared_ptr<CD3DRes<SkyboxData> >				ResSkyboxObj ;

//Texture�Glogo�ϥ�
typedef boost::shared_ptr<CD3DRes<stLogtexture> >			ResLogTextrueObj ;

//�O����϶�����
typedef boost::shared_ptr<CD3DRes<FDO::memPack> >			ResMemObj ;

//FDO::sRoleActionList������귽����
//typedef boost::shared_ptr<CD3DRes<CShareAction> >			ResActlistObj ;


//�귽���󶰦X�G
//	1.�O����϶�����
//	2.sImage���� 
//	3.X file�]��¥u������^
//	4.Texture
//	5.Ū��Log�ϥΪ�Texture
//	6.���SkinMesh�귽
//	7.�ѪŲ�
//  8.�إߪť�Texture(�ݩ�RenderTarget)
//	9.FDO::sRoleActionList������귽����

typedef	boost::variant<ResMemObj , ResImageObj , ResXfileObj ,
					   ResTextrueObj , ResLogTextrueObj , ResSkinMeshObj ,
					   ResSkyboxObj, ResTextargetObj>			Resobj ;

//�귽 map
typedef std::map<std::string , Resobj>						ResObjmap;

//////////////				�귽�����G�R����		///////////////////

class CD3DFilter
{
public:
	//�@�몫��delete
	template<typename T>
	void operator( ) (T *res)
	{
		if ( res != NULL )
		{
			res->Release() ; 
			res = NULL ;
		}
	}
	//�S�ơG��O����϶��@�S���R���覡
	template<>
	void operator( ) (FDO::memPack *mem);
	//�S�ơG��sImage���R���覡
	template<>
	void operator( ) (FDO::sImage *image);
	//template<>
	//void operator()(SkyboxData *sky) ;
	//template<>
	//void operator()(stLogtexture *logo) ;
	template<>
	void operator()(FDO::JLSkinMesh *skin) ;
};
////////////////////	�귽�����GŪ���귽�覡		////////////////

//Ū���귽�G�@��
template<typename T>
boost::shared_ptr<T> RessourceLoad(const std::string &name , unsigned int &resize)
{
	
	boost::shared_ptr<T> res(new T , CD3DFilter()) ;
	//��������j�p
	resize = sizeof(T) ;

	return res ;

}

//Ū���귽�G�@��
template<typename T>
boost::shared_ptr<T> RessourceLoad(const std::string &name , FDO::memPack *pMem , unsigned int &resize)
{
	
	boost::shared_ptr<T> res(new T , CD3DFilter()) ;
	//��������j�p
	resize = sizeof(T) ;

	return res ;

}

//Ū���귽�G�@��A�h�W�[�ѼơC���F�S�ƥX��
template<typename T>
boost::shared_ptr<T> RessourceLoad(unsigned int &resize , unsigned int width ,unsigned int height)
{
	
	boost::shared_ptr<T> res(new T , CD3DFilter()) ;
	//��������j�p
	resize = sizeof(T) ;

	return res ;

}



//Ū���귽�G�S�ơATexture
template<>
boost::shared_ptr<FDO::sImage> RessourceLoad(const std::string &name , unsigned int &resize);

//Ū���귽�G�S�ơATexture(Thread��)
template<>
boost::shared_ptr<FDO::sImage> RessourceLoad(const std::string &name , FDO::memPack *pMem ,  unsigned int &resize);

//Ū���귽�G�S�ơAX file�]��¥u������^
template<>
boost::shared_ptr<ID3DXMesh> RessourceLoad(const std::string &name , unsigned int &resize);

//Ū���귽�G�S�ơASkinMesh�귽
template<>
boost::shared_ptr<FDO::JLSkinMesh> RessourceLoad(const std::string &name , unsigned int &resize);


//Ū���귽�G�S�ơAskybox�귽
template<>
boost::shared_ptr<SkyboxData> RessourceLoad(const std::string &name , unsigned int &resize);



//Ū���귽�G�S�ơATexture
template<>
boost::shared_ptr<IDirect3DTexture9> RessourceLoad(const std::string &name , unsigned int &resize);

template<>
boost::shared_ptr<IDirect3DTexture9> RessourceLoad(const std::string &name , FDO::memPack *pMem ,  unsigned int &resize);

//Ū���귽�G�S�ơATexture(RenderTarget)
template<>
boost::shared_ptr<IDirect3DTexture9> RessourceLoad(unsigned int &resize ,
												   unsigned int width ,
												   unsigned int height);


//Ū���귽�G�S�ơAŪ��log's Texture
template<>
boost::shared_ptr<stLogtexture> RessourceLoad(const std::string &name , unsigned int &resize);

//Ū���귽�G�S�ơA�O����϶�
template<>
boost::shared_ptr<FDO::memPack> RessourceLoad(const std::string &name , unsigned int &resize);

//Ū���귽�G�S�ơA�O����϶�(Thread��)
template<>
boost::shared_ptr<FDO::memPack> RessourceLoad(const std::string &name , FDO::memPack *pMem , unsigned int &resize);

////////////			�귽��Ƭ����G�s����		///////////////////////
class CResDate
{
public:
	CResDate( const std::string &str , const boost::any &value):m_type(str) , m_value(value)
	{
	}
	~CResDate( void )
	{
	}
public:
	const std::string& GetType( void )
	{
		return m_type ;
	}
	boost::any Value( void )
	{
		return m_value ;
	}
private:
	std::string				m_type ;
	boost::any				m_value ;
};

////////////			�귽��Ƭ����G���X���		///////////////////////

class CResTypeFilter
{
public:
	CResTypeFilter( std::string &type):m_type(type)
	{

	}
	~CResTypeFilter( void )
	{

	}
public:
	bool operator() (CResDate &date)
	{
		if ( m_type == date.GetType( ))
		{
			return true ;
		}

		return false ;
	}
private:
	std::string			m_type ;
};

////////////			�귽��Ƭ����G���X���(�֨���)		///////////////////////

//�q�֨��O���餤�A�M��귽
template<typename T>
bool FindResInCatch(const std::string &filename , CD3DRes<T> *res)
{
	std::map<std::string , Resmemory>::iterator iter ;

	iter =  GetCatch()->Find(filename )  ; 

	if (iter != GetCatch()->m_mempool.end() )
	{
		CCatchtVisitFind<T> visit(res) ;

		//��쪺�ܡA��귽�ᵹCD3DRes
		boost::apply_visitor(visit , iter->second ) ;

		//�귽�w�g�ٵ�CD3DRes�F�A�ҥH�n�b�֨���������
		GetCatch()->Erase( iter ) ;

		//�����
		return true; 
	}

	//�S���
	return false ;

}

////////////			�귽�����G�إ߸귽����(CD3DRes)		///////////////////////

template<typename T>
boost::shared_ptr<CD3DRes<T> > CreateResObj( const std::string &filename)
{
	boost::shared_ptr<CD3DRes<T> > obj(new CD3DRes<T> ) ;

	obj->Load( filename ) ;

	return obj ;
}

//�M��귽����(CD3DRes)
template<typename T> boost::shared_ptr<CD3DRes<T> > FindResObj( const std::string &filename , bool &isFind)
{
	boost::shared_ptr<CD3DRes<T> > obj(new CD3DRes<T> ) ;

	isFind = obj->Find( filename ) ;

	return obj ;
}

////////////			�귽�����G�S�ƫإ�Texture�귽(RenderTarget��)		///////////////////////

//RenderTarget��
template<typename T> 
boost::shared_ptr<CD3DRes<T> > CreateResObj( const std::string &filename , 
											unsigned int width , unsigned int height)
{
	boost::shared_ptr<CD3DRes<T> > obj(new CD3DRes<T> ) ;

	obj->Load( filename , width , height) ;

	return obj ;

}
////////////			�귽�����G�S�ƫإ�Texture�귽(�䴩thread)		///////////////////////
template<typename T>
boost::shared_ptr<CD3DRes<T> > CreateResObj( const std::string &filename , FDO::memPack *pMem )
{
	boost::shared_ptr<CD3DRes<T> > obj(new CD3DRes<T> ) ;

	obj->Load( filename , pMem ) ;

	return obj ;
}


///////////////			�귽���O		///////////////


//�\��G
//			1.Ū���귽
//			2.����귽���
//			3.�귽�R���ɡ]�ߤ@�ʡ^�A��J�֨��O���餤�C

template<typename T>
class CD3DRes
{
	typedef boost::function<void ()>							LoadResFun ; 
public:
	CD3DRes( void ) : m_size(0) , m_vecdate()
	{
	}
	~CD3DRes( void )
	{
		if(strcmp(typeid(T).name()  , "class FDO::JLSkinMesh") == 0)
		{
			return ; 
		}

		//�P�_���귽�O�_�ߤ@�A�]���O���ߤ@���ܡA�h�n�b�R���귽���e�A��J�֨��O���餤
		if ( m_texptr.unique() )
		{
			//�ˬd�O�_�٦��֨��Ŷ�
			if ( GetCatch()->Check( m_size ) )
			{
				//����٦��Ŷ��A�i�H��J�֨��Ŷ�
				boost::tuple<unsigned int , std::string , boost::shared_ptr<T> > 
					res(m_size , m_filename , m_texptr) ;

				GetCatch()->Push( m_filename , res ) ;				
				
			}
			else
			{
				//�S������֨��Ŷ�
				
				//�i��R���ʧ@
				GetCatch()->Pop( ) ;
			}
		}
	}
public:
	bool Find(const std::string &name) 
	{
		return FindResInCatch<T>(name , this ) ; 
	}
	void Load(const std::string &name) 
	{
		//���M��֨��O���餤�A�O�_�s�b


		if (FindResInCatch<T>(name , this ) )
		{			//�����
			return ;
		}

		//�ɦW
		m_filename = name ;
		//�귽
		m_texptr = RessourceLoad<T>( name , m_size) ;

	}
	void Load(const std::string &name , unsigned int width , unsigned int height)
	{

		if (FindResInCatch<T>(name , this ) )
		{			//�����
			return ;
		}

		//�ɦW
		m_filename = name ;
		//�귽
		m_texptr = RessourceLoad<T>( m_size , width , height) ;

	}
	//�䴩thread�Ϊ�
	void Load(const std::string &name , FDO::memPack *pMem)
	{
		if (FindResInCatch<T>(name , this ) )
		{			//�����
			return ;
		}

		//�ɦW
		m_filename = name ;
		//�귽
		m_texptr = RessourceLoad<T>( name , pMem , m_size) ;
	}
	void SetResource(const unsigned int filesize , const std::string &filename ,const boost::shared_ptr<T> res)
	{
		m_texptr = res ;
		
		m_size = filesize ;

		m_filename = filename ;
	}
	const std::string& Filename( void ) const
	{
		return m_filename ;
	}
	unsigned int Filesize( void ) const
	{
		return m_size ;
	}
	T* Get(void) const			//���o�귽
	{
		return m_texptr.get() ;
	}
private:
	boost::shared_ptr<T>		m_texptr ;			//�s��귽
	std::vector<CResDate>		m_vecdate ;			//�����C�@�ظ귽���
	unsigned int				m_size ;			//���귽���j�p
	std::string					m_filename ;		//�귽�ɦW
};


////////////			�귽��O		////////////////

//��O���O�Gmap���e<�ɮצW�� , tuple���e>
//tuple���e�G�귽�j�p�A�b�֨��ө������ơ]�C��Load to catch �[1�^�A�귽���A�A
class CDiary
{
	//��O���D�n�\�ΡG
	friend void Updatediarytime( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date) ;
	friend void UpdatediraystateRel( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date) ;
	friend void UpdatediarytimeForResMgr( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date) ;
	friend void UpdatediraystateForResMgr( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date);
	friend void UpdatediraystateStatic( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date) ;
	//��O���귽���A
	enum ResState			
	{
		USE ,			//�ϥΤ�
		REL ,			//�R��
		STATIC			//�R�A
	};

	//��O���R�O�ܼ��ܧ�
	typedef boost::tuple<unsigned int , unsigned int , ResState>	DiraryMessage ;
	typedef std::map<std::string , DiraryMessage>					Diarymap ;
	typedef std::pair<std::string , DiraryMessage>					Diarypair ;
public:
	CDiary(void ) ;
	CDiary(const std::string &filename ) ;
	~CDiary( void );
private:
	CDiary( const CDiary&) ;
	CDiary& operator= (const CDiary&) ;
private:
	bool Check(const std::string &id );							//�ˬd�O�_�s�b�A�åB�֥[����
	void Create(const std::string &id , unsigned int filesize);	//�إߤ@����ƥؿ�
	void SetState( const std::string &id , ResState state);		//�]�w�Y�@����ƪ��A
	void Erase(const std::string &id) ;							//�M���Y�@�����
	void Delevery( void );//�N�Ҧ��귽�]�w:REL�C���L�A�ݭn�ܡH
	void Show( void );
private:
	Diarymap							m_diary ;
	std::ofstream						m_o ;
	std::string							m_filename ;
};





//�֨��귽�X�ݾ�(��O�G1.�����귽���ϥζq 2.�ܧ�귽���A��REL)
class CCatchVisitDiary : public boost::static_visitor<void>
{
	typedef boost::function<void (boost::shared_ptr<CDiary> , 
		const std::vector<boost::any>& ) > Fundiray ; 

public:
	CCatchVisitDiary(boost::shared_ptr<CDiary> pdiray) : m_pdiray(pdiray)
	{
	}
	~CCatchVisitDiary( void )
	{
	}
public:
	template<typename FUN>
	void AddFun( FUN *t )
	{
		m_pfun = t ;
	}
public:
	template<typename T>
	void operator() (const boost::tuple<unsigned int , std::string , T> &t)	//����������tuple
	{


		if (m_pfun)
		{
			std::vector<boost::any> vecany ;

			vecany.push_back( boost::tuples::get<0>(t) ) ;//�귽�j�p
			vecany.push_back( boost::tuples::get<1>(t) ) ;//�ɦW

		//	�]�i�H�o�˼g�A���O�������T
		//	vecany.push_back( t.get<0>() ) ;		//�귽�j�p
		//	vecany.push_back( t.get<1>() ) ;		//�ɦW

			//�p�G����ƫ��Ъ��ܡA�h�I�s�C
			m_pfun(m_pdiray , vecany) ;
		}
	}
	template<typename T>				//�����Ҧ�boost::shared_ptr<CD3DRes<T>>
	void operator() (const boost::shared_ptr<CD3DRes<T> > &t)
	{
		if ( m_pfun )
		{
			std::vector<boost::any> vecany ;

			vecany.push_back( t->Filesize() ) ;//�귽�j�p
			vecany.push_back( t->Filename() ) ;//�ɦW

			//�p�G����ƫ��Ъ��ܡA�h�I�s�C
			m_pfun(m_pdiray , vecany) ;
		}
	}
private:
	Fundiray							m_pfun ;
	boost::shared_ptr<CDiary>			m_pdiray;
};


//�֨��귽�X�ݾ�(�ƧǥΡG�O����j�p�C�Ѥp��j)  
class CCatchVisitSort : public boost::static_visitor<bool>
{
public:
	template<typename LHS , typename RHS>
	bool operator() (const LHS &lhs , const RHS &rhs)
	{
		if ( lhs.get<0>() < rhs.get<0>() )
		{
			return true ;
		}

		return false ;
	}

};

//�֨��귽For STL's iterator(�Q��T(std::string�ˬd�O�_�����귽�s�b��֨��O���餤)  
template<typename T>
class CCatchvisitEqual : public boost::static_visitor<bool>
{
public:
	CCatchvisitEqual(const T t) : m_t(t)
	{
	}
	~CCatchvisitEqual( void )
	{
	}
public:
	//��boost::tuple<unsigned int , std::string , U>
	template<typename U>
	bool operator() (const boost::tuple<unsigned int , std::string , U> &res) 
	{
		T t = boost::tuples::get<1>(res) ; // std::string �ɦW

		//�����䴩operator==
		if (m_t == t)
		{
			return true ;
		}

		return false ;
	}
	template<typename U>
	bool operator() (const boost::shared_ptr<CD3DRes<U> > &res) //��CD3DRes<U>
	{
		//����ɦW
		if ( m_t == res->Filename() )
		{
			return true ;
		}

		return false ;
	}
	bool operator() (const std::string &filename)
	{
		if (m_t == filename)
		{
			return true ;
		}
		return false ;
	}
private:
	 T				m_t ; 
};




//�֨��귽�X�ݾ��]�N�귽�ᵹ�귽���O�ϥΡ^
template<typename T>
class CCatchtVisitFind : public boost::static_visitor<void>
{
public:
	CCatchtVisitFind(CD3DRes<T> *res) : m_res(res)
	{

	}
public:
	template<typename U>
	void operator() (const U &res)
	{
		assert( m_res != NULL ) ;

		try
		{

		m_res->SetResource(boost::tuples::get<0>(res) ,			//�ɮפj�p
							 boost::tuples::get<1>(res) ,			//�ɦW
							 //����귽�૬�쥿�T���귽
							 boost::any_cast<boost::shared_ptr<T>>
							 (boost::tuples::get<2>(res)) ) ;		//�귽

		}catch (boost::bad_any_cast &e)
		{
			Assert(0 , e.what() ) ;
		}
	}
private:
	CD3DRes<T>				*m_res ;
};


////////////			�귽�֨�		////////////////

//�֨��D�n�ʧ@�G�C���귽�]�ߤ@�^�R���ɡA�Ұʥ�J�֨��A�ΧR���귽�����֨��C
class CCatchRes
{
	//�֨����O�����q�����G��s�֨��O����j�p
	friend void UpdateCatchSize(int offset) ;
	//�֨���귽���O�]CD3DRes�^�����q����
	template<typename T> friend bool FindResInCatch(const std::string &filename , CD3DRes<T> *res) ;
	//�֨���귽���O�]CD3DRes�^�����q����
	friend bool FindResInCatch(const std::string &filename) ;

	typedef std::map<std::string , Resmemory>					Resmap ;	

public:
	CCatchRes() ;
	~CCatchRes( void ) ;
private:
	CCatchRes (const CCatchRes&) ;
	CCatchRes& operator= (const CCatchRes&) ;
public:
	/////					�����G���u��\��					//////////
	bool Check( unsigned int ressize);												//�ˬd�֨��Ŷ�
	void Push(const std::string &filename , const Resmemory &res);					//�N�귽��J�֨�
	void Pop( void );																//�N�귽��X�֨�
	void Show( void );																//��ܩҦ��귽����
	void AllClear(void) ;															//�M�ũҦ��֨��귽
public:
	/////////				�����G���o�����ݩ�			//////////
	unsigned int MaxSize( void ) const ;				//���o���֨����̤j�Ŷ�
	unsigned int Size( void ) const ;					//���o���֨����{���Ŷ�
private:
	/////					�p���G���u��\��					//////////
	std::map<std::string , Resmemory>::iterator Find(const std::string &filename) ;			//�M��귽�O�_�b�֨���
	void Erase( std::map<std::string , Resmemory>::iterator iter ) ;	//�R���֨����Y���귽
//	void Sort( void );									//�ƧǡG�̸귽�j�p
	void Clear( void );									//�]�w�귽���GRelease
	void UpdateSize(int offset);						//��s�֨���Size
private:
	Resmap								m_mempool ;		//�O�����
	boost::shared_ptr<CDiary>			m_diray ;		//��O
	CCatchVisitDiary					m_visitdiary ;	//��O�M�ݪ��X�ݾ�
	unsigned int						m_poolsize ;	//�O������j�p
	unsigned int						m_cursize ;		//�O����{�����j�p
};


/////////////				�귽�޲z			////////////////////////

//�귽���дM��(�Q���ܼƪ����ФU�h���)
template<typename U>
class CResMgrvisitEqualpoint : public boost::static_visitor<bool>
{
public:
	CResMgrvisitEqualpoint(U t) : m_t(t)
	{
	}
	~CResMgrvisitEqualpoint( void )
	{
	}

public:
	template<typename T>
	bool operator() (const boost::shared_ptr<CD3DRes<T> > &res) 
	{
		if ( m_t == (void*)res->Get() )
		{
			return true ;
		}
		return false ;
	}
private:
	 U				m_t ; 
};

//�^�Ǹ귽�����СA�qboost::variant
template<typename T>
class CResMgrvisitGetRes : public boost::static_visitor<T*>
{
public:
	T* operator()(const boost::shared_ptr<CD3DRes<T> > &res)
	{
		return (*res).Get() ;
	}
	//�����覡�G�u���oT�������귽
	template<typename U>
	T* operator()(const boost::shared_ptr<CD3DRes<U> > &res)
	{
		return NULL ;
	}
};

//�^�Ǹ귽�����СA�qboost::variant�C(���ޤ����������귽)
class CResMgrvisitGetAllRes : public boost::static_visitor<void*>
{
public:
	template<typename T>
	void* operator()(const boost::shared_ptr<CD3DRes<T> > &res)
	{
		return (*res).Get() ;
	}
};


//�q�귽Pool���A�M��귽�A�åB�^�Ǹ귽����(T*)(�p�Gstd::string*)
template<typename T>
T* FindResFromResPool(const std::string &filename)
{

	ResObjmap::iterator iter ;

	//���q�R�A��_
	iter = GetResMgr().FindResForStatic( filename) ;

	if ( iter != GetResMgr().m_staticres.end() )
	{	
		//��s��O�G��s�귽�y�ʶq
		GetResMgr().UpdatediaryTime( iter->second ) ;

		CResMgrvisitGetRes<T> visit ;

		return boost::apply_visitor( visit , iter->second) ;
	}		

	//�A�ѰʺA��_
	iter = GetResMgr().FindResForDyanmic( filename) ;


	if(  iter != GetResMgr().m_resobjmap.end() )
	{
		//��s��O�G��s�귽�y�ʶq
		GetResMgr().UpdatediaryTime( iter->second  ) ;

		CResMgrvisitGetRes<T> visit ;
	
		///////			����쪺�ܡA�i����U�C�O�o�I�u���ʺApool�~��		////////

		T *res = boost::apply_visitor( visit , iter->second) ;

		//�w���P�ɦW�A���O���P�귽
		//cosiann
		//if ( res != NULL)
		//{
		//	GetResMgr().RegisterShare(res) ;
		//}


		return res ;

		//return boost::apply_visitor( visit , *iter) ;

	}

	return NULL ;

}

//�q�귽�޲z���R���귽�]�Q�θ귽���С^
template<typename RES>
void ReleaseResource(RES res)
{
	///////	�|��b�o�̪���]�G�[�t�P�_�O�_�n�����귽���l�Ӯɶ�		/////////

	////////			������ɭp�ƾ��A�@�ϵ��U		/////////////

	//cosiann
	//bool share = GetResMgr().UnRegisterShare(res) ;
	//if ( !share )
	//{
	//	//�٦���L�귽�A���ɪ��ܡA�����^�ǡC
	//	return ;
	//}


	/////////		�S����L�귽�A���ɩΨS�����귽�A��ʺApool�h�M��C�åB�R���귽		/////
	ResObjmap::iterator iter(GetResMgr().m_resobjmap.begin()) ;

	//�Q�θ귽���Хh���
	CResMgrvisitEqualpoint<void*> visit(res) ;

	while( iter != GetResMgr().m_resobjmap.end())
	{
		if ( boost::apply_visitor(visit , iter->second))
		{
			break ;
		}

		iter++ ; 
	}

	if ( iter == GetResMgr().m_resobjmap.end() )
	{
		//�S����쪺�ܡA�N���X
		return ;
	}
	
	//��쪺��
	GetResMgr().ReleaseResource( iter ) ;
}


//Ū���귽��귽�޲z�Gtemplate<�귽���� , �귽�������>
template<typename Type , typename TypeObj> Type* FindResource(const char *filename )
{
	ResObjmap::iterator iter ;

	////////////////	���q���귽�޲z�M��귽		/////////////////

	Type *res = FindResFromResPool<Type>( filename ) ;
	//����쪺�ܡA�����^��
	if ( res != NULL )
	{
		return res ;
	}

	//////////			�b�q�֨��O�餤�M��A�S���ɡA���|�ۦ�إ߸귽����	////////////////////

	bool isFind = false ; 

	TypeObj obj = FindResObj<Type>(filename , isFind) ; 
	
	if(isFind)
	{
		///////			�b�֨����A����쪺��			/////////

		//�إ߸귽����
		Resobj resobj(obj) ;

		//�N�귽��J�귽Pool��
		GetResMgr().PushToPool( resobj , filename ) ;
		
		return obj->Get() ;
	}
	
	return NULL ; 

}


//template<�귽���� , �귽�������>
template<typename Type , typename TypeObj>
Type* LoadResource(const char *filename) 
{
	ResObjmap::iterator iter ;

	////////////////	���q���귽�޲z�M��귽		/////////////////

	Type *res = FindResFromResPool<Type>( filename ) ;
	//����쪺�ܡA�����^��
	if ( res != NULL )
	{
		return res ;
	}


	//////////			�b�q�֨��O�餤�M��A�Φۤv�إ�	////////////////////

	//�إ�Texutre�귽(�إ߹L�{�A�M��֨��O������A�ˬd�O�_�w�s�b���귽�A�p�G�s�b���ܡA
	//�|�۰ʱN�֨��귽����귽�޲z�W�A�åB�b�֨��������귽)
	TypeObj obj = CreateResObj<Type>(filename) ;

	//�إ߸귽����
	Resobj resobj(obj) ;

	//�N�귽��J�귽Pool��
	GetResMgr().PushToPool( resobj , filename ) ;
	



	return obj->Get() ;
}

//template�����G�W�[��J����e�A�إߤ@�Ӫť�Texture�CRenderTarget��
template<typename Type , typename TypeObj> Type* LoadResource(const char *filename ,
														unsigned int width , unsigned int height)
{
	ResObjmap::iterator iter ;

	////////////////	���q���귽�޲z�M��귽		/////////////////

	Type *res = FindResFromResPool<Type>( filename ) ;
	//����쪺�ܡA�����^��
	if ( res != NULL )
	{
		return res ;
	}

	//////////			�b�q�֨��O�餤�M��A�Φۤv�إ�	////////////////////

	//�إ�Texutre�귽(�إ߹L�{�A�M��֨��O������A�ˬd�O�_�w�s�b���귽�A�p�G�s�b���ܡA
	//�|�۰ʱN�֨��귽����귽�޲z�W�A�åB�b�֨��������귽)
	TypeObj obj = CreateResObj<Type>(filename , width , height) ;

	//�إ߸귽����
	Resobj resobj(obj) ;

	//�N�귽��J�귽Pool��
	GetResMgr().PushToPool( resobj , filename ) ;

	return obj->Get() ;
}


//template<�귽���� , �귽�������>  �|�ǤJ��thread�ҫإߪ�Memory Packet
template<typename Type , typename TypeObj>
Type* LoadResource(const char *filename , FDO::memPack *pMem) 
{
	ResObjmap::iterator iter ;

	////////////////	���q���귽�޲z�M��귽		/////////////////

	Type *res = FindResFromResPool<Type>( filename ) ;
	//����쪺�ܡA�����^��
	if ( res != NULL )
	{
		return res ;
	}

	//////////			�b�q�֨��O�餤�M��A�Φۤv�إ�	////////////////////

	//�إ�Texutre�귽(�إ߹L�{�A�M��֨��O������A�ˬd�O�_�w�s�b���귽�A�p�G�s�b���ܡA
	//�|�۰ʱN�֨��귽����귽�޲z�W�A�åB�b�֨��������귽)
	TypeObj obj = CreateResObj<Type>(filename , pMem) ;

	//�إ߸귽����
	Resobj resobj(obj) ;

	//�N�귽��J�귽Pool��
	GetResMgr().PushToPool( resobj , filename ) ;

	return obj->Get() ;
}

//////////////			�귽�޲z		//////////////////////

class CResMgr
{
	//��귽�޲z�����q�����G��s�귽�޲z���e�q
	friend void UpdateResMgrSize( int offset) ;
	//��귽�޲z�����q�����G�q�귽�޲z�����o�귽
	template<typename T> friend T* FindResFromResPool(const std::string &filename) ;
	//��귽�޲z���������G�q�귽�޲z���R���귽
	template<typename RES> friend void ReleaseResource(RES res) ;
	//��귽�޲z���������GŪ���귽
	template<typename Type , typename TypeObj> friend Type* LoadResource(const char *filename) ;
	//��귽�޲z���������GŪ���귽
	template<typename Type , typename TypeObj> friend Type* LoadResource(const char *filename , 
		unsigned int width , unsigned int height) ;
	template<typename Type , typename TypeObj> friend Type* LoadResource(const char *filename , FDO::memPack *pMem) ;
	template<typename Type , typename TypeObj> friend Type* FindResource(const char *filename );

	//���ɭp�ƾ�
	typedef std::map<void* , unsigned int>				Sharecount ;
	typedef std::pair<void* , unsigned int>				Paircount ;
public:
	CResMgr( boost::shared_ptr<CCatchRes> catchres) ;
	~CResMgr( void );
public:
	////							���}�G���u�㪺�\��
	void Show( void );											//��ܸ귽�޲z���귽��T
	unsigned int Size( void ) const ;							//��ܸ귽�޲z���{��Size
	void LockRes(const char *filename)	;						//�]�w�S�w�귽�AŪ�J�R�APool
	void DirectLockRes(const char *filename)	;				//�]�w�S�w�귽�AŪ�J�R�APool(���@check�ʧ@)
	void UnLockRes(const char *filename) ;						//�Ѱ��귽�A�]�^�귽���ʺAPool
	void ClearDynamic( void )	;								//�b�귽�޲z���A�M�ũҦ��ʺA�귽
	void ClearStatic( void );									//�b�귽�޲z���A�M�ũҦ��R�A�귽
	bool CheckResource(const char *filename) ;					//�ˬd��T�O�_�b�R�A�ΰʺA�H�θ귽�^����
	////							�p���G���u�㪺�Բӳ]�w
private:
	ResObjmap::iterator FindResForDyanmic(const std::string &filename);	//�M��귽�G�q�ʺAPool
	ResObjmap::iterator FindResForStatic(const std::string &filename);		//�M��귽�G�q�R�APool


	void PushToPool(const Resobj &resobj , const std::string &filename);	//�N�귽��J�귽Pool��
	void ReleaseResource( ResObjmap::iterator iter)	;					//�����귽�A�B��s��O����
	void UpdatediaryTime(const Resobj &resobj)	;							//��s�귽�y�ʶq
	void UpdatedirayStatic(const Resobj &resobj)	;						//��s�귽�����A�G�R�A
	void UpdatediraryRel(const Resobj &resobj)	;							//��s�귽�����A�G�R��
	void UpdateSize(int offset)		;										//��s�귽�޲z��Size
	////////					���U�귽					///////
	void RegisterShare(void *res) ;											//���U���ɸ귽
	bool UnRegisterShare(void *res) ;										//�ϵ��U���ɸ귽
private:
	////						�귽Pool			///
	ResObjmap								m_resobjmap ;	//�ʺA�귽Pool
	ResObjmap								m_staticres ;//�R�A�귽Pool
	////						��O				//
	boost::shared_ptr<CDiary>				m_diray ;		//��O
	CCatchVisitDiary						m_visitdiary ;	//��O�M�ݪ��X�ݾ�
	////						�֨�				///
	boost::shared_ptr<CCatchRes>			m_catchres ; 
	////						��L				//
	Sharecount								m_count ;		//���ɭp�ƾ��C�ѨM�����^�Ǹ귽���Ъ����D�I
//	std::vector<std::string>				m_resname;		//�n�L�o���귽�W�١]�R�A�Ρ^
	std::set<std::string>					m_resname ; 
	unsigned int							m_size ;		//���귽���j�p
};

#endif