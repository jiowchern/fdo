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
//////////////			被管理的資源*.h		/////////////////
#include "packetreader.h"     //解壓縮封包用
#include "texture.h"		  //FDO::Textrue用
#include "FDO_RoleMemoryMgr.h" //sImage用
#include "JLSkinMesh.h" //JLSkinMesh用


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

//////////////////		全域變數		////////////////
const unsigned int MSIZE = 1024 * 1024 ; //(1MB)


//////////////////		全域函數		////////////////
//更新快取記憶體大小
void UpdateCatchSize(int offset) ;
//更新資源管理記憶體大小
void UpdateResMgrSize( int offset);
//取得快取記憶體
boost::shared_ptr<CCatchRes> GetCatch( void );
//取得資源管理
CResMgr& GetResMgr( void ) ;


//////								全域函數：更新日記相關			/////////

//更新：紀錄資源的使用量
void Updatediarytime( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date);

//更新：紀錄資源的使用量屬於Catch
void UpdatediarytimeForCatch( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date);

//更新：紀錄資源的使用量屬於資源管理
void UpdatediarytimeForResMgr( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date);

//更新：變更資源狀態屬於Catch：Rel
void UpdatediraystateRelForCatch( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date) ;

//更新：變更資源狀態屬於資源管理
void UpdatediraystateForResMgr( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date);

//更新：變更資源狀態：Rel
void UpdatediraystateRel( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date);

//更新：變更資源狀態：STATIC
void UpdatediraystateStatic( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date);



/////////////////				全域函數：資源管理相關		//////

//建立資源物件(CD3DRes)
template<typename T> boost::shared_ptr<CD3DRes<T> > CreateResObj( const std::string &filename) ;

//尋找資源物件(CD3DRes)
template<typename T> boost::shared_ptr<CD3DRes<T> > FindResObj( const std::string &filename , bool &isFind) ;

//重載：RenderTarget用
template<typename T> boost::shared_ptr<CD3DRes<T> >CreateResObj( const std::string &filename , 
														  unsigned int width , unsigned int height) ;

//讀取資源到資源管理：template<資源種類 , 資源物件種類>
template<typename Type , typename TypeObj> Type* LoadResource(const char *filename );

//template重載：增加輸入長跟寬，建立一個空白Texture。RenderTarget用
template<typename Type , typename TypeObj> Type* LoadResource(const char *filename ,
														unsigned int width , unsigned int height);

//template<資源種類 , 資源物件種類>  會傳入由thread所建立的Memory Packet
template<typename Type , typename TypeObj>Type* LoadResource(const char *filename , FDO::memPack *pMem) ;

//讀取資源到資源管理：template<資源種類 , 資源物件種類>
template<typename Type , typename TypeObj> Type* FindResource(const char *filename );


//從資源管理中刪除資源（利用資源指標）
template<typename RES> void ReleaseResource(RES res);

//特化：從資源管理中刪除資源（利用檔名）
//template<> void ReleaseResource(const char *filename);

//從資源Pool中，尋找資源，並且回傳資源指標(T*)(如：std::string*)
template<typename T> T* FindResFromResPool(const std::string &filename) ;



bool CheckResource(const char *filename) ;
/////////////////				全域函數：讀取壓縮封包相關		//////

boost::shared_ptr<FDO::memPack> GetMemForPacket(const std::string &name ,unsigned int &resize);

/////////////////				全域函數：從記憶體區塊抓取Texture		//////

//第三個參數和第四個參數為out
void GetTexFormMem(unsigned char* data , unsigned int DataSize , 
				   unsigned int &resize , IDirect3DTexture9 **tex);

//第三個參數和第四個參數為out
void GetTexFormMemEx(unsigned char* data , unsigned int DataSize , 
				   unsigned int &resize , IDirect3DTexture9 **tex);

//建立一個空白Texture(RenderTarget用)
void GetTexture(unsigned int &resize , unsigned int width , unsigned int height , 
				IDirect3DTexture9 **tex);

/////////////////				其他小工具運用


//從快取記憶體中，尋找資源
template<typename T>
bool FindResInCatch(const std::string &filename , CD3DRes<T> *res) ;

