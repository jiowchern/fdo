#include "stdafx.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#include "JLCamera.h"
//#include "HUISystemSet.h"
//#include "tracetool.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "Timer.h"
#include "PhenixD3D9RenderSystem.h"

namespace FDO
{

const float gRadian = D3DX_PI/180.0f;   // 1弧度
const D3DXVECTOR3 ZeroVector3(0.0f ,0.0f ,0.0f);

bool JLCamera::sm_bEnableBurstMode = false;
//---------------------------------------------------------------------------
JLCamera::JLCamera()
{
    mCamLookAt      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    mCamPos         = D3DXVECTOR3(0.0f, 0.0f, -5.0f);
    mCamUp          = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    mLockLookPos    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	mCamLookTemp    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    mCamRadius      = 5.0f;
    mCamAngleH      = 0.0f;
    mCamAngleV      = 0.0f;
    mCamHeight      = 0.0f;
    mAngleHBuf      = 0.0f;
    mAngleVBuf      = 0.0f;
    mRadiusBuf      = 0.0f;
    mLockMinAngleV  =   5.0f; // 10.0f
    mLockMaxAngleV  =  85.0f;
    mLockMinRadius  =  50.0f;
    mLockMaxRadius  = 100.0f; // 80.0f
    mFreeMinAngleV  = -40.0f;
    mFreeMaxAngleV  =  80.0f;
    mFreeMinRadius  =   5.0f;
    mFreeMaxRadius  = 100.0f;
    mIsViewChange   = true;
    mIsNeedRefresh  = true;
    mIsVectorChange = true;
    mIsInvViewChange = true;
    mIsVecInvMatrixChange = true;
    mIsAllowChange  = false;
    mbAllowChangeRadius = true;
    g_vkhome_touch = false;
    burst_attack_state = false;
    UseCamera = false;
	DTime = 0;
	IsMinus = false;
}
//---------------------------------------------------------------------------
JLCamera::~JLCamera()
{

}

void JLCamera::SetProjectMatrix(float fovy, float Aspect, float zn, float zf)
{
    D3DXMatrixPerspectiveFovLH( &mProjectMatrix, fovy, Aspect, zn, zf );
}
//---------------------------------------------------------------------------
const float& JLCamera::GetHeight() const
{
    return mCamHeight;
}
//---------------------------------------------------------------------------
void JLCamera::SetHeight(float Height)
{
    mCamHeight = Height;
}
//---------------------------------------------------------------------------
const float& JLCamera::GetRadius() const
{
    return mCamRadius;
}
//---------------------------------------------------------------------------
void JLCamera::SetAngleHBuf(float AngleH)
{
    if( mIsAllowChange )
        mAngleHBuf = AngleH;
}
//---------------------------------------------------------------------------
void JLCamera::SetAngleVBuf(float AngleV)
{
    if( mIsAllowChange )
        mAngleVBuf = AngleV;
}
//---------------------------------------------------------------------------
void JLCamera::SetRadiusBuf(float Radius)
{
    if( /*mIsAllowChange &&*/ mbAllowChangeRadius )
        mRadiusBuf = Radius;
}
//---------------------------------------------------------------------------
void JLCamera::SetAngleH(float fAngleH)
{
    if( mIsAllowChange )
    {
        mCamAngleH = fAngleH;
        RefreshCameraPos();

        mIsVectorChange = true;
        mIsVecInvMatrixChange = true; 
    }
}
//---------------------------------------------------------------------------
void JLCamera::SetLockMinAngleV(float fAngleV)
{
    mLockMinAngleV = fAngleV;
}
//---------------------------------------------------------------------------
void JLCamera::SetLockMaxAngleV(float fAngleV)
{
    mLockMaxAngleV = fAngleV;
}
//---------------------------------------------------------------------------
void JLCamera::SetLockMinRadius(float fRadius)
{
    mLockMinRadius = fRadius;
}
//---------------------------------------------------------------------------
void JLCamera::SetLockMaxRadius(float fRadius)
{
    mLockMaxRadius = fRadius;
}
//---------------------------------------------------------------------------
void JLCamera::SetFreeMinAngleV(float fAngleV)
{
    mFreeMinAngleV = fAngleV;
}
//---------------------------------------------------------------------------
void JLCamera::SetFreeMaxAngleV(float fAngleV)
{
    mFreeMaxAngleV = fAngleV;
}
//---------------------------------------------------------------------------
void JLCamera::SetFreeMinRadius(float fRadius)
{
    mFreeMinRadius = fRadius;
}
//---------------------------------------------------------------------------
void JLCamera::SetFreeMaxRadius(float fRadius)
{
    mFreeMaxRadius = fRadius;
}
//---------------------------------------------------------------------------
void JLCamera::SetLockPos(const D3DXVECTOR3& LockPos)
{
	mLockLookPos = LockPos;
	if( IsMinus )
	{
		IsMinus = false;
		mCamLookTemp = mLockLookPos - mCamLookAt;
	}
} 
//---------------------------------------------------------------------------
void JLCamera::SetIsAllowChange(bool IsValue)
{
    mIsAllowChange = IsValue;
    mAngleHBuf     = 0.0f;
    mAngleVBuf     = 0.0f;
    mRadiusBuf     = 0.0f;
}
//---------------------------------------------------------------------------
const D3DXVECTOR3& JLCamera::GetPosition() const
{
    return mCamPos;
}
//---------------------------------------------------------------------------
const D3DXVECTOR3& JLCamera::GetLockPos() const
{
    return mLockLookPos;
}
//---------------------------------------------------------------------------
const D3DXVECTOR3& JLCamera::GetLookAtPos() const
{
    return mCamLookAt;
}
//---------------------------------------------------------------------------
// 計算攝影機所在位置
//---------------------------------------------------------------------------
void JLCamera::ComputeCameraPos()
{
    if( mCamAngleV < mLockMinAngleV ) mCamAngleV = mLockMinAngleV;
    if( mCamAngleV > mLockMaxAngleV ) mCamAngleV = mLockMaxAngleV;
    while( mCamAngleH < 0.0f ) mCamAngleH += 360.0f;
    while( mCamAngleH > 360.0f ) mCamAngleH -= 360.0f;
    if( mCamRadius < mLockMinRadius ) mCamRadius = mLockMinRadius;
    if( mCamRadius > mLockMaxRadius ) mCamRadius = mLockMaxRadius;

	float Y = sin( D3DXToRadian(mCamAngleV) ) * mCamRadius;
	float H = cos( D3DXToRadian(mCamAngleV) ) * mCamRadius;
	float X = sin( D3DXToRadian(mCamAngleH) ) * H;
	float Z = cos( D3DXToRadian(mCamAngleH) ) * H;
	mCamPos = D3DXVECTOR3( mCamLookAt.x+X, mCamLookAt.y+Y, mCamLookAt.z+Z );
}
//---------------------------------------------------------------------------
// 計算攝影機注視點所在位置
//---------------------------------------------------------------------------
void JLCamera::ComputeCameraLookAt()
{
    if( mCamAngleV < mFreeMinAngleV ) mCamAngleV =  mFreeMinAngleV;
    if( mCamAngleV > mFreeMaxAngleV ) mCamAngleV =  mFreeMaxAngleV;
    if( mCamAngleH < 0.0f           ) mCamAngleH += 360.0f;
    if( mCamAngleH > 360.0f         ) mCamAngleH -= 360.0f;
    if( mCamRadius < mFreeMinRadius ) mCamRadius =  mFreeMinRadius;
    if( mCamRadius > mFreeMaxRadius ) mCamRadius =  mFreeMaxRadius;

    float X, Y, Z, H;
	Y = sin( gRadian * mCamAngleV ) * mCamRadius;
	H = cos( gRadian * mCamAngleV ) * mCamRadius;
	X = sin( gRadian * mCamAngleH ) * H;
	Z = cos( gRadian * mCamAngleH ) * H;
    mCamLookAt = D3DXVECTOR3( mCamPos.x+X, mCamPos.y+Y, mCamPos.z+Z );
}
//---------------------------------------------------------------------------
// 更新攝影機位置
//---------------------------------------------------------------------------
void JLCamera::RefreshCameraPos()
{
    //計算攝影機位置
    ComputeCameraPos();

    mIsNeedRefresh  = false;
    mIsViewChange   = true;
    mIsInvViewChange = true;
}
//---------------------------------------------------------------------------
// 更新攝影機注視點
//---------------------------------------------------------------------------
void JLCamera::RefreshCameraLookAt()
{
    //限制攝影機高度
    mCamPos.y = mCamHeight;

    //計算攝影機注視點位置
    ComputeCameraLookAt();

    mIsViewChange = true;
    mIsInvViewChange = true;
}
//-----------------------------------------------------------------------
//  設定攝影機注視點位置
//-----------------------------------------------------------------------
void JLCamera::SetLookAtPos( const D3DXVECTOR3 &vLookPos )
{
    mCamLookAt = vLookPos;
    RefreshCameraPos();

    mIsVectorChange = true;
    mIsVecInvMatrixChange = true;
}
//-----------------------------------------------------------------------
//  設定攝影機位置
//-----------------------------------------------------------------------
void JLCamera::SetCamera( float AngleH, float AngleV, float Radius )
{
    mCamAngleH = AngleH;
    mCamAngleV = AngleV;
    mCamRadius = Radius;
    RefreshCameraPos();

    mIsVectorChange = true;
    mIsVecInvMatrixChange = true;
}
//-----------------------------------------------------------------------
//  設定攝影機位置
//-----------------------------------------------------------------------
void JLCamera::SetCamera( const D3DXVECTOR3 &CamPos, float AngleH, float AngleV, float Radius )
{
    mCamPos    = CamPos;
    mCamAngleH = AngleH;
    mCamAngleV = AngleV;
    mCamRadius = Radius;
    RefreshCameraLookAt();

    mIsVectorChange = true;
    mIsVecInvMatrixChange = true;
}
//-----------------------------------------------------------------------
// 攝影機前後移動
//-----------------------------------------------------------------------
void JLCamera::Walk(float Unit)
{
    D3DXVECTOR3 PosToAt = mCamLookAt - mCamPos;
    D3DXVec3Normalize(&PosToAt, &PosToAt);
    PosToAt *= Unit;
    D3DXVec3Add(&mCamPos, &mCamPos, &PosToAt);
    RefreshCameraLookAt();
}
//-----------------------------------------------------------------------
// 攝影機左右移動
//-----------------------------------------------------------------------
void JLCamera::Strafe(float Unit)
{
    D3DXVECTOR3 AddUnit, PosToAt = mCamLookAt - mCamPos;
    D3DXVec3Cross(&AddUnit, &PosToAt, &mCamUp);
    D3DXVec3Normalize(&AddUnit, &AddUnit);
    AddUnit *= Unit;
    D3DXVec3Add(&mCamPos, &mCamPos, &AddUnit);
    RefreshCameraLookAt();
}
//-----------------------------------------------------------------------
// 攝影機上下移動
//-----------------------------------------------------------------------
void JLCamera::Fly(float Unit)
{
	mCamHeight += Unit;
    RefreshCameraLookAt();
}
//-----------------------------------------------------------------------
// 攝影機上下轉動
//-----------------------------------------------------------------------
void JLCamera::Pitch(float Angle)
{
    mCamAngleV += Angle;
    RefreshCameraLookAt();
}
//-----------------------------------------------------------------------
// 攝影機左右轉動
//-----------------------------------------------------------------------
void JLCamera::Yaw(float Angle)
{
    mCamAngleH += Angle;
    RefreshCameraLookAt();

    // 改變攝影機方向向量(水平)
    mIsVectorChange = true;
    mIsVecInvMatrixChange = true;
}

//-----------------------------------------------------------------------
// 取得攝影機垂直角度
//-----------------------------------------------------------------------
const float& JLCamera::GetAngleV() const
{
    return mCamAngleV;
}
//-----------------------------------------------------------------------
// 取得攝影機水平角度
//-----------------------------------------------------------------------
const float& JLCamera::GetAngleH() const
{
    return mCamAngleH;
}
//-----------------------------------------------------------------------
D3DXVECTOR3 JLCamera::GetViewDistance()
{
    return (mCamPos-mCamLookAt);
}
//-----------------------------------------------------------------------
// 取得攝影機方向向量
//-----------------------------------------------------------------------
const D3DXVECTOR3& JLCamera::GetVector()
{
    if(mIsVectorChange)
    {   /*
        if( mCamAngleH < 0.0f           ) mCamAngleH += 360.0f;
        if( mCamAngleH > 360.0f         ) mCamAngleH -= 360.0f;
        if( mCamRadius < mLockMinRadius ) mCamRadius =  mLockMinRadius;
        if( mCamRadius > mLockMaxRadius ) mCamRadius =  mLockMaxRadius;

        float X = sin( gRadian * mCamAngleH ) * mCamRadius;
        float Y = sin( gRadian * 20.0f      ) * mCamRadius;
        float Z = cos( gRadian * mCamAngleH ) * mCamRadius;
        mVector = D3DXVECTOR3( X, Y, Z );

        mIsVectorChange = false;
        */

        if( mCamAngleH < 0.0f           ) mCamAngleH += 360.0f;
        if( mCamAngleH > 360.0f         ) mCamAngleH -= 360.0f;
        float fX = sin( gRadian * mCamAngleH ) * 500;
        float fY = sin( gRadian * 20.0f      ) * 500;
        float fZ = cos( gRadian * mCamAngleH ) * 500;
        mVector = D3DXVECTOR3( fX, fY, fZ );

        mIsVectorChange = false;
    }

    return mVector;
}
//-----------------------------------------------------------------------
// 取得攝影機反方向向量矩陣
//-----------------------------------------------------------------------
const D3DXMATRIX& JLCamera::GetVectorInverseMatrix()
{
    if( mIsVectorChange )
    {
        if( mCamAngleH < 0.0f           ) mCamAngleH += 360.0f;
        if( mCamAngleH > 360.0f         ) mCamAngleH -= 360.0f;
        float fX = sin( gRadian * mCamAngleH ) * 500;
        float fY = sin( gRadian * 20.0f      ) * 500;
        float fZ = cos( gRadian * mCamAngleH ) * 500;
        mVector = D3DXVECTOR3( fX, fY, fZ );

        mIsVectorChange = false;
    }

    if( mIsVecInvMatrixChange )
    {
        //計算反向矩陣
        D3DXMatrixLookAtLH(&mCamVectorInvMatrix, &ZeroVector3, &mVector, &mCamUp);
        D3DXMatrixInverse(&mCamVectorInvMatrix, NULL, &mCamVectorInvMatrix);
        mIsVecInvMatrixChange = false;
    }

    return mCamVectorInvMatrix;
}
//-----------------------------------------------------------------------
// 取得攝影機檢視矩陣
//-----------------------------------------------------------------------
const D3DXMATRIX& JLCamera::GetViewMatrix()
{
    //更新攝影機檢視矩陣
    if( mIsViewChange )
    {
        D3DXVECTOR3 vFarLookAt = mCamLookAt + ( mCamLookAt - mCamPos ) * 1000.0f;
        D3DXMatrixLookAtLH( &mCamView, &mCamPos, &vFarLookAt, &mCamUp );
	    //D3DXMatrixLookAtLH(&mCamView, &mCamPos, &mCamLookAt, &mCamUp);
        mIsViewChange = false;
    }

    return mCamView;
}
//---------------------------------------------------------------------------
// 取得攝影機檢視反轉矩陣
//-----------------------------------------------------------------------
const D3DXMATRIX& JLCamera::GetViewInverseMatrix() 
{
    //更新攝影機檢視矩陣
    if( mIsViewChange )
    {
	    D3DXVECTOR3 vFarLookAt = mCamLookAt + ( mCamLookAt - mCamPos ) * 1000.0f;
        D3DXMatrixLookAtLH( &mCamView, &mCamPos, &vFarLookAt, &mCamUp );
	    //D3DXMatrixLookAtLH(&mCamView, &mCamPos, &mCamLookAt, &mCamUp);
        mIsViewChange = false;
    }

    //更新攝影機反向檢視矩陣
    if( mIsInvViewChange )
    {
        D3DXMatrixInverse(&mCamInvView, NULL, &mCamView);
        mCamInvView._41 = mCamInvView._42 = mCamInvView._43 = 0.0f;
        mIsInvViewChange = false;
    }

    return mCamInvView;
}
/*-----------------------------------
爆擊鏡頭變化函式
-------------------------------------*/
void  JLCamera::Set_Burst_state_true()
{
        burst_attack_state=true;
}

void  JLCamera::SetBurstAttack()
{
        static float Radius_temp=0.0f;
        static int test_number=0;
        static bool front_back_state = 0; //0是向前 1是向後

        if(front_back_state==0&&burst_attack_state==true)
        {
                srand((unsigned)time(NULL));
                front_back_state=1;
                Radius_temp = -float(rand()%6+1);
                g_Camera.SetRadiusBuf(Radius_temp);
        }
        else
        {
                g_Camera.SetRadiusBuf(-(Radius_temp/6));
                test_number++;
                if(test_number>=6)
                {
                        burst_attack_state = false;
                        test_number=0;
                        front_back_state=0;
                }
        }

}
//----------------------------------------------------------------
//按home_key回歸角度
//----------------------------------------------------------------
void JLCamera::touch_home_key()
{
        g_vkhome_touch = true;
        PreTime = GetTickCount();
        Count_Time=0;
}
//---------------------------------------------------------------------------
// 攝影機處理
//---------------------------------------------------------------------------
void JLCamera::Process(float fElapsedTime)
{
    // camera position
  //  D3DXVec3Lerp( &mCamLookAt, &mCamLookAt, &mLockLookPos, 1.0f - powf( 0.6f, 30 * fElapsedTime ) );
    D3DXVec3Lerp( &mCamLookAt, &mCamLookAt, &mLockLookPos, 0.05f );

	
	
    // camera angle
    D3DXVECTOR3 AngleBuf( mAngleHBuf, mAngleVBuf, mRadiusBuf );
    D3DXVec3Lerp( &AngleBuf, &AngleBuf, &D3DXVECTOR3(0.0f,0.0f,0.0f), 1.0f - powf( 0.6f, 30 * fElapsedTime ) );
    mAngleHBuf *= 0.9f;
    if(fabs(mAngleHBuf) < 0.001f)   
        mAngleHBuf = 0.0f;
	//mAngleHBuf = AngleBuf.x;
	mAngleVBuf = AngleBuf.y;
    mRadiusBuf = AngleBuf.z;
    mCamAngleH += mAngleHBuf;
    mCamAngleV += mAngleVBuf;
    mCamRadius += mRadiusBuf;

    mIsNeedRefresh = true;
    mIsVectorChange = true;
    mIsVecInvMatrixChange = true;

    /* if(mCamLookAt != mLockLookPos)
    {
		//mCamLookAt += (mLockLookPos - mCamLookAt) * 0.08f;
		float Length = 0, FPSTemp = 0;
		if( DTime != 0 )
		{
			FPSTemp = g_RenderSystem.GetFPS();
			Length = 1 / ( ( float )DTime * FPSTemp );
		}

		if( DTime != 0 )
			mCamLookAt += mCamLookTemp * Length;
		else
			mCamLookAt += (mLockLookPos - mCamLookAt) * 0.08f;        

		if( fabs(mCamLookAt.x - mLockLookPos.x) < 0.001f )
            mCamLookAt.x = mLockLookPos.x;

        if( fabs(mCamLookAt.y - mLockLookPos.y) < 0.001f )
            mCamLookAt.y = mLockLookPos.y;

        if( fabs(mCamLookAt.z - mLockLookPos.z) < 0.001f )
            mCamLookAt.z = mLockLookPos.z;

		if( mCamLookTemp.x < 0 )
		{
			if( mCamLookAt.x < mLockLookPos.x )
				mCamLookAt.x = mLockLookPos.x;
		}
		else if( mCamLookTemp.x > 0 )
		{
			if( mCamLookAt.x > mLockLookPos.x )
				mCamLookAt.x = mLockLookPos.x;
		}

		if( mCamLookTemp.y < 0 )
		{
			if( mCamLookAt.y < mLockLookPos.y )
				mCamLookAt.y = mLockLookPos.y;
		}
		else if( mCamLookTemp.y > 0 )
		{
			if( mCamLookAt.y > mLockLookPos.y )
				mCamLookAt.y = mLockLookPos.y;
		}

        if( mCamLookTemp.z < 0 )
		{
			if( mCamLookAt.z < mLockLookPos.z )
				mCamLookAt.z = mLockLookPos.z;
		}
		else if( mCamLookTemp.z > 0 )
		{
			if( mCamLookAt.z > mLockLookPos.z )
				mCamLookAt.z = mLockLookPos.z;
		}

        mIsNeedRefresh  = true; 
    }
	else
		DTime = 0;

    if(mAngleHBuf != 0.0f)
    {
        mAngleHBuf *= 0.9f;
        //mAngleHBuf = Camera_Lerp( mAngleHBuf, 0.0f, 1.0f - powf( 0.6f, 30 * fElapsedTime ) );
        mCamAngleH += mAngleHBuf;
        if(fabs(mAngleHBuf) < 0.001f)   
            mAngleHBuf = 0.0f;
        mIsNeedRefresh  = true;

        // 改變攝影機方向向量(水平)
        mIsVectorChange = true;
        mIsVecInvMatrixChange = true;
    }

    if(mAngleVBuf != 0.0f)
    {
        mAngleVBuf *= 0.8f;
        //mAngleVBuf = Camera_Lerp( mAngleVBuf, 0.0f, 1.0f - powf( 0.6f, 30 * fElapsedTime ) );
        mCamAngleV += mAngleVBuf;
        if(fabs(mAngleVBuf) < 0.001f)    
            mAngleVBuf = 0.0f;
        mIsNeedRefresh  = true;
    }

    if(mRadiusBuf != 0.0f)
    {
        mRadiusBuf *= 0.7f;
        mCamRadius += mRadiusBuf;
        if(fabs(mRadiusBuf) < 0.001f)    mRadiusBuf = 0.0f;
        mIsNeedRefresh  = true;
    } 
*/
    //測試是否有回歸攝影機角度__goodmorning
    if(g_vkhome_touch==true)
    {
            if(Count_Time==0)
            {
                Count_Time=1;
            }
            else if(Count_Time==1)
            {
                 PreTime = GetTickCount()-PreTime;
                 Count_Time=2;
            }

            if(Count_Time==2)
            {
                float change_angle_count = g_Camera.GetAngleH();
                if( change_angle_count > 180.0f )
                {
                    float change_angle_temp=change_angle_count-180.0f;
                    if(change_angle_temp >18.0f)
                    {
                            change_angle_count-=(change_angle_temp/6*PreTime/25);
                    }
                    else
                    {
                            change_angle_count-=3.0f*PreTime/25;
                            if(change_angle_count <=180.0f)
                            {
                                    change_angle_count=180.0f;
                                    g_vkhome_touch = false;
                                    Count_Time=0;
                            }
                    }
                    g_Camera.SetAngleH(change_angle_count);

                }
                else if( change_angle_count < 180.0f )
                {
                    float change_angle_temp=change_angle_count-180.0f;
                    if(change_angle_temp < (-18.0f) )
                    {
                            change_angle_count-=(change_angle_temp/6*PreTime/25);;
                    }
                    else
                    {
                            change_angle_count+=3.0f*PreTime/25;
                            if(change_angle_count >=180.0f)
                            {
                                    change_angle_count=180.0f;
                                    g_vkhome_touch = false;
                                    Count_Time=0;
                            }
                    }
                    g_Camera.SetAngleH(change_angle_count);
                }
            }
    }

    if( sm_bEnableBurstMode )//g_HUISystemSet.m_IsCheck[04] )
    {
        if(burst_attack_state==true)
        {
            SetBurstAttack();
        }
    }

    //攝影機位置更新
    if(mIsNeedRefresh)  RefreshCameraPos();
}
//---------------------------------------------------------------------------
// Handles messages
//---------------------------------------------------------------------------
LRESULT JLCamera::HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_KEYDOWN:
        {
            switch( wParam )
            {
                case 'W':
                {
  		            Walk(0.5f);
                    break;
                }

                case 'S':
                {
  		            Walk(-0.5f);
                    break;
                }

                case 'A':
                {
  		            Strafe(0.5f);
                    break;
                }

                case 'D':
                {
  		            Strafe(-0.5f);
                    break;
                }  

                case VK_LEFT:
                {
  		            Yaw(-1.0f);
                    break;
                }

                case VK_RIGHT:
                {
  		            Yaw(1.0f);
                    break;
                }

                case VK_UP:
                {
  		            Pitch(0.8f);
                    break;
                }

                case VK_DOWN:
                {
  		            Pitch(-0.8f);
                    break;
                }

                case 'Q':
                {
  		            Fly(0.2f);
                    break;
                }

                case 'E':
                {
  		            Fly(-0.2f);
                    break;
                }
            }   
        }
        break;
    }

    return S_OK;
}
//---------------------------------------------------------------------------

} //namespace FDO







