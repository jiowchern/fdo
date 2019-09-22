#include "stdafx.h"
#include "RenderComponent.h"
#include "RenderComponentTool.h"
#include "CDXUTControl.h"
#include "DrawPlane.h"


RenderComponent::RenderComponent( ElementsQueryer* pElementQueryer )
{
	SetRectEmpty( &m_rcBoundingBox ) ;
	m_pElementQueryer = pElementQueryer ;
	m_pResourceManager = CDXUTDialog::GetManager() ;
	m_bVisible = true ;
}

RenderComponent::~RenderComponent ()
{
	m_pElementQueryer->ClearElement( ( uint )this ) ;
}

void RenderComponent::UpdataBoundingBox()
{
	if( m_pElementQueryer )
	{
		CopyRect( &m_rcBoundingBox , &m_pElementQueryer->GetControlBoundingBox() ) ;
		InitialComponent() ;
	}
}

bool RenderComponent::SetTexturePath( unsigned uTextureNo , const char* pPath )
{
	int iTextureId = m_pResourceManager->AddTexture( string( pPath ) ) ;
	if( iTextureId != -1 )
	{
		if( uTextureNo < m_Elements.size() )
		{
			m_Elements[ uTextureNo ].m_TexturePath = pPath ;
			m_Elements[ uTextureNo ].m_pElement->iTexture = iTextureId ;
			return true ;
		}
		else
		{
			//沒有這個CDXUTElement
			assert( 0 ) ;
		}
	}	
	else
	{
		//建立Resource失敗
		if( uTextureNo < m_Elements.size() )
		{
			m_Elements[ uTextureNo ].m_TexturePath = "" ;
			m_Elements[ uTextureNo ].m_pElement->iTexture = -1 ;
		}
		else
		{
			//沒有這個CDXUTElement
			assert( 0 ) ;
		}
	}
	return false ;
}


bool RenderComponent::SetTextureID( unsigned uTextureNo , int iTextureId )
{
	if( uTextureNo < m_Elements.size() )
	{
		m_Elements[ uTextureNo ].m_pElement->iTexture = iTextureId ;
		return true ;
	}
	else
	{
		//沒有這個CDXUTElement
		m_Elements[ uTextureNo ].m_pElement->iTexture = -1 ;
		assert( 0 ) ;
	}
	return false ;
}

bool RenderComponent::SetTextureRect( unsigned uTextureNo , RECT& rcLocation )
{
	if( uTextureNo < m_Elements.size() )
	{
		CopyRect( &( m_Elements[ uTextureNo ].m_pElement->rcTexture ) , &rcLocation ) ;
		return true ;
	}
	else
	{
		//沒有這個CDXUTElement
		assert( 0 ) ;
	}
	return false ;
}

CDXUTElement* RenderComponent::QueryElement() 
{
	if( m_pElementQueryer )
	{
		CDXUTElement* pElement = m_pElementQueryer->QueryElement( ( uint )this ) ; 
		m_Elements.push_back( sRenderElement( pElement ) ) ;
		return pElement ;
	}
	return 0 ;
}

CDXUTElement* RenderComponent::GetElement( unsigned uElementId )
{
	if( uElementId < m_Elements.size() )
	{
		return m_Elements[ uElementId ].m_pElement ;
	}
	return 0 ;
}

