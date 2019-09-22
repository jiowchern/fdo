#ifndef _MAPCONTROL_H_
#define _MAPCONTROL_H_
#include <d3d9.h>
#include <d3dx9.h>
#include "CDXUTControl.h"

const float MOVEOFFSET = 0.05f ;//移動精準度
const float EPSINON = 0.001f ; //精準度


////
//		類別：Texture座標控制
//		建立：2008/1/9
//		最後更新：2008/1/21
//		作者：cosiann
//		更新作者：cosiann
//		註明：請注意到這類別的靜態變數，以及拷貝建構子和operator=
////


//關於縮放觀念：在頂點寬與高不變時，要對Texure作放大的效果，請降低Texture的座標。
//				反之，要對Texure作縮小的效果，請提高Texture的座標。請注意是否超過貼圖座標範圍。
//				所以，m_texUVpos是隨著人物移動，但是m_texUV's Rect會隨時更新。請看UpdateVertex( )

//想要支援(u,v) > 1.0f的話，請在s_texcoordcenter設定1.0f，則s_texcoordrightbottom會變成2.0f

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
	////////				公開介面：設定此工具的相關屬性		///////////////
	void SetUVTranslate(float x , float y) ;
	void AddUVTranslate(float offsetx , float offsety ) ;
	
	void SetVertexSize( int sx , int sy ) ;			//設定頂點的寬度與高度
	void SetVertexPos( int posx ,  int posy ) ;		//頂點位置
	void SetZoomMultiple( float multiple ) ;			//設定縮放倍數
	void SetMaxZoomOut( float value ) ;
	void SetMaxZoomIn( float worldwidth , float worldheight) ;	

	void SetTexUVSize( float u , float v ) ;//設定貼圖座標寬與高。請輸入value <= s_texcoordcenter 。
	void SetTexUVPos( float u , float v ) ; //設定貼圖座標位置
	static void SetRotation(float rot) ; 
	static float GetRotation(void) ; 
public:
	////////				公開介面：取得此工具的相關屬性		///////////////
	void GetTextureSize( float &texwidth , float &texheight ) const;
	bool IsInsideForShowUV( float u , float v ) ;
	void GetVertexLeftTop( float &left , float &top ) const ;
	void GetVertexSize( float &width , float &height ) const ;
	void GetUVpos( float &u , float &v ) const;	 //取得UV現有的位置
	void GetZoom( float &scalex , float &scaley )const  ;  //取得縮放係數
	void GetUVposRect( float &left , float &top , float &right , float &bottom ) const; //取得UV的矩型範圍
	void ResetZoomValue( float speed , float time ) ;		//重新設定縮放數值
	bool IsMaxZoomIn( void ) const ;				//判斷是否縮放到最大
	bool IsMinZoomOut( void ) const ;				//判斷是否縮放到最小
	bool GetRotUVCoord(float inScaleX , float inScaleY , float &outU , float &outV , float &outCenterX ,float &outCenterY) ;//取得範圍中的某一個UV值
public:
	/////					主要功能：縮放
	void TexZoomIn( void ) ;
	void TexZoomOut( void ) ;
	void TexZoomInMax( void ) ;						//縮小到最大極限
	void TexZoomOutMax( void ) ;					//放大到最大極限

public:
	//////					公開介面：設定此工具的d3d系統
	void SetAlphaTexture(LPDIRECT3DTEXTURE9 tex) ;
	void SetTexture(LPDIRECT3DTEXTURE9 tex) ;
	void LoadTexture( LPCWSTR filename ) ;
	void LoadAlphaTexture( LPCWSTR filename ) ;
	void DrawTex( IDirect3DDevice9 *d3d ) ;
	void DrawTexWithAlphaTex( IDirect3DDevice9 *d3d ) ;
public:
	//////					測試用：利用訊息函數		//////////////
	bool MessageHandle( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) ;
