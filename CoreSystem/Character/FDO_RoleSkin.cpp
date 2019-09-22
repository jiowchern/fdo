#include "stdafx.h"
#include "FDO_RoleMemoryMgr.h"
#include "RoleResourceManage.h"
#include "ResourceManager.h"
#include "FDO_RoleBase.h"
#include "resourceMgr.h"
#include "FDO_RoleSkin.h"
//----------------------------------------------------------------------------
namespace FDO
{
//----------------------------------------------------------------------------
// forward declaration
//----------------------------------------------------------------------------
bool LinkSkinFrmInst(SkinDir& SDirInst, SkinDir& SDirTemp, int iRoleKind);
void FillSkinFrmData(SkinFrm& SFrm, BYTE** ppData, unsigned short usVersion);
SkinAct* FindSkinAct(vSkinAct& SkinActs, const char* pName);
SkinCom* FindSkinCom(vSkinCom& SkinComs, const char* pName);
void UpdateLocalMatrix(sFrame* pFrame, SkinFrm* pSkinFrm, D3DXMATRIX* pMatLocal);
//----------------------------------------------------------------------------
bool SkinFrameSort(SkinFrmInst* i, SkinFrmInst* j)
{
    return (i->pFrame->shLayer<j->pFrame->shLayer);
}
//----------------------------------------------------------------------------
void FillSkinFrmData(SkinFrm& SFrm, BYTE** ppData, unsigned short usVersion)
{
    //SFrm.bTexInvert = *(bool*)pData;
    (*ppData) += sizeof_BOOL;

    SFrm.fWidth = *(float*)(*ppData);
    (*ppData) += sizeof_float;

    SFrm.fHeight = *(float*)(*ppData);
    (*ppData) += sizeof_float;

    SFrm.fTU1 = *(float*)(*ppData);
    (*ppData) += sizeof_float;

    SFrm.fTV1 = *(float*)(*ppData);
    (*ppData) += sizeof_float;

    SFrm.fTU2 = *(float*)(*ppData);
    (*ppData) += sizeof_float;

    SFrm.fTV2 = *(float*)(*ppData);
    (*ppData) += sizeof_float;

    memset( SFrm.szTexFile, 0, LENGTHOF_FILENAME );
    memcpy( SFrm.szTexFile, (*ppData), LENGTHOF_FILENAME-1 );
    (*ppData) += LENGTHOF_FILENAME;

    if( usVersion>1 )
    {
        SFrm.dwColor = *(DWORD*)(*ppData);
        (*ppData) += sizeof_DWORD;
        (*ppData) += 6;
    }
    else
    {
        SFrm.dwColor = 0xFFFFFFFF; // 白色
        (*ppData) += 10;    // 預留欄位
    }
}
//----------------------------------------------------------------------------
// 建立SkinFrame的實體
void CreateSkinFrm(vSkinAct& SkinActs, const char* pComName, BYTE** ppData, 
                   unsigned short usVersion)
{
    for( size_t i=0; i<SkinActs.size(); ++i )
    {
        SkinAct& SAct = SkinActs.at(i);
        memset( SAct.name, 0, LENGTHOF_NAME );
        memcpy( SAct.name, (*ppData), LENGTHOF_NAME-1 );
        (*ppData) += LENGTHOF_NAME;

        vSkinCom& SkinComs = SAct.SkinComs;
        SkinComs.resize(1); // 只有一個零件

        SkinCom& SCom = SkinComs.at(0);
        memset( SCom.name, 0, LENGTHOF_NAME );
        memcpy( SCom.name, pComName, LENGTHOF_NAME-1 );
        SCom.bVisible = true;

        for( int j=0; j<MAX_DIR; ++j )
        {
            SkinDir& SDir = SCom.SkinDirs[j];

            unsigned short usFrameCount = *(unsigned short*)(*ppData);
            (*ppData) += sizeof_nShort;

            vSkinFrm& SkinFrms = SDir.SkinFrms;
            SkinFrms.resize( usFrameCount );

            for( size_t k=0; k<SkinFrms.size(); ++k )
            {
                SkinFrm& SFrm = SkinFrms.at(k);
                FillSkinFrmData( SFrm, ppData, usVersion );
            }
        }
    }
}
//----------------------------------------------------------------------------
void UpdateLocalMatrix(sFrame* pFrame, SkinFrm* pSkinFrm, D3DXMATRIX* pMatLocal)
{
    D3DXMATRIX matTemp;

    D3DXMatrixScaling( pMatLocal, pFrame->vScale.x, pFrame->vScale.y, 1.0f );

    // 計算自轉
    float fShiftX = 0.0f;
    float fShiftY = fShiftY = -(pFrame->vScale.y*pSkinFrm->fHeight*(100.0f-pFrame->fPercentY)*0.01f);

    if( pFrame->bTexInvert )
    {
        fShiftX = -(pFrame->vScale.x*pSkinFrm->fWidth*(pFrame->fPercentX-50.0f)*0.01f*2.0f);
    }
    else
    {
        fShiftX = -(pFrame->vScale.x*pSkinFrm->fWidth*(50.0f-pFrame->fPercentX)*0.01f*2.0f);
    }

    D3DXMatrixTranslation( &matTemp, fShiftX, fShiftY, 0 );
	D3DXMatrixMultiply( pMatLocal, pMatLocal, &matTemp );
	D3DXMatrixRotationZ( &matTemp, pFrame->vScale.z );
	D3DXMatrixMultiply( pMatLocal, pMatLocal, &matTemp );
	D3DXMatrixTranslation( &matTemp, -fShiftX, -fShiftY, 0 );
	D3DXMatrixMultiply( pMatLocal, pMatLocal, &matTemp );

    // 計算位移
    D3DXMatrixTranslation( &matTemp, pFrame->vPosition.x, pFrame->vPosition.y, pFrame->vPosition.z );
    D3DXMatrixMultiply( pMatLocal, pMatLocal, &matTemp );
}
//----------------------------------------------------------------------------
bool LinkSkinFrmInst(SkinDir& SDirInst, SkinDir& SDirTemp, int iRoleKind)
{
    size_t size( SDirInst.SkinFrmInsts.size() );
    if( size != SDirTemp.SkinFrms.size() )
        return false;

    for( size_t i=0; i<size; ++i )
    {
        SkinFrm* pSkinFrm = &SDirTemp.SkinFrms.at(i);

        SDirInst.SkinFrmInsts.at(i).pSkinFrm = pSkinFrm;

        // 讀取材質
        if( pSkinFrm->szTexFile[0] == '\0' )
        {
            pSkinFrm->pImage = NULL;
        }
        else
        {
            if( iRoleKind == Role_Leader )
                pSkinFrm->pImage = g_RoleImageManager.GetDynamicImageRes( pSkinFrm->szTexFile );
            else
                pSkinFrm->pImage = g_RoleImageManager.GetDynamicImageRes( pSkinFrm->szTexFile );
        }

        sFrame* pFrame = SDirInst.SkinFrmInsts.at(i).pFrame;
        D3DXMATRIX* pMatLocal = &SDirInst.SkinFrmInsts.at(i).matLocal;

        UpdateLocalMatrix( pFrame, pSkinFrm, pMatLocal );
    }

    return true;
}

//----------------------------------------------------------------------------
void LinkSkinFrm(vSkinAct& SkinActsInst, vSkinAct& SkinActsTemp, int iRoleKind)
{
    //if( SkinActsInst.size() == SkinActsTemp.size() )
    { // 動作數目相同才比對
        for( size_t i=0; i<SkinActsTemp.size(); ++i )
        {
            SkinAct& SAct = SkinActsTemp.at(i);

            SkinAct* pSAct = FindSkinAct( SkinActsInst, SAct.name );
            if( pSAct == NULL )
            {
                continue;
            }

            for( size_t j=0; j<SAct.SkinComs.size(); ++j )
            {
                SkinCom& SCom = SAct.SkinComs.at(j);

                SkinCom* pSCom = FindSkinCom( pSAct->SkinComs, SCom.name );
                if( pSCom == NULL )
                {
                    continue;
                }

                pSCom->bVisible = SCom.bVisible;

                for( int k=0; k<MAX_DIR; ++k )
                {
                    LinkSkinFrmInst( pSCom->SkinDirs[k], SCom.SkinDirs[k], iRoleKind );
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
SkinCom* FindSkinCom(vSkinCom& SkinComs, const char* pName)
{
    for(size_t i=0; i<SkinComs.size(); ++i )
    {
        SkinCom* pSCom = &SkinComs.at(i);

        if( strcmp( pSCom->pCom->szComponentName, pName ) == 0 )
        {
            return pSCom;
        }
    }

    return NULL;
}
//----------------------------------------------------------------------------
SkinAct* FindSkinAct(vSkinAct& SkinActs, const char* pName)
{
    for(size_t i=0; i<SkinActs.size(); ++i )
    {
        SkinAct* pSAct = &SkinActs.at(i);
        if( strcmp( pSAct->pAct->szActionName, pName ) == 0 )
        {
            return pSAct;
        }
    }

    return NULL;
}
//----------------------------------------------------------------------------
void BuildRoleSkin(RoleSkin* pRoleSkin, list<sRoleAction*>& RoleActList)
{
    //if( (pRoleSkin==NULL) || (spRoleActionList==NULL) )
        //return;

    // 指向實體位置
    //pRoleSkin->pActsTemplate = spRoleActionList;
    // 設定Act數量
    //vSkinAct& SkinActs = pRoleSkin->SkinActs;
    pRoleSkin->SkinActs.resize( RoleActList.size() );
    
    size_t i=0;
    citAct ActEnd( RoleActList.end() );    
    for( itAct ActPos( RoleActList.begin() ); 
        ActPos!=ActEnd; ++ActPos, ++i )
    { // 連結action
        SkinAct& SAct = pRoleSkin->SkinActs.at(i);
        sRoleAction* pAct = (*ActPos);
        // 連結
        SAct.pAct = pAct;
        // 取出
        vSkinCom& SkinComs = SAct.SkinComs;
        // 設定Com的數量
        SkinComs.resize( pAct->ComponentList->size() );
        //
        citCom ComEnd( pAct->ComponentList->end() );
        size_t j = 0;
        for( itCom ComPos( pAct->ComponentList->begin() ); 
            ComPos!=ComEnd; ++ComPos, ++j )
        { // 建立Com的連結
            SkinCom& SCom = SkinComs.at(j);
            sRoleComponent* pCom = (*ComPos);
            // 連結
            SCom.pCom = pCom;
            SCom.bVisible = pCom->bShow;
            //
            for( int k=0; k<MAX_DIR; ++k )
            { // 連結方向
                SkinDir& SDir = SCom.SkinDirs[k];
                // 連結
                SDir.pDir = pCom->ComponentDirs[k];
                // 取出
                vSkinFrmInst& SFrmInsts = SDir.SkinFrmInsts;
                sFrameList& FrameList = *(pCom->ComponentDirs[k]->FrameList);
                // 設定Frame數量
                SFrmInsts.resize( FrameList.size() );
                //
                citFrm FrmEnd( FrameList.end() );
                int l = 0;
                for( itFrm FrmPos( FrameList.begin() ); 
                    FrmPos!=FrmEnd; ++FrmPos, ++l )
                { // 連結Frames
                    sFrame* pFrame = (*FrmPos);
                    SkinFrmInst& SFrmInst = SFrmInsts.at(l);
                    // 連結
                    SFrmInst.pFrame = pFrame;
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void FillFrameData(sFrame* pFrame, BYTE** ppData, unsigned short usVersion)
{
    // 取得資料
    //pFrame->usUnitTime = *(unsigned short*)pData;
    (*ppData) += sizeof_nShort;
    pFrame->usPlayTime = *(unsigned short*)(*ppData);
    (*ppData) += sizeof_nShort;
    pFrame->shLayer = *(short*)(*ppData);
    (*ppData) += sizeof_nShort;
    pFrame->vScale.z = *(float*)(*ppData);
    (*ppData) += sizeof_float;
    pFrame->vScale.x = *(float*)(*ppData);
    (*ppData) += sizeof_float;
    pFrame->vScale.y = *(float*)(*ppData);
    (*ppData) += sizeof_float;
    pFrame->vPosition = *(D3DXVECTOR3*)(*ppData);
    (*ppData) += sizeof_D3DXVECTOR3;
    pFrame->bTexInvert = *(bool*)(*ppData);
    (*ppData) += sizeof_BOOL;

    //pData += 9;    // 預留欄位
    if( usVersion > 1 )
    {
	    pFrame->fPercentX = *(float*)(*ppData);
        (*ppData) += sizeof_float;
        pFrame->fPercentY = *(float*)(*ppData);
        (*ppData) += sizeof_float;
        (*ppData) += 1;
    }
    else
    {
        pFrame->fPercentX = 50.0f;
        pFrame->fPercentY = 100.0f;
	    (*ppData) += 9; // 預留欄位
    }

    pFrame->pImage = NULL;
    pFrame->Color = 0xFFFFFFFF;
}
//----------------------------------------------------------------------------
void FillActionData(sRoleAction* pAction, BYTE** ppData)
{
    // 取得資料
	strcpy( pAction->szActionName, (char*)(*ppData) );
	(*ppData) += 20; // 20
	pAction->bPlayLoop = *(bool*)(*ppData);
	(*ppData) += sizeof_BOOL; // 1
	pAction->dwUpdateTime = *(DWORD*)(*ppData);
	(*ppData) += sizeof_DWORD; // 4
	pAction->bySoundKind = *(BYTE*)(*ppData);
	(*ppData) += sizeof( BYTE ); // 1
	pAction->usHitTime1 = *(unsigned short*)(*ppData);
	(*ppData) += sizeof_nShort; // 2
	pAction->usHitTime2 = *(unsigned short*)(*ppData);
	(*ppData) += sizeof_nShort; // 2
	pAction->usPlaySoundTime1 = *(unsigned short*)(*ppData);
	(*ppData) += sizeof_nShort; // 2
	pAction->usPlaySoundTime2 = *(unsigned short*)(*ppData);
	(*ppData) += sizeof_nShort; // 2
	pAction->usPlaySoundTime3 = *(unsigned short*)(*ppData);
	(*ppData) += sizeof_nShort; // 2
	pAction->usPlayParticleTime = *(unsigned short*)(*ppData);
	(*ppData) += sizeof_nShort; // 2
	strcpy( pAction->szSoundFile1, (char*)(*ppData) );
	(*ppData) += 50; // 50
	strcpy( pAction->szSoundFile2, (char*)(*ppData) );
	(*ppData) += 50; // 50
	strcpy( pAction->szSoundFile3, (char*)(*ppData) );
	(*ppData) += 50; // 50
	strcpy( pAction->szParticleFile, (char*)(*ppData) );
	(*ppData) += 50; // 50

	(*ppData) += 20; // 預留欄位 20
}
//----------------------------------------------------------------------------
/* void CreateImage(const char* pFilename)
{
    memPack* pMem = LoadFromPacket( pFilename );
    if( pMem->Data == NULL )
    {
        return;
    }
    BYTE* pData = pMem->Data;

    boost::shared_ptr<SkinRPC> pNewSkinRPC( new SkinRPC, CReleaseRoleRes() );
    // TODO:丟進記憶體池
    GetRPCManage()->Insert( pFilename , pNewSkinRPC );

    pNewSkinRPC->strFilename = pFilename;

    sRoleHeader Header;
    memset( &Header, 0, sizeof_RoleHeader );
    memcpy( &Header, pData, sizeof_RoleHeader );
    pData += sizeof_RoleHeader;

    if( strcmp( Header.szFileID, "ALL" ) != 0 )
    {
        return;
    }

    //vSkinAct& SkinActs = pNewSkinRPC->SkinActs1;
    pNewSkinRPC->SkinActs1.resize( Header.usActCount );
    for( size_t i=0; i<pNewSkinRPC->SkinActs1.size(); ++i )
    {
        SkinAct& SAct = pNewSkinRPC->SkinActs1.at(i);

        pData += 258; // action 的資料長度

        // 零件總數
        unsigned short usComponentCount = *(unsigned short*)pData;
        pData += sizeof_nShort;

        vSkinCom& SkinComs = SAct.SkinComs;
        SkinComs.resize( usComponentCount );

        for( size_t j=0; j<SkinComs.size(); ++j )
        {
            SkinCom& SCom = SkinComs.at(j);
            memset( SCom.name, 0, LENGTHOF_NAME );
            memcpy( SCom.name, pData, LENGTHOF_NAME-1 );
            pData += 20;

            SCom.bVisible = true;

            for( int k=0; k<MAX_DIR; ++k )
            {
                SkinDir& SDir = SCom.SkinDirs[k];

                // 畫面總數
                unsigned short usFrameCount = *(unsigned short*)pData;
                pData += sizeof_nShort;

                vSkinFrm& SkinFrms = SDir.SkinFrms;
                SkinFrms.resize( usFrameCount );

                for( size_t l=0; l<SkinFrms.size(); ++l )
                {
                    SkinFrm& SFrm = SkinFrms.at(k);
                    FillSkinFrmDataFromALL( SFrm, &pData, Header.usVersion );
                }
            }
        }
    }
} */
//----------------------------------------------------------------------------
/* void FillSkinFrmDataFromALL(SkinFrm& SFrm, BYTE** ppData, unsigned short usVersion)
{
    //pFrame->usUnitTime = *(unsigned short*)(*ppData);
    (*ppData) += sizeof_nShort;
    //pFrame->usPlayTime = *(unsigned short*)(*ppData);
    (*ppData) += sizeof_nShort;
    //pFrame->shLayer = *(short*)(*ppData);
    (*ppData) += sizeof_nShort;
    //pFrame->vScale.z = *(float*)(*ppData);
    (*ppData) += sizeof_float;
    //pFrame->vScale.x = *(float*)(*ppData);
    (*ppData) += sizeof_float;
    //pFrame->vScale.y = *(float*)(*ppData);
    (*ppData) += sizeof_float;
    //pFrame->vPosition = *(D3DXVECTOR3*)(*ppData);
    (*ppData) += sizeof_D3DXVECTOR3;
    //pFrame->bTexInvert = *(bool*)(*ppData);
    (*ppData) += sizeof_BOOL;

    FillSkinFrmData( SFrm, ppData, usVersion );

    // 讀取中心點資訊
    // if( usVersion > 2 )
    // {
        // pFrame->fPercentX = *(float*)(*ppData);
        // (*ppData) += sizeof_float;
        // pFrame->fPercentY = *(float*)(*ppData);
        // (*ppData) += sizeof_float;
    // }
    // else
    // {
        // pFrame->fPercentX = 50.0f;
        // (*ppData) += sizeof_float;
        // pFrame->fPercentY = 100.0f;
        // (*ppData) += sizeof_float;
    // }
    (*ppData) += sizeof_float;
    (*ppData) += sizeof_float;

    (*ppData) += 8; // 預留欄位

    //pFrame->RefreshMatrix( pFrame->vScale, pFrame->vPosition );
} */
//----------------------------------------------------------------------------
/* void FillFrameDataFromALL(sFrame* pFrame, BYTE** ppData, unsigned short usVersion)
{
    //pFrame->usUnitTime = *(unsigned short*)(*ppData);
    (*ppData) += sizeof_nShort;
    pFrame->usPlayTime = *(unsigned short*)(*ppData);
    (*ppData) += sizeof_nShort;
    pFrame->shLayer = *(short*)(*ppData);
    (*ppData) += sizeof_nShort;
    pFrame->vScale.z = *(float*)(*ppData);
    (*ppData) += sizeof_float;
    pFrame->vScale.x = *(float*)(*ppData);
    (*ppData) += sizeof_float;
    pFrame->vScale.y = *(float*)(*ppData);
    (*ppData) += sizeof_float;
    pFrame->vPosition = *(D3DXVECTOR3*)(*ppData);
    (*ppData) += sizeof_D3DXVECTOR3;
    pFrame->bTexInvert = *(bool*)(*ppData);
    (*ppData) += sizeof_BOOL;
    // pFrame->fWidth = *(float*)(*ppData);
    (*ppData) += sizeof_float;
    // pFrame->fHeight = *(float*)(*ppData);
    (*ppData) += sizeof_float;
    // pFrame->fTU1 = *(float*)(*ppData);
    (*ppData) += sizeof_float;
    // pFrame->fTV1 = *(float*)(*ppData);
    (*ppData) += sizeof_float;
    // pFrame->fTU2 = *(float*)(*ppData);
    (*ppData) += sizeof_float;
    // pFrame->fTV2 = *(float*)(*ppData);
    (*ppData) += sizeof_float;
    // strncpy( szTexFile, (char*)(*ppData), 50 );
    (*ppData) += 50;
    // if( usVersion > 1 )
        // pFrame->Color = *(DWORD*)(*ppData);
    // else
        // pFrame->Color = 0xFFFFFFFF;
    (*ppData) += sizeof_DWORD;

    // 讀取中心點資訊
    if( usVersion > 2 )
    {
        pFrame->fPercentX = *(float*)(*ppData);
        (*ppData) += sizeof_float;
        pFrame->fPercentY = *(float*)(*ppData);
        (*ppData) += sizeof_float;
    }
    else
    {
        pFrame->fPercentX = 50.0f;
        (*ppData) += sizeof_float;
        pFrame->fPercentY = 100.0f;
        (*ppData) += sizeof_float;
    }

    (*ppData) += 8; // 預留欄位

    pFrame->RefreshMatrix( pFrame->vScale, pFrame->vPosition );
} */
//----------------------------------------------------------------------------
SkinRAT::SkinRAT()
{
}
//----------------------------------------------------------------------------
SkinRAT::~SkinRAT()
{
    while( !RoleActs.empty() )
    {
        g_RoleMemoryMgr.FreeAction( RoleActs.front() );
        RoleActs.pop_front();
    }
}
//----------------------------------------------------------------------------
void HideRoleSkinCom(RoleSkin* pRoleSkin, const char* pComName)
{
    if( pComName == NULL )
    {
        return;
    }

    for( size_t i=0; i<pRoleSkin->SkinActs.size(); ++i )
    {
        SkinAct& SAct = pRoleSkin->SkinActs.at(i);

        for( size_t j=0; j<SAct.SkinComs.size(); ++j )
        {
            SkinCom& SCom = SAct.SkinComs.at(j);
            if( SCom.pCom && (strcmp( SCom.pCom->szComponentName, pComName ) == 0) )
            {
                SCom.bVisible = false;
                //SCom.pCom = NULL;                
                //for( int k=0; k<MAX_DIR; ++k )
                //{
                //    SkinDir& SDir = SCom.SkinDirs[k];
                //    SDir.pDir = NULL;
                //    SDir.SkinFrmInsts.clear();
                //}
            }
        }
    }
}
//----------------------------------------------------------------------------
} // end of namespace FDO