CDXUTControl* RenderComponent::GetControl() 
{ 
	return m_pElementQueryer->GetControl(); 
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RenderJiugongFrame::RenderJiugongFrame( ElementsQueryer* pElementQueryer , unsigned uRenderType /*= Render_Image */ ) : RenderComponent( pElementQueryer )
{
	if( uRenderType == Render_Image )
		m_iComponentType = Component_Jiugong_Frame ;
	else m_iComponentType = Component_Jiugong_ColorFrame ;
	m_uRenderType = uRenderType ;
	m_iOffsetSize = 0 ;
	m_iFrameSize = 1 ;
	m_fCumulativeTime = 0 ;
	m_FrameColor = 0xFF00FF00;
	for( int i = 0 ; i < Frame_Count ; ++i )
	{
		QueryElement() ;
	}
}

void RenderJiugongFrame::SetRenderType( unsigned uRenderType )
{
	m_uRenderType = uRenderType ;
}

void RenderJiugongFrame::SetColor( D3DCOLOR color )
{
	m_FrameColor = color;
}

void RenderJiugongFrame::SetFramesImage( const char* pPath )
{
	for( int i = 0 ; i < Frame_Count ; ++i )
	{
		SetTexturePath( i , pPath ) ;
	}
}

void RenderJiugongFrame::SetFramesRectDefault( RECT& rcLocation )
{
	RECT rcTemp;
	for( int i = 0 ; i < Frame_Count ; ++i )
	{
		if( i == 0 )
		{
			SetRect( &rcTemp, rcLocation.left, rcLocation.top, rcLocation.left+m_iFrameSize, rcLocation.top+m_iFrameSize );
		}
		else if( i == 1 )
		{
			SetRect( &rcTemp, rcLocation.left+m_iFrameSize, rcLocation.top, rcLocation.right-m_iFrameSize, rcLocation.top+m_iFrameSize );
		}
		else if( i == 2 )
		{
			SetRect( &rcTemp, rcLocation.right-m_iFrameSize, rcLocation.top, rcLocation.right, rcLocation.top+m_iFrameSize );
		}
		else if( i == 3 )
		{
			SetRect( &rcTemp, rcLocation.left, rcLocation.top+m_iFrameSize, rcLocation.left+m_iFrameSize, rcLocation.bottom-m_iFrameSize );
		}
		else if( i == 4 )
		{
			SetRect( &rcTemp, rcLocation.left+m_iFrameSize, rcLocation.top+m_iFrameSize, rcLocation.right-m_iFrameSize, rcLocation.bottom-m_iFrameSize );
		}
		else if( i == 5 )
		{
			SetRect( &rcTemp, rcLocation.right-m_iFrameSize, rcLocation.top+m_iFrameSize, rcLocation.right, rcLocation.bottom-m_iFrameSize );
		}
		else if( i == 6 )
		{
			SetRect( &rcTemp, rcLocation.left, rcLocation.bottom-m_iFrameSize, rcLocation.left+m_iFrameSize, rcLocation.bottom );
		}
		else if( i == 7 )
		{
			SetRect( &rcTemp, rcLocation.left+m_iFrameSize, rcLocation.bottom-m_iFrameSize, rcLocation.right-m_iFrameSize, rcLocation.bottom );
		}
		else if( i == 8 )
		{
			SetRect( &rcTemp, rcLocation.right-m_iFrameSize, rcLocation.bottom-m_iFrameSize, rcLocation.right, rcLocation.bottom );
		}

		SetTextureRect( i , rcTemp ) ;
	}
	InitialComponent() ;
}

void RenderJiugongFrame::SetFramesRect( unsigned uTextureNo , RECT& rcLocation , bool bUpdateRect /*=false*/ )
{
	SetTextureRect( uTextureNo , rcLocation ) ;
	if( bUpdateRect )
		InitialComponent() ;
}

void RenderJiugongFrame::SetOffsetSize( int iOffsetSize )
{
	m_iOffsetSize = iOffsetSize ; 
	InitialComponent() ;
}

void RenderJiugongFrame::SetFrameSize( int iFrameSize )
{
	m_iFrameSize = iFrameSize ;
	InitialComponent() ;
}

void RenderJiugongFrame::InitialComponent()
{
	InflateRect( &m_rcBoundingBox , m_iOffsetSize , m_iOffsetSize ) ;
	CreateJiugong( m_rcBoundingBox , m_rcFrames , m_iFrameSize ) ;
}

void RenderJiugongFrame::Render( uint uState , CDXUTDialog* pDialog , float fElapsedTime )
{
	if( m_uRenderType == Render_Image )
	{
		for( int i = 0 ; i < Jiugong_Count ; ++i )
		{
			m_Elements[ i ].m_pElement->TextureColor.Blend( uState , fElapsedTime , 0.8f ) ;
			pDialog->DrawSpriteScale( GetElement( i ) , m_rcFrames[ i ] ) ;
		}
	}
	else if( m_uRenderType == Render_Color )
	{
		for( int i = 0 ; i < Jiugong_Count ; ++i )
		{
			if( i == Jiugong_Center )
				continue ;
			else
			{
				pDialog->DrawRect( &m_rcFrames[ i ] , m_FrameColor ) ;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RenderCenteredImage::RenderCenteredImage( ElementsQueryer* pElementQueryer ) : RenderComponent( pElementQueryer )
{
	m_iComponentType = Component_Centered_Image ;
	QueryElement() ;
}

void RenderCenteredImage::SetImagePath( const char* pImagePath ) 
{
	SetTexturePath( 0 , pImagePath ) ;
	InitialComponent() ;
}

void RenderCenteredImage::InitialComponent() 
{
	CDXUTElement* pElement = GetElement( 0 ) ;
	if( !pElement || pElement->iTexture == -1 )
		return ;

	if( IsRectEmpty( &m_rcBoundingBox ) )
		return ;

	DXUTTextureNode* pTextureNode = m_pResourceManager->GetTextureNode( GetElement( 0 )->iTexture ) ;

	int iCenterX = ( m_rcBoundingBox.right + m_rcBoundingBox.left ) / 2 ;
	int iCenterY = ( m_rcBoundingBox.bottom + m_rcBoundingBox.top ) / 2 ;

	SetRect( &m_rcBoundingBox , iCenterX , iCenterY , iCenterX + 1 , iCenterY + 1 ) ;
	InflateRect( &m_rcBoundingBox , pTextureNode->dwWidth / 2 , pTextureNode->dwHeight / 2 ) ;
}

void RenderCenteredImage::Render( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime ) 
{
	CDXUTElement* pElement = GetElement( 0 ) ;
	if( pElement->iTexture != -1 )
	{
		pElement->TextureColor.Blend( uState , fElapsedTime , 0.8f ) ;
		pDialog->DrawSpriteScale( pElement , m_rcBoundingBox ) ;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RenderCropImage::RenderCropImage( ElementsQueryer* pElementQueryer ) : RenderComponent( pElementQueryer )
{
	m_iComponentType = Component_Crop_Image ;
	QueryElement() ;
}

void RenderCropImage::InitialComponent() 
{	
}

void RenderCropImage::SetImagePath( const char* pImagePath , RECT& rcTexture )
{
	SetTexturePath( 0 , pImagePath ) ;
	SetTextureRect( 0 , rcTexture ) ;
}

void RenderCropImage::Render( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime ) 
{
	CDXUTElement* pElement = GetElement( 0 ) ;
	if( pElement->iTexture != -1 )
	{
		pElement->TextureColor.Blend( uState , fElapsedTime , 0.8f ) ;
		pDialog->DrawSpriteScale( pElement , m_rcBoundingBox ) ;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RenderFlashColor::RenderFlashColor( ElementsQueryer* pElementQueryer ) : RenderComponent( pElementQueryer )
{
	m_iComponentType = Component_Flash_Color ;
	m_fRenderTime = 0.5;
	m_fFlashTime = 0.5 ;
	m_fContinueTime = 0;
	m_fNowTime = 0 ;
	m_fAllTime = 1;
	m_bNowRender = false;
	m_MaxColor = 0xFFFFFFFF;
	m_MinColor = 0x00000000;

	//QueryElement() ;
	
}

void RenderFlashColor::InitialComponent() 
{	
	//UpdataBoundingBox();
}

void RenderFlashColor::SetColor( D3DCOLOR Maxcolor, D3DCOLOR Mincolor )
{
	m_MaxColor = Maxcolor;
	m_MinColor = Mincolor;
}

void RenderFlashColor::SetVisible( bool bVisible )
{
	m_fNowTime = 0 ;	
	m_fAllTime = 0;
	m_bNowRender = bVisible ;
	m_bVisible = bVisible ;
}

void RenderFlashColor::SetFlashTime( float fTime )
{
	m_fFlashTime = fTime ;
}

void RenderFlashColor::SetRenderColorTime( float fTime ) 
{
	m_fRenderTime = fTime;
}

void RenderFlashColor::SetContinueTime( float fTime ) 
{
	m_fContinueTime = fTime;
}

void RenderFlashColor::Render( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime ) 
{
	
	if( m_fFlashTime == 0.0f )
		return ;

	m_fNowTime += fElapsedTime ;
	m_fAllTime += fElapsedTime ;
	if( !m_bNowRender  && m_fNowTime > m_fFlashTime )
	{
		m_bNowRender = true;
		m_fNowTime = 0 ;
	}
	else if( m_bNowRender  && m_fNowTime > m_fRenderTime )
	{
		m_bNowRender = false;
		m_fNowTime = 0 ;
	}

	if( m_fContinueTime > 0 && m_fAllTime > m_fContinueTime )
	{
		// 有時間限制則時間到自動關閉  否則會無限的閃下去
		m_fAllTime = 0;
		m_bVisible = false;
		m_bNowRender = false;
		m_fNowTime = 0 ;
	}

	if( m_bNowRender )
	{
		//flash
		pDialog->DrawFineRect( &GetBoundingBox() , m_MaxColor ) ;
	}
	else if( !m_bNowRender && m_MinColor != 0x00000000 )
	{
		pDialog->DrawFineRect( &GetBoundingBox() , m_MinColor ) ;
	}

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RenderFlashFrame::RenderFlashFrame( ElementsQueryer* pElementQueryer ) : RenderComponent( pElementQueryer )
{
	m_iComponentType = Component_Flash_Color ;
	m_fRenderTime = 0.5;
	m_fFlashTime = 0.5 ;
	m_fContinueTime = 0;
	m_fNowTime = 0 ;
	m_fAllTime = 1;
	m_bNowRender = false;
	m_MaxColor = 0xFFFFFFFF;
	m_MinColor = 0x00000000;
	m_iOffsetSize = 0 ;
	m_iFrameSize = 1 ;
	m_fCumulativeTime = 0 ;
	for( int i = 0 ; i < Frame_Count ; ++i )
	{
		QueryElement() ;
	}
}

void RenderFlashFrame::InitialComponent() 
{
	InflateRect( &m_rcBoundingBox , m_iOffsetSize , m_iOffsetSize ) ;
	CreateJiugong( m_rcBoundingBox , m_rcFrames , m_iFrameSize ) ;
}

void RenderFlashFrame::SetColor( D3DCOLOR Maxcolor, D3DCOLOR Mincolor )
{
	m_MaxColor = Maxcolor;
	m_MinColor = Mincolor;
}

void RenderFlashFrame::SetVisible( bool bVisible )
{
	m_fNowTime = 0 ;	
	m_fAllTime = 0;
	m_bNowRender = bVisible ;	
	m_bVisible = bVisible ;
}

void RenderFlashFrame::SetFlashTime( float fTime )
{
	m_fFlashTime = fTime ;
}

void RenderFlashFrame::SetRenderColorTime( float fTime ) 
{
	m_fRenderTime = fTime;
}

void RenderFlashFrame::SetContinueTime( float fTime ) 
{
	m_fContinueTime = fTime;
}

void RenderFlashFrame::SetOffsetSize( int iOffsetSize )
{
	m_iOffsetSize = iOffsetSize ; 
	InitialComponent() ;
}

void RenderFlashFrame::SetFrameSize( int iFrameSize )
{
	m_iFrameSize = iFrameSize ;
	InitialComponent() ;
}

void RenderFlashFrame::Render( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime ) 
{

	if( m_fFlashTime == 0.0f )
		return ;

	m_fNowTime += fElapsedTime ;
	m_fAllTime += fElapsedTime ;
	if( !m_bNowRender  && m_fNowTime > m_fFlashTime )
	{
		m_bNowRender = true;
		m_fNowTime = 0 ;
	}
	else if( m_bNowRender  && m_fNowTime > m_fRenderTime )
	{
		m_bNowRender = false;
		m_fNowTime = 0 ;
	}

	if( m_fContinueTime > 0 && m_fAllTime > m_fContinueTime )
	{
		// 有時間限制則時間到自動關閉  否則會無限的閃下去
		m_fAllTime = 0;
		m_bVisible = false;
		m_bNowRender = false;
		m_fNowTime = 0 ;
	}

	if( m_bNowRender )
	{
		//flash
		RenderEnabled( uState, pDialog, fElapsedTime );
	}
	else if( !m_bNowRender && m_MinColor != 0x00000000 )
	{
		RenderDisabled( uState, pDialog, fElapsedTime );
	}

}

void RenderFlashFrame::RenderEnabled( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime )
{
	m_fCumulativeTime += fElapsedTime ;
	//if( m_fCumulativeTime > 1.0f )
	{
		for( int i = 0 ; i < Jiugong_Count ; ++i )
		{
			if( i == Jiugong_Center )
				continue ;
			else
			{
				pDialog->DrawRect( &m_rcFrames[ i ] , m_MaxColor ) ;
			}
		}
		m_fCumulativeTime = 0 ;
	}
}

void RenderFlashFrame::RenderDisabled( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime )
{
	m_fCumulativeTime += fElapsedTime ;
	//if( m_fCumulativeTime > 1.0f )
	{
		for( int i = 0 ; i < Jiugong_Count ; ++i )
		{
			if( i == Jiugong_Center )
				continue ;
			else
			{
				pDialog->DrawRect( &m_rcFrames[ i ] , m_MinColor ) ;
			}
		}
		m_fCumulativeTime = 0 ;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RenderRunFrame::RenderRunFrame( ElementsQueryer* pElementQueryer ) : RenderComponent( pElementQueryer )
{
	m_iComponentType = Component_Run_Frame ;
	m_fRunTime    = 0.02f ;
	m_fNowTime    = 0 ;
	m_bNowRender  = false;
	m_bChangRECT  = false;
	m_loop		  = false;
	m_RunStart	  = false;
	m_Color		  = 0xFFFFFFFF;
	m_iOffsetSize = 0 ;
	m_iFrameSize  = 2 ;
	m_rcImages.resize( Image_Count ) ;
	for( int i = 0 ; i < Image_Count ; ++i )
	{
		QueryElement() ;
	}
}

void RenderRunFrame::InitialComponent() 
{
	InflateRect( &m_rcBoundingBox , m_iOffsetSize , m_iOffsetSize ) ;

	sint iLeft   = m_rcBoundingBox.left ;
	sint iTop    = m_rcBoundingBox.top ;
	sint iRight  = m_rcBoundingBox.right ;
	sint iBottom = m_rcBoundingBox.bottom ;

	//LeftTop
	iRight = iLeft + m_iFrameSize ;
	iBottom = iTop + m_iFrameSize ;
	for( int i = Image_Count-1; i >= 0; --i )
	{
		if( i == Image_Count - 1 )
		{
			SetRect( &m_rcImages[ i ] , iLeft , iTop , iRight , iBottom ) ;
		}
		else
		{
			iLeft = iRight;
			iRight = iLeft + m_iFrameSize ;
			SetRect( &m_rcImages[ i ] , iLeft , iTop , iRight , iBottom ) ;
		}
	}
}

void RenderRunFrame::SetColor( D3DCOLOR color )
{
	m_Color = color;
}

void RenderRunFrame::SetLoop( bool loop )
{
	m_loop = loop;
}

void RenderRunFrame::SetVisible( bool bVisible )
{
	m_fNowTime = 0 ;	
	m_bNowRender = bVisible ;	
	m_bVisible = bVisible ;
	InitialComponent();
}

void RenderRunFrame::SetRunTime( float fTime )
{
	m_fRunTime = fTime ;
}

void RenderRunFrame::SetOffsetSize( int iOffsetSize )
{
	m_iOffsetSize = iOffsetSize ; 
	InitialComponent() ;
}

void RenderRunFrame::SetSize( int iSize )
{
	m_iFrameSize = iSize ;
	InitialComponent() ;
}

void RenderRunFrame::Render( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime ) 
{
	if( m_fRunTime == 0.0f )
		return ;

	m_fNowTime += fElapsedTime ;
	if( m_loop )
	{
		m_bNowRender = true;
	}

	if( m_fNowTime > m_fRunTime )
	{
		m_bChangRECT = true;
		m_fNowTime = 0 ;
	}
	else if( m_fNowTime <= m_fRunTime )
	{
		m_bChangRECT = false;
	}

	if( !m_loop && IsRunOnce() )
	{
		// 有時間限制則時間到自動關閉  否則會無限的閃下去
		m_bVisible = false;
		m_bNowRender = false;
		m_fNowTime = 0 ;
	}

	if( m_bNowRender )
	{
		RenderRun( uState, pDialog, fElapsedTime );
		if( m_bChangRECT )
			CreateRunImage() ;
		if( !m_loop )
			m_RunStart = true;
	}

}

void RenderRunFrame::CreateRunImage()
{
	for( int i = Image_Count-1 ; i >= 0 ; --i )
	{
		if( i != 0 )
			m_rcImages[ i ] = m_rcImages[ i-1 ];
		else if( i == 0 )
		{
			SetFirstImageRECT();
		}
	}
}

bool RenderRunFrame::IsRunOnce()
{
	sint iLeft   = m_rcBoundingBox.left ;
	sint iTop    = m_rcBoundingBox.top ;
	sint iRight  = m_rcBoundingBox.right ;
	sint iBottom = m_rcBoundingBox.bottom ;

	sint ILeft   = m_rcImages[Image_Count-1].left ;
	sint ITop    = m_rcImages[Image_Count-1].top ;
	sint IRight  = m_rcImages[Image_Count-1].right ;
	sint IBottom = m_rcImages[Image_Count-1].bottom ;

	//LeftTop
	iRight = iLeft + m_iFrameSize ;
	iBottom = iTop + m_iFrameSize ;

	if( m_RunStart && iLeft == ILeft && iTop == ITop && iRight == IRight && iBottom == IBottom )
		return true;
	else
		return false;


}

void RenderRunFrame::SetFirstImageRECT()
{
	vector< RECT > Rects;
	Rects.resize( 4 ) ;
	sint iLeft   = m_rcBoundingBox.left ;
	sint iTop    = m_rcBoundingBox.top ;
	sint iRight  = m_rcBoundingBox.right ;
	sint iBottom = m_rcBoundingBox.bottom ;
	sint iCenterWidth = RectWidth( m_rcBoundingBox ) - 2 * m_iFrameSize ;
	sint iCenterHeight = RectHeight( m_rcBoundingBox ) - 2 * m_iFrameSize ;

	//LeftTop
	iRight = iLeft + m_iFrameSize ;
	iBottom = iTop + m_iFrameSize ;
	SetRect( &Rects[ Image_First ] , iLeft , iTop , iRight , iBottom ) ;
	
	//Top
	iLeft = iRight ;
	iRight = iLeft + iCenterWidth ;
	
	//RightTop
	iLeft = iRight ;
	iRight = iLeft + m_iFrameSize ;
	SetRect( &Rects[ Image_Second ] , iLeft , iTop , iRight , iBottom ) ;
	
	//Left
	iLeft = m_rcBoundingBox.left ;
	iTop = m_rcBoundingBox.top + m_iFrameSize ; 
	iRight = iLeft + m_iFrameSize ;
	iBottom = iTop + iCenterHeight ;

	//Center
	iLeft = iRight ;
	iRight = iLeft + iCenterWidth ;
	
	//Right
	iLeft = iRight ;
	iRight = iLeft + m_iFrameSize ;
	
	//LeftBottom
	iLeft = m_rcBoundingBox.left ;
	iTop = iBottom ;
	iRight = m_rcBoundingBox.left + m_iFrameSize ;
	iBottom = iTop + m_iFrameSize ;
	SetRect( &Rects[ Image_Fourth ] , iLeft , iTop , iRight , iBottom ) ;
	
	//Bottom
	iLeft = iRight ;
	iRight = iLeft + iCenterWidth ;
	
	//RightBottom
	iLeft = iRight ;
	iRight = iLeft + m_iFrameSize ;
	SetRect( &Rects[ Image_Third ] , iLeft , iTop , iRight , iBottom ) ;

	sint ILeft   = m_rcImages[ Image_First ].left  ;
	sint ITop    = m_rcImages[ Image_First ].top ;
	sint IRight  = m_rcImages[ Image_First ].right ;
	sint IBottom = m_rcImages[ Image_First ].bottom ;
	uint Side = 0;

	iLeft = m_rcBoundingBox.left ;
	iTop    = m_rcBoundingBox.top ;
	iRight  = m_rcBoundingBox.right ;
	iBottom = m_rcBoundingBox.bottom;

	if( ILeft >= iLeft && IRight < iRight && ITop == iTop && IBottom < iBottom )
		Side = 0;
	else if( ILeft > iLeft && IRight == iRight && ITop >= iTop && IBottom < iBottom )
		Side = 1;
	else if( ILeft > iLeft && IRight <= iRight && ITop > iTop && IBottom == iBottom )
		Side = 2;
	else if( ILeft == iLeft && IRight < iRight && ITop > iTop && IBottom <= iBottom )
		Side = 3;

	if( Side == 0 )
	{
		ILeft = ILeft + m_iFrameSize;
		IRight  = IRight + m_iFrameSize;
		SetRect( &m_rcImages[ Image_First ] , ILeft , ITop , IRight , IBottom ) ;
		if( m_rcImages[ Image_First ].right > m_rcBoundingBox.right )
		{
			SetRect( &m_rcImages[ Image_First ] , Rects[ Image_Second ].left , Rects[ Image_Second ].top , Rects[ Image_Second ].right , Rects[ Image_Second ].bottom );
			for( uint i = 1; i < Image_Count; ++i )
			{
				SetRect( &m_rcImages[ i ] , m_rcImages[ i-1 ].left - m_iFrameSize , m_rcImages[ i-1 ].top, 
											m_rcImages[ i-1 ].right - m_iFrameSize , m_rcImages[ i-1 ].bottom );
			}
		}
	}
	else if( Side == 1 )
	{
		ITop = ITop + m_iFrameSize;
		IBottom  = IBottom + m_iFrameSize;
		SetRect( &m_rcImages[ Image_First ] , ILeft , ITop , IRight , IBottom ) ;
		if( m_rcImages[ Image_First ].bottom > m_rcBoundingBox.bottom )
		{
			SetRect( &m_rcImages[ Image_First ] , Rects[ Image_Third ].left , Rects[ Image_Third ].top , Rects[ Image_Third ].right , Rects[ Image_Third ].bottom );
			for( uint i = 1; i < Image_Count; ++i )
			{
				SetRect( &m_rcImages[ i ] , m_rcImages[ i-1 ].left , m_rcImages[ i-1 ].top  - m_iFrameSize , 
											m_rcImages[ i-1 ].right , m_rcImages[ i-1 ].bottom  - m_iFrameSize );
			}
		}
	}
	else if( Side == 2 )
	{
		ILeft = ILeft - m_iFrameSize;
		IRight  = IRight - m_iFrameSize;
		SetRect( &m_rcImages[ Image_First ] , ILeft , ITop , IRight , IBottom ) ;
		if( m_rcImages[ Image_First ].left < m_rcBoundingBox.left )
		{
			SetRect( &m_rcImages[ Image_First ] , Rects[ Image_Fourth ].left , Rects[ Image_Fourth ].top , Rects[ Image_Fourth ].right , Rects[ Image_Fourth ].bottom );
			for( uint i = 1; i < Image_Count; ++i )
			{
				SetRect( &m_rcImages[ i ] , m_rcImages[ i-1 ].left + m_iFrameSize , m_rcImages[ i-1 ].top , 
											m_rcImages[ i-1 ].right + m_iFrameSize , m_rcImages[ i-1 ].bottom );
			}
		}
	}
	else if( Side == 3 )
	{
		ITop = ITop - m_iFrameSize;
		IBottom  = IBottom - m_iFrameSize;
		SetRect( &m_rcImages[ Image_First ] , ILeft , ITop , IRight , IBottom ) ;
		if( m_rcImages[ Image_First ].top < m_rcBoundingBox.top )
		{
			SetRect( &m_rcImages[ Image_First ] , Rects[ Image_First ].left , Rects[ Image_First ].top , Rects[ Image_First ].right , Rects[ Image_First ].bottom );
			for( uint i = 1; i < Image_Count; ++i )
			{
				SetRect( &m_rcImages[ i ] , m_rcImages[ i-1 ].left , m_rcImages[ i-1 ].top  + m_iFrameSize, 
											m_rcImages[ i-1 ].right , m_rcImages[ i-1 ].bottom + m_iFrameSize );
			}
		}
	}
}

void RenderRunFrame::RenderRun( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime )
{
	D3DCOLOR Alpha = 0xFFFFFFFF;
	for(  int i = 0 ; i < Image_Count ; ++i )
	{
//		int mAlpha = 255 * ( ( 10 - i ) / 10 );
		int mAlpha = (255 *  ( 10 - i )) / 10 ;
		Alpha = D3DCOLOR_ARGB( mAlpha, 255, 255, 255 );
		D3DCOLOR Color = m_Color & Alpha;
		pDialog->DrawRect( &m_rcImages[ i ] , Color ) ;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RenderMaskImage::RenderMaskImage( ElementsQueryer* pElementQueryer ) : RenderComponent( pElementQueryer )
{
	m_iComponentType = Component_Crop_Image ;
	m_CutAspet = 0;
	m_uRenderType = 0; 
	m_Color = 0xFFFFFFFF;
	m_MaxValue = 1;
	m_NowValue = 1;

	QueryElement() ;
}

void RenderMaskImage::InitialComponent() 
{	
}

void RenderMaskImage::SetRenderType( unsigned uRenderType )
{
	m_uRenderType = uRenderType ;
}

void RenderMaskImage::SetColor( D3DCOLOR color )
{
	m_Color = color;
}

void RenderMaskImage::SetImagePath( const char* pImagePath , RECT& rcTexture )
{
	SetTexturePath( 0 , pImagePath ) ;
	SetTextureRect( 0 , rcTexture ) ;
}

void RenderMaskImage::SetCutAspet( uint uAspet )
{
	m_CutAspet = uAspet;
}

void RenderMaskImage::SetValue( uint uNowValue, uint uMaxValue )
{
	m_MaxValue = uMaxValue;
	m_NowValue = uNowValue;
	if( m_NowValue > m_MaxValue )
		m_NowValue = m_MaxValue ;

	if( m_NowValue < 0 )
		m_NowValue = 0 ;

	if(m_MaxValue == 0)
		m_MaxValue = m_NowValue + 1;
}

void RenderMaskImage::Render( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime ) 
{
	RECT m_rcDrawRect ;
	CopyRect( &m_rcDrawRect, &m_rcBoundingBox );
	switch( m_CutAspet )
	{
		case CA_UP :
			{
				int iHight = m_rcBoundingBox.bottom - m_rcBoundingBox.top;
				if( iHight > 0 )
				{
					iHight = (iHight * m_NowValue) / m_MaxValue;
					m_rcDrawRect.bottom = (m_rcBoundingBox.top + iHight);
				}
			}
		case CA_DOWN :
			{			
				int iHight = m_rcBoundingBox.bottom - m_rcBoundingBox.top;
				if( iHight > 0 )
				{
					iHight = (iHight * m_NowValue) / m_MaxValue;
					m_rcDrawRect.top = (m_rcBoundingBox.bottom - iHight);	
				}
			}
			break;

		case CA_LEFT :
			{
				int iWidth = m_rcBoundingBox.right - m_rcBoundingBox.left;
				if( iWidth > 0 )
				{
					iWidth = (iWidth * m_NowValue) / m_MaxValue;
					m_rcDrawRect.right = (m_rcBoundingBox.left + iWidth);
				}
			}
			break;

		case CA_RIGHT :
			{
				int iWidth = m_rcBoundingBox.right - m_rcBoundingBox.left;
				if( iWidth > 0 )
				{
					iWidth = (iWidth * m_NowValue) / m_MaxValue;
					m_rcDrawRect.left = (m_rcBoundingBox.right - iWidth);
				}
			}
			break;
	}
		
	if( m_uRenderType == 0 )
	{
		CDXUTElement* pElement = GetElement( 0 ) ;
		if( pElement->iTexture != -1 )
		{
			pElement->TextureColor.Blend( uState , fElapsedTime , 0.8f ) ;
			pDialog->DrawSpriteScale( pElement , m_rcDrawRect ) ;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RenderColdDown::RenderColdDown( ElementsQueryer* pElementQueryer ) : RenderComponent( pElementQueryer )
{
	m_iComponentType = Component_ColdDown ;
	m_fCurTime = 0 ;
	m_fDuration = 0 ;
	m_bPause = true ;
	m_Color = 0xA0000000 ;
	m_fSendInterval = 0 ;
	m_bDrawBlack = false;
}

void RenderColdDown::InitialComponent()
{
	CopyRect( &m_rcBoundingBox , &m_pElementQueryer->GetControlScreenBoundingBox() ) ;
}

void RenderColdDown::Render( unsigned uState , CDXUTDialog* pDialog , float fElapsedTime )
{
	if( m_bPause )
		return ;

	m_fCurTime -= fElapsedTime;
	if( m_fCurTime < 0 )
	{
		m_bPause = true ;
		m_fCurTime = m_fDuration ;
	}

	pDialog->GetManager()->m_pSprite->End();

	DrawTimeSquare( 1.0f - m_fCurTime / m_fDuration , m_rcBoundingBox , m_Color, m_bDrawBlack ) ;

	pDialog->GetManager()->m_pSprite->Begin( D3DXSPRITE_DONOTSAVESTATE ) ;

	//送cd時間的間隔事件
	if( !m_bPause && m_fSendInterval > 0 )
	{
		//m_DelayTime = m_fSendInterval ;
		m_DelayTime -= fElapsedTime; 
		if( m_DelayTime <= 0 )
		{
			CDXUTControl* pCtrl = GetControl();
			wstring str;
			StringFormat16( str,L"\n%d", (int)pCtrl );
			OutputDebugStringW( str.c_str() );
			if( pCtrl == 0 )
			{
				int i = 0;
			}
			pDialog->SendEvent( EVENT_COMPONENT_CD_INTERVAL, true, GetControl(), 0 );
			m_DelayTime = m_fSendInterval ;
		}
	}
}