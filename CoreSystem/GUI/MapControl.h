#ifndef _MAPCONTROL_H_
#define _MAPCONTROL_H_
#include <d3d9.h>
#include <d3dx9.h>
#include "CDXUTControl.h"

const float MOVEOFFSET = 0.05f ;//���ʺ�ǫ�
const float EPSINON = 0.001f ; //��ǫ�


////
//		���O�GTexture�y�б���
//		�إߡG2008/1/9
//		�̫��s�G2008/1/21
//		�@�̡Gcosiann
//		��s�@�̡Gcosiann
//		�����G�Ъ`�N��o���O���R�A�ܼơA�H�Ϋ����غc�l�Moperator=
////


//�����Y���[���G�b���I�e�P�����ܮɡA�n��Texure�@��j���ĪG�A�Э��CTexture���y�СC
//				�Ϥ��A�n��Texure�@�Y�p���ĪG�A�д���Texture���y�СC�Ъ`�N�O�_�W�L�K�Ϯy�нd��C
//				�ҥH�Am_texUVpos�O�H�ۤH�����ʡA���Om_texUV's Rect�|�H�ɧ�s�C�Ь�UpdateVertex( )

//�Q�n�䴩(u,v) > 1.0f���ܡA�Цbs_texcoordcenter�]�w1.0f�A�hs_texcoordrightbottom�|�ܦ�2.0f

class CTextureCoordControl 
{
	static const int MY_FVF_VERTEX = (D3DFVF_XYZRHW | D3DFVF_TEX2);
	struct Vertex
	{
		float x ; 
		float y ;
		float z ;
		float rhw ;
		float u0 ; 
		float v0 ;
		float u1 ;
		float v1 ;
	};

	typedef Vertex VERTEX2D ;

public:
	CTextureCoordControl( void ) ;
	~CTextureCoordControl( void ) ;
private:
	CTextureCoordControl( const CTextureCoordControl&) ;
	CTextureCoordControl* operator =(  const CTextureCoordControl & ) ;
public:				
	////////				���}�����G�]�w���u�㪺�����ݩ�		///////////////
	void SetUVTranslate(float x , float y) ;
	void AddUVTranslate(float offsetx , float offsety ) ;
	
	void SetVertexSize( int sx , int sy ) ;			//�]�w���I���e�׻P����
	void SetVertexPos( int posx ,  int posy ) ;		//���I��m
	void SetZoomMultiple( float multiple ) ;			//�]�w�Y�񭿼�
	void SetMaxZoomOut( float value ) ;
	void SetMaxZoomIn( float worldwidth , float worldheight) ;	

	void SetTexUVSize( float u , float v ) ;//�]�w�K�Ϯy�мe�P���C�п�Jvalue <= s_texcoordcenter �C
	void SetTexUVPos( float u , float v ) ; //�]�w�K�Ϯy�Ц�m
	static void SetRotation(float rot) ; 
	static float GetRotation(void) ; 
public:
	////////				���}�����G���o���u�㪺�����ݩ�		///////////////
	void GetTextureSize( float &texwidth , float &texheight ) const;
	bool IsInsideForShowUV( float u , float v ) ;
	void GetVertexLeftTop( float &left , float &top ) const ;
	void GetVertexSize( float &width , float &height ) const ;
	void GetUVpos( float &u , float &v ) const;	 //���oUV�{������m
	void GetZoom( float &scalex , float &scaley )const  ;  //���o�Y��Y��
	void GetUVposRect( float &left , float &top , float &right , float &bottom ) const; //���oUV���x���d��
	void ResetZoomValue( float speed , float time ) ;		//���s�]�w�Y��ƭ�
	bool IsMaxZoomIn( void ) const ;				//�P�_�O�_�Y���̤j
	bool IsMinZoomOut( void ) const ;				//�P�_�O�_�Y���̤p
	bool GetRotUVCoord(float inScaleX , float inScaleY , float &outU , float &outV , float &outCenterX ,float &outCenterY) ;//���o�d�򤤪��Y�@��UV��
public:
	/////					�D�n�\��G�Y��
	void TexZoomIn( void ) ;
	void TexZoomOut( void ) ;
	void TexZoomInMax( void ) ;						//�Y�p��̤j����
	void TexZoomOutMax( void ) ;					//��j��̤j����

public:
	//////					���}�����G�]�w���u�㪺d3d�t��
	void SetAlphaTexture(LPDIRECT3DTEXTURE9 tex) ;
	void SetTexture(LPDIRECT3DTEXTURE9 tex) ;
	void LoadTexture( LPCWSTR filename ) ;
	void LoadAlphaTexture( LPCWSTR filename ) ;
	void DrawTex( IDirect3DDevice9 *d3d ) ;
	void DrawTexWithAlphaTex( IDirect3DDevice9 *d3d ) ;
public:
	//////					���եΡG�Q�ΰT�����		//////////////
	bool MessageHandle( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) ;
private:
	///////					�p���G�Բӱ���u�㪺����		///////////////
	void UpdateTextureAtt( void ) ;
	void UpdateVertex( void ) ;
	void UpdateZoomCenter( float &u , float &v ) ; //��s�Y�񤤤��I
	void UpdateUVCoord( float centeru , float centerv , float uwidth , float vheight ) ; //��s�K�Ϯy�Ъ��|���I
	void SetVertex( VERTEX2D *vec , float x , float y , float z , float rhw , 
		float u0 , float v0 , float u1 , float v1  ) ;
	bool CheckInside( float &left , float &top , float &right , float &bottom , float min , float max ,
					  float &u , float &v) ;
private:
	void Init( void ) ;
	void Release( void ) ;
private:
	//////////					���I�����ݩʡG�W���i�o�ץ�(��)			//////////////
	D3DXVECTOR2 				m_versize ;
	D3DXVECTOR2					m_verpos;
	D3DXVECTOR2					m_verbasepos[4] ;
	VERTEX2D					m_mapvertex[4] ;

