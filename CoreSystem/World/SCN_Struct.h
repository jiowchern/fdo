/**
 * @file SCN_Struct.h
 * @author Yuming Ho
 * @brief SCN結構
 */
#ifndef SCN_StructH
#define SCN_StructH

#include <vector>
//---------------------------------------------------------------------------
namespace FDO
{

const unsigned int FNAME_MAX = 255;

#pragma pack(push)
#pragma pack(1)
//---------------------------------------------------------------------------
struct SCN_Header                                           // Scene儲存結構
{
    char            ID[3];                                  //檔案辨視ID
    unsigned short  Version;                                //檔案版本
    UINT            MapInformationOffset;                   //地形資訊檔案位置
    int             ObjCount;                               //建物總數
    UINT            ObjInformationIndexOffset;              //建物資訊檔案位置
    int             ExternObjCount;                         //特殊物件總數
    UINT            ExternObjInfotmationIndexOffset;        //特殊物件資訊檔案位置
    int             PCLCount;                               //分子特效總數
    UINT            PCLOffset;                              //分子特效檔案位置
    int             Reserved[5];                            //預留結構空間
};
//---------------------------------------------------------------------------
struct SCN_MapNfo                                           // D3DMap物件儲存結構
{
    char            MapName[FNAME_MAX];                     //地形名稱
    float           MapWidth;                               //地形寬度
    float           MapHeight;                              //地形長度
    char            UpNearMapFileName[FNAME_MAX];           //相鄰UP地形檔名
    char            DownNearMapFileName[FNAME_MAX];         //相鄰DOWN地形檔
    char            LeftNearMapFileName[FNAME_MAX];         //相鄰LEFT地形檔
    char            RightNearMapFileName[FNAME_MAX];        //相鄰RIGHT地形檔
    char            BWMapName[FNAME_MAX];                   //地形黑白圖
    char            Text0Name[FNAME_MAX];                   //地形第一層材質圖
    unsigned short  Text0ColOp;                             //材質混色參數
    unsigned short  Text0ColArg1;                           //材質混色參數
    unsigned short  Text0ColArg2;                           //材質混色參數
    unsigned short  Text0AlpOp;                             //材質ALPHA參數
    unsigned short  Text0AlpArg1;                           //材質ALPHA參數
    unsigned short  Text0AlpArg2;                           //材質ALPHA參數
    char            Text1Name[FNAME_MAX];                   //地形第二層材質圖
    unsigned short  Text1ColOp;                             //材質混色參數
    unsigned short  Text1ColArg1;                           //材質混色參數
    unsigned short  Text1ColArg2;                           //材質混色參數
    unsigned short  Text1AlpOp;                             //材質ALPHA參數
    unsigned short  Text1AlpArg1;                           //材質ALPHA參數
    unsigned short  Text1AlpArg2;                           //材質ALPHA參數
    float           vRatio;                                 //地形長寬比
    float           hRatio;                                 //地形高度比
    bool            FogEnable;                              //煙霧作用開關
    DWORD           FogColor;                               //煙霧顏色
    float           FogStart;                               //煙霧起始範圍
    float           FogEnd;                                 //煙霧終止範圍
    int             GridWCount;                             //資料層寬度
    int             GridHCount;                             //資料層長度
    int             CollideIdxCount;                        //碰撞索引值總數
    UINT            CollideIdxOffset;                       //碰撞索引值檔案位置
    UINT            CollideInforOffset;                     //碰撞陣列值檔案位置
    int             AltitudeIdxCount;                       //高度索引值總數
    UINT            AltitudeIdxOffset;                      //高度索引值檔案位置
    UINT            AltitudeInforOffset;                    //高度陣列值檔案位置
    int             Reserved[10];                           //預留結構空間
};
//---------------------------------------------------------------------------
typedef struct SCN_MapCollideIdx                            // Collide儲存結構 for Verson 4.0
{
    int             CollideIdx;                             //碰撞索引
    UINT            CollideID;                              //碰撞ID
    char            CollideName[FNAME_MAX];                 //碰撞層名稱
    BYTE            CollideClass;                           //碰撞層類別
    DWORD           Color;                                  //碰撞層顏色
} *SCN_MapCollideIdxPtr;
//---------------------------------------------------------------------------
typedef struct SCN_MapCollideIdx1                           // Collide儲存結構 for verson 1.0
{
    UINT            CollideID;                              //碰撞ID
    char            CollideName[FNAME_MAX];                 //碰撞層名稱
    DWORD           Color;                                  //碰撞層顏色
} *SCN_MapCollideIdx1Ptr;
//---------------------------------------------------------------------------
typedef struct SCN_MapCollideIdx2                           // Collide儲存結構 for verson 2.0~3.0
{
    int             CollideIdx;                             //碰撞索引
    UINT            CollideID;                              //碰撞ID
    char            CollideName[FNAME_MAX];                 //碰撞層名稱
    DWORD           Color;                                  //碰撞層顏色
} *SCN_MapCollideIdx2Ptr;
//---------------------------------------------------------------------------
typedef struct SCN_MapAltitudeIdx                           // Altitude儲存結構
{
    int             AltitudeIdx;                            //高度索引
    UINT            AltitudeID;                             //高度ID
    char            AltitudeName[FNAME_MAX];                //高度層名稱
    float           AltitudeHigh;                           //高度層高度
    DWORD           Color;                                  //高度層顏色
} *SCN_MapAltitudeIdxPtr;
//---------------------------------------------------------------------------
typedef struct SCN_MapAltitudeIdx1                          // Altitude儲存結構 for Verson 1.0
{
    UINT            AltitudeID;                             //高度ID
    char            AltitudeName[FNAME_MAX];                //高度層名稱
    float           AltitudeHigh;                           //高度層高度
    DWORD           Color;                                  //高度層顏色
} *SCN_MapAltitudeIdx1Ptr;
//---------------------------------------------------------------------------
typedef struct SCN_ObjInfor                                 // Obj物件儲存結構
{
    char            ObjName[FNAME_MAX];                     //物件名稱
    char            ObjFileName[FNAME_MAX];                 //物件檔案名稱
    float           PosX,PosY,PosZ;                         //物件座標
    float           RotX,RotY,RotZ;                         //物件角度
    float           ScalX,ScalY,ScalZ;                      //物件大小
    int             Reserved[10];                           //預留結構空間
} *SCN_ObjInforPtr;
//---------------------------------------------------------------------------
struct SCN_ExternObj                                        // 擴充物件儲存結構
{
    char            ObjName[FNAME_MAX];                     //物件名稱
    float           Width;                                  //物件寬度
    float           Height;                                 //物件長度
    float           PosX,PosY,PosZ;                         //物件座標
    int             VertexWCount;                           //物件寬頂點數
    int             VertexHCount;                           //物件長頂點數
    float           Amplitude;                              //物件振幅
    float           AmplitudeInterval;                      //物件振幅頻率
    float           TextureTU;                              //物件材質軸
    float           TextureTV;                              //物件材質軸
    float           TextureCount;                           //物件材質總數
    UINT            TextureFileNameOffset;                  //物件材質名稱檔案位置(char TextureName[100])
    float           TextureSpeed;                           //物件材質切換速度
    DWORD           ObjColor;                               //物件材質顏色
    float           RotX,RotY,RotZ;                         //物件座標
    int             Reserved[7];                            //預留結構空間
};
//---------------------------------------------------------------------------
typedef struct SCN_PCLInfor                                 // 分子特效儲存結構
{
    char            PCLName[FNAME_MAX];                     //物件名稱
    char            PCLFileName[FNAME_MAX];                 //物件檔案名稱
    float           PosX,PosY,PosZ;                         //物件座標
    float           RotX,RotY,RotZ;                         //物件角度
    float           ScalX,ScalY,ScalZ;                      //物件大小
    int             Reserved[10];                           //預留結構空間
} *SCN_PCLInforPtr;
//---------------------------------------------------------------------------
struct SCN_EObjTexture                                      // 物件儲存結構
{
    char            Texture[FNAME_MAX];
};
//---------------------------------------------------------------------------
//新追加燈光
//---------------------------------------------------------------------------
struct SCN_LightStatus                                      // 燈光資訊儲存結構
{
    int             LightStatus;                            // 場景燈光狀態(0:關閉 1:開啟45度對角 2:開啟水平90度對角 3:開啟垂直90度對角)
    int             ColorR;                                 // 場景燈光顏色(R)
    int             ColorG;                                 // 場景燈光顏色(G)
    int             ColorB;                                 // 場景燈光顏色(B)
};
//---------------------------------------------------------------------------
#pragma pack( pop )
//---------------------------------------------------------------------------
enum TIME_ZONE
{
    MORNING,                                                // 早上
    EVENING,                                                // 黃昏
    NIGHT,                                                  // 晚上
    EARLY,                                                  // 清晨
    MAX_TIME_ZONE
};
//---------------------------------------------------------------------------
const unsigned short sizeof_SCN_Header = sizeof(SCN_Header);
const unsigned short sizeof_SCN_MapNfo = sizeof(SCN_MapNfo);

const unsigned short sizeof_SCN_MapCollideIdx  = sizeof(SCN_MapCollideIdx );
const unsigned short sizeof_SCN_MapCollideIdx1 = sizeof(SCN_MapCollideIdx1);
const unsigned short sizeof_SCN_MapCollideIdx2 = sizeof(SCN_MapCollideIdx2);

const unsigned short sizeof_SCN_MapAltitudeIdx  = sizeof(SCN_MapAltitudeIdx );
const unsigned short sizeof_SCN_MapAltitudeIdx1 = sizeof(SCN_MapAltitudeIdx1);

const unsigned short sizeof_SCN_ObjInfor = sizeof(SCN_ObjInfor);
const unsigned short sizeof_SCN_ExternObj = sizeof(SCN_ExternObj);
const unsigned short sizeof_SCN_PCLInfor = sizeof(SCN_PCLInfor);
const unsigned short sizeof_SCN_EObjTexture = sizeof(SCN_EObjTexture);
const unsigned short sizeof_SCN_LightStatus = sizeof(SCN_LightStatus);

typedef std::vector<SCN_EObjTexture>    TextureNameList;
typedef std::vector<SCN_ObjInfor>       ObjNfoList;
typedef std::vector<SCN_ExternObj>      ExternObjList;
typedef std::vector<SCN_PCLInfor>       PCLInforList;
//---------------------------------------------------------------------------
} // end of namespace FDO
//---------------------------------------------------------------------------
#endif // SCN_StructH