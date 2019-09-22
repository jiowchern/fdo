#ifndef _RenderComponent_H_
#define _RenderComponent_H_

class CDXUTControl ;
class ElementsQueryer ;
class CDXUTDialogResourceManager ;

enum eRenderComponentType
{
	Component_Jiugong_Frame ,
	Component_Jiugong_ColorFrame ,
	Component_Centered_Image ,
	Component_Crop_Image ,
	Component_Flash_Color ,
	Component_Flash_Frame,
	Component_Mask_Image,
	Component_ColdDown ,
	Component_Run_Frame ,
	Component_Type_Count ,
} ;

class RenderComponent 
{
public :

	struct sRenderElement
	{
		string        m_TexturePath ;
		CDXUTElement* m_pElement ;
		sRenderElement() : m_pElement( 0 ) {} 
		sRenderElement( CDXUTElement* pElement ) : m_pElement( pElement ) {}
	} ;

	RenderComponent( ElementsQueryer* pElementQueryer ) ;
	virtual ~RenderComponent () ;
	virtual void InitialComponent() {} 
	void		 UpdataBoundingBox() ;															//更新Control,Element的位置
	bool         SetTexturePath( unsigned uTextureNo , const char* pPath ) ;					//設定Texture的檔案路徑
	bool	     SetTextureID( unsigned uTextureNo , int iTextureId ) ;							//設定貼圖座標
	bool         SetTextureRect( unsigned uTextureNo , RECT& rcLocation ) ;						//設定Element,Texture資源的貼圖座標
	virtual void Render( uint uState , IDirect3DDevice9* pd3dDevice , float fElapsedTime ) {} 
	virtual void Render( uint uState , CDXUTDialog* pDialog , float fElapsedTime ) {} 
	inline  void SetVisible( bool bVisible ) { m_bVisible = bVisible ; } 
	inline  bool GetVisible() { return m_bVisible ; }
	CDXUTElement* GetElement( unsigned uElementId ) ;
	inline RECT  GetBoundingBox() { return m_rcBoundingBox ; }
	CDXUTControl* GetControl();

protected :


	ElementsQueryer* m_pElementQueryer ;
	typedef vector< sRenderElement > ElementVector ;
	ElementVector m_Elements ;
	
	bool m_bVisible ;										//是否要顯示
	unsigned m_iComponentType ;								//繪製元件Type
	RECT m_rcBoundingBox ;									//繪製範圍
	CDXUTDialogResourceManager* m_pResourceManager ;		//ResourceManager

	CDXUTElement* QueryElement() ; 							//建立CDXUTElement
} ;

//九宮方式貼圖的框
class RenderJiugongFrame : public RenderComponent
{
public :
	
	enum eFrame
	{
		Frame_LeftTop ,
		Frame_Top ,
		Frame_RightTop ,
		Frame_Left ,
		Frame_Center ,
		Frame_Right ,
		Frame_LeftBottom ,
		Frame_Bottom ,
		Frame_RightBottom ,
		Frame_Count ,
	} ;

	enum eRenderType
	{
		Render_Image ,
		Render_Color ,
	} ;

	RenderJiugongFrame( ElementsQueryer* pElementQueryer , unsigned uRenderType = Render_Image ) ;
	~RenderJiugongFrame() {}
	void SetRenderType( unsigned uRenderType );
	void SetColor( D3DCOLOR color ) ;
	void SetFramesImage( const char* pPath ) ;
	void SetFramesRectDefault( RECT& rcLocation );
	void SetFramesRect( unsigned uTextureNo , RECT& rcLocation , bool bUpdateRect = false ) ;	
	void SetOffsetSize( int iOffsetSize ) ;
	void SetFrameSize( int iFrameSize ) ;
	void InitialComponent() ;
	void Render( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime ) ;

protected :

	float m_fCumulativeTime ;
	unsigned m_uRenderType ;
	int m_iOffsetSize ;
	int m_iFrameSize ;
	D3DCOLOR m_FrameColor;
	vector< RECT > m_rcFrames ;


} ;

//置於中心的貼圖
class RenderCenteredImage : public RenderComponent
{
public :
	
	RenderCenteredImage( ElementsQueryer* pElementQueryer ) ;
	~RenderCenteredImage() {}

	void SetImagePath( const char* pImagePath ) ;
	void InitialComponent() ;
	void Render( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime ) ;

} ;

class RenderCropImage : public RenderComponent
{
public :
	
	RenderCropImage( ElementsQueryer* pElementQueryer ) ;
	~RenderCropImage() {}

	void SetImagePath( const char* pImagePath , RECT& rcTexture ) ;
	void InitialComponent() ;
	void Render( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime ) ;

} ;

class RenderFlashColor : public RenderComponent
{
public :
	
	RenderFlashColor( ElementsQueryer* pElementQueryer ) ;
	~RenderFlashColor() {}

	void InitialComponent() ;
	void SetVisible( bool bVisible ) ;
	//	閃爍色塊控制項
	void SetColor( D3DCOLOR Maxcolor, D3DCOLOR Mincolor = 0x00000000 ) ;
	void SetFlashTime( float fTime ) ;			//	重啟色塊間隔時間
	void SetRenderColorTime( float fTime ) ;	//	持續繪製色塊時間
	void SetContinueTime( float fTime ) ;		//	維持不斷閃爍的時間
	void Render( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime ) ;

protected :
	float m_fContinueTime;		//	維持不斷閃爍的時間
	float m_fRenderTime ;		//	持續繪製色塊時間
	float m_fFlashTime ;		//	重啟色塊間隔時間
	float m_fNowTime ;			//	現在的時間
	float m_fAllTime ;			//	總時間
	bool  m_bNowRender ; 

