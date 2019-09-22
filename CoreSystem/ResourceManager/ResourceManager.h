//---------------------------------------------------------------------------
#ifndef ResourceManager_H
#define ResourceManager_H
//---------------------------------------------------------------------------
#include "singleton.h" 
#include <d3d9.h>
#include <string>
#include <list>
#include <map>
#include <boost\thread\thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/xtime.hpp>

using std::string;
using std::list;
using std::map; 
//////////////////////								////////////////////////////

#ifdef RESMGR
#pragma message("資源管理模式啟動") 

//cosiann
#include <set>

#else
#pragma message("資源一般模式啟動") 
#endif
//////////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------------
namespace FDO
{
//---------------------------------------------------------------------------
// CImageManager Class
//---------------------------------------------------------------------------
#define g_RoleImageManager CImageManager::GetSingleton()
struct sImage;
class JLSkinMesh;
class CRATFactory ; 

class CImageManager : public Singleton<CImageManager>
{
private:
	boost::shared_ptr<boost::thread>			m_pThread ; 
private:
// 變數-----------------------------------------------------------------------
    bool                    m_bHighTexture;
    map<string,sImage*>     m_StaticImageResMap;
    map<string,sImage*>     m_DynamicImageResMap;

	list<sImage*>			*m_currImageList ; 

    list<sImage*>           m_FreeImageResList;
	vector<sImage*>			m_loadingJobList;
	bool					m_JobEnd;
#ifdef OBJMGR

	list<sImage*>           m_LoadImageListBuffer; // 暫存緩衝區
#else
   list<sImage*>           m_LoadImageListBuffer; // 暫存緩衝區

#endif



// 函式-----------------------------------------------------------------------
    sImage*                 FindStaticImageRes( const char* pTexName );         // 尋找靜態影像資源
    sImage*                 FindDynamicImageRes( const char* pTexName );        // 尋找動態影像資源

public:
    CImageManager();
    ~CImageManager();

// 函式-----------------------------------------------------------------------
    sImage*                 GetStaticImageRes( const char* pTexName );          // 取得靜態影像資源
    sImage*                 GetDynamicImageRes( const char* pTexName );         // 取得動態影像資源

    bool                    RemoveDynamicImageRes( const char* pTexName );      // 刪除動態影像資源
    void                    RemoveAllDynamicImageRes();                         // 清除所有動態影像資源
    void                    Destory();                                          // 摧毀釋放所有資源
    bool 					ProcessImageBuffer();                               // 影像讀取緩衝處理
    void 					RemoveAllImageBuffer();                              
    bool                    LoadImageAhead();   // 預先讀取
	void					ClearImageBuffer() ; 
	bool					PreLoadRAT(CRATFactory* pRAT) ; 
};
//---------------------------------------------------------------------------
// API 函式
//---------------------------------------------------------------------------
LPDIRECT3DTEXTURE9  LoadStaticTexture( const string& strFileName );             // 讀取靜態材質
LPDIRECT3DTEXTURE9  LoadParticleTexture( const string& strFileName );           // 分子特效材質管理員函式
LPD3DXMESH          LoadParticleMesh( const string& strFileName );              // 分子特效Mesh管理員函式
LPD3DXMESH          LoadStaticMesh( const string& strFileName );
LPDIRECT3DTEXTURE9  LoadTexture(const char* pFilename);		
JLSkinMesh*         LoadSkinMesh(const char* pFilename);	
void                RemoveAllDynamicResource();			                        // 釋放所有動態資源
//<gino add>
void				RemoveAllStaticResource();
//</gino>

void				LoadImageBuffer() ; 
void				LoadImageBuffer2(vector<sImage*> &imageList, bool *end);
//struct LoadImageBufferOp
//{
//public:
//	LoadImageBufferOp(list<sImage*> *pImageList)
//	{
//		m_threadImageList = pImageList ; 
//		m_bLock = false ; 
//	}
//
//	void operator()() ; 
//
//	bool isLock() { return m_bLock ; } 
//
//	list<sImage*>			*m_threadImageList ; 
//	bool					m_bLock ; 
//};

//---------------------------------------------------------------------------
} // end of namespace FDO
//---------------------------------------------------------------------------
#endif

