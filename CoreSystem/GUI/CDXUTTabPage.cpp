#include "stdafx.h"
#include "CDXUTControl.h"
#include "CDXUTButton.h"
#include "CDXUTTabPage.h"

CDXUTTabPage::CDXUTTabPage( CDXUTDialog *pDialog ) 
{
	m_Type = DXUT_CONTROL_TABPAGE ;
	m_ID = 0 ;
	m_PageStrVector.resize( 0 ) ;
	m_CheckButtonVector.resize( 0 ) ;
	m_BeginIndex = 0 ;
	m_SelectIndex = 0 ;
	m_iLastSelectedIndex = -1 ;
	m_Stytle = BUTTON ;
	m_ButtonStytle = AUTO_HIDE ;
	m_pDialog = pDialog ;
	m_LeftButton = NULL ;
	m_RightButton = NULL ;
	SetOffSetSize( 1 ) ;
	SetButtonOffsetSize( 0 ) ;
	SetTabSize( 30 , 10 ) ;
	m_PageSize = 3 ;
	SetButtonSize(	20 , 19 ) ;

	if( m_pDialog != NULL )
	{
		m_pDialog->InitControl( this ) ;
		if( m_Stytle == BUTTON )
		{
			m_pDialog->SetListener( ( int )&*this , EVENT_CONTROL_MOUSE_OUT , this , &CDXUTTabPage::MouseOut , 0 ) ;
			
			m_LeftButton = new CDXUTButton( pDialog ) ;
			m_RightButton = new CDXUTButton( pDialog ) ;
			m_LeftButton->SetID( ( int )&*m_LeftButton ) ;
			m_RightButton->SetID( ( int )&*m_RightButton ) ;
			m_LeftButton->SetHint( L"上一頁" ) ;
			m_RightButton->SetHint( L"下一頁" ) ;

			m_pDialog->SetListener( (int)&*m_LeftButton , EVENT_BUTTON_CLICKED , this , &CDXUTTabPage::LeftButtonClk , 0 ) ;
			m_pDialog->SetListener( (int)&*m_RightButton , EVENT_BUTTON_CLICKED , this , &CDXUTTabPage::RightButtonClk , 0 ) ;

			for( unsigned i = 0 ; i < m_PageSize ; ++i )
			{
				m_CheckButtonVector.push_back( new CDXUTCheckBox( m_pDialog ) ) ;
				m_CheckButtonVector[ i ]->SetStyle( CDXUTCheckBox::STYLE_BUTTON ) ;
				m_CheckButtonVector[ i ]->SetElement( 0 , GetElement( 4 ) ) ;
				m_CheckButtonVector[ i ]->SetElement( 1 , GetElement( 5 ) ) ;
				m_CheckButtonVector[ i ]->SetID( (int)&*m_CheckButtonVector[ i ] ) ;
				m_pDialog->SetListener( (int)&*m_CheckButtonVector[ i ] , EVENT_CHECKBOX_CHANGED , this , &CDXUTTabPage::CheckButtonClk , i ) ;
			}
			//SetPageNormalElement() ;
			//SetPageFocusElement() ;
			Refreshform() ;
			if( m_BeginIndex == 0 && !m_PageStrVector.empty() )
			{
				m_CheckButtonVector[ 0 ]->SetChecked( true ) ;
				m_pDialog->RequestFocus( m_CheckButtonVector[ 0 ] ) ;
				m_LeftButton->SetVisible( false ) ;
				m_LeftButton->OnMouseLeave() ;
			}
		}
		else
		{
			//NO_BUTTON
		}
	}
}

CDXUTTabPage::~CDXUTTabPage()
{
	m_pDialog->RiseListener( (int)&*m_LeftButton , EVENT_CHECKBOX_CHANGED ) ;
	m_pDialog->RiseListener( (int)&*m_RightButton , EVENT_CHECKBOX_CHANGED ) ;
	SAFE_DELETE( m_LeftButton ) ;
	SAFE_DELETE( m_RightButton ) ;
	for( size_t i = 0 ; i < m_CheckButtonVector.size() ; ++i )
	{
		m_pDialog->RiseListener( (int)&*m_CheckButtonVector[ i ] , EVENT_CHECKBOX_CHANGED ) ;
		SAFE_DELETE( m_CheckButtonVector[ i ] ) ;
	}
}