//重載：從快取記憶體中，尋找資源
bool FindResInCatch(const std::string &filename) ;

//取得texture的資源記憶體大小
void GetTextrueResSize(unsigned int &resize , IDirect3DTexture9 **tex) ;

////////////////////////命令變動部分：資源種類		 ///////////////  

//自訂資源種類：logo圖讀取texture資源用。建立此是為了達成靜態多型的用處
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

//自訂資源種類：skybox資源資料
typedef boost::tuple<LPD3DXMESH , std::vector<D3DMATERIAL9> , std::vector<LPDIRECT3DTEXTURE9> , 
					 DWORD>																	SkyboxData ;



//資源的大小、資源檔名、資源指標
//sImage物件
typedef boost::tuple<unsigned int , std::string , boost::shared_ptr<FDO::sImage> >			ResImage ;

//Texture
typedef boost::tuple<unsigned int , std::string , boost::shared_ptr<IDirect3DTexture9> >	ResTextrue ;

//Texture：logo圖用
typedef boost::tuple<unsigned int , std::string , boost::shared_ptr<stLogtexture> >			ResLogTextrue ;

//Texture：RenderTarget用
typedef boost::tuple<unsigned int , std::string , boost::shared_ptr<IDirect3DTexture9> >	ResTextarget ;


//X file（單純只抓網格）
typedef boost::tuple<unsigned int , std::string , boost::shared_ptr<ID3DXMesh> >			ResXfile ;

//JLSkinMesh(讀地圖Mesh用)
typedef boost::tuple<unsigned int , std::string , boost::shared_ptr<FDO::JLSkinMesh> >		ResSkinMesh ;

//JLSkyBox（讀取天空盒用的資料）
typedef boost::tuple<unsigned int , std::string , boost::shared_ptr<SkyboxData> >			ResSkybox ;


//一般的記憶體區塊資料
typedef boost::tuple<unsigned int , std::string , boost::shared_ptr<FDO::memPack> >			ResMemdata ;

//關於FDO::sRoleActionList的物件資源分享
//typedef boost::tuple<unsigned int , std::string , boost::shared_ptr<CShareAction> >			ResActionlist ;

//資源種類：
//	1.記憶體區塊資料 
//	2.sImage 
//	3.X file（單純只有網格）
//	4.Texture
//  5.讀取Log圖用的Texture
//	6.抓取SkinMesh資源
//	7.天空盒
//  8.建立空白Texture(屬於RenderTarget)  (no use)
//	9.FDO::sRoleActionList的物件資源分享

typedef boost::variant<ResMemdata , ResImage , ResXfile ,
					ResTextrue , ResLogTextrue , ResSkinMesh , 
					ResSkybox , ResTextarget>								Resmemory; 

/////////////				資源管理的命令變動部分：資源物件		////////////////////////

//sImage物件
typedef boost::shared_ptr<CD3DRes<FDO::sImage> >			ResImageObj ;	

//X file（單純只抓網格）
typedef boost::shared_ptr<CD3DRes<ID3DXMesh> >				ResXfileObj ;

//JLSkinMesh(讀地圖Mesh用)
typedef boost::shared_ptr<CD3DRes<FDO::JLSkinMesh> >		ResSkinMeshObj ;

//Texture
typedef boost::shared_ptr<CD3DRes<IDirect3DTexture9> >		ResTextrueObj ;

//Texture：RenderTarget用
typedef boost::shared_ptr<CD3DRes<IDirect3DTexture9> >		ResTextargetObj ;

//天空盒
typedef boost::shared_ptr<CD3DRes<SkyboxData> >				ResSkyboxObj ;

//Texture：logo圖用
typedef boost::shared_ptr<CD3DRes<stLogtexture> >			ResLogTextrueObj ;

//記憶體區塊物件
typedef boost::shared_ptr<CD3DRes<FDO::memPack> >			ResMemObj ;

//FDO::sRoleActionList的物件資源分享
//typedef boost::shared_ptr<CD3DRes<CShareAction> >			ResActlistObj ;


