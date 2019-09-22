#include "stdafx.h"
//#pragma hdrstop
//----------------------------------------------------------------------------
//#include "J_SoundManager.h"
#include "PhenixD3D9RenderSystem.h"
#include "PacketReader.h"
#include "ResourceManager.h"
#include "DrawPlane.h"
#include "FDO_RoleMemoryMgr.h"
#include "PhenixAssert.h"
#include <vector>
//#include "BinaryTableManager.h"
#pragma hdrstop
#include "JLAnimation2D.h"

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }    

//////////////////////								////////////////////////////

//#define RESMGR

#ifdef RESMGR
#pragma message("�޲z�Ҧ��Ұ�") 
#else
#pragma message("�@��Ҧ��Ұ�") 
#endif

////////////////////////////////////////////////////////////////////////////////////
#define DirectXCursor //�ϥ�DirectXø�s�ƹ����
//----------------------------------------------------------------------------
namespace FDO
{   

//----------------------------------------------------------------------------
JLAnimation2D::JLAnimation2D()
{
    m_pDevice         = g_RenderSystem.GetD3DDevice();
    m_pNowAction      = NULL;
    m_szNowActName[0] = '\0';
    m_szCursorName[0] = '\0';
    m_dwFrameNum      = 1;
    m_dwFrameMax      = 1;
    m_dwPassTime      = 0;
    m_IsVisible       = true;
    m_PlayStatus      = Status_Stop;
    m_iLeftPos        = 0;
    m_iTopPos         = 0;
    m_dwOldFrameNum   = 0;
    m_IsInsertPlay    = false;
    SetRect( &m_RenderRect, 0, 0, 0, 0 );
}
//----------------------------------------------------------------------------
JLAnimation2D::~JLAnimation2D()
{
    Cleanup();
}
//----------------------------------------------------------------------------
// ����귽
//----------------------------------------------------------------------------
void JLAnimation2D::Cleanup()
{
    RoleActionList::iterator end ( m_ActionList.end() );
    for( RoleActionList::iterator i = m_ActionList.begin(); i != end; ++i )
    {
        SAFE_DELETE(*i);
    } // end for
    
    m_ActionList.clear();
    m_pNowAction = NULL;
}
//---------------------------------------------------------------------------
// �����Ҧ��e����T
//---------------------------------------------------------------------------
void RoleAction::RemoveAllFrame()
{
    FrameDataList::iterator end ( FrameList.end() );
    for( FrameDataList::iterator i = FrameList.begin(); i != end; ++i )
    {
        (*i)->pSurface->Release() ;
		SAFE_DELETE( *i );
    }
    FrameList.clear();
}
//----------------------------------------------------------------------------
// ���J�ʵe
//----------------------------------------------------------------------------
bool JLAnimation2D::LoadFromFile( const string& FileName )
{
    FILE* pFileHandle;
    if( ( pFileHandle = fopen( FileName.c_str(), "rb" ) ) == 0 )
        return false;

    // �p���ɮפj�p
    fseek( pFileHandle, 0, SEEK_END);
    vector<unsigned char> MemData( ftell( pFileHandle ) );
    fseek( pFileHandle, 0, SEEK_SET);

    // Ū����Ƭy�������
    fread( &MemData[0], 1, MemData.size(), pFileHandle );

    // �����@�Ӥw�g�}�Ҫ��ɮ׸�Ƭy
    fclose( pFileHandle );

    BYTE *pData = &MemData[0];

    // Ū�����Y��
    sAniHeader *pHeader = (sAniHeader*)pData;

    // �T�{�ɮ�
    if( strcmp( pHeader->FileID, "AN2" ) != 0 )
    {
        //string str;
        //str.assign( "AN2" );
        //str.append(g_BinTable.mCodeInlineTextBin.pFastFindW( 773, CodeInlineText_Text ));//�ɮ׮榡���~
        //str.append( ",LoadAnimation" );
        //str.append(g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ));//����
        Assert( 0, "AN2 �ɮ׮榡���~,  LoadAnimation ����" );
        return false;
    }

    pData += sizeof( sAniHeader );

    sRole2DAction* pAction = NULL;
    sFrameData*  pFrame  = NULL;
    DWORD dwTotalFrame   = 0;

    // ���o�ɮ׸��|
    string strFilePath = FileName;
    strFilePath.erase( strFilePath.find_last_of( "\\" ) +1 );

    // ����귽
    Cleanup();

    // �إ߰ʧ@���
    for( unsigned short i=0; i<pHeader->TotalAction; ++i )
    {
        pAction = new sRole2DAction;
        Assert( pAction != NULL, "Call new failed !!" );
        m_ActionList.push_back( pAction );

        // �x�s���
        strcpy( pAction->szActionName, (char*)pData );
        pData += 20;
        pAction->IsPlayLoop = *(bool*)pData;
        pData += sizeof( bool );
        pAction->IsAimCenter = *(bool*)pData;
        pData += sizeof( bool );
        pAction->usOffsetX = *(unsigned short*)pData;
        pData += sizeof( unsigned short );
        pAction->usOffsetY = *(unsigned short*)pData;
        pData += sizeof( unsigned short );
        pAction->dwUpdateTime = *(DWORD*)pData;
        pData += sizeof( DWORD );
        pData += 50;    // �w�d���

        dwTotalFrame = *(DWORD*)pData;
        pData += sizeof( DWORD );

        // �إߵe�����
        for( DWORD j=0; j<dwTotalFrame; ++j )
        {
            pFrame = new sFrameData;
            Assert( pFrame != NULL, "Call new failed !!" );
            pAction->FrameList.push_back( pFrame );

            pFrame->IsInverse = *(bool*)pData;
            pData += sizeof( bool );
            pFrame->IsPlaySound = *(bool*)pData;
            pData += sizeof( bool );
            pFrame->usFrameWidth = *(unsigned short*)pData;
            pData += sizeof( unsigned short );
            pFrame->usFrameHeight = *(unsigned short*)pData;
            pData += sizeof( unsigned short );
            pFrame->dwFrameNum = *(DWORD*)pData;
            pData += sizeof( DWORD );  
            sprintf( pFrame->szImageFile, "%s%s", strFilePath.c_str(), (char*)pData );
            pData += 30;
            sprintf( pFrame->szSoundFile, "%s%s", strFilePath.c_str(), (char*)pData );
            pData += 30;
            pData += 50;    // �w�d���
            pFrame->pTexture = LoadStaticTexture( pFrame->szImageFile );
			pFrame->pTexture->GetSurfaceLevel( 0 , &(pFrame->pSurface) ) ; 
        }
    }

    return true;
}
//----------------------------------------------------------------------------
// ���J�ʵe
//----------------------------------------------------------------------------
bool JLAnimation2D::LoadFromFileInMemory( const string& FileName )
{
#ifdef RESMGR

    memPack* pMem = LoadFromPacket( FileName );
    
    if( pMem->Data == NULL ) return false;
        

    BYTE* pData = pMem->Data;

#else

    // Ū���ʸ���
    memPack* pMem = LoadFromPacket( FileName );
    if( pMem == NULL )
        return false;

    // �x�s�ʸ��ɸ��
    BYTE* pData = GetNewMemory( pMem->DataSize );
    memcpy( pData, pMem->Data, pMem->DataSize );


#endif


    // Ū�����Y��
    sAniHeader *pHeader = (sAniHeader*)pData;

    // �T�{�ɮ�
    if( strcmp( pHeader->FileID, "AN2" ) != 0 )
    {
        //string str;
        //str.assign( "AN2" );
       // str.append(g_BinTable.mCodeInlineTextBin.pFastFindW( 773, CodeInlineText_Text ));//�ɮ׮榡���~
        //str.append( ",LoadAnimation" );
       // str.append(g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ));//����
        Assert( 0, "AN2 �ɮ׮榡���~,  LoadAnimation ����" );
        return false;
    }

    pData += sizeof( sAniHeader );

    sRole2DAction* pAction = NULL;
    sFrameData*  pFrame  = NULL;
    DWORD dwTotalFrame   = 0;

    // ���o�ɮ׸��|
    string strFilePath = FileName;
    strFilePath.erase( strFilePath.find_last_of( "\\" ) +1 );

    // ����귽
    Cleanup();

    // �إ߰ʧ@���
    for( unsigned short i=0; i<pHeader->TotalAction; ++i )
    {
        pAction = new sRole2DAction;
        Assert( pAction != NULL, "Call new failed !!" );
        m_ActionList.push_back( pAction );

        // �x�s���
        strcpy( pAction->szActionName, (char*)pData );
        pData += 20;
        pAction->IsPlayLoop = *(bool*)pData;
        pData += sizeof( bool );
        pAction->IsAimCenter = *(bool*)pData;
        pData += sizeof( bool );
        pAction->usOffsetX = *(unsigned short*)pData;
        pData += sizeof( unsigned short );
        pAction->usOffsetY = *(unsigned short*)pData;
        pData += sizeof( unsigned short );
        pAction->dwUpdateTime = *(DWORD*)pData;
        pData += sizeof( DWORD );
        pData += 50;    // �w�d���

        dwTotalFrame = *(DWORD*)pData;
        pData += sizeof( DWORD );

        // �إߵe�����
        for( DWORD j=0; j<dwTotalFrame; ++j )
        {
            pFrame = new sFrameData;
            Assert( pFrame != NULL, "Call new failed !!" );
            pAction->FrameList.push_back( pFrame );

            pFrame->IsInverse = *(bool*)pData;
            pData += sizeof( bool );
            pFrame->IsPlaySound = *(bool*)pData;
            pData += sizeof( bool );
            pFrame->usFrameWidth = *(unsigned short*)pData;
            pData += sizeof( unsigned short );
            pFrame->usFrameHeight = *(unsigned short*)pData;
            pData += sizeof( unsigned short );
            pFrame->dwFrameNum = *(DWORD*)pData;
            pData += sizeof( DWORD );  
            sprintf( pFrame->szImageFile, "%s%s", strFilePath.c_str(), (char*)pData );
            pData += 30;
            sprintf( pFrame->szSoundFile, "%s%s", strFilePath.c_str(), (char*)pData );
            pData += 30;
            pData += 50;    // �w�d���
            pFrame->pTexture = LoadStaticTexture( pFrame->szImageFile );
			pFrame->pTexture->GetSurfaceLevel( 0 , &(pFrame->pSurface) ) ; 
        }
    }

    return true;
}  
//----------------------------------------------------------------------------
// �W�[�ʧ@
//----------------------------------------------------------------------------
void JLAnimation2D::AddAction( sRole2DAction* pAction )
{
    m_ActionList.push_back( pAction );
}
//----------------------------------------------------------------------------
// �����ʧ@
//----------------------------------------------------------------------------
bool JLAnimation2D::RemoveAction( const char* pActName )
{
    RoleActionList::iterator end ( m_ActionList.end() );
    for( RoleActionList::iterator i = m_ActionList.begin(); i != end; ++i )
    {
        if( strcmp( (*i)->szActionName, pActName ) == 0 )
        {
            SAFE_DELETE( *i );
            m_ActionList.erase( i );
            m_pNowAction = NULL;

            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
// ���o�ʧ@����
//----------------------------------------------------------------------------
sRole2DAction* JLAnimation2D::GetAction( const char* pActName )
{
    RoleActionList::iterator end ( m_ActionList.end() );
    for( RoleActionList::iterator i = m_ActionList.begin(); i != end; ++i )
    {
        if( strcmp( (*i)->szActionName, pActName ) == 0 )
            return *i;
    }

    return NULL;
}
//----------------------------------------------------------------------------
// ���o�ʧ@���Ц�C
//----------------------------------------------------------------------------
const RoleActionList* JLAnimation2D::GetActionList()
{
    return &m_ActionList;
}     
//----------------------------------------------------------------------------
// �]�w��ܧP�_�l
//----------------------------------------------------------------------------
void JLAnimation2D::SetVisible( bool bValue )
{
    m_IsVisible = bValue;
}
//----------------------------------------------------------------------------
// ���o��ܧP�_�l
//----------------------------------------------------------------------------
bool JLAnimation2D::GetVisible() const
{
    return m_IsVisible;
}
//----------------------------------------------------------------------------
// �]�w��аʵe�W��
//----------------------------------------------------------------------------
bool JLAnimation2D::SetCursorName( const char* pCursorName )
{
    if( strlen( pCursorName ) > 19 )
        return false;

    strcpy( m_szCursorName, pCursorName );
    return true;
}
//----------------------------------------------------------------------------
// �]�w�_�l��m
//----------------------------------------------------------------------------
void JLAnimation2D::SetPosition( const int& iLeft, const int& iTop )
{
    m_iLeftPos = iLeft;
    m_iTopPos  = iTop;
} 
//----------------------------------------------------------------------------
// ���A��s
//----------------------------------------------------------------------------
void JLAnimation2D::FrameMove( const DWORD& dwPassTime )
{
    if( ( m_pNowAction == NULL ) || ( !m_IsVisible ) )
        return;

    // ��s�ʵe��ܱi��
    if( m_PlayStatus == Status_Play )
    {
        m_dwPassTime += dwPassTime;

        if( m_dwPassTime >= m_pNowAction->dwUpdateTime )
        {
            m_dwPassTime = 0;

            if( ++m_dwFrameNum > m_dwFrameMax )
            {
                m_dwFrameNum = 1;

                if( !m_pNowAction->IsPlayLoop )
                {
                    m_dwFrameNum  = m_dwFrameMax;
                    m_PlayStatus  = Status_Pause;

                    // �ˬd�O�_���J����ʧ@
                    if( m_IsInsertPlay )
                    {
                        m_IsInsertPlay = false;

                        if( m_IsPlayCursor )
                            Play( m_szCursorName, false );
                        else
                            Play( m_szNowActName, false );
                    }
                }
            }
        }
    }

#ifdef DirectXCursor

	FrameDataList::iterator itNowFrame, end ( m_pNowAction->FrameList.end() );
    for( FrameDataList::iterator i = m_pNowAction->FrameList.begin(); i != end; ++i )
    {
        if( (*i)->dwFrameNum <= m_dwFrameNum )
            itNowFrame = i;
        else
            break;
    }

	HRESULT hr ;
	IDirect3DSurface9 * pCursorBitmap = (*itNowFrame)->pSurface ;	
	hr = m_pDevice->SetCursorProperties( m_pNowAction->usOffsetX , m_pNowAction->usOffsetY  , (*itNowFrame)->pSurface ) ;
		
#endif

}
//----------------------------------------------------------------------------
// ��Vø�s
//----------------------------------------------------------------------------
void JLAnimation2D::Render()
{
	if( ( m_pNowAction == NULL ) || ( m_pNowAction->FrameList.empty() ) || ( !m_IsVisible ) )
        return;

#ifndef DirectXCursor

	FrameDataList::iterator itNowFrame, end ( m_pNowAction->FrameList.end() );
    for( FrameDataList::iterator i = m_pNowAction->FrameList.begin(); i != end; ++i )
    {
        if( (*i)->dwFrameNum <= m_dwFrameNum )
            itNowFrame = i;
        else
            break;
    }

    m_pDevice->SetTexture( 0, (*itNowFrame)->pTexture );

    // �]�w��V�ϰ�
    // �������
    if( m_pNowAction->IsAimCenter )
    {
        m_RenderRect.left   = m_iLeftPos - m_pNowAction->usOffsetX;
        m_RenderRect.top    = m_iTopPos  - m_pNowAction->usOffsetY;
        m_RenderRect.right  = m_RenderRect.left + (*itNowFrame)->usFrameWidth;
        m_RenderRect.bottom = m_RenderRect.top  + (*itNowFrame)->usFrameHeight;
    }
    // ������W��
    else
    {
        m_RenderRect.left   = m_iLeftPos;
        m_RenderRect.top    = m_iTopPos;
        m_RenderRect.right  = m_iLeftPos + (*itNowFrame)->usFrameWidth;
        m_RenderRect.bottom = m_iTopPos  + (*itNowFrame)->usFrameHeight;
    }

    // ø�s�ϧ�    
    DrawAnimation( m_RenderRect, (*itNowFrame)->IsInverse );

#endif
    /*
    // ���񭵮�
    if( ( (*itNowFrame)->IsPlaySound ) && ( m_dwOldFrameNum != m_dwFrameNum ) )
        g_SoundManager._Play2DSoundInMemory( (*itNowFrame)->szSoundFile );   //���� ***

    // ��������i��,�H�P�_�O�_���񤣦P�i��
    if( m_dwOldFrameNum != m_dwFrameNum )
        m_dwOldFrameNum = m_dwFrameNum;
    */
} 
//----------------------------------------------------------------------------
// ����   //�즲 ***
//----------------------------------------------------------------------------
void JLAnimation2D::Play( const char* pActionName, bool IsCheck )
{
    if( IsCheck )
    {
        // �ˬd�O�_���J���񤤻P�O�_����ۦP�ʧ@
        if( m_IsInsertPlay || ( strcmp( m_szNowActName, pActionName ) == 0 ) )
            return;
    }

    strcpy( m_szNowActName, pActionName );
    m_PlayStatus = Status_Play;      //???
    m_dwFrameNum = 1;
    m_dwOldFrameNum = 0;
    m_dwFrameMax = 1;
    m_pNowAction = NULL;

    // �M��ʧ@�W�٨ó]�w�i��
    RoleActionList::iterator end ( m_ActionList.end() );
    for( RoleActionList::iterator i = m_ActionList.begin(); i != end; ++i )
    {
        if( strcmp( (*i)->szActionName, m_szNowActName ) == 0 )
        {
            m_pNowAction = (*i);
            if(!m_pNowAction->FrameList.empty())
                m_dwFrameMax = m_pNowAction->FrameList.back()->dwFrameNum;//?? what purpose?

            break;
        }
    }
}
//----------------------------------------------------------------------------
// ���J����
//----------------------------------------------------------------------------
void JLAnimation2D::InsertPlay( const char* pActionName, bool IsPlayCursor )
{
    // �ˬd�O�_���J����
    if( m_IsInsertPlay )
        return;

    m_IsInsertPlay = true;
    m_IsPlayCursor = IsPlayCursor;
    m_PlayStatus = Status_Play;
    m_dwFrameNum = 1;
    m_dwOldFrameNum = 0;
    m_dwFrameMax = 1;
    m_pNowAction = NULL;

    // �M��ʧ@�W�٨ó]�w�i��
    RoleActionList::iterator end ( m_ActionList.end() );
    for( RoleActionList::iterator i = m_ActionList.begin(); i != end; ++i )
    {
        if( strcmp( (*i)->szActionName, pActionName ) == 0 )
        {
            m_pNowAction = (*i);

            if( !m_pNowAction->FrameList.empty() )
                m_dwFrameMax = m_pNowAction->FrameList.back()->dwFrameNum;
            
            break;
        }
    }
}
//----------------------------------------------------------------------------
// ����(after Stop position goto top)
//----------------------------------------------------------------------------
void JLAnimation2D::Stop()
{
    m_PlayStatus = Status_Stop;
    m_dwFrameNum = 1;
}
//----------------------------------------------------------------------------
// �Ȱ�
//----------------------------------------------------------------------------
void JLAnimation2D::Pause( DWORD dwFrameNum )
{
    m_PlayStatus = Status_Pause;
    m_dwFrameNum = dwFrameNum;
}
//----------------------------------------------------------------------------
// ���o���񪬺A
//----------------------------------------------------------------------------
PlayStatus JLAnimation2D::GetPlayStatus()
{
    return m_PlayStatus;
}   
//----------------------------------------------------------------------------


//---------------------------------------------------------------------------
// FrameData ���c�禡
//---------------------------------------------------------------------------
FrameData::FrameData()
{
    IsInverse      = false;
    IsPlaySound    = false;
    usFrameWidth   = 0;
    usFrameHeight  = 0;
    dwFrameNum     = 0;
    szImageFile[0] = '\0';
    szSoundFile[0] = '\0';
    pTexture       = NULL;
    ZeroMemory( Reserved, 50 * sizeof( BYTE ) );
}
//---------------------------------------------------------------------------
// RoleAction ���c�禡
//---------------------------------------------------------------------------
RoleAction::RoleAction()
{
    szActionName[0] = '\0';
    IsPlayLoop      = false;
    IsAimCenter     = false;
    usOffsetX       = 0;
    usOffsetY       = 0;
    dwUpdateTime    = 0;
    ZeroMemory( Reserved, 50 * sizeof( BYTE ) );
}
//---------------------------------------------------------------------------
RoleAction::~RoleAction()
{
    RemoveAllFrame();
}
//---------------------------------------------------------------------------
// �Ƨǵe����T
//---------------------------------------------------------------------------
void RoleAction::SortFrames()
{
    DWORD dwFrameNum = 0;

    FrameDataList::iterator end ( FrameList.end() );
    for( FrameDataList::iterator i = FrameList.begin(); i != end; ++i )
    {
        (*i)->dwFrameNum = ++dwFrameNum;
    }
}
//---------------------------------------------------------------------------
// ���o�e����T
//---------------------------------------------------------------------------
sFrameData* RoleAction::GetFrame( DWORD dwFrameNum )
{
    FrameDataList::iterator end ( FrameList.end() );
    for( FrameDataList::iterator i = FrameList.begin(); i != end; ++i )
    {
        if( (*i)->dwFrameNum == dwFrameNum )
            return *i;
    }

    return NULL;
}
//---------------------------------------------------------------------------
// �����e����T
//---------------------------------------------------------------------------
bool RoleAction::RemoveFrame( DWORD dwFrameNum )
{
    FrameDataList::iterator end ( FrameList.end() );
    for( FrameDataList::iterator i = FrameList.begin(); i != end; ++i )
    {
        if( (*i)->dwFrameNum == dwFrameNum )
        {
            SAFE_DELETE( *i );
            FrameList.erase( i );

            // ���s�Ƨ�
            SortFrames();
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
// �ƻs�e����T
//---------------------------------------------------------------------------
bool RoleAction::CopyFrame( DWORD dwFrameNum )
{
    FrameDataList::iterator end ( FrameList.end() );
    for( FrameDataList::iterator i = FrameList.begin(); i != end; ++i )
    {
        if( (*i)->dwFrameNum == dwFrameNum )
        {
            // �ƻs�e��
            sFrameData* pNewFrame = new sFrameData;
            Assert( pNewFrame != NULL, "Call new failed !!" );
            pNewFrame->IsInverse = (*i)->IsInverse;
            pNewFrame->usFrameWidth = (*i)->usFrameWidth;
            pNewFrame->usFrameHeight = (*i)->usFrameWidth;
            pNewFrame->dwFrameNum = (*i)->dwFrameNum;
            pNewFrame->pTexture = (*i)->pTexture;
            strcpy( pNewFrame->szImageFile, (*i)->szImageFile );
            FrameList.insert( i, pNewFrame );

            // ���s�Ƨ�
            SortFrames();
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
// ���J�e����T
//---------------------------------------------------------------------------
bool RoleAction::InsertFrame( DWORD dwFrameNum, sFrameData* pFrame )
{
    FrameDataList::iterator end ( FrameList.end() );
    for( FrameDataList::iterator i = FrameList.begin(); i != end; ++i )
    {
        if( (*i)->dwFrameNum == dwFrameNum )
        {
            // ���J�e��
            FrameList.insert( i, pFrame );

            // ���s�Ƨ�
            SortFrames();
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
// �e�����ǩ��e�@�i
//---------------------------------------------------------------------------
bool RoleAction::MoveFrontFrame( DWORD dwFrameNum )
{
    FrameDataList::iterator end ( FrameList.end() );
    for( FrameDataList::iterator i = FrameList.begin(); i != end; ++i )
    {
        if( (*i)->dwFrameNum == dwFrameNum )
        {
            if( i == FrameList.begin() )
                return false;

            // �����{�b�e��
            sFrameData* pBackFrame = *i;
            FrameDataList::iterator itBack = i;

            // ���e�@�i�e��
            --i;

            // �洫�e��
            *itBack = *i;
            *i = pBackFrame;

            // ���s�Ƨ�
            SortFrames();
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
// �e�����ǩ���@�i
//---------------------------------------------------------------------------
bool RoleAction::MoveBackFrame( DWORD dwFrameNum )
{
    FrameDataList::iterator end ( FrameList.end() );
    for( FrameDataList::iterator i = FrameList.begin(); i != end; ++i )
    {
        if( (*i)->dwFrameNum == dwFrameNum )
        {
            // �����{�b�e��
            sFrameData* pFrontFrame = *i;
            FrameDataList::iterator itFront = i;

            // ����@�i�e��
            ++i;

            if( i == FrameList.end() )
                return false;

            // �洫�e��
            *itFront = *i;
            *i = pFrontFrame;

            // ���s�Ƨ�
            SortFrames();
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------

} // namespace FDO