void CDXUTTabPage::Refreshform()
{
	CDXUTElement *NormalElement , *FocusElement ;

	int width = 0 ;

	if( m_Stytle == BUTTON )
	{
		if( m_LeftButton )
		{
			NormalElement = GetElement( 0 ) ;
			FocusElement  = GetElement( 1 ) ;
			m_LeftButton->SetElement( 0 , NormalElement ) ;
			m_LeftButton->SetElement( 1 , FocusElement ) ;
			m_LeftButton->SetLocation( m_x , m_y ) ;
			m_LeftButton->SetSize( m_ButtonWidth , m_ButtonHeight ) ;
			width += m_x + m_ButtonWidth + m_ButtonOffSetSize ;
			if( m_PageStrVector.size() <= m_CheckButtonVector.size() )
				m_LeftButton->SetVisible( false ) ;
			else
				m_LeftButton->SetVisible( true ) ;
		}
		if( m_BeginIndex == 0 && !m_PageStrVector.empty() )
		{
			for( size_t i =0 ; i < m_CheckButtonVector.size(); ++i )
				m_CheckButtonVector[ i ]->SetChecked( false ) ;

			m_CheckButtonVector[ 0 ]->SetChecked( true ) ;
			m_pDialog->RequestFocus( m_CheckButtonVector[ 0 ] ) ;
			m_pDialog->SendEvent( EVENT_TABPAGE_SELECTION , true , this ) ;
		}
		if( m_BeginIndex == 0 && m_LeftButton != NULL )
		{
			m_LeftButton->SetVisible( false ) ;
			m_LeftButton->OnMouseLeave() ;
		}
		unsigned j = m_BeginIndex ;
		if( !m_PageStrVector.empty() )
		{
			NormalElement = GetElement( 4 ) ;
			FocusElement  = GetElement( 5 ) ;
			for( unsigned i = 0 ; i < m_PageSize ; ++i )
			{
				if( i > ( unsigned )m_CheckButtonVector.size() - 1 )
					width += m_TabWidth ;
				else
				{
					m_CheckButtonVector[ i ]->SetElement( 0 , NormalElement ) ;
					m_CheckButtonVector[ i ]->SetElement( 1 , FocusElement ) ;
					
					m_CheckButtonVector[ i ]->SetLocation( width , m_y ) ;
					m_CheckButtonVector[ i ]->SetSize( m_TabWidth , m_TabHeight ) ;
					width += m_TabWidth ;

					if( j < ( unsigned )m_PageStrVector.size()  )
					{
						m_CheckButtonVector[ i ]->SetVisible( true ) ;
						m_CheckButtonVector[ i ]->SetText( m_PageStrVector[ j ].c_str() ) ; 
					}
					else
					{
						m_CheckButtonVector[ i ]->SetVisible( false ) ;
					}
					j++ ;
				}
			}
		}
		else
		{
			for( size_t i = 0 ; i < m_CheckButtonVector.size() ; ++i )
				m_CheckButtonVector[ i ]->SetVisible( false ) ;
		}
		if( m_RightButton )
		{	
			if( m_PageStrVector.size() <= m_CheckButtonVector.size() )
				m_RightButton->SetVisible( false ) ;
			else
				m_RightButton->SetVisible( true ) ;
			NormalElement = GetElement( 2 ) ;
			FocusElement  = GetElement( 3 ) ;
			m_RightButton->SetElement( 0 , NormalElement ) ;
			m_RightButton->SetElement( 1 , FocusElement ) ;
			width += m_ButtonOffSetSize ;
			m_RightButton->SetLocation( width , m_y ) ;
			m_RightButton->SetSize( m_ButtonWidth , m_ButtonHeight ) ;
			width += m_ButtonWidth - m_x ;
			//this->SetSize( width + m_ButtonWidth , m_TabHeight ) ;
			//return ;
		}
		if( m_ButtonHeight >= m_TabHeight ) 
			this->SetSize( width , m_ButtonHeight ) ;
		this->SetSize( width , m_TabHeight ) ;
	}
	else	//NO_BUTTON
	{
	}
}

void CDXUTTabPage::SetTabSize( unsigned pWidth , unsigned pHeight )
{
	m_TabWidth = pWidth ;
	m_TabHeight = pHeight ;
	Refreshform() ;
}