//資源物件集合：
//	1.記憶體區塊物件
//	2.sImage物件 
//	3.X file（單純只有網格）
//	4.Texture
//	5.讀取Log圖用的Texture
//	6.抓取SkinMesh資源
//	7.天空盒
//  8.建立空白Texture(屬於RenderTarget)
//	9.FDO::sRoleActionList的物件資源分享

typedef	boost::variant<ResMemObj , ResImageObj , ResXfileObj ,
					   ResTextrueObj , ResLogTextrueObj , ResSkinMeshObj ,
					   ResSkyboxObj, ResTextargetObj>			Resobj ;

//資源 map
typedef std::map<std::string , Resobj>						ResObjmap;

//////////////				資源相關：刪除器		///////////////////

class CD3DFilter
{
public:
	//一般物件的delete
	template<typename T>
	void operator( ) (T *res)
	{
		if ( res != NULL )
		{
			res->Release() ; 
			res = NULL ;
		}
	}
	//特化：對記憶體區塊作特殊的刪除方式
	template<>
	void operator( ) (FDO::memPack *mem);
	//特化：對sImage的刪除方式
	template<>
	void operator( ) (FDO::sImage *image);
	//template<>
	//void operator()(SkyboxData *sky) ;
	//template<>
	//void operator()(stLogtexture *logo) ;
	template<>
	void operator()(FDO::JLSkinMesh *skin) ;
};
////////////////////	資源相關：讀取資源方式		////////////////

//讀取資源：一般
template<typename T>
boost::shared_ptr<T> RessourceLoad(const std::string &name , unsigned int &resize)
{
	
	boost::shared_ptr<T> res(new T , CD3DFilter()) ;
	//紀錄物件大小
	resize = sizeof(T) ;

	return res ;

}

//讀取資源：一般
template<typename T>
boost::shared_ptr<T> RessourceLoad(const std::string &name , FDO::memPack *pMem , unsigned int &resize)
{
	
	boost::shared_ptr<T> res(new T , CD3DFilter()) ;
	//紀錄物件大小
	resize = sizeof(T) ;

	return res ;

}

//讀取資源：一般，多增加參數。為了特化出來
template<typename T>
boost::shared_ptr<T> RessourceLoad(unsigned int &resize , unsigned int width ,unsigned int height)
{
	
	boost::shared_ptr<T> res(new T , CD3DFilter()) ;
	//紀錄物件大小
	resize = sizeof(T) ;

	return res ;

}



//讀取資源：特化，Texture
template<>
boost::shared_ptr<FDO::sImage> RessourceLoad(const std::string &name , unsigned int &resize);

//讀取資源：特化，Texture(Thread用)
template<>
boost::shared_ptr<FDO::sImage> RessourceLoad(const std::string &name , FDO::memPack *pMem ,  unsigned int &resize);

//讀取資源：特化，X file（單純只有網格）
template<>
boost::shared_ptr<ID3DXMesh> RessourceLoad(const std::string &name , unsigned int &resize);

//讀取資源：特化，SkinMesh資源
template<>
boost::shared_ptr<FDO::JLSkinMesh> RessourceLoad(const std::string &name , unsigned int &resize);


//讀取資源：特化，skybox資源
template<>
boost::shared_ptr<SkyboxData> RessourceLoad(const std::string &name , unsigned int &resize);



//讀取資源：特化，Texture
template<>
boost::shared_ptr<IDirect3DTexture9> RessourceLoad(const std::string &name , unsigned int &resize);

template<>
boost::shared_ptr<IDirect3DTexture9> RessourceLoad(const std::string &name , FDO::memPack *pMem ,  unsigned int &resize);

//讀取資源：特化，Texture(RenderTarget)
template<>
boost::shared_ptr<IDirect3DTexture9> RessourceLoad(unsigned int &resize ,
												   unsigned int width ,
												   unsigned int height);


//讀取資源：特化，讀取log's Texture
template<>
boost::shared_ptr<stLogtexture> RessourceLoad(const std::string &name , unsigned int &resize);

//讀取資源：特化，記憶體區塊
template<>
boost::shared_ptr<FDO::memPack> RessourceLoad(const std::string &name , unsigned int &resize);

//讀取資源：特化，記憶體區塊(Thread用)
template<>
boost::shared_ptr<FDO::memPack> RessourceLoad(const std::string &name , FDO::memPack *pMem , unsigned int &resize);

