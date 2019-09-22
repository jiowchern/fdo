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
#pragma message("管理模式啟動") 
#else
#pragma message("一般模式啟動") 
#endif

////////////////////////////////////////////////////////////////////////////////////
#define DirectXCursor //使用DirectX繪製滑鼠游標
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
// 釋放資源
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
// 移除所有畫面資訊
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
// 載入動畫
//----------------------------------------------------------------------------
bool JLAnimation2D::LoadFromFile( const string& FileName )
{
    FILE* pFileHandle;
    if( ( pFileHandle = fopen( FileName.c_str(), "rb" ) ) == 0 )
        return false;

    // 計算檔案大小
    fseek( pFileHandle, 0, SEEK_END);
    vector<unsigned char> MemData( ftell( pFileHandle ) );
    fseek( pFileHandle, 0, SEEK_SET);

    // 讀取資料流中的資料
    fread( &MemData[0], 1, MemData.size(), pFileHandle );

    // 關閉一個已經開啟的檔案資料流
    fclose( pFileHandle );

    BYTE *pData = &MemData[0];

    // 讀取標頭檔
    sAniHeader *pHeader = (sAniHeader*)pData;

    // 確認檔案
    if( strcmp( pHeader->FileID, "AN2" ) != 0 )
    {
        //string str;
        //str.assign( "AN2" );
        //str.append(g_BinTable.mCodeInlineTextBin.pFastFindW( 773, CodeInlineText_Text ));//檔案格式錯誤
        //str.append( ",LoadAnimation" );
        //str.append(g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ));//失敗
        Assert( 0, "AN2 檔案格式錯誤,  LoadAnimation 失敗" );
        return false;
    }

    pData += sizeof( sAniHeader );

    sRole2DAction* pAction = NULL;
    sFrameData*  pFrame  = NULL;
    DWORD dwTotalFrame   = 0;

    // 取得檔案路徑
    string strFilePath = FileName;
    strFilePath.erase( strFilePath.find_last_of( "\\" ) +1 );

    // 釋放資源
    Cleanup();

    // 建立動作資料
    for( unsigned short i=0; i<pHeader->TotalAction; ++i )
    {
        pAction = new sRole2DAction;
        Assert( pAction != NULL, "Call new failed !!" );
        m_ActionList.push_back( pAction );

        // 儲存資料
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
        pData += 50;    // 預留欄位

        dwTotalFrame = *(DWORD*)pData;
        pData += sizeof( DWORD );

        // 建立畫面資料
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
            pData += 50;    // 預留欄位
            pFrame->pTexture = LoadStaticTexture( pFrame->szImageFile );
			pFrame->pTexture->GetSurfaceLevel( 0 , &(pFrame->pSurface) ) ; 
        }
    }

    return true;
}
//----------------------------------------------------------------------------
// 載入動畫
//----------------------------------------------------------------------------
bool JLAnimation2D::LoadFromFileInMemory( const string& FileName )
{
#ifdef RESMGR

    memPack* pMem = LoadFromPacket( FileName );
    
    if( pMem->Data == NULL ) return false;
        

    BYTE* pData = pMem->Data;

#else

    // 讀取封裝檔
    memPack* pMem = LoadFromPacket( FileName );
    if( pMem == NULL )
        return false;

    // 儲存封裝檔資料
    BYTE* pData = GetNewMemory( pMem->DataSize );
    memcpy( pData, pMem->Data, pMem->DataSize );


#endif


    // 讀取標頭檔
    sAniHeader *pHeader = (sAniHeader*)pData;

    // 確認檔案
    if( strcmp( pHeader->FileID, "AN2" ) != 0 )
    {
        //string str;
        //str.assign( "AN2" );
       // str.append(g_BinTable.mCodeInlineTextBin.pFastFindW( 773, CodeInlineText_Text ));//檔案格式錯誤
        //str.append( ",LoadAnimation" );
       // str.append(g_BinTable.mCodeInlineTextBin.pFastFindW( 3, CodeInlineText_Text ));//失敗
        Assert( 0, "AN2 檔案格式錯誤,  LoadAnimation 失敗" );
        return false;
    }

    pData += sizeof( sAniHeader );

    sRole2DAction* pAction = NULL;
    sFrameData*  pFrame  = NULL;
    DWORD dwTotalFrame   = 0;

    // 取得檔案路徑
    string strFilePath = FileName;
    strFilePath.erase( strFilePath.find_last_of( "\\" ) +1 );

    // 釋放資源
    Cleanup();

    // 建立動作資料
    for( unsigned short i=0; i<pHeader->TotalAction; ++i )
    {
        pAction = new sRole2DAction;
        Assert( pAction != NULL, "Call new failed !!" );
        m_ActionList.push_back( pAction );

        // 儲存資料
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
        pData += 50;    // 預留欄位

        dwTotalFrame = *(DWORD*)pData;
        pData += sizeof( DWORD );

        // 建立畫面資料
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
            pData += 50;    // 預留欄位
            pFrame->pTexture = LoadStaticTexture( pFrame->szImageFile );
			pFrame->pTexture->GetSurfaceLevel( 0 , &(pFrame->pSurface) ) ; 
        }
    }

    return true;
}  
//----------------------------------------------------------------------------
// 增加動作
//----------------------------------------------------------------------------
void JLAnimation2D::AddAction( sRole2DAction* pAction )
{
    m_ActionList.push_back( pAction );
}
//----------------------------------------------------------------------------
// 移除動作
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
// 取得動作指標
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
// 取得動作指標串列
//----------------------------------------------------------------------------
const RoleActionList* JLAnimation2D::GetActionList()
{
    return &m_ActionList;
}     
//----------------------------------------------------------------------------
// 設定顯示判斷子
//----------------------------------------------------------------------------
void JLAnimation2D::SetVisible( bool bValue )
{
    m_IsVisible = bValue;
}
//----------------------------------------------------------------------------
// 取得顯示判斷子
//----------------------------------------------------------------------------
bool JLAnimation2D::GetVisible() const
{
    return m_IsVisible;
}
//----------------------------------------------------------------------------
// 設定游標動畫名稱
//----------------------------------------------------------------------------
bool JLAnimation2D::SetCursorName( const char* pCursorName )
{
    if( strlen( pCursorName ) > 19 )
        return false;

    strcpy( m_szCursorName, pCursorName );
    return true;
}
//----------------------------------------------------------------------------
// 設定起始位置
//----------------------------------------------------------------------------
void JLAnimation2D::SetPosition( const int& iLeft, const int& iTop )
{
    m_iLeftPos = iLeft;
    m_iTopPos  = iTop;
} 
//----------------------------------------------------------------------------
// 狀態更新
//----------------------------------------------------------------------------
void JLAnimation2D::FrameMove( const DWORD& dwPassTime )
{
    if( ( m_pNowAction == NULL ) || ( !m_IsVisible ) )
        return;

    // 更新動畫顯示張數
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

                    // 檢查是否插入播放動作
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
// 渲染繪製
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

    // 設定渲染區域
    // 對齊中央
    if( m_pNowAction->IsAimCenter )
    {
        m_RenderRect.left   = m_iLeftPos - m_pNowAction->usOffsetX;
        m_RenderRect.top    = m_iTopPos  - m_pNowAction->usOffsetY;
        m_RenderRect.right  = m_RenderRect.left + (*itNowFrame)->usFrameWidth;
        m_RenderRect.bottom = m_RenderRect.top  + (*itNowFrame)->usFrameHeight;
    }
    // 對齊左上角
    else
    {
        m_RenderRect.left   = m_iLeftPos;
        m_RenderRect.top    = m_iTopPos;
        m_RenderRect.right  = m_iLeftPos + (*itNowFrame)->usFrameWidth;
        m_RenderRect.bottom = m_iTopPos  + (*itNowFrame)->usFrameHeight;
    }

    // 繪製圖形    
    DrawAnimation( m_RenderRect, (*itNowFrame)->IsInverse );

#endif
    /*
    // 播放音效
    if( ( (*itNowFrame)->IsPlaySound ) && ( m_dwOldFrameNum != m_dwFrameNum ) )
        g_SoundManager._Play2DSoundInMemory( (*itNowFrame)->szSoundFile );   //音效 ***

    // 紀錄播放張數,以判斷是否播放不同張數
    if( m_dwOldFrameNum != m_dwFrameNum )
        m_dwOldFrameNum = m_dwFrameNum;
    */
} 
//----------------------------------------------------------------------------
// 播放   //拖曳 ***
//----------------------------------------------------------------------------
void JLAnimation2D::Play( const char* pActionName, bool IsCheck )
{
    if( IsCheck )
    {
        // 檢查是否插入播放中與是否播放相同動作
        if( m_IsInsertPlay || ( strcmp( m_szNowActName, pActionName ) == 0 ) )
            return;
    }

    strcpy( m_szNowActName, pActionName );
    m_PlayStatus = Status_Play;      //???
    m_dwFrameNum = 1;
    m_dwOldFrameNum = 0;
    m_dwFrameMax = 1;
    m_pNowAction = NULL;

    // 尋找動作名稱並設定張數
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
// 插入播放
//----------------------------------------------------------------------------
void JLAnimation2D::InsertPlay( const char* pActionName, bool IsPlayCursor )
{
    // 檢查是否插入播放中
    if( m_IsInsertPlay )
        return;

    m_IsInsertPlay = true;
    m_IsPlayCursor = IsPlayCursor;
    m_PlayStatus = Status_Play;
    m_dwFrameNum = 1;
    m_dwOldFrameNum = 0;
    m_dwFrameMax = 1;
    m_pNowAction = NULL;

    // 尋找動作名稱並設定張數
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
// 停止(after Stop position goto top)
//----------------------------------------------------------------------------
void JLAnimation2D::Stop()
{
    m_PlayStatus = Status_Stop;
    m_dwFrameNum = 1;
}
//----------------------------------------------------------------------------
// 暫停
//----------------------------------------------------------------------------
void JLAnimation2D::Pause( DWORD dwFrameNum )
{
    m_PlayStatus = Status_Pause;
    m_dwFrameNum = dwFrameNum;
}
//----------------------------------------------------------------------------
// 取得播放狀態
//----------------------------------------------------------------------------
PlayStatus JLAnimation2D::GetPlayStatus()
{
    return m_PlayStatus;
}   
//----------------------------------------------------------------------------


//---------------------------------------------------------------------------
// FrameData 結構函式
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
// RoleAction 結構函式
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
// 排序畫面資訊
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
// 取得畫面資訊
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
// 移除畫面資訊
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

            // 重新排序
            SortFrames();
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
// 複製畫面資訊
//---------------------------------------------------------------------------
bool RoleAction::CopyFrame( DWORD dwFrameNum )
{
    FrameDataList::iterator end ( FrameList.end() );
    for( FrameDataList::iterator i = FrameList.begin(); i != end; ++i )
    {
        if( (*i)->dwFrameNum == dwFrameNum )
        {
            // 複製畫面
            sFrameData* pNewFrame = new sFrameData;
            Assert( pNewFrame != NULL, "Call new failed !!" );
            pNewFrame->IsInverse = (*i)->IsInverse;
            pNewFrame->usFrameWidth = (*i)->usFrameWidth;
            pNewFrame->usFrameHeight = (*i)->usFrameWidth;
            pNewFrame->dwFrameNum = (*i)->dwFrameNum;
            pNewFrame->pTexture = (*i)->pTexture;
            strcpy( pNewFrame->szImageFile, (*i)->szImageFile );
            FrameList.insert( i, pNewFrame );

            // 重新排序
            SortFrames();
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
// 插入畫面資訊
//---------------------------------------------------------------------------
bool RoleAction::InsertFrame( DWORD dwFrameNum, sFrameData* pFrame )
{
    FrameDataList::iterator end ( FrameList.end() );
    for( FrameDataList::iterator i = FrameList.begin(); i != end; ++i )
    {
        if( (*i)->dwFrameNum == dwFrameNum )
        {
            // 插入畫面
            FrameList.insert( i, pFrame );

            // 重新排序
            SortFrames();
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
// 畫面順序往前一張
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

            // 紀錄現在畫面
            sFrameData* pBackFrame = *i;
            FrameDataList::iterator itBack = i;

            // 往前一張畫面
            --i;

            // 交換畫面
            *itBack = *i;
            *i = pBackFrame;

            // 重新排序
            SortFrames();
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
// 畫面順序往後一張
//---------------------------------------------------------------------------
bool RoleAction::MoveBackFrame( DWORD dwFrameNum )
{
    FrameDataList::iterator end ( FrameList.end() );
    for( FrameDataList::iterator i = FrameList.begin(); i != end; ++i )
    {
        if( (*i)->dwFrameNum == dwFrameNum )
        {
            // 紀錄現在畫面
            sFrameData* pFrontFrame = *i;
            FrameDataList::iterator itFront = i;

            // 往後一張畫面
            ++i;

            if( i == FrameList.end() )
                return false;

            // 交換畫面
            *itFront = *i;
            *i = pFrontFrame;

            // 重新排序
            SortFrames();
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------

} // namespace FDO


