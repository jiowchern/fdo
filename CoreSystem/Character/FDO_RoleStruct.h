/**
 * file FDO_RoleStruct.h
 * author Yuming Ho
 */
#ifndef FDO_RoleStructH
#define FDO_RoleStructH

#include "FDO_RolePreRequest.h"
#include "elementdef.h" // ref: ObjFactory
#include "PacketReader.h"

namespace FDO
{

#pragma pack(push)
#pragma pack(1)

//---------------------------------------------------------------------------
//  方向輸出入檔頭
//---------------------------------------------------------------------------
struct sDirHeader
{
    /// 檔案辨視ID
    char szFileID[ID_LEN];
    /// 檔案版本
    unsigned short usVersion;
    /// 方向數
    unsigned short usDirCount;
};

struct sImage
{
    char                szTexFile[50];          // 畫面材質檔名
//private:
    LPDIRECT3DTEXTURE9  pTexture;               // 畫面材質
	bool				bLoadDone;
	memPack				*pMem ; 
	enLoadingState		curState ; 
public:
     sImage();
    ~sImage() { Clear(); }

    void Clear();
};

//----------------------------------------------------------------------------
// 畫面
//----------------------------------------------------------------------------
struct sFrame
{
    /// 畫面播放時間
    unsigned short usPlayTime;
    /// 畫面圖層
    short shLayer;
    /// 材質反向判斷子
    bool bTexInvert;
    /// 畫面寬度
    float fWidth;
    /// 畫面高度
    float fHeight;
    /// 材質座標u1
    float fTU1;
    /// 材質座標v1
    float fTV1;
    /// 材質座標u2
    float fTU2;
    /// 材質座標v2
    float fTV2;
    /// 影像
    RoleImgPtr pImage;
    /// 本地矩陣
    D3DXMATRIX matLocal;
    /// Frame顏色
    DWORD Color;
    /// X位移百分比
    float fPercentX;
    /// Y位移百分比
	float fPercentY;
    /// 畫面位置座標
    D3DXVECTOR3 vPosition;
    /// 畫面位置座標
    D3DXVECTOR3 vScale;
    /// 線性旗標
    DWORD LinearFlag;

    sFrame();
	~sFrame();
    void Reset();
    void RefreshMatrix(const D3DXVECTOR3& vScale, const D3DXVECTOR3& vPosition);
    void Lerp(float s, const sFrame* rhs);
    void EnableControl(bool b, int which);
    bool IsEnableControl(int which);
	void Clone(const sFrame *frame) ; 
//    sFrame& operator=(const sFrame& right);
//    void* operator new (size_t);
//    void operator delete (void* p);
//
//private:
//    typedef ObjFactory<sFrame> FrameFactory;
//    static FrameFactory sFrameFactory;
};

//----------------------------------------------------------------------------
// 單一零件方向
//----------------------------------------------------------------------------
struct sComponentDir
{
    /// 零件方向
    RoleDirect eDir;
    /// 畫面最大張數
    unsigned short usMaxFrameNum;
    /// 裝備畫面陣列
    sFrameList *FrameList;

    sComponentDir();
    ~sComponentDir();
    void Reset();
    /// 移除所有畫面
    void RemoveAllFrame();
};


//----------------------------------------------------------------------------
// 角色零件
//----------------------------------------------------------------------------
struct sRoleComponent
{
    /// 顯示判斷子
    bool bShow;                      
    /// 零件名稱
    char szComponentName[NAME_LEN];   
    /// 零件方向陣列
    sComponentDir *ComponentDirs[RD_Count];           

    sRoleComponent();
	~sRoleComponent(); 
};

//----------------------------------------------------------------------------
// 角色動作
//----------------------------------------------------------------------------
struct sRoleAction
{
    /// 角色動作名稱
    char szActionName[NAME_LEN];
    /// 迴圈播放判斷子
    bool bPlayLoop;
    /// 更新間隔時間
    DWORD dwUpdateTime;
    /// 音效種類
    BYTE bySoundKind;
    /// 擊中點時間1
    unsigned short usHitTime1;
    /// 擊中點時間2
    unsigned short usHitTime2;
    /// 播放音效時間1
    unsigned short usPlaySoundTime1;
    /// 播放音效時間2
    unsigned short usPlaySoundTime2;
    /// 播放音效時間3
    unsigned short usPlaySoundTime3;
    /// 播放分子時間
    unsigned short usPlayParticleTime;
    /// 音效檔名1  //音效 ***
    char szSoundFile1[FILE_LEN];
    /// 音效檔名2  //音效 ***
    char szSoundFile2[FILE_LEN];
    /// 音效檔名3  //音效 ***
    char szSoundFile3[FILE_LEN];
    /// 分子檔名
    char szParticleFile[FILE_LEN];
    /// 角色零件陣列
    sRoleComponentList *ComponentList;

    sRoleAction();
    ~sRoleAction();
    void Reset();
    /// 取得零件指標
    RoleComPtr GetComponent(const char* pComponentName);
    /// 移除所有零件
    void RemoveAllComponent();
    /// 隱藏動作下的所有零件 by kof1016
	void HideComponents();
    /// 取得最大畫面張數
    unsigned short GetMaxFrameNum(RoleDirect eDir);
};

//---------------------------------------------------------------------------
//  角色輸出入檔頭
//---------------------------------------------------------------------------
struct sRoleHeader
{
    /// 檔案辨視ID
    char szFileID[ID_LEN];
    /// 檔案版本
    unsigned short usVersion;
    /// 角色名稱
    char szRoleName[NAME_LEN];
    /// 角色縮放X
    float fScaleX;
    /// 角色縮放Y
    float fScaleY;
    /// 影子大小
    unsigned short usShadowSize;
    /// 動作數
    unsigned short usActCount;
    /// 影子偏移值
    unsigned short usShadowOffset;
    /// 邊界盒大小
    unsigned short usBoundingBoxSize;
    /// 死亡邊界盒大小
    unsigned short usDeadBoundBoxSize;
    /// 保留欄位
    char szReserved[4];
};

//---------------------------------------------------------------------------
//  零件輸出入檔頭
//---------------------------------------------------------------------------
struct sComHeader
{
    // 檔案辨視ID
    char szFileID[ID_LEN];
    // 檔案版本
    unsigned short usVersion;
    // 動作數1
    unsigned short usActCount1;
    // 動作數2
    unsigned short usActCount2;
    // 零件名稱1
    char szComponentName1[NAME_LEN];
    // 零件名稱2
    char szComponentName2[NAME_LEN];
    // 保留欄位
    char szReserved[10];
};

#pragma pack(pop)

const int sizeof_DirHeader = sizeof(sDirHeader);
//const int sizeof_EmoHeader = sizeof(sEmoHeader);
const int sizeof_Image = sizeof(sImage);
const int sizeof_Frame = sizeof(sFrame);
const int sizeof_ComponentDir = sizeof(sComponentDir);
const int sizeof_RoleComponent = sizeof(sRoleComponent);
const int sizeof_RoleAction = sizeof(sRoleAction);
const int sizeof_nShort = sizeof(unsigned short);
const int sizeof_short = sizeof(short);
const int sizeof_float = sizeof(float);
const int sizeof_D3DXVECTOR3 = sizeof(D3DXVECTOR3);
const int sizeof_BOOL = sizeof(bool);
const int sizeof_ComHeader = sizeof(sComHeader);
const int sizeof_RoleHeader = sizeof(sRoleHeader);

} // namespace FDO

#endif // FDO_RoleStructH