	/////////					Texture�����ݩ�
	//��tex�y�Ъ��K�Ϥ����I(�̹Ϫ���Ҥj�p�ӨM�w�A���F��ܤ����u�ӳ]�w)
	D3DXVECTOR2					m_texUVcenter ; 
	D3DXVECTOR2 				m_texUVpos ; //tex�y�Ъ��K�Ϧ�m(�����I�@�묰0.5,0.5) �O�����a��UV��m
	D3DXVECTOR2					m_texUV ;	 //����tex�y�Ъ��e��
	D3DXVECTOR2					m_texSize;	 //����tex���e��
	D3DXVECTOR2					m_cameraUV;  //����camera��UV
	float						m_texUVleft; //����tex���|�I�y��
	float						m_texUVtop;
	float						m_texUVright;
	float						m_texUVbottom;



	/////						��j�ݩ�			/////////
	D3DXVECTOR2					m_texzoom ;			//��j�������q
	float						m_texzoommultiple;  //�Y�񭿼�
	float						m_texmaxzoom ;		//tex���̤j��j�Y��
	D3DXVECTOR2					m_texminzoom ;		//tex���̤p��j�Y��

	/////						d3d�ݩ�			///////////
	LPDIRECT3DTEXTURE9			m_texture ;
	LPDIRECT3DTEXTURE9			m_alphatex ;

	//////						��L				///////////
	static float				s_texcoordcenter ; //�K�Ϯy�Ф��ߦ�m
	static float				s_texcoordrightbottom ;//�K�Ϯy�Ъ��k�U�I
	static float				s_worldviewrect ; //���@�ɮy�Шӻ��A�i�����d��
	static float				s_rotation ;		//�����ݩ�
	bool						m_bshare ;			//�O�_����


	D3DXVECTOR4					m_rotated ; 

};
////					��@����Ϊk�G
//							GameInit 

//g_texcon = new CTextureCoordControl ;
//g_texcon->LoadTexture( L"map1.dds" ) ;			//�o��ơG�̭��w�g�]�w�FVertex�e�P���A�٦���l�j�p
//g_texcon->LoadAlphaTexture( L"alphatex.dds" ) ;
//g_texcon->SetVertexSize( 256.0f , 256.0f ) ;		//�Ѽ��H�N��J�A�u�n�j��0�N��F
//g_texcon->SetVertexPos( 200.0f , 200.0f ) ;		//�bWindowSize����]�w�A��������l��m
//g_texcon->SetMaxZoomOut( 0.05f ) ;				//�п�J�p��0.5
//g_texcon->SetZoomOffset( 1.1f ) ;					//�`�N��J�ѼơG�p��1 �M �j��1�C�|�����P����j���

////SetTexUVSize�n���]�w�A�b�]�wSetTexUVPos
//g_texcon->SetTexUVSize( 0.2f , 0.2f ) ; //�п�Jvalue <= 0.5f �C���w��(0.5f , 0.5f)
//g_texcon->SetTexUVPos( 0.0f , 0.0f ) ;//�п�J0.0 <= value <= 1.0f�C
//
//							GameRun
//g_texcon->DrawTex( pd3dDevice ) ;               ��ӤG��@�I
//g_texcon->DrawTexWithAlphaTex( pd3dDevice ) ;