void CDXUTTabPage::SetButtonSize( unsigned pWidth , unsigned pHeight )
{
	m_ButtonWidth = pWidth ;
	m_ButtonHeight = pHeight ;
	Refreshform() ;
}

void CDXUTTabPage::SetButtonOffsetSize( unsigned pSize )
{
	m_ButtonOffSetSize = pSize ;
	Refreshform() ;
}

void CDXUTTabPage::SetOffSetSize( unsigned pSize )
{
	if( pSize > 1 )
	{
		m_PageStytle = EMPTY ;
	}
	else m_PageStytle = NO_EMPTY ;
	m_OffSetSize = pSize ;
}

void CDXUTTabPage::SetPageSize( unsigned pSize )
{
	if( pSize == m_PageSize )
		return ;

	while( pSize < ( unsigned )m_CheckButtonVector.size() )
	{
		delete *( m_CheckButtonVector.rbegin() ) ;
		m_pDialog->RiseListener( (int)&*(*m_CheckButtonVector.rbegin()) , EVENT_CHECKBOX_CHANGED ) ;
		m_CheckButtonVector.pop_back() ;
	}

	//while( pSize < m_PageStrVector.size() )
	//{
	//	m_PageStrVector.pop_back() ;
	//}

	if( pSize > (unsigned)m_CheckButtonVector.size() )
	{
		for( unsigned i = (int)m_CheckButtonVector.size() ; i < pSize ; ++i )
		{
			m_CheckButtonVector.push_back( new CDXUTCheckBox( m_pDialog ) ) ;
			m_CheckButtonVector[ i ]->SetID( (int)&*m_CheckButtonVector[ i ] ) ;
			m_CheckButtonVector[ i ]->SetStyle( CDXUTCheckBox::STYLE_BUTTON ) ;
			m_CheckButtonVector[ i ]->SetElement( 0 , GetElement( 4 ) ) ;
			m_CheckButtonVector[ i ]->SetElement( 1 , GetElement( 5 ) ) ;
			m_pDialog->SetListener( (int)&*m_CheckButtonVector[ i ] , EVENT_CHECKBOX_CHANGED , this , &CDXUTTabPage::CheckButtonClk , i ) ;
		}
	}
	m_PageSize = pSize ;
	Refreshform() ;
}

bool CDXUTTabPage::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{	
	if( ContainsPoint( pt ) )
	{
		for( size_t i = 0 ; i < m_CheckButtonVector.size() ; ++i )
		{
			if( m_CheckButtonVector[ i ]->ContainsPoint( pt ) )
				m_CheckButtonVector[ i ]->HandleMouse( uMsg, pt, wParam, lParam ) ;
		}
		
		if( m_Stytle == BUTTON )
		{
			if( m_LeftButton->ContainsPoint( pt ) && m_LeftButton->GetVisible() )
			{
				m_LeftButton->OnMouseEnter() ;
				m_pDialog->ClearFocus() ;
				m_LeftButton->HandleMouse( uMsg, pt, wParam, lParam ) ;	
			}
			else if( !m_LeftButton->ContainsPoint( pt ) )
			{
				m_LeftButton->OnMouseLeave() ;
			}

			if( m_RightButton->ContainsPoint( pt ) && m_RightButton->GetVisible() )
			{
				m_RightButton->OnMouseEnter() ;
				m_pDialog->ClearFocus() ;
				m_RightButton->HandleMouse( uMsg, pt, wParam, lParam ) ; 	
			}
			else if( !m_RightButton->ContainsPoint( pt ) )
			{
				m_RightButton->OnMouseLeave() ;
			}
		}
		return true ;
	}
	return false ;
}

void CDXUTTabPage::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{	
	for( size_t i = 0 ; i < m_CheckButtonVector.size() ; ++i )
	{
		if( i >= m_PageSize )
			break ;
		m_CheckButtonVector[ i ]->Render( pd3dDevice, fElapsedTime ) ;
	}
	
	if( m_Stytle == BUTTON )
	{
		m_LeftButton->Render( pd3dDevice, fElapsedTime ) ;
		m_LeftButton->RenderHint( pd3dDevice , fElapsedTime ) ;
		m_RightButton->Render( pd3dDevice, fElapsedTime ) ;
		m_RightButton->RenderHint( pd3dDevice , fElapsedTime ) ;
	}
}

