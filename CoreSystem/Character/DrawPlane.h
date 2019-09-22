//---------------------------------------------------------------------------
#ifndef _DRAWPLANE_H_
#define _DRAWPLANE_H_

#include "../RoleStruct/CRoleDef.h"
//#include "CRoleDef.h"
#include "FDO_RoleMemoryMgr.h"
#include <d3d9.h>
#include <d3dx9math.h>


//----------------------------------------------------------------------------

namespace FDO
{
//----------------------------------------------------------------------------
// struct
//----------------------------------------------------------------------------
struct PlaneVertex
{
    D3DXVECTOR3 Position;   // The original position of the vertex
    D3DXVECTOR3 Normal;     // 法向量
    D3DCOLOR	Color;      // The vertex color
    float       tu, tv;     // 材質軸

    static const DWORD FVF;
};

struct PlaneVertex1
{
    float x, y, z, h;
    D3DCOLOR Color;
    float    tu, tv;

    static const DWORD FVF;
};

struct PlaneVertex2
{
    D3DXVECTOR3 Position;
    float       tu,tv;

    static const DWORD FVF;
};

struct PlaneVertex3
{
    D3DXVECTOR3 Position;
    D3DCOLOR	Color;
    float       tu,tv;
    
    static const DWORD FVF;
};

struct PlaneVertex4
{
    float x, y, z;
    float tu,tv;

    static const DWORD FVF;
};

struct PlaneVertex5
{
    float x, y, z, h;
    D3DCOLOR Color;
    float    tu0, tv0;
	float	 tu1 ,tv1 ; 

    static const DWORD FVF;
};

//----------------------------------------------------------------------------
// Class DrawPlane
//----------------------------------------------------------------------------
struct SkinFrmInst; // forward declaration
class DrawPlane
{
private:
    LPDIRECT3DDEVICE9       m_pDevice;
    LPDIRECT3DVERTEXBUFFER9 m_pVB;
    D3DCOLOR                m_Color;

    bool Create();  

public:
     DrawPlane();
    ~DrawPlane();

    void RenderPlane( const sFrame* pFrame, const D3DCOLOR& Color );
    void RenderPlane( const SkinFrmInst* pSFrmInst, const D3DCOLOR& Color );
	void RenderPlaneNew( const SRoleSkinImageNew* pSkinImage, const D3DCOLOR& Color );
};

//----------------------------------------------------------------------------
// Class DrawPlane1
//----------------------------------------------------------------------------
class DrawPlane1
{
private:
    LPDIRECT3DDEVICE9       m_pDevice;
    D3DCOLOR                m_Color;

public:
     DrawPlane1( const D3DCOLOR& Color );
    ~DrawPlane1();

    void RenderPlane( const RECT& rect );
};

//----------------------------------------------------------------------------
// Class DrawPlane2
//----------------------------------------------------------------------------
class DrawPlane2
{
private:
    LPDIRECT3DDEVICE9       m_pDevice;
    LPDIRECT3DTEXTURE9      m_Texture;
    D3DXMATRIX              m_matWorld;   

    bool Create( const char* szTexName );

public:
     DrawPlane2( const char* szTexName );
    ~DrawPlane2();

    void RenderPlane( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3 );
};

//----------------------------------------------------------------------------
// Class DrawPlane3
//----------------------------------------------------------------------------
class DrawPlane3
{
private:
    LPDIRECT3DDEVICE9       m_pDevice;
    LPDIRECT3DVERTEXBUFFER9 m_pVB[13];                                  
    D3DCOLOR                m_Color[13];
    LPDIRECT3DTEXTURE9      m_Texture;

    bool Create();
    const LPDIRECT3DVERTEXBUFFER9 GetVertexBuf( int iIndex, const DWORD& Color );  

public:
     DrawPlane3();
    ~DrawPlane3();

    void RenderPlane( const BYTE& Type, const int& iNum, const D3DCOLOR& Color, const D3DXMATRIX& matWorld );
};

//----------------------------------------------------------------------------
// Class DrawPlane4
//----------------------------------------------------------------------------
class DrawPlane4
{
private:
    LPDIRECT3DDEVICE9 m_pDevice;

public:
     DrawPlane4();
    ~DrawPlane4();

    void RenderPlane(const RECT &rect, const D3DCOLOR& Color );
    void RenderPlane( int w, int h, const D3DCOLOR& Color );
};

//----------------------------------------------------------------------------
// Class DrawPlane5
//----------------------------------------------------------------------------
class DrawPlane5
{
private:
    LPDIRECT3DDEVICE9       m_pDevice;
    LPDIRECT3DVERTEXBUFFER9 m_pVB;