//							GameMessage
//
//g_texcon->MessageHandle( hWnd , uMsg , wParam , lParam ) ;
////




////
//		���O�G�p�a�ϱ���
//		�إߡG2008/1/11
//		�̫��s�G2008/1/21
//		�@�̡Gcosiann
//		��s�@�̡G cosiann
//		�����G�Ъ`�N��o���O�������غc�l�Moperator=
//			  �~�Ӧ�CDXUTControl
////

//�Y�񪬺A
enum ZoomState
{
	ZOOMNULL = 0,	//�L�Y��
	ZOOMIN ,	//��j
	ZOOMOUT		//�Y�p
};

class CMapControl : public CDXUTControl
{
public:
	CMapControl( CDXUTDialog *pDialog = NULL ) ;
	~CMapControl( void ) ;
private:
	CMapControl( const CMapControl& ) ;
	CMapControl& operator = (const CMapControl& ) ;
public:
	//////*****				�����G�e�U		(start)				*****/////////	

	//						���}�����G�]�w���u�㪺�����ݩ�
	void AddUVTranslate(float offsetx , float offsety ) ;
	
	void SetWindowSize( int sx , int sy ) ;			//�]�w�������e�׻P����
	void SetWindowPos( int posx , int posy ) ;		//�]�w��������m
	void SetMaxZoomOut( float value ) ;

	void SetTexUVPos( float u , float v ) ; //�]�w�K�Ϯy�Ц�m
	/////					���}�����G�D�n�\��----�Y��
	void TexZoomInMax( void ) ;						//��j��̤j����
	void TexZoomOutMax( void ) ;					//�Y�p��̤j����
	bool IsMaxZoomIn( void ) const ;				//�P�_�O�_�Y���̤j
	bool IsMinZoomOut( void ) const ;				//�P�_�O�_�Y���̤p
public:
	//////					���}�����G�]�w���u�㪺d3d�t��
	void SetMapFile( LPCWSTR filename ) ;							//Ū���a����
	void SetMapMaskFile( LPCWSTR filename ) ;						//
	void Render( IDirect3DDevice9 *d3d , float fElapsedTime ) ;
	void RenderWithAlphaTex( IDirect3DDevice9 *d3d , float fElapsedTime) ;
private:
	////////				�p������		///////////////
	bool IsInside( float u , float v ) ;
	void TexZoomIn( void ) ;						//��j
	void TexZoomOut( void ) ;						//�Y�p
	void Zoom( void ) ;								//�Y��
	void UpdateRects( ) ;
	void SetMaxZoomIn( float u , float v) ;			//�]�w��j�Y��
public:
	//////					���եΡG�Q�ΰT�����		//////////////
	bool MessageHandle( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) ;