	D3DCOLOR m_MaxColor;
	D3DCOLOR m_MinColor;
} ;

class RenderFlashFrame : public RenderComponent
{
public :
	
	enum eFrame
	{
		Frame_LeftTop ,
		Frame_Top ,
		Frame_RightTop ,
		Frame_Left ,
		Frame_Center ,
		Frame_Right ,
		Frame_LeftBottom ,
		Frame_Bottom ,
		Frame_RightBottom ,
		Frame_Count ,
	} ;
	
	RenderFlashFrame( ElementsQueryer* pElementQueryer ) ;
	~RenderFlashFrame() {}

	void InitialComponent() ;
	void SetVisible( bool bVisible ) ;
	//	閃爍色塊控制項
	void SetColor( D3DCOLOR Maxcolor, D3DCOLOR Mincolor = 0x00000000 ) ;
	void SetFlashTime( float fTime ) ;			//	重啟色塊間隔時間
	void SetRenderColorTime( float fTime ) ;	//	持續繪製色塊時間
	void SetContinueTime( float fTime ) ;		//	維持不斷閃爍的時間
	void SetOffsetSize( int iOffsetSize ) ;
	void SetFrameSize( int iFrameSize ) ;
	void Render( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime ) ;

protected :
	float m_fContinueTime;		//	維持不斷閃爍的時間
	float m_fRenderTime ;		//	持續繪製色塊時間
	float m_fFlashTime ;		//	重啟色塊間隔時間
	float m_fNowTime ;			//	現在的時間
	float m_fAllTime ;			//	總時間
	bool  m_bNowRender ; 

	D3DCOLOR m_MaxColor;
	D3DCOLOR m_MinColor;
	int m_iOffsetSize ;
	int m_iFrameSize ;
	float m_fCumulativeTime ;
	vector< RECT > m_rcFrames ;

	void RenderEnabled( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime );
	void RenderDisabled( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime );
} ;

class RenderRunFrame : public RenderComponent
{
public :
	
	enum eFrame
	{
		Image_First ,
		Image_Second ,
		Image_Third ,
		Image_Fourth ,
		Image_Fifth ,
		Image_Sixth ,
		Image_Count ,
	} ;
	
	RenderRunFrame( ElementsQueryer* pElementQueryer ) ;
	~RenderRunFrame() {}

	void InitialComponent() ;
	void SetVisible( bool bVisible ) ;
	//	閃爍色塊控制項
	void SetColor( D3DCOLOR color ) ;
	void SetLoop( bool loop );					//  設定是否循環(預設為否)
	void SetRunTime( float fTime ) ;			//	移動色塊間隔時間
	void SetOffsetSize( int iOffsetSize ) ;
	void SetSize( int iSize ) ;
	void Render( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime ) ;

protected :
	float m_fRunTime ;			//	移動色塊間隔時間
	float m_fNowTime ;			//	現在的時間
	bool  m_bNowRender ; 
	bool  m_bChangRECT ; 
	bool  m_loop ;				//  循環
	bool  m_RunStart;			//  開始移動

	D3DCOLOR m_Color;
	int m_iOffsetSize ;
	int m_iFrameSize ;
	float m_fCumulativeTime ;
	vector< RECT > m_rcImages ;

	void RenderRun( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime );

private:
	void SetFirstImageRECT();
	void CreateRunImage();
	bool IsRunOnce();
} ;

class RenderMaskImage : public RenderComponent
{
public :
	enum eCutAspet
	{
		CA_UP,
		CA_DOWN,
		CA_LEFT,
		CA_RIGHT,
		CA_RCYCLE,
		CA_LCYCLE,
	};

	enum eRenderType
	{
		Render_Image ,
		Render_Color ,
	} ;
	
	RenderMaskImage( ElementsQueryer* pElementQueryer ) ;
	~RenderMaskImage() {}

	void SetImagePath( const char* pImagePath , RECT& rcTexture ) ;
	void InitialComponent() ;
	void SetRenderType( unsigned uRenderType );
	void SetColor( D3DCOLOR color ) ;
	void SetCutAspet( unsigned uAspet );
	void SetValue( unsigned uNowValue, unsigned uMaxValue );
	void Render( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime ) ;

protected :
	unsigned m_CutAspet;
	unsigned m_uRenderType ;
	D3DCOLOR m_Color;
	unsigned m_MaxValue;
	unsigned m_NowValue;

} ;

//ColdDown
class RenderColdDown : public RenderComponent
{
public :
	
	RenderColdDown( ElementsQueryer* pElementQueryer ) ;
	~RenderColdDown() {}

	void InitialComponent() ;
	inline void Pause( bool bPause ) { m_bPause = bPause ; }
	inline void SetCurTime( float fCurTime ) { m_fCurTime = fCurTime ; }
	inline void SetDuration( float fDuration ) { m_fCurTime = m_fDuration = fDuration ; }
	inline void SetColor( D3DCOLOR& Color ) { m_Color = Color ; } 
	inline float GetRemainder(){ return m_fCurTime; }
	inline float GetTotalTime(){ return m_fDuration; }
	inline void SetIntervalTime( float fTime) {m_DelayTime = m_fSendInterval = fTime ;}
	void Render( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime ) ;
	void SetRrawBlack( bool bRrawBlack = false )	{ m_bDrawBlack = true; }

protected :

	D3DCOLOR m_Color ;
	bool  m_bPause ;
	float m_fDuration ;
	float m_fCurTime ;
	float m_fSendInterval;
	float m_DelayTime;
	bool  m_bDrawBlack;

} ;



#endif