    bool Create();

public:
     DrawPlane5();
    ~DrawPlane5();

    void RenderPlane();
};

//----------------------------------------------------------------------------
// Class DrawPlane6
//----------------------------------------------------------------------------
class DrawPlane6
{
private:
    LPDIRECT3DDEVICE9       m_pDevice;
    LPDIRECT3DVERTEXBUFFER9 m_pVB;

    bool Create();

public:
     DrawPlane6();
    ~DrawPlane6();

    void RenderPlane();
};

//----------------------------------------------------------------------------
// Class DrawPlane8
//----------------------------------------------------------------------------
class DrawPlane8
{
private:
    LPDIRECT3DDEVICE9 m_pDevice;

public:
     DrawPlane8();
    ~DrawPlane8();

    void RenderPlane( const RECT& rect, bool bInverse );
};

//----------------------------------------------------------------------------
// Class DrawPlane9
//----------------------------------------------------------------------------
class DrawPlane9
{
private:
    LPDIRECT3DDEVICE9 m_pDevice;

public:
     DrawPlane9();
    ~DrawPlane9();

    void RenderPlane( const RECT& rect, const sFrame* pFrame );
};



//----------------------------------------------------------------------------
// Class DrawPlane11
//----------------------------------------------------------------------------
class DrawPlane11
{
private:
    LPDIRECT3DDEVICE9       m_pDevice; 
    LPDIRECT3DTEXTURE9      m_Texture;
    D3DXMATRIX              m_matWorld;   

    bool Create( const char* szTexName );

public:
     DrawPlane11( const char* szTexName );
    ~DrawPlane11();

    void RenderPlane( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2,
                      const D3DXVECTOR3& v3, const D3DXVECTOR3& v4 );
};

//----------------------------------------------------------------------------
// Class DrawPlane12
//----------------------------------------------------------------------------
class DrawPlane12
{
private:
    LPDIRECT3DDEVICE9 m_pDevice;
    D3DCOLOR          m_Color;

public:
     DrawPlane12( const D3DCOLOR& Color );
    ~DrawPlane12();

    void RenderPlane( const RECT& rect, const D3DCOLOR& Color );
};

//----------------------------------------------------------------------------
// Class DrawPlane13 
//----------------------------------------------------------------------------
class DrawPlane13
{
private:
	LPDIRECT3DDEVICE9 m_pDevice;	
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
public:
	DrawPlane13();
	~DrawPlane13();

	void RenderPlane( const PlaneVertex3* p1, const PlaneVertex3* p2 , const PlaneVertex3* p3,const D3DCOLOR& Color );
};

//----------------------------------------------------------------------------
// Class CDrawFrame
//----------------------------------------------------------------------------
class CDrawFrame
{
private:
    LPDIRECT3DDEVICE9 m_pDevice;
    D3DXMATRIX        m_matWorld;   

public:
     CDrawFrame();
    ~CDrawFrame();

    void RenderPlane( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2,
                      const D3DXVECTOR3& v3);
};

//----------------------------------------------------------------------------
// Class CDrawArrow
//----------------------------------------------------------------------------
class CDrawArrow
{
private:
    LPDIRECT3DDEVICE9       m_pDevice;
    LPDIRECT3DTEXTURE9      m_Texture;
    D3DXMATRIX              m_matWorld;   

    bool Create( const char* szTexName );

public:
     CDrawArrow( const char* szTexName );
    ~CDrawArrow();

