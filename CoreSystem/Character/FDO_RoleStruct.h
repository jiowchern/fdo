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
//  ��V��X�J���Y
//---------------------------------------------------------------------------
struct sDirHeader
{
    /// �ɮ׿��ID
    char szFileID[ID_LEN];
    /// �ɮת���
    unsigned short usVersion;
    /// ��V��
    unsigned short usDirCount;
};

struct sImage
{
    char                szTexFile[50];          // �e�������ɦW
//private:
    LPDIRECT3DTEXTURE9  pTexture;               // �e������
	bool				bLoadDone;
	memPack				*pMem ; 
	enLoadingState		curState ; 
public:
     sImage();
    ~sImage() { Clear(); }

    void Clear();
};

//----------------------------------------------------------------------------
// �e��
//----------------------------------------------------------------------------
struct sFrame
{
    /// �e������ɶ�
    unsigned short usPlayTime;
    /// �e���ϼh
    short shLayer;
    /// ����ϦV�P�_�l
    bool bTexInvert;
    /// �e���e��
    float fWidth;
    /// �e������
    float fHeight;
    /// ����y��u1
    float fTU1;
    /// ����y��v1
    float fTV1;
    /// ����y��u2
    float fTU2;
    /// ����y��v2
    float fTV2;
    /// �v��
    RoleImgPtr pImage;
    /// ���a�x�}
    D3DXMATRIX matLocal;
    /// Frame�C��
    DWORD Color;
    /// X�첾�ʤ���
    float fPercentX;
    /// Y�첾�ʤ���
	float fPercentY;
    /// �e����m�y��
    D3DXVECTOR3 vPosition;
    /// �e����m�y��
    D3DXVECTOR3 vScale;
    /// �u�ʺX��
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
// ��@�s���V
//----------------------------------------------------------------------------
struct sComponentDir
{
    /// �s���V
    RoleDirect eDir;
    /// �e���̤j�i��
    unsigned short usMaxFrameNum;
    /// �˳Ƶe���}�C
    sFrameList *FrameList;

    sComponentDir();
    ~sComponentDir();
    void Reset();
    /// �����Ҧ��e��
    void RemoveAllFrame();
};


//----------------------------------------------------------------------------
// ����s��
//----------------------------------------------------------------------------
struct sRoleComponent
{
    /// ��ܧP�_�l
    bool bShow;                      
    /// �s��W��
    char szComponentName[NAME_LEN];   
    /// �s���V�}�C
    sComponentDir *ComponentDirs[RD_Count];           

    sRoleComponent();
	~sRoleComponent(); 
};

//----------------------------------------------------------------------------
// ����ʧ@
//----------------------------------------------------------------------------
struct sRoleAction
{
    /// ����ʧ@�W��
    char szActionName[NAME_LEN];
    /// �j�鼽��P�_�l
    bool bPlayLoop;
    /// ��s���j�ɶ�
    DWORD dwUpdateTime;
    /// ���ĺ���
    BYTE bySoundKind;
    /// �����I�ɶ�1
    unsigned short usHitTime1;
    /// �����I�ɶ�2
    unsigned short usHitTime2;
    /// ���񭵮Įɶ�1
    unsigned short usPlaySoundTime1;
    /// ���񭵮Įɶ�2
    unsigned short usPlaySoundTime2;
    /// ���񭵮Įɶ�3
    unsigned short usPlaySoundTime3;
    /// ������l�ɶ�
    unsigned short usPlayParticleTime;
    /// �����ɦW1  //���� ***
    char szSoundFile1[FILE_LEN];
    /// �����ɦW2  //���� ***
    char szSoundFile2[FILE_LEN];
    /// �����ɦW3  //���� ***
    char szSoundFile3[FILE_LEN];
    /// ���l�ɦW
    char szParticleFile[FILE_LEN];
    /// ����s��}�C
    sRoleComponentList *ComponentList;

    sRoleAction();
    ~sRoleAction();
    void Reset();
    /// ���o�s�����
    RoleComPtr GetComponent(const char* pComponentName);
    /// �����Ҧ��s��
    void RemoveAllComponent();
    /// ���ðʧ@�U���Ҧ��s�� by kof1016
	void HideComponents();
    /// ���o�̤j�e���i��
    unsigned short GetMaxFrameNum(RoleDirect eDir);
};

//---------------------------------------------------------------------------
//  �����X�J���Y
//---------------------------------------------------------------------------
struct sRoleHeader
{
    /// �ɮ׿��ID
    char szFileID[ID_LEN];
    /// �ɮת���
    unsigned short usVersion;
    /// ����W��
    char szRoleName[NAME_LEN];
    /// �����Y��X
    float fScaleX;
    /// �����Y��Y
    float fScaleY;
    /// �v�l�j�p
    unsigned short usShadowSize;
    /// �ʧ@��
    unsigned short usActCount;
    /// �v�l������
    unsigned short usShadowOffset;
    /// ��ɲ��j�p
    unsigned short usBoundingBoxSize;
    /// ���`��ɲ��j�p
    unsigned short usDeadBoundBoxSize;
    /// �O�d���
    char szReserved[4];
};

//---------------------------------------------------------------------------
//  �s���X�J���Y
//---------------------------------------------------------------------------
struct sComHeader
{
    // �ɮ׿��ID
    char szFileID[ID_LEN];
    // �ɮת���
    unsigned short usVersion;
    // �ʧ@��1
    unsigned short usActCount1;
    // �ʧ@��2
    unsigned short usActCount2;
    // �s��W��1
    char szComponentName1[NAME_LEN];
    // �s��W��2
    char szComponentName2[NAME_LEN];
    // �O�d���
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