////////////			資源資料相關：存放資料		///////////////////////
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

////////////			資源資料相關：取出資料		///////////////////////

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

////////////			資源資料相關：取出資料(快取中)		///////////////////////

//從快取記憶體中，尋找資源
template<typename T>
bool FindResInCatch(const std::string &filename , CD3DRes<T> *res)
{
	std::map<std::string , Resmemory>::iterator iter ;

	iter =  GetCatch()->Find(filename )  ; 

	if (iter != GetCatch()->m_mempool.end() )
	{
		CCatchtVisitFind<T> visit(res) ;

		//找到的話，把資源丟給CD3DRes
		boost::apply_visitor(visit , iter->second ) ;

		//資源已經還給CD3DRes了，所以要在快取中移除掉
		GetCatch()->Erase( iter ) ;

		//有找到
		return true; 
	}

	//沒找到
	return false ;

}

////////////			資源相關：建立資源物件(CD3DRes)		///////////////////////

template<typename T>
boost::shared_ptr<CD3DRes<T> > CreateResObj( const std::string &filename)
{
	boost::shared_ptr<CD3DRes<T> > obj(new CD3DRes<T> ) ;

	obj->Load( filename ) ;

	return obj ;
}

//尋找資源物件(CD3DRes)
template<typename T> boost::shared_ptr<CD3DRes<T> > FindResObj( const std::string &filename , bool &isFind)
{
	boost::shared_ptr<CD3DRes<T> > obj(new CD3DRes<T> ) ;

	isFind = obj->Find( filename ) ;

	return obj ;
}

////////////			資源相關：特化建立Texture資源(RenderTarget用)		///////////////////////

//RenderTarget用
template<typename T> 
boost::shared_ptr<CD3DRes<T> > CreateResObj( const std::string &filename , 
											unsigned int width , unsigned int height)
{
	boost::shared_ptr<CD3DRes<T> > obj(new CD3DRes<T> ) ;

	obj->Load( filename , width , height) ;

	return obj ;

}
////////////			資源相關：特化建立Texture資源(支援thread)		///////////////////////
template<typename T>
boost::shared_ptr<CD3DRes<T> > CreateResObj( const std::string &filename , FDO::memPack *pMem )
{
	boost::shared_ptr<CD3DRes<T> > obj(new CD3DRes<T> ) ;

	obj->Load( filename , pMem ) ;

	return obj ;
}


///////////////			資源類別		///////////////