    void RenderPlane( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3 );
};

//----------------------------------------------------------------------------
//  static 物件之函式
//----------------------------------------------------------------------------
void DrawRole( const sFrame* pFrame, const D3DCOLOR& Color );
void DrawRole( const SkinFrmInst* pSFrmInst, const D3DCOLOR& Color ); // <yuming>

void DrawRole( const SRoleSkinImageNew* pSkinImage, const D3DCOLOR& Color );	//kof1016

void DrawEmotion();
void DrawChargeBackBar();
void DrawBackground( const RECT& rect );
void DrawBackground( const RECT& rect, const D3DCOLOR& Color );
void DrawFadeOut(const RECT& rect, const D3DCOLOR& Color );
void DrawFadeOut( int w, int h, const D3DCOLOR& Color );
void DrawBar( const RECT& rect, const D3DCOLOR& Color );
void DrawCursor( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3 );
void DrawFrame( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3 );
void DrawDamage( const BYTE& Type, const int& iNum, const D3DCOLOR& Color, const D3DXMATRIX& matWorld );
void DrawAnimation( const RECT& rect, bool bInverse );
void DrawDisplayRole( const RECT& rect, const sFrame* pFrame );
void DrawHalo( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2,
               const D3DXVECTOR3& v3, const D3DXVECTOR3& v4 );
void DrawShadow( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2,
                 const D3DXVECTOR3& v3, const D3DXVECTOR3& v4 );
void DrawShadow( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3 );

void DrawTimeSquare( float rate , const RECT& rect, const D3DCOLOR& Color, bool DrawBlack = false );

void DrawArrow( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3 );

void GetProjection2D(D3DXMATRIX &outMat , unsigned int width , unsigned int hegith) ; 
void BeginRender2D(LPDIRECT3DDEVICE9 pd3d , D3DVIEWPORT9 &tempViewPort ,D3DXMATRIX &tempProjMat , D3DXMATRIX &tempViewMat ,
				   D3DVIEWPORT9 &viewPort , D3DXMATRIX &projMat , D3DXMATRIX &viewMat) ; 
void EndRender2D(LPDIRECT3DDEVICE9 pd3d , D3DVIEWPORT9 &viewPort , D3DXMATRIX &projMat , D3DXMATRIX &viewMat) ; 

struct Draw2DTextureInfo
{
	LPDIRECT3DDEVICE9 pDevice;
	IDirect3DTexture9* pTxr;
	int nWidth;
	int nHeight;
	const RECT* prcSrc;
	RECT rcScreen;
	bool		bDiffColor;
	D3DCOLOR	colors[4];
};
void Draw2DTexture	(const Draw2DTextureInfo& inf);

void Draw2DTexture(LPDIRECT3DDEVICE9 pDevice , IDirect3DTexture9* pTxr , const RECT &screen , const RECT* prcSrc , 
				   unsigned int texW , unsigned int texH , DWORD color);

//////////////////////////////////				new rendering technique				/////////////////////////////

struct stFreameEx
{
public:
	PlaneVertex3			ver[6] ; 
	short					layer ; 
	LPDIRECT3DTEXTURE9		pTexture ; 
	bool					divide ;
	DWORD					curDivided ; 
};

struct stUiStream 
{
	LPDIRECT3DTEXTURE9 tex ; 
	PlaneVertex1	   ver[6] ;
};

class CGeometryBuffer
{
public:
	CGeometryBuffer(LPDIRECT3DDEVICE9 pDevice) 
	{
		m_pDevice = pDevice ; 
		m_pVb = NULL ; 
	}
	~CGeometryBuffer()
	{
		if(m_pVb != NULL)
		{
			m_pVb->Release() ; 
			m_pVb = NULL ; 

		}
	}
public:
	void CreateGeometryBuffer(UINT vbSize , DWORD FVF)
	{
		HRESULT hr = m_pDevice->CreateVertexBuffer(vbSize , 0 , FVF , D3DPOOL_MANAGED , &m_pVb , NULL) ; 

		if(hr != S_OK)
		{
			Assert(0 , "create vertex buffer error") ; 
		}
	}
	void* Lock() 
	{
		void *ptr = NULL ; 

		if(m_pVb != NULL)
		{
			if(SUCCEEDED(m_pVb->Lock(0 , 0 , &ptr , 0)))
			{
				return ptr ; 
			}
		}

		return ptr; 

	}
	void UnLock()
	{

		if(m_pVb != NULL)
		{
			m_pVb->Unlock() ; 
		}
	}
	void SetStreamSource(unsigned int stride , unsigned int offset)
	{
		if(m_pVb != NULL)
		{
			m_pDevice->SetStreamSource( 0, m_pVb, offset, stride );

		}
	}
private:
	LPDIRECT3DVERTEXBUFFER9					m_pVb ; 
	LPDIRECT3DDEVICE9						m_pDevice;
};

class CRenderer
{
	struct stFrameStream
	{
		unsigned int verCount ; 
		unsigned int offset ; 
		short		 layer ; 
		LPDIRECT3DTEXTURE9 tex ;
		
	};
	struct stNumberStream
	{
		float width ; 
		float height ; 
		short layer ; 
		D3DCOLOR Color ; 
		D3DXVECTOR2 uv ;
		D3DXVECTOR2 Uv ; 
		D3DXVECTOR2 uV ; 
		D3DXVECTOR2 UV ; 
	};

public:
	CRenderer(LPDIRECT3DDEVICE9 pDevice , unsigned int shadowVerCount , unsigned int frameVerCount) ;
	~CRenderer();
	
public:
	void CreateShadowOp(const D3DXVECTOR3 &center , const D3DXVECTOR3 &offset , float shadowSize);
	void CreateFrameOp(const sFrame *pFrame , const D3DXMATRIX &invWorldMat , const D3DCOLOR& Color , bool divide = false) ; 
	void CreateFrameOp(const SRoleSkinImageNew* pSkinImage, const D3DXMATRIX &invWorldMat , const D3DCOLOR& Color , bool divide = false) ;
	void CreateDamageOp(const BYTE& Type, const int& iNum, const D3DCOLOR& Color, const D3DXMATRIX& matWorld ,
		 LPDIRECT3DTEXTURE9 pTex, bool bIsPlus = false ) ; 
	void CreateUiOp(IDirect3DTexture9* pTxr , const RECT &screen , const RECT* prcSrc , 
				   unsigned int texW , unsigned int texH , DWORD color); 
	
