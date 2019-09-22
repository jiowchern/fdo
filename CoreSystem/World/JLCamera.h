//---------------------------------------------------------------------------
#ifndef _JLCamera_H
#define _JLCamera_H

#include <d3dx9math.h>
#include "singleton.h"
#define g_Camera JLCamera::GetSingleton()

//---------------------------------------------------------------------------
namespace FDO
{

class JLCamera : public Singleton<JLCamera>
{
private:
    D3DXVECTOR3 mCamLookAt;             //�`���I��m
    D3DXVECTOR3 mCamPos;                //��v����m
    D3DXVECTOR3 mCamUp;                 //��v���V�W�V�q
    D3DXVECTOR3 mLockLookPos;           //��w�`���I�y��
    D3DXVECTOR3 mVector;                //��V�V�q
	D3DXVECTOR3 mCamLookTemp;

    D3DXVECTOR3 burst_attack_A;         //�z���e���m
    D3DXVECTOR3 burst_attack_B;         //�z���᭭���m

    D3DXMATRIX  mCamView;               //�˵��x�}
    D3DXMATRIX  mCamInvView;            //�ϦV�˵��x�}
    D3DXMATRIX  mCamVectorInvMatrix;    //��V�V�q�ϦV�x�}

	D3DXMATRIX  mProjectMatrix;			//��v�x�} (project matrix)

    bool        mIsViewChange;          //�˵��x�}��s�P�_�l
    bool        mIsNeedRefresh;         //��s�P�_�l
    bool        mIsVectorChange;        //�V�q���ܧP�_�l
    bool        mIsInvViewChange;       //�ϦV�˵��x�}��s�P�_�l
    bool        mIsVecInvMatrixChange;  //��V�V�q�ϦV�x�}���ܧP�_�l

    bool        mIsAllowChange;         //���\���ܧP�_�l
    bool        mbAllowChangeRadius;    //���\���ܥb�|�P�_�l
    bool        g_vkhome_touch;         //����vkhome�O�_���Q���� __goodmorning
    bool        burst_attack_state;     //���լO�_�B���z�����A_goodmorning

    DWORD       PreTime;                //�ΨӺ����ɶ�
    BYTE        Count_Time;             //�p�����g��

    float       mCamRadius;             //��v����`���I���Z��
    float       mCamAngleH;             //��������
    float       mCamAngleV;             //��������
    float       mCamHeight;             //����
    float       mAngleHBuf;             //�������׽w�İ�
    float       mAngleVBuf;             //�������׽w�İ�
    float       mRadiusBuf;             //�b�|�w�İ�

    // ��w�`���I�Ҧ�����v���Ѽ�
    float       mLockMinAngleV;         // �̤p��������(����)
    float       mLockMaxAngleV;         // �̤j��������(����)
    float       mLockMinRadius;         // �̪���Z
    float       mLockMaxRadius;         // �̻����Z

    // �ۥѪ`���I�Ҧ�����v���Ѽ�
    float       mFreeMinAngleV;         // �̤p��������(����)
    float       mFreeMaxAngleV;         // �̤j��������(����)
    float       mFreeMinRadius;         // �̪���Z
    float       mFreeMaxRadius;         // �̻����Z

    void        ComputeCameraPos();     //�p����v����m
    void        ComputeCameraLookAt();  //�p��`���I��m
    void        RefreshCameraPos();     //��s��m�禡
    void        RefreshCameraLookAt();  //��s�`���I�禡


	static bool		sm_bEnableBurstMode;
public:
	JLCamera();
	~JLCamera();

    bool        UseCamera;
    double      DTime;
	bool		IsMinus;

    void    SetHeight(float Height);
    void    SetLookAtPos(const D3DXVECTOR3 &vLookPos);
    void    SetCamera(float AngleH, float AngleV, float Radius);
    void    SetCamera(const D3DXVECTOR3 &CamPos, float AngleH, float AngleV, float Radius);
	void    Strafe(float Unit);    // left/right
	void    Fly(float Unit);       // up/down
	void    Walk(float Unit);      // forward/backward
	void    Pitch(float Angle);    // rotate on right vector
	void    Yaw(float Angle);      // rotate on up vector
    void    touch_home_key();      // �^�k����
    void    Process(float fElapsedTime);

    void    SetAngleHBuf(float AngleH);
    void    SetAngleVBuf(float AngleV);
    void    SetRadiusBuf(float Radius);
	void    SetLockPos(const D3DXVECTOR3& LockPos);
    void    SetAngleH(float fAngleH);
    void    SetIsAllowChange(bool IsValue);
    void    SetAllowChangeRadius(bool bValue) { mbAllowChangeRadius = bValue; }

    void    SetLockMinAngleV(float fAngleV);
    void    SetLockMaxAngleV(float fAngleV);
    void    SetLockMinRadius(float fRadius);
    void    SetLockMaxRadius(float fRadius);

    void    SetFreeMinAngleV(float fAngleV);
    void    SetFreeMaxAngleV(float fAngleV);
    void    SetFreeMinRadius(float fRadius);
    void    SetFreeMaxRadius(float fRadius);

    //�z�������Y���ʧ@_goodmorning
    void    Set_Burst_state_true();
    void    SetBurstAttack();

    D3DXVECTOR3 GetViewDistance();
    const D3DXVECTOR3& GetPosition() const;
    const D3DXVECTOR3& GetLockPos() const;
    const D3DXVECTOR3& GetLookAtPos() const;
    const D3DXVECTOR3& GetVector();
    const float&       GetAngleV() const;
    const float&       GetAngleH() const;
    const float&       GetRadius() const;
    const float&       GetHeight() const;

    const D3DXMATRIX&  GetViewMatrix();
    const D3DXMATRIX&  GetViewInverseMatrix();
    const D3DXMATRIX&  GetVectorInverseMatrix();

	void SetProjectMatrix(float fov, float aspect, float nearPlane, float farPlane);
	const D3DXMATRIX& GetProjectMatrix() { return mProjectMatrix; }

    //�T���B�z�禡
    LRESULT HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	//<gino>
	static void EnableBurstMode(bool bTrue){ sm_bEnableBurstMode = bTrue;}
	static bool IsEnableBurstMode(){ return sm_bEnableBurstMode; }
	//</gino>
};
//---------------------------------------------------------------------------

} //namespace FDO

#endif