void CDXUTTabPage::SetVisible( bool bVisible )
{
	for( size_t i = 0 ; i < m_CheckButtonVector.size() ; ++i )
	{
		m_CheckButtonVector[ i ]->SetVisible( bVisible ) ;
	}
	if( m_Stytle == BUTTON )
	{
		m_LeftButton->SetVisible( bVisible ) ; 
		m_RightButton->SetVisible( bVisible ) ;
	}
}

int CDXUTTabPage::LeftButtonClk( const UIEventData& data )
{
	if( m_BeginIndex == 0 )
	{
		return 0 ;
	}
	else if( m_BeginIndex - m_OffSetSize >= 0 )
	{
		m_BeginIndex -= m_OffSetSize ;
		if( m_BeginIndex == 0 )
		{
			m_LeftButton->SetVisible( false ) ;
			m_LeftButton->OnMouseLeave() ;
		}
	}
	else return 0 ;

	m_pDialog->ClearFocus() ;
	unsigned j = m_BeginIndex ;
	for( int i = 0 ; i < (int)m_CheckButtonVector.size() ; ++i )
	{
		if( j < ( unsigned )m_PageStrVector.size()  )
		{
			m_CheckButtonVector[ i ]->SetVisible( true ) ;
			m_CheckButtonVector[ i ]->SetText( m_PageStrVector[ j ].c_str() ) ; 
		}
		else
		{
			m_CheckButtonVector[ i ]->SetVisible( false ) ;
		}
		
		if( j == m_SelectIndex )
		{
			m_CheckButtonVector[ i ]->SetChecked( true ) ;
			m_pDialog->RequestFocus( m_CheckButtonVector[ i ] ) ;
		}
		else
		{
			m_CheckButtonVector[ i ]->SetChecked( false ) ;
		}
		j++ ;

		if( m_ButtonStytle == AUTO_HIDE && m_BeginIndex >= 0 )
			m_RightButton->SetVisible( true ) ;
	}
	return 0 ;
}

int CDXUTTabPage::RightButtonClk( const UIEventData& data )
{
	if( m_BeginIndex + m_OffSetSize < ( int )m_PageStrVector.size() )
	{
		m_BeginIndex += m_OffSetSize ;
		if( m_BeginIndex != 0 )
			m_LeftButton->SetVisible( true ) ;
	}
	else return 0 ;

	m_pDialog->ClearFocus() ;
	unsigned j = m_BeginIndex ;

	for( int i = 0 ; i < (int)m_CheckButtonVector.size() ; ++i )
	{
		if( j <= ( unsigned )m_PageStrVector.size() - 1 )
		{
			m_CheckButtonVector[ i ]->SetVisible( true ) ;
			m_CheckButtonVector[ i ]->SetText( m_PageStrVector[ j ].c_str() ) ; 
		}
		else
		{
			m_CheckButtonVector[ i ]->SetVisible( false ) ;
		}
		
		if( j == m_SelectIndex )
		{
			m_CheckButtonVector[ i ]->SetChecked( true ) ;
			m_pDialog->RequestFocus( m_CheckButtonVector[ i ] ) ;
		}
		else
		{
			m_CheckButtonVector[ i ]->SetChecked( false ) ;
		}
		j++ ;
	}

	if( m_ButtonStytle == AUTO_HIDE && m_BeginIndex >= ( unsigned )m_PageStrVector.size() - m_PageSize  )
	{
		m_RightButton->SetVisible( false ) ;
		m_RightButton->OnMouseLeave() ;
	}

	return 0 ;
}

int CDXUTTabPage::CheckButtonClk( const UIEventData& data )
{
	for( size_t i = 0 ; i < m_CheckButtonVector.size() ; ++i )
	{
		if( (unsigned)i == data.nParam )
		{
			m_CheckButtonVector[ i ]->SetChecked( true ) ;
			m_SelectIndex = m_BeginIndex + (unsigned)i ;
			if( m_iLastSelectedIndex != m_SelectIndex )
				m_iLastSelectedIndex = m_SelectIndex ;
			m_pDialog->SendEvent( EVENT_TABPAGE_SELECTION , true , this ) ;
		}
		else
			m_CheckButtonVector[ i ]->SetChecked( false ) ;
	}
	return 0 ;
}

