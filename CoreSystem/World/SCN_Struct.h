/**
 * @file SCN_Struct.h
 * @author Yuming Ho
 * @brief SCN���c
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
struct SCN_Header                                           // Scene�x�s���c
{
    char            ID[3];                                  //�ɮ׿��ID
    unsigned short  Version;                                //�ɮת���
    UINT            MapInformationOffset;                   //�a�θ�T�ɮצ�m
    int             ObjCount;                               //�ت��`��
    UINT            ObjInformationIndexOffset;              //�ت���T�ɮצ�m
    int             ExternObjCount;                         //�S�����`��
    UINT            ExternObjInfotmationIndexOffset;        //�S�����T�ɮצ�m
    int             PCLCount;                               //���l�S���`��
    UINT            PCLOffset;                              //���l�S���ɮצ�m
    int             Reserved[5];                            //�w�d���c�Ŷ�
};
//---------------------------------------------------------------------------
struct SCN_MapNfo                                           // D3DMap�����x�s���c
{
    char            MapName[FNAME_MAX];                     //�a�ΦW��
    float           MapWidth;                               //�a�μe��
    float           MapHeight;                              //�a�Ϊ���
    char            UpNearMapFileName[FNAME_MAX];           //�۾FUP�a���ɦW
    char            DownNearMapFileName[FNAME_MAX];         //�۾FDOWN�a����
    char            LeftNearMapFileName[FNAME_MAX];         //�۾FLEFT�a����
    char            RightNearMapFileName[FNAME_MAX];        //�۾FRIGHT�a����
    char            BWMapName[FNAME_MAX];                   //�a�ζ¥չ�
    char            Text0Name[FNAME_MAX];                   //�a�βĤ@�h�����
    unsigned short  Text0ColOp;                             //����V��Ѽ�
    unsigned short  Text0ColArg1;                           //����V��Ѽ�
    unsigned short  Text0ColArg2;                           //����V��Ѽ�
    unsigned short  Text0AlpOp;                             //����ALPHA�Ѽ�
    unsigned short  Text0AlpArg1;                           //����ALPHA�Ѽ�
    unsigned short  Text0AlpArg2;                           //����ALPHA�Ѽ�
    char            Text1Name[FNAME_MAX];                   //�a�βĤG�h�����
    unsigned short  Text1ColOp;                             //����V��Ѽ�
    unsigned short  Text1ColArg1;                           //����V��Ѽ�
    unsigned short  Text1ColArg2;                           //����V��Ѽ�
    unsigned short  Text1AlpOp;                             //����ALPHA�Ѽ�
    unsigned short  Text1AlpArg1;                           //����ALPHA�Ѽ�
    unsigned short  Text1AlpArg2;                           //����ALPHA�Ѽ�
    float           vRatio;                                 //�a�Ϊ��e��
    float           hRatio;                                 //�a�ΰ��פ�
    bool            FogEnable;                              //�����@�ζ}��
    DWORD           FogColor;                               //�����C��
    float           FogStart;                               //�����_�l�d��
    float           FogEnd;                                 //�����פ�d��
    int             GridWCount;                             //��Ƽh�e��
    int             GridHCount;                             //��Ƽh����
    int             CollideIdxCount;                        //�I�����ޭ��`��
    UINT            CollideIdxOffset;                       //�I�����ޭ��ɮצ�m
    UINT            CollideInforOffset;                     //�I���}�C���ɮצ�m
    int             AltitudeIdxCount;                       //���ׯ��ޭ��`��
    UINT            AltitudeIdxOffset;                      //���ׯ��ޭ��ɮצ�m
    UINT            AltitudeInforOffset;                    //���װ}�C���ɮצ�m
    int             Reserved[10];                           //�w�d���c�Ŷ�
};
//---------------------------------------------------------------------------
typedef struct SCN_MapCollideIdx                            // Collide�x�s���c for Verson 4.0
{
    int             CollideIdx;                             //�I������
    UINT            CollideID;                              //�I��ID
    char            CollideName[FNAME_MAX];                 //�I���h�W��
    BYTE            CollideClass;                           //�I���h���O
    DWORD           Color;                                  //�I���h�C��
} *SCN_MapCollideIdxPtr;
//---------------------------------------------------------------------------
typedef struct SCN_MapCollideIdx1                           // Collide�x�s���c for verson 1.0
{
    UINT            CollideID;                              //�I��ID
    char            CollideName[FNAME_MAX];                 //�I���h�W��
    DWORD           Color;                                  //�I���h�C��
} *SCN_MapCollideIdx1Ptr;
//---------------------------------------------------------------------------
typedef struct SCN_MapCollideIdx2                           // Collide�x�s���c for verson 2.0~3.0
{
    int             CollideIdx;                             //�I������
    UINT            CollideID;                              //�I��ID
    char            CollideName[FNAME_MAX];                 //�I���h�W��
    DWORD           Color;                                  //�I���h�C��
} *SCN_MapCollideIdx2Ptr;
//---------------------------------------------------------------------------
typedef struct SCN_MapAltitudeIdx                           // Altitude�x�s���c
{
    int             AltitudeIdx;                            //���ׯ���
    UINT            AltitudeID;                             //����ID
    char            AltitudeName[FNAME_MAX];                //���׼h�W��
    float           AltitudeHigh;                           //���׼h����
    DWORD           Color;                                  //���׼h�C��
} *SCN_MapAltitudeIdxPtr;
//---------------------------------------------------------------------------
typedef struct SCN_MapAltitudeIdx1                          // Altitude�x�s���c for Verson 1.0
{
    UINT            AltitudeID;                             //����ID
    char            AltitudeName[FNAME_MAX];                //���׼h�W��
    float           AltitudeHigh;                           //���׼h����
    DWORD           Color;                                  //���׼h�C��
} *SCN_MapAltitudeIdx1Ptr;
//---------------------------------------------------------------------------
typedef struct SCN_ObjInfor                                 // Obj�����x�s���c
{
    char            ObjName[FNAME_MAX];                     //����W��
    char            ObjFileName[FNAME_MAX];                 //�����ɮצW��
    float           PosX,PosY,PosZ;                         //����y��
    float           RotX,RotY,RotZ;                         //���󨤫�
    float           ScalX,ScalY,ScalZ;                      //����j�p
    int             Reserved[10];                           //�w�d���c�Ŷ�
} *SCN_ObjInforPtr;
//---------------------------------------------------------------------------
struct SCN_ExternObj                                        // �X�R�����x�s���c
{
    char            ObjName[FNAME_MAX];                     //����W��
    float           Width;                                  //����e��
    float           Height;                                 //�������
    float           PosX,PosY,PosZ;                         //����y��
    int             VertexWCount;                           //����e���I��
    int             VertexHCount;                           //��������I��
    float           Amplitude;                              //���󮶴T
    float           AmplitudeInterval;                      //���󮶴T�W�v
    float           TextureTU;                              //�������b
    float           TextureTV;                              //�������b
    float           TextureCount;                           //��������`��
    UINT            TextureFileNameOffset;                  //�������W���ɮצ�m(char TextureName[100])
    float           TextureSpeed;                           //�����������t��
    DWORD           ObjColor;                               //��������C��
    float           RotX,RotY,RotZ;                         //����y��
    int             Reserved[7];                            //�w�d���c�Ŷ�
};
//---------------------------------------------------------------------------
typedef struct SCN_PCLInfor                                 // ���l�S���x�s���c
{
    char            PCLName[FNAME_MAX];                     //����W��
    char            PCLFileName[FNAME_MAX];                 //�����ɮצW��
    float           PosX,PosY,PosZ;                         //����y��
    float           RotX,RotY,RotZ;                         //���󨤫�
    float           ScalX,ScalY,ScalZ;                      //����j�p
    int             Reserved[10];                           //�w�d���c�Ŷ�
} *SCN_PCLInforPtr;
//---------------------------------------------------------------------------
struct SCN_EObjTexture                                      // �����x�s���c
{
    char            Texture[FNAME_MAX];
};
//---------------------------------------------------------------------------
//�s�l�[�O��
//---------------------------------------------------------------------------
struct SCN_LightStatus                                      // �O����T�x�s���c
{
    int             LightStatus;                            // �����O�����A(0:���� 1:�}��45�׹﨤 2:�}�Ҥ���90�׹﨤 3:�}�ҫ���90�׹﨤)
    int             ColorR;                                 // �����O���C��(R)
    int             ColorG;                                 // �����O���C��(G)
    int             ColorB;                                 // �����O���C��(B)
};
//---------------------------------------------------------------------------
#pragma pack( pop )
//---------------------------------------------------------------------------
enum TIME_ZONE
{
    MORNING,                                                // ���W
    EVENING,                                                // ����
    NIGHT,                                                  // �ߤW
    EARLY,                                                  // �M��
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