//功能：
//			1.讀取資源
//			2.抓取資源資料
//			3.資源刪除時（唯一性），丟入快取記憶體中。

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

		//判斷此資源是否唯一，因為是此唯一的話，則要在刪除資源之前，放入快取記憶體中
		if ( m_texptr.unique() )
		{
			//檢查是否還有快取空間
			if ( GetCatch()->Check( m_size ) )
			{
				//表示還有空間，可以丟入快取空間
				boost::tuple<unsigned int , std::string , boost::shared_ptr<T> > 
					res(m_size , m_filename , m_texptr) ;

				GetCatch()->Push( m_filename , res ) ;				
				
			}
			else
			{
				//沒有任何快取空間
				
				//進行刪除動作
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
		//先尋找快取記憶體中，是否存在


		if (FindResInCatch<T>(name , this ) )
		{			//有找到
			return ;
		}

		//檔名
		m_filename = name ;
		//資源
		m_texptr = RessourceLoad<T>( name , m_size) ;

	}
	void Load(const std::string &name , unsigned int width , unsigned int height)
	{

		if (FindResInCatch<T>(name , this ) )
		{			//有找到
			return ;
		}

		//檔名
		m_filename = name ;
		//資源
		m_texptr = RessourceLoad<T>( m_size , width , height) ;

	}
	//支援thread用的
	void Load(const std::string &name , FDO::memPack *pMem)
	{
		if (FindResInCatch<T>(name , this ) )
		{			//有找到
			return ;
		}

		//檔名
		m_filename = name ;
		//資源
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
	T* Get(void) const			//取得資源
	{
		return m_texptr.get() ;
	}
private:
	boost::shared_ptr<T>		m_texptr ;			//存放資源
	std::vector<CResDate>		m_vecdate ;			//紀錄每一種資源資料
	unsigned int				m_size ;			//此資源的大小
	std::string					m_filename ;		//資源檔名
};


////////////			資源日記		////////////////

//日記類別：map內容<檔案名稱 , tuple內容>
//tuple內容：資源大小，在快取來往的次數（每當Load to catch 加1），資源狀態，
class CDiary
{
	//日記的主要功用：
	friend void Updatediarytime( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date) ;
	friend void UpdatediraystateRel( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date) ;
	friend void UpdatediarytimeForResMgr( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date) ;
	friend void UpdatediraystateForResMgr( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date);
	friend void UpdatediraystateStatic( boost::shared_ptr<CDiary> pdiray , const std::vector<boost::any>& date) ;
	//日記的資源狀態
	enum ResState			
	{
		USE ,			//使用中
		REL ,			//刪除
		STATIC			//靜態
	};

	//日記的命令變數變更
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
	bool Check(const std::string &id );							//檢查是否存在，並且累加次數
	void Create(const std::string &id , unsigned int filesize);	//建立一筆資料目錄
	void SetState( const std::string &id , ResState state);		//設定某一筆資料狀態
	void Erase(const std::string &id) ;							//清除某一筆資料
	void Delevery( void );//將所有資源設定:REL。不過，需要嗎？
	void Show( void );
private:
	Diarymap							m_diary ;
	std::ofstream						m_o ;
	std::string							m_filename ;
};





//快取資源訪問器(日記：1.紀錄資源的使用量 2.變更資源狀態為REL)
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
	void operator() (const boost::tuple<unsigned int , std::string , T> &t)	//接受此類的tuple
	{


		if (m_pfun)
		{
			std::vector<boost::any> vecany ;

			vecany.push_back( boost::tuples::get<0>(t) ) ;//資源大小
			vecany.push_back( boost::tuples::get<1>(t) ) ;//檔名

		//	也可以這樣寫，但是不夠明確
		//	vecany.push_back( t.get<0>() ) ;		//資源大小
		//	vecany.push_back( t.get<1>() ) ;		//檔名

			//如果有函數指標的話，去呼叫。
			m_pfun(m_pdiray , vecany) ;
		}
	}
	template<typename T>				//接受所有boost::shared_ptr<CD3DRes<T>>
	void operator() (const boost::shared_ptr<CD3DRes<T> > &t)
	{
		if ( m_pfun )
		{
			std::vector<boost::any> vecany ;

			vecany.push_back( t->Filesize() ) ;//資源大小
			vecany.push_back( t->Filename() ) ;//檔名

			//如果有函數指標的話，去呼叫。
			m_pfun(m_pdiray , vecany) ;
		}
	}
private:
	Fundiray							m_pfun ;
	boost::shared_ptr<CDiary>			m_pdiray;
};


//快取資源訪問器(排序用：記憶體大小。由小到大)  
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

//快取資源For STL's iterator(利用T(std::string檢查是否有此資源存在於快取記憶體中)  
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
	//對boost::tuple<unsigned int , std::string , U>
	template<typename U>
	bool operator() (const boost::tuple<unsigned int , std::string , U> &res) 
	{
		T t = boost::tuples::get<1>(res) ; // std::string 檔名

		//必須支援operator==
		if (m_t == t)
		{
			return true ;
		}

		return false ;
	}
	template<typename U>
	bool operator() (const boost::shared_ptr<CD3DRes<U> > &res) //對CD3DRes<U>
	{
		//比對檔名
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




//快取資源訪問器（將資源丟給資源類別使用）
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

		m_res->SetResource(boost::tuples::get<0>(res) ,			//檔案大小
							 boost::tuples::get<1>(res) ,			//檔名
							 //先把資源轉型到正確的資源
							 boost::any_cast<boost::shared_ptr<T>>
							 (boost::tuples::get<2>(res)) ) ;		//資源

		}catch (boost::bad_any_cast &e)
		{
			Assert(0 , e.what() ) ;
		}
	}
private:
	CD3DRes<T>				*m_res ;
};


////////////			資源快取		////////////////