private:
	///////					私有：詳細控制工具的介面		///////////////
	void UpdateTextureAtt( void ) ;
	void UpdateVertex( void ) ;
	void UpdateZoomCenter( float &u , float &v ) ; //更新縮放中心點
	void UpdateUVCoord( float centeru , float centerv , float uwidth , float vheight ) ; //更新貼圖座標的四個點
	void SetVertex( VERTEX2D *vec , float x , float y , float z , float rhw , 
		float u0 , float v0 , float u1 , float v1  ) ;
	bool CheckInside( float &left , float &top , float &right , float &bottom , float min , float max ,
					  float &u , float &v) ;
private:
	void Init( void ) ;
	void Release( void ) ;
private:
	//////////					頂點相關屬性：名稱懶得修正(註)			//////////////
	D3DXVECTOR2 				m_versize ;
	D3DXVECTOR2					m_verpos;
	D3DXVECTOR2					m_verbasepos[4] ;
	VERTEX2D					m_mapvertex[4] ;

	/////////					Texture相關屬性
	//此tex座標的貼圖中心點(依圖的比例大小而決定，為了顯示不失真而設定)
	D3DXVECTOR2					m_texUVcenter ; 
	D3DXVECTOR2 				m_texUVpos ; //tex座標的貼圖位置(中心點一般為0.5,0.5) 記錄玩家的UV位置
	D3DXVECTOR2					m_texUV ;	 //紀錄tex座標的寬跟高
	D3DXVECTOR2					m_texSize;	 //紀錄tex的寬跟高
	D3DXVECTOR2					m_cameraUV;  //紀錄camera的UV
	float						m_texUVleft; //紀錄tex的四點座標
	float						m_texUVtop;
	float						m_texUVright;
	float						m_texUVbottom;



	/////						放大屬性			/////////
	D3DXVECTOR2					m_texzoom ;			//放大的偏移量
	float						m_texzoommultiple;  //縮放倍數
	float						m_texmaxzoom ;		//tex的最大放大係數
	D3DXVECTOR2					m_texminzoom ;		//tex的最小放大係數

	/////						d3d屬性			///////////
	LPDIRECT3DTEXTURE9			m_texture ;
	LPDIRECT3DTEXTURE9			m_alphatex ;

	//////						其他				///////////
	static float				s_texcoordcenter ; //貼圖座標中心位置
	static float				s_texcoordrightbottom ;//貼圖座標的右下點
	static float				s_worldviewrect ; //對於世界座標來說，可視的範圍
	static float				s_rotation ;		//旋轉屬性
	bool						m_bshare ;			//是否分享


	D3DXVECTOR4					m_rotated ; 

};
////					單一物件用法：
//							GameInit 

//g_texcon = new CTextureCoordControl ;
//g_texcon->LoadTexture( L"map1.dds" ) ;			//這函數：裡面已經設定了Vertex寬與高，還有原始大小
//g_texcon->LoadAlphaTexture( L"alphatex.dds" ) ;
//g_texcon->SetVertexSize( 256.0f , 256.0f ) ;		//參數隨意輸入，只要大於0就行了
//g_texcon->SetVertexPos( 200.0f , 200.0f ) ;		//在WindowSize之後設定，視窗的初始位置
//g_texcon->SetMaxZoomOut( 0.05f ) ;				//請輸入小於0.5
//g_texcon->SetZoomOffset( 1.1f ) ;					//注意輸入參數：小於1 和 大於1。會有不同的放大比例

////SetTexUVSize要先設定，在設定SetTexUVPos
//g_texcon->SetTexUVSize( 0.2f , 0.2f ) ; //請輸入value <= 0.5f 。內定為(0.5f , 0.5f)
//g_texcon->SetTexUVPos( 0.0f , 0.0f ) ;//請輸入0.0 <= value <= 1.0f。
//
//							GameRun
//g_texcon->DrawTex( pd3dDevice ) ;               兩個二選一！
//g_texcon->DrawTexWithAlphaTex( pd3dDevice ) ;


//							GameMessage
//
//g_texcon->MessageHandle( hWnd , uMsg , wParam , lParam ) ;
////




