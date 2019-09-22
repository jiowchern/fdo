#ifndef CControlParticleRender_H_20100107pm0306
#define CControlParticleRender_H_20100107pm0306

#include <stack>
namespace FDO
{
	class ParticleObject ; 

	//////////////////////////////////////////////////////////////////////////
	//在ctrl上播放分子的物件
	//////////////////////////////////////////////////////////////////////////
	class CControlParticleRender : public IDXUTRender
	{
	private:
		D3DVIEWPORT9			m_ViewPort;
		D3DXMATRIX				m_matView;
		D3DXMATRIX				m_matProj;

		float					m_disScale ; 
		
		static D3DVIEWPORT9 	s_ViewPort;
		static D3DXMATRIX 		s_matProj;
		static D3DXMATRIX 		s_matView;

		LPDIRECT3DDEVICE9       m_pd3dDevice;
		ParticleObject*			m_pParticle ; 

	public:
		CControlParticleRender							();
		virtual	~CControlParticleRender					();
		
	public:
		//////////////////////////////////////////////////////////////////////////
		//實作介面函式
		//////////////////////////////////////////////////////////////////////////
		void 			RenderPipeline					();
		void 			Update							(float fElapsedTime);
		void 			SetViewPosition					(float left , float top) ;
		void 			SetViewSize						(unsigned int width , unsigned int height);

		//////////////////////////////////////////////////////////////////////////
		//
		//////////////////////////////////////////////////////////////////////////
		void 			SetScale						(float scale){m_disScale = scale ; }
		void			PushParticle					(const char *emtName , bool isStatic);
		void			RenderParticle					();

		bool 			BeginRender						();
		bool 			EndRender						();
	};
}
#endif
