//---------------------------------------------------------------------------
#ifndef _FDO_RoleMemoryMgr_H_
#define _FDO_RoleMemoryMgr_H_

#include "singleton.h"
#include <d3dx9math.h>
#include <d3d9.h>
#include <list>
#include <string>
#include <map>
#include "FDO_RoleStruct.h"
#include <boost/pool/object_pool.hpp> 
#include <boost\thread\thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/thread/tss.hpp>
#include "packetReader.h"
#include "CRoleDef.h"
#include "ElementDef.h"
using std::string;
using std::list;
using std::map;

#define g_RoleMemoryMgr RoleMemoryMgr::GetSingleton()

//////////////////////								////////////////////////////

#ifdef RESMGR
#pragma message("�귽�޲z�Ҧ��Ұ�") 

#else
#pragma message("�귽�@��Ҧ��Ұ�") 
#endif

//---------------------------------------------------------------------------
namespace FDO
{
	class CRoleRenderer ; 

#ifdef RESMGR

//---------------------------------------------------------------------------

//����ActionPack����귽
void FreeActionPack(std::list<std::list<FDO::sRoleAction*> > &actpack) ;
//����ActionList����귽
void FreeActionList(std::list<FDO::sRoleAction*> &actlist) ;
//����Action����귽
void FreeAction(FDO::sRoleAction *act) ;

class CFreeAction
{
	typedef std::list<FDO::sRoleAction*>				   Actlist ;
	typedef std::list<Actlist>							   Actpack ;
	typedef std::map<std::string , Actpack>				   Actmap;
public:
	CFreeAction(void );
	~CFreeAction(void);
private:
	CFreeAction(const CFreeAction&) ;
	CFreeAction& operator= (const CFreeAction&) ;
public:
	void Init(unsigned int packnums , unsigned int actnums , const DWORD time) ;
	bool Push(const std::string &actname , std::list<FDO::sRoleAction*> &act);
	void Get(const std::string &actname , std::list<FDO::sRoleAction*> &act);

	/////////					�s�W�\�� 080520					///////////////////////
    void UpdateTime(DWORD time) ;							//��s�R���ɶ��C
private:
	bool CheckPackNums(void)  const ;
	bool CheackActNums(const std::string &actname)  ;
	void CheckActMaxNums(void)  ;
	/////////					�s�W�\�� 080520					///////////////////////
	void PushCatch(const std::string &actname , Actlist &act) ;
	void PushCatch(const std::string &actname , Actpack &pack) ; 
	void PopCatch(void) ;									//�R���@��action pack
private:
	Actmap													m_act ; 
	Actmap::iterator										m_tempact ;	//�������J��
	Actmap::iterator										m_muchact ; //�����Y�ʧ@�ʥ]���̤j�q
	unsigned int											m_maxactnums ;
	unsigned int											m_maxpacknums ;


	/////////					�s�W�ݩ� 080520					///////////////////////

	struct PackDate
	{
		std::string	      filename ; 
		Actlist			  actlist ; 
	};
	std::list<PackDate>										m_packlist ; //��m�ݧR����action pack
	
	DWORD													m_curtime ; //�������ɶ�
	DWORD													m_maxtime ; //�̤j���ɶ�
	

};


//---------------------------------------------------------------------------


#else

#endif

struct stUnLoadRPC
{
	SP_RPCFile	spRPCFile ; 
	SP_RATFile	spRATFile ; 
	BYTE*		pData;
	int			RoleID ;
	DWORD		SkinID ; 
	std::string pFileName ;
	CRPCFactory *pRPCFactory ; 
	CRoleRenderer  *pRoleRender ; 
};

struct stRPCThreadStream
{
	enLoadingState	state ; 
	std::string		filename ; 
	memPack			*pMem ; 
	stUnLoadRPC		rpc ;
	CRoleRenderer  *pRoleRender ;

};

struct stUnLoadingAction
{
	unsigned short		version ; 
	sRoleActionList*	pActionList ; 
	BYTE*				pData ;
	unsigned int		actCount;
};



struct stRoleRenderAsset
{
	int RoleID ; 
	CRoleRenderer *pRoleRender ;
	DWORD			SkinID ; 
};

void LoadActionThread(stUnLoadingAction *pUnloadAct) ; 

class RoleMemoryMgr : public Singleton<RoleMemoryMgr>
{

private:
	boost::object_pool<sFrame>				*m_pFramePool ;
//	boost::pool<>							*m_pTempFramePool ;

