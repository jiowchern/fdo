#ifndef _FDO_ROLESKIN_H_
#define _FDO_ROLESKIN_H_
//----------------------------------------------------------------------------
#include "CRoleDef.h"
#include <d3dx9.h>
//#include <boost\shared_ptr.hpp>
#include <vector>
#include <list>
#include <string>


using std::vector;
using std::list;
using std::string;
//----------------------------------------------------------------------------
namespace FDO
{
//const int MAX_DIR = 8;
//const int LENGTHOF_NAME = 20;
//const int LENGTHOF_FILENAME = 50;
//const int sizeof_float = sizeof( float );

//const int sizeof_BYTE = sizeof( BYTE );
//----------------------------------------------------------------------------
// forward declaration
//----------------------------------------------------------------------------
struct sImage;
struct sFrame;
struct sRoleComponent;
struct sRoleAction;
//----------------------------------------------------------------------------
struct SkinFrm
{
    float fWidth;
    float fHeight;
    float fTU1;
    float fTV1;
    float fTU2;
    float fTV2;
    char  szTexFile[LENGTHOF_FILENAME];
	
    DWORD dwColor;

    sImage* pImage; // 影像
};
typedef vector<SkinFrm> vSkinFrm;
//---------------------------------------------------------------------------
struct SkinFrmInst
{
    sFrame* pFrame; // for link
    SkinFrm* pSkinFrm;

    D3DXMATRIX matLocal;
};
typedef vector<SkinFrmInst> vSkinFrmInst;
//---------------------------------------------------------------------------
struct SkinDir
{
    sComponentDir* pDir; // for link

    //union
    //{
        vSkinFrm SkinFrms;
        vSkinFrmInst SkinFrmInsts;
    //};
};
//---------------------------------------------------------------------------
struct SkinCom
{
    char name[LENGTHOF_NAME];
    sRoleComponent* pCom; // for link
    bool bVisible;
    SkinDir SkinDirs[MAX_DIR];
};
typedef vector<SkinCom> vSkinCom;
//---------------------------------------------------------------------------
struct SkinAct
{
    char name[LENGTHOF_NAME];
    sRoleAction* pAct; // for link
    vSkinCom SkinComs;
};
typedef vector<SkinAct> vSkinAct;
//---------------------------------------------------------------------------
struct RoleSkin
{
    //boost::shared_ptr<sRoleActionList> m_spActionList; // 指向實體的ActionList
    vSkinAct SkinActs;
};
//---------------------------------------------------------------------------
struct SkinRAT
{
     SkinRAT();
    ~SkinRAT();

    list<sRoleAction*> RoleActs;

    string strFilename; // 紀錄RAT檔名
    float fScaleX;
    float fScaleY;
    float fScaleZ;
    float fShadowSize;
    float fShadowOffset;
    unsigned short usBoundingBoxSize;
    unsigned short usDeadBoundBoxSize;
};
//---------------------------------------------------------------------------
struct SkinRPC
{
    string strFilename; // 紀錄RPC檔名
    vSkinAct SkinActs1;
    vSkinAct SkinActs2;
};
//---------------------------------------------------------------------------
bool SkinFrameSort(SkinFrmInst* i, SkinFrmInst* j);
void BuildRoleSkin(RoleSkin* pRoleSkin, list<sRoleAction*>& RoleActList);
void FillActionData(sRoleAction* pAction, BYTE** ppData);
void HideRoleSkinCom(RoleSkin* pRoleSkin, const char* pComName);
//void FillSkinFrmDataFromALL(SkinFrm& SFrm, BYTE** ppData, unsigned short usVersion);
void FillFrameData(sFrame* pFrame, BYTE** ppData, unsigned short usVersion);
//void FillFrameDataFromALL(sFrame* pFrame, BYTE** ppData, unsigned short usVersion); // for CreateActionListFromAll use
//void CreateImage(const char* pFilename);
void CreateSkinFrm(vSkinAct& SkinActs, const char* pComName, BYTE** ppData, unsigned short usVersion);
void LinkSkinFrm(vSkinAct& SkinActsInst, vSkinAct& SkinActsTemp, int iRoleKind);
//----------------------------------------------------------------------------
} // end of namespace FDO
//----------------------------------------------------------------------------
#endif // _FDO_ROLESKIN_H_