void CDXUTTabPage::AddItem( const wchar_t* pStr )
{
	m_PageStrVector.push_back( wstring( pStr ) ) ;
	Refreshform() ;
}

void CDXUTTabPage::SetItemSize( unsigned uiItemSize )
{
	m_PageStrVector.resize( uiItemSize ) ;
	m_BeginIndex = 0 ;
	m_SelectIndex = 0 ;
	Refreshform() ;
}

void CDXUTTabPage::SetItemStr( unsigned pIndex , const wchar_t* pStr )
{
	//if( pIndex > m_CheckButtonVector.size() - 1 )
	if( pIndex > m_PageStrVector.size() - 1 )
		return ;
	m_PageStrVector[ pIndex ] = pStr ;

	if( pIndex >= m_BeginIndex && pIndex < m_BeginIndex + m_PageSize )
		m_CheckButtonVector[ pIndex ]->SetText( pStr ) ;
}

void CDXUTTabPage::SetPageNormalElement( CDXUTElement* pElement )
{
	if( pElement != NULL )
	{
		for( size_t i = 0 ; i < m_CheckButtonVector.size() ; ++i )
		{
			m_CheckButtonVector[ i ]->SetElement( 0 , pElement ) ;
		}
	}
	else
	{
		RECT rcTexture ;
		
		m_PageNormalElement.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_CENTER | DT_VCENTER | DT_SINGLELINE );
		m_PageNormalElement.FontColor.States[ DXUT_STATE_DISABLED ]  = D3DXCOLOR( 0xc8808080 ) ;
		m_PageNormalElement.FontColor.States[ DXUT_STATE_NORMAL ]    = D3DXCOLOR( 0xFF461616 ) ; 
		m_PageNormalElement.FontColor.States[ DXUT_STATE_FOCUS ]     = D3DXCOLOR( 0xFF461616 ) ;
		m_PageNormalElement.FontColor.States[ DXUT_STATE_PRESSED ]   = D3DXCOLOR( 0xFF461616 ) ;
		m_PageNormalElement.FontColor.States[ DXUT_STATE_MOUSEOVER ] = D3DXCOLOR( 0xFF461616 ) ;
		m_PageNormalElement.FontColor.States[ DXUT_STATE_UNFOCUS ]   = D3DXCOLOR( 0xc8c8c8c8 ) ;
		m_PageNormalElement.FontColor.States[ DXUT_STATE_HIDDEN ]    = D3DXCOLOR( (DWORD)0x00000000 ) ;

		SetRect( &rcTexture, 299, 256, 364, 285 ) ;
		m_PageNormalElement.SetTexture( 0 , &rcTexture ) ;
		m_PageNormalElement.TextureColor.States[ DXUT_STATE_DISABLED ]  = D3DXCOLOR( 0xc8808080 ) ;
		m_PageNormalElement.TextureColor.States[ DXUT_STATE_NORMAL ]    = D3DXCOLOR( 0xffffffff ) ; 
		m_PageNormalElement.TextureColor.States[ DXUT_STATE_FOCUS ]     = D3DXCOLOR( 0xffffffff ) ;
		m_PageNormalElement.TextureColor.States[ DXUT_STATE_PRESSED ]   = D3DXCOLOR( 0xffffffff ) ;
		m_PageNormalElement.TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DXCOLOR( 0xffffffff ) ;
		m_PageNormalElement.TextureColor.States[ DXUT_STATE_UNFOCUS ]   = D3DXCOLOR( 0xc8c8c8c8 ) ;
		m_PageNormalElement.TextureColor.States[ DXUT_STATE_HIDDEN ]    = D3DXCOLOR( (DWORD)0x00000000 ) ;

		for( size_t i = 0 ; i < m_CheckButtonVector.size() ; ++i )
		{
			m_CheckButtonVector[ i ]->SetElement( 0 , &m_PageNormalElement ) ;
		}
	}
}

