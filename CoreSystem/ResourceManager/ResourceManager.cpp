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
#pragma message("�޲z�Ҧ��Ұ�") 
#else
#pragma message("�@��Ҧ��Ұ�") 
#endif


#ifdef OBJMGR
#pragma message("����޲z�Ҧ��Ұ�") 

#else
#pragma message("����@��Ҧ��Ұ�") 
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

    LPDIRECT3DTEXTURE9      LoadTextureFromFileInMemory(const string& FileName);//Ū������
    LPDIRECT3DTEXTURE9      LoadTextureFromFile(const string& FileName);        //Ū������
    LPDIRECT3DTEXTURE9      FindTexture(const string &FileName);                //�M�����O�_�s�b
    void                    RemoveTexture(const string &FileName);              //�R������귽
    void                    RemoveAllTexture();                                 //�����Ҧ�����귽
};

TextureMgr& GetPtcTexMgr()
{
    static TextureMgr PtcTexMgr; // �ŧi�������
    return PtcTexMgr;
}
//<gino>
//for static texture
TextureMgr& GetPtcStaticTexMgr()
{
    static TextureMgr PtcTexMgr; // �ŧi�������
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

//Mesh�禡-------------------------------------------------------------------
    LPD3DXMESH              LoadMeshFromFileInMemory(const string &FileName);   //Ū��Mesh
    LPD3DXMESH              LoadMeshFromFile(const string &FileName);           //Ū��Mesh
    LPD3DXMESH              FindMesh(const string &FileName);                   //�M��Mesh�O�_�s�b
    void                    RemoveMesh(const string &FileName);                 //�R��Mesh�귽
    void                    RemoveAllMesh();                                    //�����Ҧ�Mesh�귽
};
MeshMgr& GetPtcMeshMgr()
{
    static MeshMgr PtcMeshMgr; // �ŧi�������
    return PtcMeshMgr;
}

//<gino>
// for static mesh
MeshMgr& GetPtcStaticMeshMgr()
{
    static MeshMgr PtcMeshMgr; // �ŧi�������
    return PtcMeshMgr;
}
//</gino>
//---------------------------------------------------------------------------
//  �M��귽��Texture�O�_�w�s�b
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
//  �R������귽
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
//�����Ҧ�����귽
//---------------------------------------------------------------------------
void TextureMgr::RemoveAllTexture()
{
    for_each( Textures.begin(), Textures.end(), ReleaseTexture2 );
    Textures.clear();
}
//---------------------------------------------------------------------------
// �q�O����Ū������
//---------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 TextureMgr::LoadTextureFromFileInMemory(const string &FileName)
{
    LPDIRECT3DTEXTURE9 pTexture = NULL;
    ib ret( Textures.insert( TexturePair(FileName,pTexture) ) );
    it pos( ret.first );

    if( ret.second == false )
        return pos->second;

    // �q�O���餤Ū�����
    memPack *pMem = LoadFromPacket( FileName );
    if( pMem == NULL )
        return NULL;

    // �إߧ���귽
    D3DXCreateTextureFromFileInMemory( g_RenderSystem.GetD3DDevice(),
        pMem->Data, pMem->DataSize, &pos->second );

    return pos->second;
}

//---------------------------------------------------------------------------
// �q�ɮ�Ū������
//---------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 TextureMgr::LoadTextureFromFile(const string &FileName)
{
    LPDIRECT3DTEXTURE9 pTexture = NULL;
    ib ret( Textures.insert( TexturePair(FileName,pTexture) ) );
    it pos( ret.first );

    if( ret.second == false )
        return pos->second;

    // �q�O���餤Ū�����
    memPack *pMem = LoadFromPacket( FileName );
    if( pMem == NULL )
        return NULL;

    // �إߧ���귽
    D3DXCreateTextureFromFile( g_RenderSystem.GetD3DDevice(),
        AnsiToUnicode( FileName.c_str() ), &pos->second );

    return pos->second;
}

