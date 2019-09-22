//----------------------------------------------------------------------------
#ifndef _JLAnimation2D_H_
#define _JLAnimation2D_H_  

#include "JLAnimation.h"
#include <d3dx9math.h>
#include <string>
#include <list>

//----------------------------------------------------------------------------

namespace FDO
{   

using namespace std;    

//---------------------------------------------------------------------------
//  動畫輸出入檔頭
//---------------------------------------------------------------------------
typedef struct AniHeader
{
    char            FileID[4];                                                  //檔案辨視ID
    unsigned short  Version;                                                    //檔案版本
    unsigned short  TotalAction;                                                //全部動畫數量
    BYTE            Reserved[50];                                               //預留欄位
} sAniHeader;

//----------------------------------------------------------------------------
// 畫面資料
//----------------------------------------------------------------------------
typedef struct FrameData
{
    bool                IsInverse;          // 反向判斷子
    bool                IsPlaySound;        // 播放聲音判斷子
    unsigned short      usFrameWidth;       // 畫面長度
    unsigned short      usFrameHeight;      // 畫面寬度
    DWORD               dwFrameNum;         // 畫面編號
    char                szImageFile[100];   // 圖形檔名
    char                szSoundFile[100];   // 音效檔名  //音效 ***
    LPDIRECT3DTEXTURE9  pTexture;           // 圖形材質
	IDirect3DSurface9   *pSurface ;
    BYTE                Reserved[50];       // 預留欄位

    FrameData();
} sFrameData;

typedef list<sFrameData*> FrameDataList;

//----------------------------------------------------------------------------
// 角色動作
//----------------------------------------------------------------------------
typedef struct RoleAction
{
    char            szActionName[20];       // 動作名稱
    bool            IsPlayLoop;             // 播放聲音判斷子
    bool            IsAimCenter;            // 對齊中央判斷子
    unsigned short  usOffsetX;              // 偏移位置X
    unsigned short  usOffsetY;              // 偏移位置Y
    DWORD           dwUpdateTime;           // 播放間隔時間
    FrameDataList   FrameList;              // 畫面串列
    BYTE            Reserved[50];           // 預留欄位

    RoleAction();
    ~RoleAction();

    void            SortFrames();                    // 排序畫面
    sFrameData*     GetFrame( DWORD dwFrameNum );    // 取得畫面資訊
    bool            RemoveFrame( DWORD dwFrameNum ); // 移除畫面資訊
    void            RemoveAllFrame();                // 移除所有畫面資訊
    bool            CopyFrame( DWORD dwFrameNum );   // 複製畫面資訊
    bool            InsertFrame( DWORD dwFrameNum, sFrameData* pFrame ); // 插入畫面資訊
    bool            MoveFrontFrame( DWORD dwFrameNum );// 畫面順序往前一張
    bool            MoveBackFrame( DWORD dwFrameNum ); // 畫面順序往後一張

} sRole2DAction;

typedef list<sRole2DAction*> RoleActionList;

//----------------------------------------------------------------------------
// Class JLAnimation2D
//----------------------------------------------------------------------------
class JLAnimation2D
{
private:
    LPDIRECT3DDEVICE9       m_pDevice;                                          //D3D裝置

//動畫變數--------------------------------------------------------------------
    RoleActionList          m_ActionList;                                       //動作陣列
    sRole2DAction          *m_pNowAction;                                      //目前使用之動作
    char                    m_szNowActName[20];                                 //現在動作名稱
    char                    m_szCursorName[20];                                 //游標動作名稱
    int                     m_iLeftPos;                                         //Left位置
    int                     m_iTopPos;                                          //Top位置
    RECT                    m_RenderRect;                                       //渲染區域範圍

//播放控制變數----------------------------------------------------------------
    DWORD                   m_dwFrameNum;                                       //動畫顯示張數
    DWORD                   m_dwFrameMax;                                       //動畫最大張數
    DWORD                   m_dwPassTime;                                       //經過時間
    DWORD                   m_dwOldFrameNum;                                    //上次動畫顯示張數
    bool                    m_IsInsertPlay;                                     //插入播放判斷子
    bool                    m_IsPlayCursor;                                     //播放游標判斷子

//屬性變數
    bool                    m_IsVisible;                                        //顯示判斷子
    PlayStatus              m_PlayStatus;                                       //播放狀態

public:
    JLAnimation2D();
    ~JLAnimation2D();

    void                    Cleanup();                                          //釋放資源

// 檔案處理函式----------------------------------------------------------------
    bool                    LoadFromFile( const string& FileName );             //從檔案載入動畫
    bool                    LoadFromFileInMemory( const string& FileName );     //從記憶體載入動畫       

// 動作函式--------------------------------------------------------------------
   void                     AddAction( sRole2DAction* pAction );                //增加動作
   bool                     RemoveAction( const char* pActName );               //移除動作
   sRole2DAction*           GetAction( const char* pActName );                  //取得動作指標
   const RoleActionList*    GetActionList();                                    //取得動作指標串列

// 設定函式--------------------------------------------------------------------
    void                    SetPosition( const int& iLeft, const int& iTop );   //設定起始位置   
    void                    SetVisible( bool bValue );                          //設定顯示判斷子
    bool                    GetVisible() const;                                 //取得顯示判斷子
    bool                    SetCursorName( const char* pCursorName );           //設定游標動畫名稱

//播放控制函式---------------------------------------------------------------
    void                    Play( const char* pActionName, bool IsCheck = true );//播放
    void                    InsertPlay( const char* pActionName, bool IsPlayCursor = false );//插入播放
    void                    Stop();                                             //停止(after Stop position goto top)
    void                    Pause( DWORD dwFrameNum );                          //暫停
    PlayStatus              GetPlayStatus();                                    //取得播放狀態

// 繪製函式--------------------------------------------------------------------
    void                    FrameMove( const DWORD& dwPassTime );
    void                    Render();

};

} // namespace FDO


//----------------------------------------------------------------------------
#endif // ROLE_H