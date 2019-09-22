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
//  �ʵe��X�J���Y
//---------------------------------------------------------------------------
typedef struct AniHeader
{
    char            FileID[4];                                                  //�ɮ׿��ID
    unsigned short  Version;                                                    //�ɮת���
    unsigned short  TotalAction;                                                //�����ʵe�ƶq
    BYTE            Reserved[50];                                               //�w�d���
} sAniHeader;

//----------------------------------------------------------------------------
// �e�����
//----------------------------------------------------------------------------
typedef struct FrameData
{
    bool                IsInverse;          // �ϦV�P�_�l
    bool                IsPlaySound;        // �����n���P�_�l
    unsigned short      usFrameWidth;       // �e������
    unsigned short      usFrameHeight;      // �e���e��
    DWORD               dwFrameNum;         // �e���s��
    char                szImageFile[100];   // �ϧ��ɦW
    char                szSoundFile[100];   // �����ɦW  //���� ***
    LPDIRECT3DTEXTURE9  pTexture;           // �ϧΧ���
	IDirect3DSurface9   *pSurface ;
    BYTE                Reserved[50];       // �w�d���

    FrameData();
} sFrameData;

typedef list<sFrameData*> FrameDataList;

//----------------------------------------------------------------------------
// ����ʧ@
//----------------------------------------------------------------------------
typedef struct RoleAction
{
    char            szActionName[20];       // �ʧ@�W��
    bool            IsPlayLoop;             // �����n���P�_�l
    bool            IsAimCenter;            // ��������P�_�l
    unsigned short  usOffsetX;              // ������mX
    unsigned short  usOffsetY;              // ������mY
    DWORD           dwUpdateTime;           // ���񶡹j�ɶ�
    FrameDataList   FrameList;              // �e����C
    BYTE            Reserved[50];           // �w�d���

    RoleAction();
    ~RoleAction();

    void            SortFrames();                    // �Ƨǵe��
    sFrameData*     GetFrame( DWORD dwFrameNum );    // ���o�e����T
    bool            RemoveFrame( DWORD dwFrameNum ); // �����e����T
    void            RemoveAllFrame();                // �����Ҧ��e����T
    bool            CopyFrame( DWORD dwFrameNum );   // �ƻs�e����T
    bool            InsertFrame( DWORD dwFrameNum, sFrameData* pFrame ); // ���J�e����T
    bool            MoveFrontFrame( DWORD dwFrameNum );// �e�����ǩ��e�@�i
    bool            MoveBackFrame( DWORD dwFrameNum ); // �e�����ǩ���@�i

} sRole2DAction;

typedef list<sRole2DAction*> RoleActionList;

//----------------------------------------------------------------------------
// Class JLAnimation2D
//----------------------------------------------------------------------------
class JLAnimation2D
{
private:
    LPDIRECT3DDEVICE9       m_pDevice;                                          //D3D�˸m

//�ʵe�ܼ�--------------------------------------------------------------------
    RoleActionList          m_ActionList;                                       //�ʧ@�}�C
    sRole2DAction          *m_pNowAction;                                      //�ثe�ϥΤ��ʧ@
    char                    m_szNowActName[20];                                 //�{�b�ʧ@�W��
    char                    m_szCursorName[20];                                 //��аʧ@�W��
    int                     m_iLeftPos;                                         //Left��m
    int                     m_iTopPos;                                          //Top��m
    RECT                    m_RenderRect;                                       //��V�ϰ�d��

//���񱱨��ܼ�----------------------------------------------------------------
    DWORD                   m_dwFrameNum;                                       //�ʵe��ܱi��
    DWORD                   m_dwFrameMax;                                       //�ʵe�̤j�i��
    DWORD                   m_dwPassTime;                                       //�g�L�ɶ�
    DWORD                   m_dwOldFrameNum;                                    //�W���ʵe��ܱi��
    bool                    m_IsInsertPlay;                                     //���J����P�_�l
    bool                    m_IsPlayCursor;                                     //�����ЧP�_�l

//�ݩ��ܼ�
    bool                    m_IsVisible;                                        //��ܧP�_�l
    PlayStatus              m_PlayStatus;                                       //���񪬺A

public:
    JLAnimation2D();
    ~JLAnimation2D();

    void                    Cleanup();                                          //����귽

// �ɮ׳B�z�禡----------------------------------------------------------------
    bool                    LoadFromFile( const string& FileName );             //�q�ɮ׸��J�ʵe
    bool                    LoadFromFileInMemory( const string& FileName );     //�q�O������J�ʵe       

// �ʧ@�禡--------------------------------------------------------------------
   void                     AddAction( sRole2DAction* pAction );                //�W�[�ʧ@
   bool                     RemoveAction( const char* pActName );               //�����ʧ@
   sRole2DAction*           GetAction( const char* pActName );                  //���o�ʧ@����
   const RoleActionList*    GetActionList();                                    //���o�ʧ@���Ц�C

// �]�w�禡--------------------------------------------------------------------
    void                    SetPosition( const int& iLeft, const int& iTop );   //�]�w�_�l��m   
    void                    SetVisible( bool bValue );                          //�]�w��ܧP�_�l
    bool                    GetVisible() const;                                 //���o��ܧP�_�l
    bool                    SetCursorName( const char* pCursorName );           //�]�w��аʵe�W��

//���񱱨�禡---------------------------------------------------------------
    void                    Play( const char* pActionName, bool IsCheck = true );//����
    void                    InsertPlay( const char* pActionName, bool IsPlayCursor = false );//���J����
    void                    Stop();                                             //����(after Stop position goto top)
    void                    Pause( DWORD dwFrameNum );                          //�Ȱ�
    PlayStatus              GetPlayStatus();                                    //���o���񪬺A

// ø�s�禡--------------------------------------------------------------------
    void                    FrameMove( const DWORD& dwPassTime );
    void                    Render();

};

} // namespace FDO


//----------------------------------------------------------------------------
#endif // ROLE_H