	std::list<sFrame*>						m_FreeFrameList ; 

	boost::object_pool<sImage>				*m_pImagePool ;

	//boost::pool<>							*m_pImagePool ;
	boost::object_pool<sImage>				*m_pImageStaticPool ;

	//boost::pool<>							*m_pComponentDir  ;
	//boost::pool<>							*m_pRoleComponent  ;
	//boost::pool<>							*m_pRoleAction ;


	boost::object_pool<sComponentDir>		*m_pComponentDir  ;
	boost::object_pool<sRoleComponent>		*m_pRoleComponent  ;


	//ObjFactory<sRoleAction>			*m_pRoleAction ;
	boost::object_pool<sRoleAction>		    *m_pRoleAction ;

	std::list<stUnLoadingAction>			m_unloadingActVec ; 
public:
	void CreatePools(); 
	void DeletePools() ; 
	bool LoadActions() ; 
	bool LoadRPC() ; 
	bool LoadRPCFile() ;
	void ClearLoadActions() ; 
	void ClearLoadRPCs(); 
	void ClearSteamRPCs() ; 

	void PushUnLoadingAction(unsigned short	version , sRoleActionList* pActList , BYTE *pData , unsigned int actCount) ;
	void PushUnLoadingRPC(CRoleRenderer *pRoleRenderer , CRPCFactory *pRPCFactory , SP_RPCFile &spRPCFile , SP_RATFile &spRATFile , 
							BYTE*	pData ,int RoleID , int SkinID  ,const char* pFileName) ; 
	void PushRPCStream(const char *filename , CRoleRenderer *pRoleRenderer , CRPCFactory *pRPCFactory , SP_RPCFile &spRPCFile , SP_RATFile &spRATFile , 
							uint32 &roleID) ; 
	void UnRegisterLinkRPC(DWORD skinID) ; 
private:
//     sRoleActionList         m_FreeActionList;
//     sRoleComponentList      m_FreeComponentList;
//     sFrameList              m_FreeFrameList;
	bool RegisterLinkRPC(CRoleRenderer *pRoleRenderer , int RoleID , DWORD SkinID) ; 


    void                    Destory();
public:
    RoleMemoryMgr();
    ~RoleMemoryMgr();

    sRoleAction*            GetAction	();
    sRoleComponent*         GetComponent();
    sFrame*                 GetFrame();
	sFrame*					GetTempFrame() ; 
	void					ReleaseTempFrame(sFrame* pFrame) ; 
	sImage*					GetDynamicImage() ; 
	sImage*					GetStaticImage() ; 

    void FreeAction( sRoleAction* p );
    void FreeComponent( sRoleComponent* p );
    void FreeFrame( sFrame* p );  

	void					ResourceRequesterRegister	(TdgHandle hRequester);
	void					ResourceRequesterUnregister	(TdgHandle hRequester);
private:
	
	//boost::thread_group			m_threads ; 
	boost::shared_ptr<boost::thread>		m_pThread ; 
	boost::shared_ptr<boost::thread>		m_pRPCThread ; 
	boost::shared_ptr<boost::thread>		m_pRPCStreamThread ; 


	std::list<stRoleRenderAsset>			m_roleRendererList; 
	std::list<stUnLoadRPC>					m_unloadingRPCList ; 
	std::list<stRPCThreadStream>			m_unloadingStreamList; 
	std::list<TdgHandle>					m_Requesters; 
};

//---------------------------------------------------------------------------
// MemDataMgr Class
//---------------------------------------------------------------------------
struct sMemData;
class MemDataMgr
{
private:
    map<string, sMemData*>  m_MemDataMap;
public:
     MemDataMgr();
    ~MemDataMgr();

//����禡-------------------------------------------------------------------
    unsigned char*          GetMemData( const string& strName );                // Ū���O������
    unsigned char*          AddMemData( const string& strName, unsigned char* pData, unsigned int iSize );// �W�[�O������
    bool                    RemoveMemData( const string& strName );             // �R���O������
    void                    RemoveAllMemData();                                 // �����Ҧ��O������
};

//---------------------------------------------------------------------------
// API �禡
//---------------------------------------------------------------------------
unsigned char*          GetNewMemory( unsigned int iMemorySize );               // ���o�ťհO����
unsigned char*          GetNewMemory2( unsigned int iMemorySize );              // ���o�ťհO����2
//----------------------------------------------------------------------------
bool FrameSort(sFrame* i, sFrame* j);
//---------------------------------------------------------------------------
} //namespace FDO
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------


