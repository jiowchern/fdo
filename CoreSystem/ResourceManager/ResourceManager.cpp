#include "stdafx.h"
//---------------------------------------------------------------------------
#include "PacketReader.h"
#include "PhenixD3D9RenderSystem.h"
#include "PhenixAssert.h" // for Assert
#include "iniReader.h" // for GetConfig
#include "Common.h" // for AnsiToUnicode
#include "FDO_RoleMemoryMgr.h"
#include "CRATFileFactory.h"
#include <algorithm>
#pragma hdrstop
#include "ResourceManager.h"

#include <boost/bind.hpp>
//////////////////////								////////////////////////////

//#define RESMGR

#ifdef RESMGR
#pragma message("管理模式啟動") 
#else
#pragma message("一般模式啟動") 
#endif


#ifdef OBJMGR
#pragma message("物件管理模式啟動") 

#else
#pragma message("物件一般模式啟動") 
#endif


////////////////////////////////////////////////////////////////////////////////////

//#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
//#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
//---------------------------------------------------------------------------
namespace FDO
{
//---------------------------------------------------------------------------
// Forward declarations
//---------------------------------------------------------------------------
// texture
typedef map<string,LPDIRECT3DTEXTURE9> TextureMap;
typedef TextureMap::iterator it;
typedef TextureMap::const_iterator cit;
typedef pair< it, bool > ib;
typedef pair<string,LPDIRECT3DTEXTURE9> TexturePair;
// mesh
typedef map<string,LPD3DXMESH> MeshMap;
typedef MeshMap::iterator itMesh;
typedef MeshMap::const_iterator citMesh;
typedef pair< itMesh, bool > ibMesh;
typedef pair<string,LPD3DXMESH> MeshPair;
//---------------------------------------------------------------------------
// Internal functions forward declarations
//---------------------------------------------------------------------------
void ReleaseTexture(LPDIRECT3DTEXTURE9 p) { if(p) { (p)->Release(); (p)=NULL; } }
void ReleaseTexture2(TexturePair p) { ReleaseTexture(p.second); }
void ReleaseMesh(LPD3DXMESH p) { if(p) { (p)->Release(); (p)=NULL; } }
void ReleaseMesh2(MeshPair p) { ReleaseMesh(p.second); }
//---------------------------------------------------------------------------
// TextureMgr Class
//---------------------------------------------------------------------------
class TextureMgr
{
private:
    TextureMap Textures;
public:
     TextureMgr() {}
    ~TextureMgr() { RemoveAllTexture(); }