	void FlushShadow(D3DXMATRIX	*pMat , IDirect3DTexture9 *pTxr);
	void FlushFrame(void) ; 
	void FlushDamage(void) ; 
	void FlushUI(void) ; 
	void Clear();
	void SetDivideTime(DWORD maxTime) { m_divideTime = maxTime;} 
	void Update(DWORD dwPassTime) ; 
private:
	void CreateOp(LPDIRECT3DTEXTURE9 pTex , const D3DXMATRIX &invWorldMat , float width , float height ,
		float tu1 , float tv1 , float tu2 , float tv2 , DWORD color , short layer , bool bInvert , bool divide = false)  ; 
	void CreateOp(float width , float height , short layer , const D3DCOLOR& Color , 
						 const D3DXVECTOR2 &uv , const D3DXVECTOR2 &Uv , 
						 const D3DXVECTOR2 &uV , const D3DXVECTOR2 &UV , 
						 const D3DXMATRIX& matWorld , LPDIRECT3DTEXTURE9 pTex);
	void Flush(std::vector<stFreameEx*> &frameVec) ;  
	PlaneVertex4* GetShadowPoint() ; 
private:
//	std::vector<PlaneVertex4*>		m_shadowOpVec ; 
//	boost::pool<>					m_shadowOpPool ; 
//	std::list<PlaneVertex4*>		m_freeshadowOpPool ; 

	std::vector<PlaneVertex4>		m_shadowOpVec;
	std::vector<stUiStream*>		m_uiOpVec ; 
	boost::pool<>					m_uiOpPool ; 

	std::list<stFreameEx*>			m_freeFrameOpList ; 
	std::list<stFreameEx*>			m_freeDamageOpList ; 


	std::vector<stFreameEx*>		m_frameOpList ; 
	std::vector<stFreameEx*>		m_damageOpList ; 
	std::list<stFreameEx*>			m_divideOpList ; 

	boost::pool<>					m_frameOpPool ; 

	LPDIRECT3DDEVICE9				m_pDevice ; 
	CGeometryBuffer					*m_pShadowBuffer ; 
	CGeometryBuffer					*m_pFrameBuffer ; 
	CGeometryBuffer					*m_pUiBuffer ; 

	unsigned int					m_frameCount ; 
	unsigned int					m_maxShadowVertexCount ; 

	float							m_numberUnit ; 
	stNumberStream					m_numberV[17] ; 

	DWORD							m_divideTime ; 

};


//inline bool sortFrameEx(const FDO::stFreameEx *left ,const  FDO::stFreameEx *right)
//{
//		//if(left->layer != right->layer)
//		{
//		return (left->layer < right->layer); 
//		}
//	
//	//	return (left->pTexture < right->pTexture);
//}

struct sortFrameEx
{
	inline bool operator()(const stFreameEx *left ,const  stFreameEx *right)
	{
		if(left->layer != right->layer)
		{
		return (left->layer < right->layer); 
		}
	
		return (left->pTexture < right->pTexture);
	}
};





//BlurTextureHelper是用來處理貼圖的柔邊效果
//目前是提供給DisplayRole顯示使用
//柔化效果的處理是很制式的, 尚未參數化
class BlurTextureHelper
{
	PlaneVertex1 Vertices[4*5];
	SHORT indexBuffer[6*5];
public:
	BlurTextureHelper();
	void InitHelper(int texW, int texH);
	void BlurTexture(LPDIRECT3DDEVICE9 pd3dDevice , IDirect3DTexture9* pTex);
private:
	void InitGeometry(float w, float h);
	void InitVertex(PlaneVertex1 &v, float x, float y, float z, float w, DWORD color, float tu, float tv)
	{
		v.x = x, v.y = y, v.z = z, v.h = 1, v.Color = color, v.tu = tu, v.tv = tv;
	}
};

 
//----------------------------------------------------------------------------
} // namespace FDO
//----------------------------------------------------------------------------
#endif // _DRAWPLANE_H_