	//////*****				�����G�e�U	(end)					*****/////////	

public:
	////////				���}�����G���u�㪺�\��		///////////////
	void ResetMapAtt( float worldwidth , float worldheight , float x = 0.0f  , float z = 0.0f ) ;
	bool GetWindowPos(float wx, float wz, int &x, int &y) ;
	bool GetWorldPos(float screenX , float screenY , float screenWidth , float screenHieght , float &outWorldX , float &outWorldY) ;
	void SetRoleWorldPos( float x , float z ) ;					//�]�w�H�����ù��y�С]�q�@�ɮy�ФW�^
	void SetZoomState( ZoomState state) ;						//�]�w�Y�񪬺A
	void SetZoomParam( float zoomtime ) ;						//�]�w�Y�񶡮�ɶ��A�H�έ��v
	void GetRoleViewcoord( int &posx , int &posy) const ;		//���o�H���o�ù��y��
	ZoomState GetZoomState( void ) const;									//���o�{�b�Y�񪺪��A
	////////				�p�������G���u�㪺�Բӳ]�w		///////////////
private:
	void Init( void ) ;
	void Release( void ) ;
	void UpdateScale( void ) ;
	void UpdateZoomTime( float fElapsedTime ) ;										//��s�Y��ɶ�
	void Transform( float worldx , float worldz , float &texu , float &texv , float &pixelx , float &pixely ) ;
	bool TexTransformView( float pixelx , float pixely , float &viewx , float &viewy ) ;
	void LimitroleInWorld( float &x , float &z  ) ;									//����H���b�a�Ͻd�򤧤�
	void SetNavigationPos( float worldx , float worldz ) ;
	void UpdateView( void ) ;											//��s���������ݩ�
	void AddRoleWorldPos( float offsetx , float offsety ) ;
private:
	////					Texture's Control				/////////////////////
	CTextureCoordControl		*m_pmap ;
	////					��ܤp�a�ϵ������ݩ�			/////////////////
	D3DXVECTOR2					m_viewposfortex ;			//�����bTexture�y�Ц�m
	D3DXVECTOR2					m_viewposfortexlefttop ;	//���������W����Texture�y�Ц�m
	D3DXVECTOR2					m_viewposfortexrightbottom; //�������k�U����Texture�y�Ц�m
	D3DXVECTOR2					m_viewposforstart ;			//�������_�l��m
	D3DXVECTOR2					m_viewsize;					//������Size
	D3DXVECTOR2					m_viewsizefortexlefttop;	//������Texture�ҹ�����Size(���W)
	D3DXVECTOR2					m_viewsizefortexrightbottom;//������Texture�ҹ�����Size(�k�U)
	D3DXVECTOR2					m_texsize	;				//�����p�a�Ϫ�size(�T�w)
	////					�@�ɻP�p�a�Ϫ����				//////////////
	D3DXVECTOR2					m_worldtomapscale ;
	////					�H�����ݩ�						/////
	D3DXVECTOR2					m_roleposforview ;			//�H���b��������m(view�y��)
	D3DXVECTOR2					m_roleposfortex ;			//�H���b�p�a�Ϫ���m(texture�y��)
	D3DXVECTOR2					m_rolefortex ;				//�H���bTexture������m(pixel�y��)
	D3DXVECTOR2					m_roleworldpos ;			//�H���b�a�ϤW���@�ɮy��
	////					�@�ɮy���ݩ�							///
	D3DXVECTOR2					m_worldsize ;				//�@�ɦa�Ϫ��j�p
	D3DXVECTOR2					m_wroldforstart ;			//�@�ɮy�Ъ��_�l�I
	////					�Y���ݩ�								///
	ZoomState					m_zoomstate ;				//�{�����Y�񪬺A
	float						m_zoomtime ;				//�Y��ɶ�
};



////					   ����Ϊk�G
//							GameInit 

	//g_mapcontrol->SetMapFile( L"map1.dds" ) ; //�o��ơG�̭��w�g�]�w�FVertex�e�P���A�٦���l�j�p
	//g_mapcontrol->LoadRoleTexture( L"role.bmp" ) ;
	//g_mapcontrol->SetMapMaskFile( L"alphatex.dds" ) ;
	//g_mapcontrol->ResetMapAtt( 256.0f , 256.0f) ; //�]�w�M�@�ɪ��`�e�M���C��3.4�Ѽƹw�]�Ȭ�0.0f
	//g_mapcontrol->SetWindowSize( 256.0f , 256.0f ) ; //�ЦbLoadTexture��Ƥ���]�w�C�Ѽ��H�N��J�A�u�n�j��0�N��F
	//g_mapcontrol->SetWindowPos( 200.0f , 200.0f ) ;	//��������l��m
	//g_mapcontrol->SetMaxZoomOut( 0.05f ) ; //�̤j����j�Y��(�п�J�p��0.5)
	//g_mapcontrol->SetZoomParam( 1.5f ) ; //�]�w�Y�񶡮�ɶ��]�p�G�q�̤j�Y�񭿼ƨ�̤p�Y�񭿼ƪ��ɶ��C1.5��ܭn��1.5��C�w�]�Ȭ�1��^�C
	//g_mapcontrol->SetRoleWorldPos( 128.0f , 128.0f ); //�]�w���a�b�@�ɮy�Ъ���m


//							GameUpdate

	//���onpc���@�ɮy��
	//g_npc->GetWorldCoord( worldx , worldz ) ;

	//�p��npc�O�_�b�����d�򤧤�
	//if ( g_mapcontrol->GetWindowPos( worldx , worldz , screenx , screeny ) )
	//{
	//	�����ܳ]�wnpc�b�������y��
	//	g_npc->SetScreenCoord( screenx , screeny ) ;
	//	g_npc->SetDraw( true ) ;
	//}
	//else
	//{
	//	g_npc->SetScreenCoord( -1 , -1 ) ;
	//	g_npc->SetDraw( false ) ;

	//}

//							GameRun

	//					��ӤG��@�I
	//	g_mapcontrol->Render( pd3dDevice , fElapsedTime ) ;
	//g_mapcontrol->RenderWithAlphaTex( pd3dDevice , fElapsedTime) ;

//							GameMessage
//
//g_mapcontrol->MessageHandle( hWnd , uMsg , wParam , lParam ) ;
////
#endif