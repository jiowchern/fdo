#include "stdafx.h"
#include "CControlParticleRender.h"
#include "FDO_ParticleObject.h"
#include "ParticleSystem.h"
#include "PhenixD3D9RenderSystem.h"
#include "DrawPlane.h"

namespace FDO
{
	D3DVIEWPORT9 CControlParticleRender::s_ViewPort;
	D3DXMATRIX CControlParticleRender::s_matProj;
	D3DXMATRIX CControlParticleRender::s_matView;

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	CControlParticleRender::CControlParticleRender() 
		: m_pParticle(0)
		, m_pd3dDevice( g_RenderSystem.GetD3DDevice() ) 
		, m_disScale(1.0f)
	{
		memset( &m_ViewPort, 0, sizeof(D3DVIEWPORT9) );
		
		D3DXMatrixLookAtLH( &m_matView,
			&D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), // 攝影機位置
			&D3DXVECTOR3( 0.0f, 0.0f, 1.0f ), // 注視點
			&D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) ); // up vector
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	CControlParticleRender::~CControlParticleRender()
	{
		delete m_pParticle ; 
		m_pParticle = NULL ; 
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	void CControlParticleRender::RenderPipeline()
	{
		RenderParticle();
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	void CControlParticleRender::Update(float fElapsedTime)
	{
		if(m_pParticle != NULL)
		{
			m_pParticle->FrameMoveParticle(fElapsedTime) ; 
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	void CControlParticleRender::SetViewPosition(float left , float top) 
	{
		m_ViewPort.X = (DWORD)left ; 
		m_ViewPort.Y = (DWORD)top ; 
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	void CControlParticleRender::SetViewSize(uint width , uint height)
	{
		m_ViewPort.Width = width ; 
		m_ViewPort.Height = height ; 
		GetProjection2D(m_matProj , width , height) ; 
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	void CControlParticleRender::PushParticle(const char* emtName , bool isStatic)
	{
		delete m_pParticle ;
		m_pParticle = NULL ;

		m_pParticle = new ParticleObject() ; 

		m_pParticle->CreateParticle(emtName , isStatic) ;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	void CControlParticleRender::RenderParticle()
	{
		if( this->BeginRender() )
		{
			if(m_pParticle != NULL)
			{
				D3DXMATRIX mat ; 
				D3DXMatrixIdentity(&mat) ; 

				GetParticleSystemPtr()->SetViewMatrix(mat);
				GetParticleSystemPtr()->SetInvViewMatrix(mat);

				D3DXMatrixScaling(&mat , m_disScale ,  m_disScale , 1.0f) ; 
				mat._41 = 0.0f ; 
				mat._42 = 0.0f ;
				mat._43 = 1.0f ; 

				m_pParticle->SetPosition(mat._41 , mat._42 , mat._43 ) ; 
				m_pParticle->SetScale(mat._11 , mat._22 , mat._33) ; 
				m_pParticle->RenderParticle(0.0f , m_matView , false ) ; 
			}
			this->EndRender();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	bool CControlParticleRender::BeginRender()
	{
		//LockView() ; 
		BeginRender2D(m_pd3dDevice , s_ViewPort , s_matProj , s_matView ,  m_ViewPort , m_matProj , m_matView) ; 
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	bool CControlParticleRender::EndRender()
	{
		// 還原攝影機矩陣
		//   m_pd3dDevice->SetTransform( D3DTS_VIEW, &s_matView );
		//UnLockView() ; 
		EndRender2D(m_pd3dDevice , s_ViewPort , s_matProj , s_matView) ; 


		D3DXMATRIX mat ; 
		D3DXMatrixIdentity(&mat) ; 

		m_pd3dDevice->SetTransform(D3DTS_WORLD , &mat) ; 
		m_pd3dDevice->SetTexture(0 , NULL) ; 

		return true;
	}


}
