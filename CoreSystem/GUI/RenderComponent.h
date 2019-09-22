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
	void		 UpdataBoundingBox() ;															//��sControl,Element����m
	bool         SetTexturePath( unsigned uTextureNo , const char* pPath ) ;					//�]�wTexture���ɮ׸��|
	bool	     SetTextureID( unsigned uTextureNo , int iTextureId ) ;							//�]�w�K�Ϯy��
	bool         SetTextureRect( unsigned uTextureNo , RECT& rcLocation ) ;						//�]�wElement,Texture�귽���K�Ϯy��
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
	
	bool m_bVisible ;										//�O�_�n���
	unsigned m_iComponentType ;								//ø�s����Type
	RECT m_rcBoundingBox ;									//ø�s�d��
	CDXUTDialogResourceManager* m_pResourceManager ;		//ResourceManager

	CDXUTElement* QueryElement() ; 							//�إ�CDXUTElement
} ;

//�E�c�覡�K�Ϫ���
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

//�m�󤤤ߪ��K��
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
	//	�{�{������
	void SetColor( D3DCOLOR Maxcolor, D3DCOLOR Mincolor = 0x00000000 ) ;
	void SetFlashTime( float fTime ) ;			//	���Ҧ�����j�ɶ�
	void SetRenderColorTime( float fTime ) ;	//	����ø�s����ɶ�
	void SetContinueTime( float fTime ) ;		//	�������_�{�{���ɶ�
	void Render( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime ) ;

protected :
	float m_fContinueTime;		//	�������_�{�{���ɶ�
	float m_fRenderTime ;		//	����ø�s����ɶ�
	float m_fFlashTime ;		//	���Ҧ�����j�ɶ�
	float m_fNowTime ;			//	�{�b���ɶ�
	float m_fAllTime ;			//	�`�ɶ�
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
	//	�{�{������
	void SetColor( D3DCOLOR Maxcolor, D3DCOLOR Mincolor = 0x00000000 ) ;
	void SetFlashTime( float fTime ) ;			//	���Ҧ�����j�ɶ�
	void SetRenderColorTime( float fTime ) ;	//	����ø�s����ɶ�
	void SetContinueTime( float fTime ) ;		//	�������_�{�{���ɶ�
	void SetOffsetSize( int iOffsetSize ) ;
	void SetFrameSize( int iFrameSize ) ;
	void Render( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime ) ;

protected :
	float m_fContinueTime;		//	�������_�{�{���ɶ�
	float m_fRenderTime ;		//	����ø�s����ɶ�
	float m_fFlashTime ;		//	���Ҧ�����j�ɶ�
	float m_fNowTime ;			//	�{�b���ɶ�
	float m_fAllTime ;			//	�`�ɶ�
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
	//	�{�{������
	void SetColor( D3DCOLOR color ) ;
	void SetLoop( bool loop );					//  �]�w�O�_�`��(�w�]���_)
	void SetRunTime( float fTime ) ;			//	���ʦ�����j�ɶ�
	void SetOffsetSize( int iOffsetSize ) ;
	void SetSize( int iSize ) ;
	void Render( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime ) ;

protected :
	float m_fRunTime ;			//	���ʦ�����j�ɶ�
	float m_fNowTime ;			//	�{�b���ɶ�
	bool  m_bNowRender ; 
	bool  m_bChangRECT ; 
	bool  m_loop ;				//  �`��
	bool  m_RunStart;			//  �}�l����

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