    LPDIRECT3DTEXTURE9      LoadTextureFromFileInMemory(const string& FileName);//讀取材質
    LPDIRECT3DTEXTURE9      LoadTextureFromFile(const string& FileName);        //讀取材質
    LPDIRECT3DTEXTURE9      FindTexture(const string &FileName);                //尋找材質是否存在
    void                    RemoveTexture(const string &FileName);              //刪除材質資源
    void                    RemoveAllTexture();                                 //移除所有材質資源
};

TextureMgr& GetPtcTexMgr()
{
    static TextureMgr PtcTexMgr; // 宣告物件實體
    return PtcTexMgr;
}
//<gino>
//for static texture
TextureMgr& GetPtcStaticTexMgr()
{
    static TextureMgr PtcTexMgr; // 宣告物件實體
    return PtcTexMgr;
}
//</gino>
//---------------------------------------------------------------------------
// MeshMgr Class
//---------------------------------------------------------------------------
class MeshMgr
{
private:
    MeshMap Meshs;
public:
     MeshMgr() {}
    ~MeshMgr() { RemoveAllMesh(); }

//Mesh函式-------------------------------------------------------------------
    LPD3DXMESH              LoadMeshFromFileInMemory(const string &FileName);   //讀取Mesh
    LPD3DXMESH              LoadMeshFromFile(const string &FileName);           //讀取Mesh
    LPD3DXMESH              FindMesh(const string &FileName);                   //尋找Mesh是否存在
    void                    RemoveMesh(const string &FileName);                 //刪除Mesh資源
    void                    RemoveAllMesh();                                    //移除所有Mesh資源
};
MeshMgr& GetPtcMeshMgr()
{
    static MeshMgr PtcMeshMgr; // 宣告物件實體
    return PtcMeshMgr;
}

//<gino>
// for static mesh
MeshMgr& GetPtcStaticMeshMgr()
{
    static MeshMgr PtcMeshMgr; // 宣告物件實體
    return PtcMeshMgr;
}
//</gino>
//---------------------------------------------------------------------------
//  尋找資源中Texture是否已存在
//---------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 TextureMgr::FindTexture(const string& FileName)
{
    cit pos( Textures.find(FileName) );
    if( pos == Textures.end() )
        return NULL;
    else
        return pos->second;
}
//---------------------------------------------------------------------------
//  刪除材質資源
//---------------------------------------------------------------------------
void TextureMgr::RemoveTexture(const string &FileName)
{
    it pos( Textures.find(FileName) );
    if( pos != Textures.end() )
    {
        ReleaseTexture( pos->second );
        Textures.erase( pos );
    }
}
//---------------------------------------------------------------------------
//移除所有材質資源
//---------------------------------------------------------------------------
void TextureMgr::RemoveAllTexture()
{
    for_each( Textures.begin(), Textures.end(), ReleaseTexture2 );
    Textures.clear();
}
//---------------------------------------------------------------------------
// 從記憶體讀取材質
//---------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 TextureMgr::LoadTextureFromFileInMemory(const string &FileName)
{
    LPDIRECT3DTEXTURE9 pTexture = NULL;
    ib ret( Textures.insert( TexturePair(FileName,pTexture) ) );
    it pos( ret.first );

    if( ret.second == false )
        return pos->second;

    // 從記憶體中讀取資料
    memPack *pMem = LoadFromPacket( FileName );
    if( pMem == NULL )
        return NULL;

    // 建立材質資源
    D3DXCreateTextureFromFileInMemory( g_RenderSystem.GetD3DDevice(),
        pMem->Data, pMem->DataSize, &pos->second );

    return pos->second;
}

//---------------------------------------------------------------------------
// 從檔案讀取材質
//---------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 TextureMgr::LoadTextureFromFile(const string &FileName)
{
    LPDIRECT3DTEXTURE9 pTexture = NULL;
    ib ret( Textures.insert( TexturePair(FileName,pTexture) ) );
    it pos( ret.first );

    if( ret.second == false )
        return pos->second;

    // 從記憶體中讀取資料
    memPack *pMem = LoadFromPacket( FileName );
    if( pMem == NULL )
        return NULL;

    // 建立材質資源
    D3DXCreateTextureFromFile( g_RenderSystem.GetD3DDevice(),
        AnsiToUnicode( FileName.c_str() ), &pos->second );

    return pos->second;
}

//---------------------------------------------------------------------------
// MeshMgr Class
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  尋找資源中Mesh是否已存在
//---------------------------------------------------------------------------
LPD3DXMESH MeshMgr::FindMesh(const string &FileName)
{
    citMesh pos( Meshs.find(FileName) );
    if( pos == Meshs.end() )
        return NULL;
    else
        return pos->second;
}
//---------------------------------------------------------------------------
//  刪除Mesh資源
//---------------------------------------------------------------------------
void MeshMgr::RemoveMesh(const string &FileName)
{
    itMesh pos( Meshs.find( FileName ) );
    if( pos != Meshs.end() )
    {
        ReleaseMesh( pos->second );
        Meshs.erase( pos );
    }
}
//---------------------------------------------------------------------------
//  刪除所有Mesh資源
//---------------------------------------------------------------------------
void MeshMgr::RemoveAllMesh()
{
    for_each( Meshs.begin(), Meshs.end(), ReleaseMesh2 );
    Meshs.clear();
}
//---------------------------------------------------------------------------
// 從記憶體讀取Mesh
//---------------------------------------------------------------------------
LPD3DXMESH MeshMgr::LoadMeshFromFileInMemory(const string &FileName)
{
    LPD3DXMESH pMesh = NULL;
    ibMesh ret( Meshs.insert( MeshPair(FileName,pMesh) ) );
    itMesh pos( ret.first );

    if( ret.second == false )
        return pos->second;

    // 從記憶體中讀取資料
    memPack *pMem = LoadFromPacket( FileName );
    if( pMem == NULL )
        return NULL;

    // 建立模型資源
    LPD3DXBUFFER pAdjBuffer = NULL;
    if( SUCCEEDED( D3DXLoadMeshFromXInMemory(
        pMem->Data, pMem->DataSize,
        D3DXMESH_SYSTEMMEM,             // 旗標--該網格將放於受管理的記憶體區
        g_RenderSystem.GetD3DDevice(),  // Our d3dDevice
        &pAdjBuffer,                    // 用於描述網格的鄰接資訊
        0,                              // materials buffer 包含網格的材質資料
        0,                              //
        0,                              // materials number
        &pos->second )))                // Our mesh
    {
        DWORD* pAdjacency = (DWORD*)pAdjBuffer->GetBufferPointer();
        // Optimize the mesh for performance
        pos->second->OptimizeInplace(
            D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
            pAdjacency, NULL, NULL, NULL );
        pAdjBuffer->Release();
    }

    return pos->second;
}
//---------------------------------------------------------------------------
// 從檔案讀取Mesh
//---------------------------------------------------------------------------
LPD3DXMESH MeshMgr::LoadMeshFromFile(const string &FileName)
{
    LPD3DXMESH pMesh = NULL;
    ibMesh ret( Meshs.insert( MeshPair(FileName,pMesh) ) );
    itMesh pos( ret.first );

    if( ret.second == false )
        return pos->second;

    // 從記憶體中讀取資料
    memPack *pMem = LoadFromPacket( FileName );
    if( pMem == NULL ) return NULL;

    // 建立模型資源
    LPD3DXBUFFER pAdjBuffer = NULL;
    if( SUCCEEDED( D3DXLoadMeshFromX(
        AnsiToUnicode( FileName.c_str() ),  // 檔案名稱
        D3DXMESH_SYSTEMMEM,                 // 旗標--該網格將放於受管理的記憶體區
        g_RenderSystem.GetD3DDevice(),      // Our d3dDevice
        &pAdjBuffer,                        // 用於描述網格的鄰接資訊
        0,                                  // materials buffer 包含網格的材質資料
        0,
        0,                                  // materials number
        &pos->second ) ) )
    {
        DWORD* pAdjacency = (DWORD*)pAdjBuffer->GetBufferPointer();
        // Optimize the mesh for performance
        pos->second->OptimizeInplace(
            D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
            pAdjacency, NULL, NULL, NULL );
        pAdjBuffer->Release();
    }

    return pos->second;
}

//---------------------------------------------------------------------------
// CImageManager Class
//---------------------------------------------------------------------------
typedef map<string,sImage*> ImgMap;
typedef ImgMap::iterator itImg;
typedef ImgMap::const_iterator citImg;
typedef pair<itImg,bool> ibImg;


typedef pair<string,sImage*> ImgPair;





typedef list<sImage*> ImageList;

void DelImg(sImage* p) { 


#ifdef OBJMGR 

	// 由g_RoleMemoryMgr 去處理 ;
	//DeleteObj(p) ;

#else
	delete p; 

#endif

}
void DeleteImg(ImgPair p) { DelImg(p.second); }


// MemPack Pool for multi-threaded loading
std::vector<std::vector<byte>*> g_rawMemList;
std::vector<memPack*> g_memPackList;

void InitMemPackPool(int size)
{
	if(g_memPackList.empty())
	{
		for(int i=0; i<size; ++i)
		{
			g_memPackList.push_back(new memPack());
			g_rawMemList.push_back(new std::vector<byte>());
		}
	}
}

void FreeMemPackPool()
{
	for(int i=0; i<g_memPackList.size() ; ++i)
		delete g_memPackList[i];
	g_memPackList.clear();

	for(int i=0; i<g_memPackList.size() ; ++i)
		delete g_rawMemList[i];
	g_memPackList.clear();
}

memPack * CopyMemPack(int i, const memPack* src)
{
	g_memPackList[i]->DataSize = src->DataSize;
	if(g_rawMemList[i]->size()<src->DataSize)
	{
		g_rawMemList[i]->resize(src->DataSize);
	}
	memcpy(&g_rawMemList[i]->front(), src->Data, src->DataSize);
	g_memPackList[i]->Data = &g_rawMemList[i]->front();
	return g_memPackList[i];
}

#define MAX_LOAD_JOB 16

//---------------------------------------------------------------------------
CImageManager::CImageManager() 
{
    if( GetConfig( "HighTex" ) == "Yes" )
        m_bHighTexture = true;
    else
        m_bHighTexture = false;

	InitMemPackPool(MAX_LOAD_JOB);
}

//---------------------------------------------------------------------------
CImageManager::~CImageManager()
{
    Destory();
	FreeMemPackPool();
}

//---------------------------------------------------------------------------
// 尋找靜態影像資源
//---------------------------------------------------------------------------
sImage* CImageManager::FindStaticImageRes( const char* pTexName )
{

    citImg pos( m_StaticImageResMap.find( pTexName ) );
                 
    if( pos == m_StaticImageResMap.end() )
        return NULL; // 找不到資源
    else
        return pos->second;


}

//---------------------------------------------------------------------------
// 尋找動態影像資源
//---------------------------------------------------------------------------
sImage* CImageManager::FindDynamicImageRes( const char* pTexName )
{

    citImg pos( m_DynamicImageResMap.find( pTexName ) );
                  
    if( pos == m_DynamicImageResMap.end() )
        return NULL; // 找不到資源
    else
        return pos->second;

}

//---------------------------------------------------------------------------
// 取得靜態影像資源
//---------------------------------------------------------------------------
sImage* CImageManager::GetStaticImageRes( const char* pTexName )
{
#ifdef RESMGR

	//鎖定為靜態資源
	GetResMgr().LockRes(pTexName) ;

	map<string , sImage*>::iterator iter ;

	//檢查是否同樣的資源
	iter = MapFind(m_StaticImageResMap , pTexName) ;

	if( iter != m_StaticImageResMap.end())
	{
		//只要有建立過的，表示資源一定存在。因為是靜態
		return iter->second ;
	}
	else
	{
		sImage* pImageRes = NULL ;

		//建立物件資源所有權
		//CreateObj(&pImageRes) ;
		pImageRes =	g_RoleMemoryMgr.GetStaticImage() ;

		//拷貝檔名
		strcpy( pImageRes->szTexFile, pTexName );
		//讀取資源
		pImageRes->pTexture = LoadResource<IDirect3DTexture9 , ResTextrueObj>(pImageRes->szTexFile) ;
		pImageRes->bLoadDone = true;

		if(pImageRes->pTexture != NULL)
		{
			//丟入物件儲存區
			m_StaticImageResMap.insert( make_pair(pTexName , pImageRes)) ;

		}
		//Assert(pImageRes->pTexture != NULL , "GetStaticImageRes() error") ;


		return pImageRes ;

	}

	return NULL ;


//	return LoadResource<sImage , ResImageObj>(pTexName) ;

#else
   sImage* pImageRes = FindStaticImageRes( pTexName );
    if( pImageRes )
        return pImageRes;

    pImageRes = new sImage;
    strcpy( pImageRes->szTexFile, pTexName );
    m_StaticImageResMap.insert( make_pair( pImageRes->szTexFile, pImageRes ) );

    char szTexName[60];
    if( m_bHighTexture )
        strcpy( szTexName, pImageRes->szTexFile );
    else
        strcpy( szTexName, pImageRes->szTexFile );
        //sprintf( szTexName, "Low\\%s", pImage->szTexFile );

    // 讀取封裝檔
    memPack* pMem = LoadFromPacket( szTexName );
    if( pMem == NULL )
        return pImageRes;

    if( FAILED( D3DXCreateTextureFromFileInMemory(
        g_RenderSystem.GetD3DDevice(), pMem->Data,
        pMem->DataSize, &pImageRes->pTexture ) ) )
        pImageRes->pTexture = NULL;

    return pImageRes;

#endif

 
}

//---------------------------------------------------------------------------
// 取得動態影像資源
//---------------------------------------------------------------------------
sImage* CImageManager::GetDynamicImageRes( const char* pTexName )
{


#if defined(RESMGR) && defined(OBJMGR)

	bool exist = false ;
	sImage* pImageRes = NULL ;
	IDirect3DTexture9 *tex = NULL ;

	map<string , sImage*>::iterator iter ;

	iter = MapFind(m_StaticImageResMap , pTexName) ;

	if ( iter != m_StaticImageResMap.end())
	{
		//在靜態的map有資源，表示一定有圖片的資源
		return iter->second ;
	}

	//往資源管理中的記憶體池去尋找

	//tex = FindResFromResPool<IDirect3DTexture9>(pTexName) ;

	//if (tex == NULL)
	//{
	//	//尋找快取中是否有此資源
	//	exist = FindResInCatch<ResTextrueObj>(pTexName) ;
	//	
	//	if (exist)
	//	{
	//		//假如存在的話，按照正常程序去跑，自然會拿到資源
	//		tex = LoadResource<IDirect3DTexture9 , ResTextrueObj>(pTexName) ;
	//	}

	//}

	//檢查是否同樣的資源
	iter = MapFind(m_DynamicImageResMap , pTexName) ;

	/////////			確保物件已經建立完畢		//////////////

	if ( iter == m_DynamicImageResMap.end())
	{
		//沒有找到的話
		//建立物件資源所有權
		//CreateObj(&pImageRes) ;

		pImageRes =	g_RoleMemoryMgr.GetDynamicImage() ; 

		//拷貝檔名
		strcpy( pImageRes->szTexFile, pTexName );
	
		//pImageRes->pTexture = tex ;
		pImageRes->pTexture = NULL ;

		//丟入物件儲存區
		m_DynamicImageResMap.insert( make_pair(pTexName , pImageRes)) ;

	//	if ( tex == NULL)
		{
			//丟入預備讀取區。因為上面的資源搜尋的動作：有可能資源在靜態裡面，
			//但是此物件是第一次建立。常出現於：主角(靜態)跟配角(動態)的資源一樣。
			m_LoadImageListBuffer.push_back( pImageRes) ;
		}


	}
	else
	{
		//傳遞物件指標。
		pImageRes = iter->second;


	}



	//回傳，一定要給需要此物件的位址。
	return pImageRes ;


	

	//return LoadResource<sImage , ResImageObj>(pTexName) ;

#else
  // 先找尋靜態影像資源
    sImage* pImageRes = FindStaticImageRes( pTexName );
    if( pImageRes )
        return pImageRes;
    // 先找尋靜態影像資源
    pImageRes = FindDynamicImageRes( pTexName );
    if( pImageRes )
        return pImageRes;

    // 取得記憶體
    if( m_FreeImageResList.empty() )
    {
        pImageRes = new sImage;
        Assert( pImageRes != 0, "Call new failed !!" );
    }
    else
    {
        pImageRes = m_FreeImageResList.front();
        m_FreeImageResList.pop_front();
    }

    // 儲存資料到Map
    strcpy( pImageRes->szTexFile, pTexName );
    m_DynamicImageResMap.insert( make_pair( pImageRes->szTexFile, pImageRes ) );//Map<>

	IDirect3DTexture9* pLoadingTex = FindResource<IDirect3DTexture9 , ResTextrueObj>(pImageRes->szTexFile) ; 
	if(pLoadingTex==NULL)
		m_LoadImageListBuffer.push_back( pImageRes ); //List<>
	else
	{
		pImageRes->pTexture = pLoadingTex ;
		pImageRes->bLoadDone = true ; 
	}
    return pImageRes;

#endif

  
}

//---------------------------------------------------------------------------
// 刪除動態影像資源
//---------------------------------------------------------------------------
bool CImageManager::RemoveDynamicImageRes( const char* pTexName )
{

#ifdef OBJMGR

	//禁止使用

#else
    itImg pos( m_DynamicImageResMap.find(pTexName) );
    if( pos == m_DynamicImageResMap.end() )
        return false;

    sImage* pImage = pos->second;
    m_DynamicImageResMap.erase( pos );
	pImage->Clear();
    m_FreeImageResList.push_back( pImage );
#endif

   
    return true;



}

//---------------------------------------------------------------------------
// 清除所有動態
//---------------------------------------------------------------------------

void CImageManager::ClearImageBuffer()
{
	if(m_pThread != NULL)
	{
		if(m_pThread->joinable())
		{
			m_pThread->join() ; 
		}
		m_pThread.reset() ; 

	}
	
	m_LoadImageListBuffer.clear();

}


void CImageManager::RemoveAllDynamicImageRes()
{
#ifdef OBJMGR
	if(this->m_pThread)
	{
		m_pThread->join();
		m_pThread.reset() ; 	
	}
	
    // 清除動態影像資源
    for_each( m_DynamicImageResMap.begin(), m_DynamicImageResMap.end(), DeleteImg );


    // 清除暫存資源
    //m_LoadImageListBuffer.clear();

#else

    itImg end( m_DynamicImageResMap.end() );
    for( itImg pos( m_DynamicImageResMap.begin() ); pos!=end; ++pos )
    {
        pos->second->Clear();
        m_FreeImageResList.push_back( pos->second );
     }
#endif


    m_DynamicImageResMap.clear();

}

//---------------------------------------------------------------------------
// 摧毀釋放所有資源
//---------------------------------------------------------------------------
void CImageManager::Destory()
{

	if(m_pThread != NULL)
	{
		if(m_pThread->joinable())
		{
			m_pThread->join() ; 
		}
		m_pThread.reset() ; 

	}
    //// 清除靜態影像資源
    for_each( m_StaticImageResMap.begin(), m_StaticImageResMap.end(), DeleteImg );
    m_StaticImageResMap.clear();

    //// 清除動態影像資源
    for_each( m_DynamicImageResMap.begin(), m_DynamicImageResMap.end(), DeleteImg );
    m_DynamicImageResMap.clear();

    // 清除資源指標串列
    for_each( m_FreeImageResList.begin() , m_FreeImageResList.end(), DelImg );
    m_FreeImageResList.clear();

     // 清除暫存資源
    m_LoadImageListBuffer.clear();

}

//void LoadImageBufferOp::operator ()
//{
//	static PacketReader PR; 
//
//	m_bLock = false ; 
//
//	if(m_threadImageList->empty())
//	{
//		m_bLock = true ; 
//
//		return ; 
//	}
//
//	list<sImage*>::iterator iter = m_threadImageList->begin() ; 
//
//
//	for(; iter != m_threadImageList->end() ; ++iter)
//	{
//		sImage* pImage = *iter ; 
//
//		//建立資源。
//		pImage->curState = THREAD ; 
//	 
//		pImage->pMem = PR.LoadFromFileEx( pImage->szTexFile );
//
//		if(pImage->pMem == NULL)
//		{
//			pImage->curState = FILEVAILD ;
//		}
//		else
//		{
//			pImage->curState = LOADDONE ;
//
//		}
//	}
//
//	m_bLock = true ; 
//
//}

void LoadImageBuffer2(vector<sImage*> &imageList, bool *jobEnd)
{
	static PacketReader PR; 

	std::vector<sImage*>::iterator it;
	int i=0; 
	int totalsize = 0;
	for(it=imageList.begin(); it!=imageList.end(); it++, i++)
	{
		sImage *pImage = *it;
		//建立資源。
		pImage->curState = THREAD ; 
	 	memPack *p = PR.LoadFromFileEx( pImage->szTexFile );

		if(p!=NULL)
		{
			totalsize += p->DataSize;
			pImage->pMem = CopyMemPack(i, p);
			//_LOGA(0, "Load Image [%d]: %s (%d)", i, pImage->szTexFile, p->DataSize);
		}
		else 
			pImage->pMem = NULL;

		if(pImage->pMem == NULL)
		{
			pImage->curState = FILEVAILD ;
		}
		else
		{
			pImage->curState = LOADDONE ;

		}
	}
	//_LOGA(0, "Total Size: (%d)", totalsize);
	*jobEnd = true;
}



//void LoadImageBuffer(sImage* pImage) 
//{
//	static PacketReader PR; 
//
//	//_LOGA(0, "thread loading %s\n", pImage->szTexFile);
//
//	boost::xtime xt;
//	boost::xtime_get(&xt, boost::TIME_UTC);
//
//	xt.nsec += 1000000 * 100;
//
//    //建立資源。
//	pImage->curState = THREAD ; 
// 
//	memPack *p = PR.LoadFromFileEx( pImage->szTexFile );
//
//	if(p!=NULL)
//		pImage->pMem = p;
//
//	if(pImage->pMem == NULL)
//	{
//		pImage->curState = FILEVAILD ;
//	}
//	else
//	{
//		pImage->curState = LOADDONE ;
//
//	}
//
//	//boost::thread::sleep(xt) ; 
//}
//---------------------------------------------------------------------------
// 處理影像讀取緩衝區 - 一次處理一個
//---------------------------------------------------------------------------
bool CImageManager::ProcessImageBuffer()
{
	if(m_pThread != NULL)
	{
		if(m_JobEnd)
		{
			//表示已經讀取完畢
			for(int i=0; i<m_loadingJobList.size(); ++i)
			{
				sImage *pLoadingImage =  m_loadingJobList[i];
				
				//assert(!(pLoadingImage->curState == FILEVAILD || pLoadingImage->curState == THREAD));
				if(pLoadingImage->curState == ::LOADDONE)
				{
					IDirect3DTexture9* pLoadingTex = LoadResource<IDirect3DTexture9 , ResTextrueObj>(pLoadingImage->szTexFile ,
						pLoadingImage->pMem) ;

					pLoadingImage->pMem = NULL ; 
					pLoadingImage->pTexture = pLoadingTex ; 
					pLoadingImage->bLoadDone = true ;
				}
			}
			m_loadingJobList.clear();
			m_pThread->join();
			m_pThread.reset() ; 		
		}
	}
	else
	{
	    if( m_LoadImageListBuffer.size() )
		{
			m_JobEnd = false;
			m_loadingJobList.resize(0);
			list<sImage*>::iterator it;
			for(it=m_LoadImageListBuffer.begin(); it!=m_LoadImageListBuffer.end(); ++it)
			{
				m_loadingJobList.push_back(*it);	
				if(m_loadingJobList.size()==MAX_LOAD_JOB)
					break;
			}
			if(m_loadingJobList.size()==m_LoadImageListBuffer.size())
				m_LoadImageListBuffer.clear();
			else
			{
				for(int i=0; i<m_loadingJobList.size() ; i++)
				{
					m_LoadImageListBuffer.pop_front();
				}
			}
			m_pThread.reset(new boost::thread(boost::bind(&LoadImageBuffer2 , m_loadingJobList, &m_JobEnd))) ; 
		}
	}
	
	return true; 
}

//---------------------------------------------------------------------------
// 移除全部影像讀取緩衝區
//---------------------------------------------------------------------------
void CImageManager::RemoveAllImageBuffer()
{ // 不需釋放指標記憶體,有其他物件在管理
    m_LoadImageListBuffer.clear();
}

bool CImageManager::PreLoadRAT(CRATFactory* pRAT)
{
	memPack* pRATMem = LoadFromPacket("Bin\\RATPreLoad.BIN" ) ;

	Assert(pRATMem->Data != NULL , "RATPreLoad Error") ;
    const unsigned int iSize = 50;

//   // 計算資料筆數
    unsigned int iDataCount = pRATMem->DataSize / iSize;

   char szFileName[iSize];



    // 檢查資料大小
    if( pRATMem->DataSize % iSize )
    {
        Assert( 0, "RATPreLoad failed, DataSize error !!" );
        return false;
    }

	////////////////////////		pre_load RAT			//////////////////////////

	unsigned char *temp = pRATMem->Data ; 

	iDataCount = pRATMem->DataSize / iSize;


    for( unsigned int i=0; i<iDataCount; ++i )
    {
        memcpy( &szFileName, temp, iSize );

		SP_RATFile	pRATFile ; 

		if(!pRAT->GetRATFile( szFileName, pRATFile ) )
		{
			Assert(0 , "pre-load RAT error") ; 
		}

	
        temp += iSize;
    }


	return true ; 
}

//---------------------------------------------------------------------------
// 預先讀取影像
//---------------------------------------------------------------------------
bool CImageManager::LoadImageAhead()
{
#ifdef RESMGR




	//抓壓縮資料
	memPack* pMem = LoadFromPacket("Bin\\Preload.Bin" ) ;// LoadResource<memPack , ResMemObj>("Bin\\Preload.Bin") ;

	//memPack* pRPCMem = LoadFromPacket("Bin\\RPCPreLoad.BIN" ) ;


	//memPack* pPARTMem = LoadFromPacket("Bin\\PARTPreLoad.BIN" ) ;

	memPack* pMeshMem = LoadFromPacket("Bin\\MeshPreLoad.BIN" ) ;
	

	Assert(pMem->Data != NULL , "LoadImageAhead Error") ;
	//Assert(pRPCMem->Data != NULL , "RPCPreLoad Error") ;
	//Assert(pPARTMem->Data != NULL , "PARTPreLoad Error") ;

    const unsigned int iSize = 50;

    // 檢查資料大小
    if( pMem->DataSize % iSize )
    {
        Assert( 0, "LoadImageAhead failed, DataSize error !!" );
        return false;
    }
    // 檢查資料大小
    //if( pRPCMem->DataSize % iSize )
    //{
    //    Assert( 0, "RPCPreLoad failed, DataSize error !!" );
    //    return false;
    //}

    //// 檢查資料大小
    //if( pPARTMem->DataSize % iSize )
    //{
    //    Assert( 0, "PRATPreLoad failed, DataSize error !!" );
    //    return false;
    //}

    // 計算資料筆數
    unsigned int iDataCount = pMem->DataSize / iSize;

   char szFileName[iSize];

	//利用一個暫存指標幫助資料偏移。
	//pMem->Data是記錄資料的起始點，千萬不能偏移掉。確保要刪除時，pMem->Data為資料的起始指標
	unsigned char *temp = pMem->Data ;

    // 讀取影像圖檔
    for( unsigned int i=0; i<iDataCount; ++i )
    {
        memcpy( &szFileName, temp, iSize );

		for(int i = 0 ; i < iSize ; ++i)
		{
			if (isupper(szFileName[i]) != 0)                 // 若為大寫字元
				  szFileName[i] = tolower(szFileName[i]);               // 轉成小寫字元

		}

		GetResMgr().DirectLockRes(szFileName) ;
	//	LoadResource<sImage , ResImageObj>(szFileName);

		//粒子需要此函示讀取資源
		LoadStaticTexture(szFileName) ; 
        temp += iSize;
    }

	///////////////////////		pre_load Mesh			////////////////////////////

	temp = pMeshMem->Data ; 

	iDataCount = pMeshMem->DataSize / iSize;


    for( unsigned int i=0; i<iDataCount; ++i )
    {
        memcpy( &szFileName, temp, iSize );

	//	GetResMgr().DirectLockRes(szFileName) ;
//		LoadFromPacket(szFileName ) ;

		for(int i = 0 ; i < iSize ; ++i)
		{
			if (isupper(szFileName[i]) != 0)                 // 若為大寫字元
				  szFileName[i] = tolower(szFileName[i]);               // 轉成小寫字元

		}


		LoadStaticMesh(szFileName) ; 

        temp += iSize;
    }


	//////////////////////		pre_load RPC			//////////////////////////

	//temp = pRPCMem->Data ; 

	//iDataCount = pRPCMem->DataSize / iSize;


 //   for( unsigned int i=0; i<iDataCount; ++i )
 //   {
 //       memcpy( &szFileName, temp, iSize );

	//	std::string name(szFileName) ; 
	//	if(name.find(".ALL") != string::npos)
	//	{
	//		GetResMgr().DirectLockRes(szFileName) ;
	//		LoadFromPacket(szFileName ) ;
	//	}

 //       temp += iSize;
 //   }




	////////////////////////		pre_load PART			//////////////////////////

	//temp = pPARTMem->Data ; 

	//iDataCount = pPARTMem->DataSize / iSize;


 //   for( unsigned int i=0; i<iDataCount; ++i )
 //   {
 //       memcpy( &szFileName, temp, iSize );
	//	GetResMgr().DirectLockRes(szFileName) ;
	//	LoadFromPacket(szFileName ) ;
 //       temp += iSize;
 //   }

	return true ;	



#else
    memPack* pMem = LoadFromPacket( "Bin\\Preload.Bin" );
    if( pMem == NULL )
        return false;

    const unsigned int iSize = 50;

    // 檢查資料大小
    if( pMem->DataSize % iSize )
    {
        Assert( 0, "LoadImageAhead failed, DataSize error !!" );
        return false;
    }

    // 計算資料筆數
    unsigned int iDataCount = pMem->DataSize / iSize;

    // 儲存封裝檔資料
    unsigned char* pData = GetNewMemory( pMem->DataSize );
    memcpy( pData, pMem->Data, pMem->DataSize );

    char szFileName[iSize];

    // 讀取影像圖檔
    for( unsigned int i=0; i<iDataCount; ++i )
    {
        memcpy( &szFileName, pData, iSize );
        GetStaticImageRes( szFileName );
        pData += iSize;
    }

    return true;

#endif


}

//---------------------------------------------------------------------------
// 不會釋放的模型資源
//---------------------------------------------------------------------------
LPD3DXMESH LoadStaticMesh( const string& strFileName )
{
#ifdef RESMGR

//鎖定為靜態資源
	GetResMgr().LockRes(strFileName.c_str()) ;

	return LoadResource<ID3DXMesh , ResXfileObj>(strFileName.c_str()) ;

#else

    return GetPtcStaticMeshMgr().LoadMeshFromFileInMemory( strFileName );

#endif

}

//---------------------------------------------------------------------------
// 讀取靜態材質
//---------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 LoadStaticTexture( const string& strFileName )
{
#ifdef RESMGR

	//鎖定為靜態資源
	GetResMgr().LockRes(strFileName.c_str()) ;

	return LoadResource<IDirect3DTexture9 , ResTextrueObj>(strFileName.c_str()) ;

#else

    return GetPtcStaticTexMgr().LoadTextureFromFileInMemory( strFileName );

#endif


}

//---------------------------------------------------------------------------
// 讀取分子特效材質
//---------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 LoadParticleTexture( const string& strFileName )
{
#ifdef RESMGR

	return LoadResource<IDirect3DTexture9 , ResTextrueObj>(strFileName.c_str()) ;

#else
    return GetPtcTexMgr().LoadTextureFromFileInMemory( strFileName );

#endif

}

//---------------------------------------------------------------------------
// 讀取分子特效Mesh
//---------------------------------------------------------------------------
LPD3DXMESH LoadParticleMesh( const string& strFileName )
{
#ifdef RESMGR

	return LoadResource<ID3DXMesh , ResXfileObj>(strFileName.c_str()) ;

#else

    return GetPtcMeshMgr().LoadMeshFromFileInMemory( strFileName );

#endif

}
LPDIRECT3DTEXTURE9 LoadTexture(const char* pFilename)
{
    return LoadResource<IDirect3DTexture9 , ResTextrueObj>(pFilename) ;
}
JLSkinMesh* LoadSkinMesh(const char* pFilename)
{
    return LoadResource<JLSkinMesh , ResSkinMeshObj>(pFilename) ;
}
//---------------------------------------------------------------------------
// 釋放所有動態材質
//---------------------------------------------------------------------------
void RemoveAllDynamicResource()
{
#ifdef RESMGR

	g_RoleImageManager.RemoveAllDynamicImageRes();

	GetResMgr().ClearDynamic() ;


  
#else

    GetPtcTexMgr().RemoveAllTexture();
    GetPtcMeshMgr().RemoveAllMesh();
    g_RoleImageManager.RemoveAllDynamicImageRes();

#endif

}

//---------------------------------------------------------------------------

void RemoveAllStaticResource()
{
#ifdef RESMGR

	GetResMgr().ClearStatic() ;


#else
    GetPtcStaticTexMgr().RemoveAllTexture();
    GetPtcStaticMeshMgr().RemoveAllMesh();	

#endif

}

}   //namespace FDO