//快取主要動作：每次資源（唯一）刪除時，啟動丟入快取，或刪除資源中的快取。
class CCatchRes
{
	//快取跟日記的溝通介面：更新快取記憶體大小
	friend void UpdateCatchSize(int offset) ;
	//快取跟資源類別（CD3DRes）的溝通介面
	template<typename T> friend bool FindResInCatch(const std::string &filename , CD3DRes<T> *res) ;
	//快取跟資源類別（CD3DRes）的溝通介面
	friend bool FindResInCatch(const std::string &filename) ;

	typedef std::map<std::string , Resmemory>					Resmap ;	

public:
	CCatchRes() ;
	~CCatchRes( void ) ;
private:
	CCatchRes (const CCatchRes&) ;
	CCatchRes& operator= (const CCatchRes&) ;
public:
	/////					公有：此工具功能					//////////
	bool Check( unsigned int ressize);												//檢查快取空間
	void Push(const std::string &filename , const Resmemory &res);					//將資源丟入快取
	void Pop( void );																//將資源丟出快取
	void Show( void );																//顯示所有資源項目
	void AllClear(void) ;															//清空所有快取資源
public:
	/////////				公有：取得相關屬性			//////////
	unsigned int MaxSize( void ) const ;				//取得此快取的最大空間
	unsigned int Size( void ) const ;					//取得此快取的現有空間
private:
	/////					私有：此工具功能					//////////
	std::map<std::string , Resmemory>::iterator Find(const std::string &filename) ;			//尋找資源是否在快取中
	void Erase( std::map<std::string , Resmemory>::iterator iter ) ;	//刪除快取中某項資源
//	void Sort( void );									//排序：依資源大小
	void Clear( void );									//設定資源為：Release
	void UpdateSize(int offset);						//更新快取的Size
private:
	Resmap								m_mempool ;		//記憶體池
	boost::shared_ptr<CDiary>			m_diray ;		//日記
	CCatchVisitDiary					m_visitdiary ;	//日記專屬的訪問器
	unsigned int						m_poolsize ;	//記憶體池大小
	unsigned int						m_cursize ;		//記憶體現有的大小
};


/////////////				資源管理			////////////////////////

//資源指標尋找(利用變數的指標下去比較)
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

//回傳資源的指標，從boost::variant
template<typename T>
class CResMgrvisitGetRes : public boost::static_visitor<T*>
{
public:
	T* operator()(const boost::shared_ptr<CD3DRes<T> > &res)
	{
		return (*res).Get() ;
	}
	//重載方式：只取得T類型的資源
	template<typename U>
	T* operator()(const boost::shared_ptr<CD3DRes<U> > &res)
	{
		return NULL ;
	}
};

//回傳資源的指標，從boost::variant。(不管什麼類型的資源)
class CResMgrvisitGetAllRes : public boost::static_visitor<void*>
{
public:
	template<typename T>
	void* operator()(const boost::shared_ptr<CD3DRes<T> > &res)
	{
		return (*res).Get() ;
	}
};