////
//		類別：小地圖控制
//		建立：2008/1/11
//		最後更新：2008/1/21
//		作者：cosiann
//		更新作者： cosiann
//		註明：請注意到這類別的拷貝建構子和operator=
//			  繼承自CDXUTControl
////

//縮放狀態
enum ZoomState
{
	ZOOMNULL = 0,	//無縮放
	ZOOMIN ,	//放大
	ZOOMOUT		//縮小
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
	//////*****				介面：委託		(start)				*****/////////	

	//						公開介面：設定此工具的相關屬性
	void AddUVTranslate(float offsetx , float offsety ) ;
	
	void SetWindowSize( int sx , int sy ) ;			//設定視窗的寬度與高度
	void SetWindowPos( int posx , int posy ) ;		//設定視窗的位置
	void SetMaxZoomOut( float value ) ;

	void SetTexUVPos( float u , float v ) ; //設定貼圖座標位置
	/////					公開介面：主要功能----縮放
	void TexZoomInMax( void ) ;						//放大到最大極限
	void TexZoomOutMax( void ) ;					//縮小到最大極限
	bool IsMaxZoomIn( void ) const ;				//判斷是否縮放到最大
	bool IsMinZoomOut( void ) const ;				//判斷是否縮放到最小
public:
	//////					公開介面：設定此工具的d3d系統
	void SetMapFile( LPCWSTR filename ) ;							//讀取地圖檔
	void SetMapMaskFile( LPCWSTR filename ) ;						//
	void Render( IDirect3DDevice9 *d3d , float fElapsedTime ) ;
	void RenderWithAlphaTex( IDirect3DDevice9 *d3d , float fElapsedTime) ;
private:
	////////				私有介面		///////////////
	bool IsInside( float u , float v ) ;
	void TexZoomIn( void ) ;						//放大
	void TexZoomOut( void ) ;						//縮小
	void Zoom( void ) ;								//縮放
	void UpdateRects( ) ;
	void SetMaxZoomIn( float u , float v) ;			//設定放大係數
public:
	//////					測試用：利用訊息函數		//////////////
	bool MessageHandle( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) ;

	//////*****				介面：委託	(end)					*****/////////	

public:
	////////				公開介面：此工具的功能		///////////////
	void ResetMapAtt( float worldwidth , float worldheight , float x = 0.0f  , float z = 0.0f ) ;
	bool GetWindowPos(float wx, float wz, int &x, int &y) ;
	bool GetWorldPos(float screenX , float screenY , float screenWidth , float screenHieght , float &outWorldX , float &outWorldY) ;
	void SetRoleWorldPos( float x , float z ) ;					//設定人物的螢幕座標（從世界座標上）
	void SetZoomState( ZoomState state) ;						//設定縮放狀態
	void SetZoomParam( float zoomtime ) ;						//設定縮放間格時間，以及倍率
	void GetRoleViewcoord( int &posx , int &posy) const ;		//取得人物得螢幕座標
	ZoomState GetZoomState( void ) const;									//取得現在縮放的狀態
	////////				私有介面：此工具的詳細設定		///////////////
private:
	void Init( void ) ;
	void Release( void ) ;
	void UpdateScale( void ) ;
	void UpdateZoomTime( float fElapsedTime ) ;										//更新縮放時間
	void Transform( float worldx , float worldz , float &texu , float &texv , float &pixelx , float &pixely ) ;
	bool TexTransformView( float pixelx , float pixely , float &viewx , float &viewy ) ;
	void LimitroleInWorld( float &x , float &z  ) ;									//限制人物在地圖範圍之內
	void SetNavigationPos( float worldx , float worldz ) ;
	void UpdateView( void ) ;											//更新視窗相關屬性
	void AddRoleWorldPos( float offsetx , float offsety ) ;
private:
	////					Texture's Control				/////////////////////
	CTextureCoordControl		*m_pmap ;
	////					顯示小地圖視窗的屬性			/////////////////
	D3DXVECTOR2					m_viewposfortex ;			//視窗在Texture座標位置
	D3DXVECTOR2					m_viewposfortexlefttop ;	//視窗的左上角的Texture座標位置
	D3DXVECTOR2					m_viewposfortexrightbottom; //視窗的右下角的Texture座標位置
	D3DXVECTOR2					m_viewposforstart ;			//視窗的起始位置
	D3DXVECTOR2					m_viewsize;					//視窗的Size
	D3DXVECTOR2					m_viewsizefortexlefttop;	//視窗對Texture所對應的Size(左上)
	D3DXVECTOR2					m_viewsizefortexrightbottom;//視窗對Texture所對應的Size(右下)
	D3DXVECTOR2					m_texsize	;				//紀錄小地圖的size(固定)
	////					世界與小地圖的比例				//////////////
	D3DXVECTOR2					m_worldtomapscale ;
	////					人物的屬性						/////
	D3DXVECTOR2					m_roleposforview ;			//人物在視窗的位置(view座標)
	D3DXVECTOR2					m_roleposfortex ;			//人物在小地圖的位置(texture座標)
	D3DXVECTOR2					m_rolefortex ;				//人物在Texture中的位置(pixel座標)
	D3DXVECTOR2					m_roleworldpos ;			//人物在地圖上的世界座標
	////					世界座標屬性							///
	D3DXVECTOR2					m_worldsize ;				//世界地圖的大小
	D3DXVECTOR2					m_wroldforstart ;			//世界座標的起始點
	////					縮放屬性								///
	ZoomState					m_zoomstate ;				//現有的縮放狀態
	float						m_zoomtime ;				//縮放時間
};