void CDXUTTabPage::SetPageFocusElement( CDXUTElement* pElement )
{
	if( pElement != NULL )
	{
		for( size_t i = 0 ; i < m_CheckButtonVector.size() ; ++i )
		{
			m_CheckButtonVector[ i ]->SetElement( 1 , pElement ) ;
		}
	}
	else
	{
		RECT rcTexture ;
		
		m_PageFocusElement.SetFont( 1, D3DCOLOR_ARGB(255, 255, 255, 255), DT_CENTER | DT_VCENTER | DT_SINGLELINE );
		m_PageFocusElement.FontColor.States[ DXUT_STATE_DISABLED ]  = D3DXCOLOR( 0xc8c8c8c8 ) ;
		m_PageFocusElement.FontColor.States[ DXUT_STATE_NORMAL ]    = D3DXCOLOR( 0xFF461616 ) ; 
		m_PageFocusElement.FontColor.States[ DXUT_STATE_FOCUS ]     = D3DXCOLOR( 0xFF461616 ) ;
		m_PageFocusElement.FontColor.States[ DXUT_STATE_PRESSED ]   = D3DXCOLOR( 0xFF461616 ) ;
		m_PageFocusElement.FontColor.States[ DXUT_STATE_MOUSEOVER ] = D3DXCOLOR( 0xFF461616 ) ;
		m_PageFocusElement.FontColor.States[ DXUT_STATE_UNFOCUS ]   = D3DXCOLOR( 0xc8c8c8c8 ) ;
		m_PageFocusElement.FontColor.States[ DXUT_STATE_HIDDEN ]    = D3DXCOLOR( (DWORD)0x00000000 ) ;

		SetRect( &rcTexture, 231, 256, 296, 285 ) ;
		m_PageFocusElement.SetTexture( 0 , &rcTexture ) ;
		m_PageFocusElement.TextureColor.States[ DXUT_STATE_DISABLED ]  = D3DXCOLOR( 0xc8808080 ) ;
		m_PageFocusElement.TextureColor.States[ DXUT_STATE_NORMAL ]    = D3DXCOLOR( 0xffffffff ) ; 
		m_PageFocusElement.TextureColor.States[ DXUT_STATE_FOCUS ]     = D3DXCOLOR( 0xffffffff ) ;
		m_PageFocusElement.TextureColor.States[ DXUT_STATE_PRESSED ]   = D3DXCOLOR( 0xffffffff ) ;
		m_PageFocusElement.TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DXCOLOR( 0xffffffff ) ;
		m_PageFocusElement.TextureColor.States[ DXUT_STATE_UNFOCUS ]   = D3DXCOLOR( 0xc8c8c8c8 ) ;
		m_PageFocusElement.TextureColor.States[ DXUT_STATE_HIDDEN ]    = D3DXCOLOR( (DWORD)0x00000000 ) ;
		
		for( size_t i = 0 ; i < m_CheckButtonVector.size() ; ++i )
		{
			m_CheckButtonVector[ i ]->SetElement( 1 , &m_PageFocusElement ) ;
		}
	}
}

void CDXUTTabPage::SetButtonElement( unsigned pSit , unsigned pElementID , CDXUTElement* pElement )
{
	if( pElementID != 0 || pElementID != 1 )
		return ;

	if( pSit == 0 )
	{
		m_LeftButton->SetElement( pElementID , pElement ) ;
	}
	else if( pSit == 1 )
	{
		m_RightButton->SetElement( pElementID , pElement ) ;
	}
}

int CDXUTTabPage::MouseOut( const UIEventData& data )
{
	m_pDialog->ClearFocus() ;
	m_LeftButton->OnMouseLeave() ;
	m_RightButton->OnMouseLeave() ;
	return 0 ;
}

void CDXUTTabPage::SetID( int pID )
{
	m_ID = pID ;
	if( m_ID != 0 )
	{
		m_pDialog->RiseListener( ( int )&*this , EVENT_CONTROL_MOUSE_OUT ) ;
		m_pDialog->SetListener( m_ID , EVENT_CONTROL_MOUSE_OUT , this , &CDXUTTabPage::MouseOut , 0 ) ;
	}
}

void CDXUTTabPage::SetSelectedIndex( unsigned pIndex )
{
	if( pIndex <= m_PageStrVector.size() - 1 )
	{
		m_SelectIndex = pIndex ;
		Refreshform() ;
	}
}

unsigned CDXUTTabPage::GetSelectedIndex()
{
	return m_SelectIndex ;
}
