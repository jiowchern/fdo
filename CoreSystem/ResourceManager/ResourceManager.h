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
#pragma message("�귽�޲z�Ҧ��Ұ�") 

//cosiann
#include <set>

#else
#pragma message("�귽�@��Ҧ��Ұ�") 
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
// �ܼ�-----------------------------------------------------------------------
    bool                    m_bHighTexture;
    map<string,sImage*>     m_StaticImageResMap;
    map<string,sImage*>     m_DynamicImageResMap;

	list<sImage*>			*m_currImageList ; 

    list<sImage*>           m_FreeImageResList;
	vector<sImage*>			m_loadingJobList;
	bool					m_JobEnd;
#ifdef OBJMGR

	list<sImage*>           m_LoadImageListBuffer; // �Ȧs�w�İ�
#else
   list<sImage*>           m_LoadImageListBuffer; // �Ȧs�w�İ�

#endif



// �禡-----------------------------------------------------------------------
    sImage*                 FindStaticImageRes( const char* pTexName );         // �M���R�A�v���귽
    sImage*                 FindDynamicImageRes( const char* pTexName );        // �M��ʺA�v���귽

public:
    CImageManager();
    ~CImageManager();

// �禡-----------------------------------------------------------------------
    sImage*                 GetStaticImageRes( const char* pTexName );          // ���o�R�A�v���귽
    sImage*                 GetDynamicImageRes( const char* pTexName );         // ���o�ʺA�v���귽

    bool                    RemoveDynamicImageRes( const char* pTexName );      // �R���ʺA�v���귽
    void                    RemoveAllDynamicImageRes();                         // �M���Ҧ��ʺA�v���귽
    void                    Destory();                                          // �R������Ҧ��귽
    bool 					ProcessImageBuffer();                               // �v��Ū���w�ĳB�z
    void 					RemoveAllImageBuffer();                              
    bool                    LoadImageAhead();   // �w��Ū��
	void					ClearImageBuffer() ; 
	bool					PreLoadRAT(CRATFactory* pRAT) ; 
};
//---------------------------------------------------------------------------
// API �禡
//---------------------------------------------------------------------------
LPDIRECT3DTEXTURE9  LoadStaticTexture( const string& strFileName );             // Ū���R�A����
LPDIRECT3DTEXTURE9  LoadParticleTexture( const string& strFileName );           // ���l�S�ħ���޲z���禡
LPD3DXMESH          LoadParticleMesh( const string& strFileName );              // ���l�S��Mesh�޲z���禡
LPD3DXMESH          LoadStaticMesh( const string& strFileName );
LPDIRECT3DTEXTURE9  LoadTexture(const char* pFilename);		
JLSkinMesh*         LoadSkinMesh(const char* pFilename);	
void                RemoveAllDynamicResource();			                        // ����Ҧ��ʺA�귽
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