////					   物件用法：
//							GameInit 

	//g_mapcontrol->SetMapFile( L"map1.dds" ) ; //這函數：裡面已經設定了Vertex寬與高，還有原始大小
	//g_mapcontrol->LoadRoleTexture( L"role.bmp" ) ;
	//g_mapcontrol->SetMapMaskFile( L"alphatex.dds" ) ;
	//g_mapcontrol->ResetMapAtt( 256.0f , 256.0f) ; //設定和世界的總寬和高。第3.4參數預設值為0.0f
	//g_mapcontrol->SetWindowSize( 256.0f , 256.0f ) ; //請在LoadTexture函數之後設定。參數隨意輸入，只要大於0就行了
	//g_mapcontrol->SetWindowPos( 200.0f , 200.0f ) ;	//視窗的初始位置
	//g_mapcontrol->SetMaxZoomOut( 0.05f ) ; //最大的放大係數(請輸入小於0.5)
	//g_mapcontrol->SetZoomParam( 1.5f ) ; //設定縮放間格時間（如：從最大縮放倍數到最小縮放倍數的時間。1.5表示要花1.5秒。預設值為1秒）。
	//g_mapcontrol->SetRoleWorldPos( 128.0f , 128.0f ); //設定玩家在世界座標的位置


//							GameUpdate

	//取得npc的世界座標
	//g_npc->GetWorldCoord( worldx , worldz ) ;

	//計算npc是否在視窗範圍之內
	//if ( g_mapcontrol->GetWindowPos( worldx , worldz , screenx , screeny ) )
	//{
	//	有的話設定npc在視窗的座標
	//	g_npc->SetScreenCoord( screenx , screeny ) ;
	//	g_npc->SetDraw( true ) ;
	//}
	//else
	//{
	//	g_npc->SetScreenCoord( -1 , -1 ) ;
	//	g_npc->SetDraw( false ) ;

	//}

//							GameRun

	//					兩個二選一！
	//	g_mapcontrol->Render( pd3dDevice , fElapsedTime ) ;
	//g_mapcontrol->RenderWithAlphaTex( pd3dDevice , fElapsedTime) ;

//							GameMessage
//
//g_mapcontrol->MessageHandle( hWnd , uMsg , wParam , lParam ) ;
////
#endif