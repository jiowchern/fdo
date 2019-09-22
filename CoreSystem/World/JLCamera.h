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
    D3DXVECTOR3 mCamLookAt;             //注視點位置
    D3DXVECTOR3 mCamPos;                //攝影機位置
    D3DXVECTOR3 mCamUp;                 //攝影機向上向量
    D3DXVECTOR3 mLockLookPos;           //鎖定注視點座標
    D3DXVECTOR3 mVector;                //方向向量
	D3DXVECTOR3 mCamLookTemp;

    D3DXVECTOR3 burst_attack_A;         //爆擊前原位置
    D3DXVECTOR3 burst_attack_B;         //爆擊後限制位置

    D3DXMATRIX  mCamView;               //檢視矩陣
    D3DXMATRIX  mCamInvView;            //反向檢視矩陣
    D3DXMATRIX  mCamVectorInvMatrix;    //方向向量反向矩陣

	D3DXMATRIX  mProjectMatrix;			//投影矩陣 (project matrix)

    bool        mIsViewChange;          //檢視矩陣更新判斷子
    bool        mIsNeedRefresh;         //更新判斷子
    bool        mIsVectorChange;        //向量改變判斷子
    bool        mIsInvViewChange;       //反向檢視矩陣更新判斷子
    bool        mIsVecInvMatrixChange;  //方向向量反向矩陣改變判斷子

    bool        mIsAllowChange;         //允許改變判斷子
    bool        mbAllowChangeRadius;    //允許改變半徑判斷子
    bool        g_vkhome_touch;         //測試vkhome是否有被按到 __goodmorning
    bool        burst_attack_state;     //測試是否處於爆擊狀態_goodmorning

    DWORD       PreTime;                //用來算執行時間
    BYTE        Count_Time;             //計算執行週期

    float       mCamRadius;             //攝影機到注視點的距離
    float       mCamAngleH;             //水平角度
    float       mCamAngleV;             //垂直角度
    float       mCamHeight;             //高度
    float       mAngleHBuf;             //水平角度緩衝區
    float       mAngleVBuf;             //垂直角度緩衝區
    float       mRadiusBuf;             //半徑緩衝區

    // 鎖定注視點模式的攝影機參數
    float       mLockMinAngleV;         // 最小垂直角度(仰視)
    float       mLockMaxAngleV;         // 最大垂直角度(俯視)
    float       mLockMinRadius;         // 最近視距
    float       mLockMaxRadius;         // 最遠視距

    // 自由注視點模式的攝影機參數
    float       mFreeMinAngleV;         // 最小垂直角度(仰視)
    float       mFreeMaxAngleV;         // 最大垂直角度(俯視)
    float       mFreeMinRadius;         // 最近視距
    float       mFreeMaxRadius;         // 最遠視距

    void        ComputeCameraPos();     //計算攝影機位置
    void        ComputeCameraLookAt();  //計算注視點位置
    void        RefreshCameraPos();     //更新位置函式
    void        RefreshCameraLookAt();  //更新注視點函式


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
    void    touch_home_key();      // 回歸角度
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

    //爆擊時鏡頭的動作_goodmorning
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

    //訊息處理函式
    LRESULT HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	//<gino>
	static void EnableBurstMode(bool bTrue){ sm_bEnableBurstMode = bTrue;}
	static bool IsEnableBurstMode(){ return sm_bEnableBurstMode; }
	//</gino>
};
//---------------------------------------------------------------------------

} //namespace FDO

#endif