//從資源Pool中，尋找資源，並且回傳資源指標(T*)(如：std::string*)
template<typename T>
T* FindResFromResPool(const std::string &filename)
{

	ResObjmap::iterator iter ;

	//先從靜態找起
	iter = GetResMgr().FindResForStatic( filename) ;

	if ( iter != GetResMgr().m_staticres.end() )
	{	
		//更新日記：更新資源流動量
		GetResMgr().UpdatediaryTime( iter->second ) ;

		CResMgrvisitGetRes<T> visit ;

		return boost::apply_visitor( visit , iter->second) ;
	}		

	//再由動態找起
	iter = GetResMgr().FindResForDyanmic( filename) ;


	if(  iter != GetResMgr().m_resobjmap.end() )
	{
		//更新日記：更新資源流動量
		GetResMgr().UpdatediaryTime( iter->second  ) ;

		CResMgrvisitGetRes<T> visit ;
	
		///////			有找到的話，進行註冊。記得！只有動態pool才有		////////

		T *res = boost::apply_visitor( visit , iter->second) ;

		//預防同檔名，但是不同資源
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

//從資源管理中刪除資源（利用資源指標）
template<typename RES>
void ReleaseResource(RES res)
{
	///////	會放在這裡的原因：加速判斷是否要移除資源的損耗時間		/////////

	////////			先對分享計數器，作反註冊		/////////////

	//cosiann
	//bool share = GetResMgr().UnRegisterShare(res) ;
	//if ( !share )
	//{
	//	//還有其他資源再分享的話，直接回傳。
	//	return ;
	//}


	/////////		沒有其他資源再分享或沒有找到資源，到動態pool去尋找。並且刪除資源		/////
	ResObjmap::iterator iter(GetResMgr().m_resobjmap.begin()) ;

	//利用資源指標去比較
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
		//沒有找到的話，就跳出
		return ;
	}
	
	//找到的話
	GetResMgr().ReleaseResource( iter ) ;
}


//讀取資源到資源管理：template<資源種類 , 資源物件種類>
template<typename Type , typename TypeObj> Type* FindResource(const char *filename )
{
	ResObjmap::iterator iter ;

	////////////////	先從此資源管理尋找資源		/////////////////

	Type *res = FindResFromResPool<Type>( filename ) ;
	//有找到的話，直接回傳
	if ( res != NULL )
	{
		return res ;
	}

	//////////			在從快取記體中尋找，沒找到時，不會自行建立資源物件	////////////////////

	bool isFind = false ; 

	TypeObj obj = FindResObj<Type>(filename , isFind) ; 
	
	if(isFind)
	{
		///////			在快取中，有找到的話			/////////

		//建立資源物件
		Resobj resobj(obj) ;

		//將資源丟入資源Pool中
		GetResMgr().PushToPool( resobj , filename ) ;
		
		return obj->Get() ;
	}
	
	return NULL ; 

}


//template<資源種類 , 資源物件種類>
template<typename Type , typename TypeObj>
Type* LoadResource(const char *filename) 
{
	ResObjmap::iterator iter ;

	////////////////	先從此資源管理尋找資源		/////////////////

	Type *res = FindResFromResPool<Type>( filename ) ;
	//有找到的話，直接回傳
	if ( res != NULL )
	{
		return res ;
	}


	//////////			在從快取記體中尋找，或自己建立	////////////////////

	//建立Texutre資源(建立過程，尋找快取記憶體當中，檢查是否已存在的資源，如果存在的話，
	//會自動將快取資源移到資源管理上，並且在快取中移除資源)
	TypeObj obj = CreateResObj<Type>(filename) ;

	//建立資源物件
	Resobj resobj(obj) ;

	//將資源丟入資源Pool中
	GetResMgr().PushToPool( resobj , filename ) ;
	



	return obj->Get() ;
}

//template重載：增加輸入長跟寬，建立一個空白Texture。RenderTarget用
template<typename Type , typename TypeObj> Type* LoadResource(const char *filename ,
														unsigned int width , unsigned int height)
{
	ResObjmap::iterator iter ;

	////////////////	先從此資源管理尋找資源		/////////////////

	Type *res = FindResFromResPool<Type>( filename ) ;
	//有找到的話，直接回傳
	if ( res != NULL )
	{
		return res ;
	}

	//////////			在從快取記體中尋找，或自己建立	////////////////////

	//建立Texutre資源(建立過程，尋找快取記憶體當中，檢查是否已存在的資源，如果存在的話，
	//會自動將快取資源移到資源管理上，並且在快取中移除資源)
	TypeObj obj = CreateResObj<Type>(filename , width , height) ;

	//建立資源物件
	Resobj resobj(obj) ;

	//將資源丟入資源Pool中
	GetResMgr().PushToPool( resobj , filename ) ;

	return obj->Get() ;
}


//template<資源種類 , 資源物件種類>  會傳入由thread所建立的Memory Packet
template<typename Type , typename TypeObj>
Type* LoadResource(const char *filename , FDO::memPack *pMem) 
{
	ResObjmap::iterator iter ;

	////////////////	先從此資源管理尋找資源		/////////////////

	Type *res = FindResFromResPool<Type>( filename ) ;
	//有找到的話，直接回傳
	if ( res != NULL )
	{
		return res ;
	}

	//////////			在從快取記體中尋找，或自己建立	////////////////////

	//建立Texutre資源(建立過程，尋找快取記憶體當中，檢查是否已存在的資源，如果存在的話，
	//會自動將快取資源移到資源管理上，並且在快取中移除資源)
	TypeObj obj = CreateResObj<Type>(filename , pMem) ;

	//建立資源物件
	Resobj resobj(obj) ;

	//將資源丟入資源Pool中
	GetResMgr().PushToPool( resobj , filename ) ;

	return obj->Get() ;
}

//////////////			資源管理		//////////////////////

class CResMgr
{
	//跟資源管理的溝通介面：更新資源管理的容量
	friend void UpdateResMgrSize( int offset) ;
	//跟資源管理的溝通介面：從資源管理中取得資源
	template<typename T> friend T* FindResFromResPool(const std::string &filename) ;
	//跟資源管理的溝介面：從資源管理中刪除資源
	template<typename RES> friend void ReleaseResource(RES res) ;
	//跟資源管理的溝介面：讀取資源
	template<typename Type , typename TypeObj> friend Type* LoadResource(const char *filename) ;
	//跟資源管理的溝介面：讀取資源
	template<typename Type , typename TypeObj> friend Type* LoadResource(const char *filename , 
		unsigned int width , unsigned int height) ;
	template<typename Type , typename TypeObj> friend Type* LoadResource(const char *filename , FDO::memPack *pMem) ;
	template<typename Type , typename TypeObj> friend Type* FindResource(const char *filename );

	//分享計數器
	typedef std::map<void* , unsigned int>				Sharecount ;
	typedef std::pair<void* , unsigned int>				Paircount ;
public:
	CResMgr( boost::shared_ptr<CCatchRes> catchres) ;
	~CResMgr( void );
public:
	////							公開：此工具的功能
	void Show( void );											//顯示資源管理的資源資訊
	unsigned int Size( void ) const ;							//顯示資源管理的現有Size
	void LockRes(const char *filename)	;						//設定特定資源，讀入靜態Pool
	void DirectLockRes(const char *filename)	;				//設定特定資源，讀入靜態Pool(不作check動作)
	void UnLockRes(const char *filename) ;						//解除資源，設回資源為動態Pool
	void ClearDynamic( void )	;								//在資源管理中，清空所有動態資源
	void ClearStatic( void );									//在資源管理中，清空所有靜態資源
	bool CheckResource(const char *filename) ;					//檢查資訊是否在靜態或動態以及資源回收桶中
	////							私有：此工具的詳細設定
private:
	ResObjmap::iterator FindResForDyanmic(const std::string &filename);	//尋找資源：從動態Pool
	ResObjmap::iterator FindResForStatic(const std::string &filename);		//尋找資源：從靜態Pool


	void PushToPool(const Resobj &resobj , const std::string &filename);	//將資源丟入資源Pool中
	void ReleaseResource( ResObjmap::iterator iter)	;					//移除資源，且更新日記紀錄
	void UpdatediaryTime(const Resobj &resobj)	;							//更新資源流動量
	void UpdatedirayStatic(const Resobj &resobj)	;						//更新資源的狀態：靜態
	void UpdatediraryRel(const Resobj &resobj)	;							//更新資源的狀態：刪除
	void UpdateSize(int offset)		;										//更新資源管理的Size
	////////					註冊資源					///////
	void RegisterShare(void *res) ;											//註冊分享資源
	bool UnRegisterShare(void *res) ;										//反註冊分享資源
private:
	////						資源Pool			///
	ResObjmap								m_resobjmap ;	//動態資源Pool
	ResObjmap								m_staticres ;//靜態資源Pool
	////						日記				//
	boost::shared_ptr<CDiary>				m_diray ;		//日記
	CCatchVisitDiary						m_visitdiary ;	//日記專屬的訪問器
	////						快取				///
	boost::shared_ptr<CCatchRes>			m_catchres ; 
	////						其他				//
	Sharecount								m_count ;		//分享計數器。解決直接回傳資源指標的問題點
//	std::vector<std::string>				m_resname;		//要過濾的資源名稱（靜態用）
	std::set<std::string>					m_resname ; 
	unsigned int							m_size ;		//此資源的大小
};

#endif