//---------------------------------------------------------------------------
// MeshMgr Class
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  �M��귽��Mesh�O�_�w�s�b
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
//  �R��Mesh�귽
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
//  �R���Ҧ�Mesh�귽
//---------------------------------------------------------------------------
void MeshMgr::RemoveAllMesh()
{
    for_each( Meshs.begin(), Meshs.end(), ReleaseMesh2 );
    Meshs.clear();
}
//---------------------------------------------------------------------------
// �q�O����Ū��Mesh
//---------------------------------------------------------------------------
LPD3DXMESH MeshMgr::LoadMeshFromFileInMemory(const string &FileName)
{
    LPD3DXMESH pMesh = NULL;
    ibMesh ret( Meshs.insert( MeshPair(FileName,pMesh) ) );
    itMesh pos( ret.first );

    if( ret.second == false )
        return pos->second;

    // �q�O���餤Ū�����
    memPack *pMem = LoadFromPacket( FileName );
    if( pMem == NULL )
        return NULL;

    // �إ߼ҫ��귽
    LPD3DXBUFFER pAdjBuffer = NULL;
    if( SUCCEEDED( D3DXLoadMeshFromXInMemory(
        pMem->Data, pMem->DataSize,
        D3DXMESH_SYSTEMMEM,             // �X��--�Ӻ���N�����޲z���O�����
        g_RenderSystem.GetD3DDevice(),  // Our d3dDevice
        &pAdjBuffer,                    // �Ω�y�z���檺�F����T
        0,                              // materials buffer �]�t���檺������
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
// �q�ɮ�Ū��Mesh
//---------------------------------------------------------------------------
LPD3DXMESH MeshMgr::LoadMeshFromFile(const string &FileName)
{
    LPD3DXMESH pMesh = NULL;
    ibMesh ret( Meshs.insert( MeshPair(FileName,pMesh) ) );
    itMesh pos( ret.first );

    if( ret.second == false )
        return pos->second;

    // �q�O���餤Ū�����
    memPack *pMem = LoadFromPacket( FileName );
    if( pMem == NULL ) return NULL;

    // �إ߼ҫ��귽
    LPD3DXBUFFER pAdjBuffer = NULL;
    if( SUCCEEDED( D3DXLoadMeshFromX(
        AnsiToUnicode( FileName.c_str() ),  // �ɮצW��
        D3DXMESH_SYSTEMMEM,                 // �X��--�Ӻ���N�����޲z���O�����
        g_RenderSystem.GetD3DDevice(),      // Our d3dDevice
        &pAdjBuffer,                        // �Ω�y�z���檺�F����T
        0,                                  // materials buffer �]�t���檺������
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

	// ��g_RoleMemoryMgr �h�B�z ;
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
// �M���R�A�v���귽
//---------------------------------------------------------------------------
sImage* CImageManager::FindStaticImageRes( const char* pTexName )
{

    citImg pos( m_StaticImageResMap.find( pTexName ) );
                 
    if( pos == m_StaticImageResMap.end() )
        return NULL; // �䤣��귽
    else
        return pos->second;


}

//---------------------------------------------------------------------------
// �M��ʺA�v���귽
//---------------------------------------------------------------------------
sImage* CImageManager::FindDynamicImageRes( const char* pTexName )
{

    citImg pos( m_DynamicImageResMap.find( pTexName ) );
                  
    if( pos == m_DynamicImageResMap.end() )
        return NULL; // �䤣��귽
    else
        return pos->second;

}

//---------------------------------------------------------------------------
// ���o�R�A�v���귽
//---------------------------------------------------------------------------
sImage* CImageManager::GetStaticImageRes( const char* pTexName )
{
#ifdef RESMGR

	//��w���R�A�귽
	GetResMgr().LockRes(pTexName) ;

	map<string , sImage*>::iterator iter ;

	//�ˬd�O�_�P�˪��귽
	iter = MapFind(m_StaticImageResMap , pTexName) ;

	if( iter != m_StaticImageResMap.end())
	{
		//�u�n���إ߹L���A��ܸ귽�@�w�s�b�C�]���O�R�A
		return iter->second ;
	}
	else
	{
		sImage* pImageRes = NULL ;

		//�إߪ���귽�Ҧ��v
		//CreateObj(&pImageRes) ;
		pImageRes =	g_RoleMemoryMgr.GetStaticImage() ;

		//�����ɦW
		strcpy( pImageRes->szTexFile, pTexName );
		//Ū���귽
		pImageRes->pTexture = LoadResource<IDirect3DTexture9 , ResTextrueObj>(pImageRes->szTexFile) ;
		pImageRes->bLoadDone = true;

		if(pImageRes->pTexture != NULL)
		{
			//��J�����x�s��
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

    // Ū���ʸ���
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
// ���o�ʺA�v���귽
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
		//�b�R�A��map���귽�A��ܤ@�w���Ϥ����귽
		return iter->second ;
	}

	//���귽�޲z�����O������h�M��

	//tex = FindResFromResPool<IDirect3DTexture9>(pTexName) ;

	//if (tex == NULL)
	//{
	//	//�M��֨����O�_�����귽
	//	exist = FindResInCatch<ResTextrueObj>(pTexName) ;
	//	
	//	if (exist)
	//	{
	//		//���p�s�b���ܡA���ӥ��`�{�ǥh�]�A�۵M�|����귽
	//		tex = LoadResource<IDirect3DTexture9 , ResTextrueObj>(pTexName) ;
	//	}

	//}

	//�ˬd�O�_�P�˪��귽
	iter = MapFind(m_DynamicImageResMap , pTexName) ;

	/////////			�T�O����w�g�إߧ���		//////////////

	if ( iter == m_DynamicImageResMap.end())
	{
		//�S����쪺��
		//�إߪ���귽�Ҧ��v
		//CreateObj(&pImageRes) ;

		pImageRes =	g_RoleMemoryMgr.GetDynamicImage() ; 

		//�����ɦW
		strcpy( pImageRes->szTexFile, pTexName );
	
		//pImageRes->pTexture = tex ;
		pImageRes->pTexture = NULL ;

		//��J�����x�s��
		m_DynamicImageResMap.insert( make_pair(pTexName , pImageRes)) ;

	//	if ( tex == NULL)
		{
			//��J�w��Ū���ϡC�]���W�����귽�j�M���ʧ@�G���i��귽�b�R�A�̭��A
			//���O������O�Ĥ@���إߡC�`�X�{��G�D��(�R�A)��t��(�ʺA)���귽�@�ˡC
			m_LoadImageListBuffer.push_back( pImageRes) ;
		}


	}
	else
	{
		//�ǻ�������СC
		pImageRes = iter->second;


	}



	//�^�ǡA�@�w�n���ݭn�����󪺦�}�C
	return pImageRes ;


	

	//return LoadResource<sImage , ResImageObj>(pTexName) ;

#else
  // ����M�R�A�v���귽
    sImage* pImageRes = FindStaticImageRes( pTexName );
    if( pImageRes )
        return pImageRes;
    // ����M�R�A�v���귽
    pImageRes = FindDynamicImageRes( pTexName );
    if( pImageRes )
        return pImageRes;

    // ���o�O����
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

    // �x�s��ƨ�Map
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
// �R���ʺA�v���귽
//---------------------------------------------------------------------------
bool CImageManager::RemoveDynamicImageRes( const char* pTexName )
{

#ifdef OBJMGR

	//�T��ϥ�

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
// �M���Ҧ��ʺA
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
	
    // �M���ʺA�v���귽
    for_each( m_DynamicImageResMap.begin(), m_DynamicImageResMap.end(), DeleteImg );


    // �M���Ȧs�귽
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
// �R������Ҧ��귽
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
    //// �M���R�A�v���귽
    for_each( m_StaticImageResMap.begin(), m_StaticImageResMap.end(), DeleteImg );
    m_StaticImageResMap.clear();

    //// �M���ʺA�v���귽
    for_each( m_DynamicImageResMap.begin(), m_DynamicImageResMap.end(), DeleteImg );
    m_DynamicImageResMap.clear();

    // �M���귽���Ц�C
    for_each( m_FreeImageResList.begin() , m_FreeImageResList.end(), DelImg );
    m_FreeImageResList.clear();

     // �M���Ȧs�귽
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
//		//�إ߸귽�C
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
		//�إ߸귽�C
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
//    //�إ߸귽�C
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
// �B�z�v��Ū���w�İ� - �@���B�z�@��
//---------------------------------------------------------------------------
bool CImageManager::ProcessImageBuffer()
{
	if(m_pThread != NULL)
	{
		if(m_JobEnd)
		{
			//��ܤw�gŪ������
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
// ���������v��Ū���w�İ�
//---------------------------------------------------------------------------
void CImageManager::RemoveAllImageBuffer()
{ // ����������аO����,����L����b�޲z
    m_LoadImageListBuffer.clear();
}

bool CImageManager::PreLoadRAT(CRATFactory* pRAT)
{
	memPack* pRATMem = LoadFromPacket("Bin\\RATPreLoad.BIN" ) ;

	Assert(pRATMem->Data != NULL , "RATPreLoad Error") ;
    const unsigned int iSize = 50;

//   // �p���Ƶ���
    unsigned int iDataCount = pRATMem->DataSize / iSize;

   char szFileName[iSize];



    // �ˬd��Ƥj�p
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
// �w��Ū���v��
//---------------------------------------------------------------------------
bool CImageManager::LoadImageAhead()
{
#ifdef RESMGR




	//�����Y���
	memPack* pMem = LoadFromPacket("Bin\\Preload.Bin" ) ;// LoadResource<memPack , ResMemObj>("Bin\\Preload.Bin") ;

	//memPack* pRPCMem = LoadFromPacket("Bin\\RPCPreLoad.BIN" ) ;


	//memPack* pPARTMem = LoadFromPacket("Bin\\PARTPreLoad.BIN" ) ;

	memPack* pMeshMem = LoadFromPacket("Bin\\MeshPreLoad.BIN" ) ;
	

	Assert(pMem->Data != NULL , "LoadImageAhead Error") ;
	//Assert(pRPCMem->Data != NULL , "RPCPreLoad Error") ;
	//Assert(pPARTMem->Data != NULL , "PARTPreLoad Error") ;

    const unsigned int iSize = 50;

    // �ˬd��Ƥj�p
    if( pMem->DataSize % iSize )
    {
        Assert( 0, "LoadImageAhead failed, DataSize error !!" );
        return false;
    }
    // �ˬd��Ƥj�p
    //if( pRPCMem->DataSize % iSize )
    //{
    //    Assert( 0, "RPCPreLoad failed, DataSize error !!" );
    //    return false;
    //}

    //// �ˬd��Ƥj�p
    //if( pPARTMem->DataSize % iSize )
    //{
    //    Assert( 0, "PRATPreLoad failed, DataSize error !!" );
    //    return false;
    //}

    // �p���Ƶ���
    unsigned int iDataCount = pMem->DataSize / iSize;

   char szFileName[iSize];

	//�Q�Τ@�ӼȦs�������U��ư����C
	//pMem->Data�O�O����ƪ��_�l�I�A�d�U���ా�����C�T�O�n�R���ɡApMem->Data����ƪ��_�l����
	unsigned char *temp = pMem->Data ;

    // Ū���v������
    for( unsigned int i=0; i<iDataCount; ++i )
    {
        memcpy( &szFileName, temp, iSize );

		for(int i = 0 ; i < iSize ; ++i)
		{
			if (isupper(szFileName[i]) != 0)                 // �Y���j�g�r��
				  szFileName[i] = tolower(szFileName[i]);               // �ন�p�g�r��

		}

		GetResMgr().DirectLockRes(szFileName) ;
	//	LoadResource<sImage , ResImageObj>(szFileName);

		//�ɤl�ݭn�����Ū���귽
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
			if (isupper(szFileName[i]) != 0)                 // �Y���j�g�r��
				  szFileName[i] = tolower(szFileName[i]);               // �ন�p�g�r��

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

    // �ˬd��Ƥj�p
    if( pMem->DataSize % iSize )
    {
        Assert( 0, "LoadImageAhead failed, DataSize error !!" );
        return false;
    }

    // �p���Ƶ���
    unsigned int iDataCount = pMem->DataSize / iSize;

    // �x�s�ʸ��ɸ��
    unsigned char* pData = GetNewMemory( pMem->DataSize );
    memcpy( pData, pMem->Data, pMem->DataSize );

    char szFileName[iSize];

    // Ū���v������
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
// ���|���񪺼ҫ��귽
//---------------------------------------------------------------------------
LPD3DXMESH LoadStaticMesh( const string& strFileName )
{
#ifdef RESMGR

//��w���R�A�귽
	GetResMgr().LockRes(strFileName.c_str()) ;

	return LoadResource<ID3DXMesh , ResXfileObj>(strFileName.c_str()) ;

#else

    return GetPtcStaticMeshMgr().LoadMeshFromFileInMemory( strFileName );

#endif

}

//---------------------------------------------------------------------------
// Ū���R�A����
//---------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 LoadStaticTexture( const string& strFileName )
{
#ifdef RESMGR

	//��w���R�A�귽
	GetResMgr().LockRes(strFileName.c_str()) ;

	return LoadResource<IDirect3DTexture9 , ResTextrueObj>(strFileName.c_str()) ;

#else

    return GetPtcStaticTexMgr().LoadTextureFromFileInMemory( strFileName );

#endif


}

//---------------------------------------------------------------------------
// Ū�����l�S�ħ���
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
// Ū�����l�S��Mesh
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
// ����Ҧ��ʺA����
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

