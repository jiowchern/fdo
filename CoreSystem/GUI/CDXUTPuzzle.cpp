#include "stdafx.h"
#include "CDXUTDef.h"
#include "CDXUTControl.h"
#include "CDXUTPuzzle.h"
//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------
CDXUTPuzzle::CDXUTPuzzle( CDXUTDialog* pDialog ) : CDXUTControl( pDialog )
{
	m_Type = DXUT_CONTROL_PUZZLE ;
	m_uCellWidth = 10 ;
	m_uCellHeight = 10 ;
	m_uLineNumber = 5 ;
	m_iSelectIndex = -1 ;
	m_iLastSelectIndex = -1 ;
	m_iMouseOverIndex = -1 ;
	m_iLastMouseOverIndex = -1 ;
}
//--------------------------------------------------------------------------------------
//	Puzzle解構時,要把裡面的Control一起刪除
//--------------------------------------------------------------------------------------
CDXUTPuzzle::~CDXUTPuzzle()
{
	_FOREACH( ControlVector , m_ControlVector , it )
	{
		if( *it )
			m_pDialog->RemoveControl( *it ) ;
	}	
}
//--------------------------------------------------------------------------------------
//	設定格子大小
//--------------------------------------------------------------------------------------
void CDXUTPuzzle::SetCellSize( uint uWidth , uint uHeight )
{
	m_uCellWidth = uWidth ;
	m_uCellHeight = uHeight ;
	UpdateRects() ;
}
//--------------------------------------------------------------------------------------
//	設定Puzzle的行數
//--------------------------------------------------------------------------------------
void CDXUTPuzzle::SetLineNumber( uint uNumber )
{
	m_uLineNumber = uNumber ;
	UpdateRects() ;
}
//--------------------------------------------------------------------------------------
//	每當改變Puzzle行數或Cell的大小或加入新的Cell時更新
//--------------------------------------------------------------------------------------
void CDXUTPuzzle::UpdateRects()
{
	uint uCellaCount = m_ControlVector.size() ;
	
	if( !uCellaCount )
	{
		m_width = 0 ;
		m_height = 0 ;
		SetRect( &m_rcBoundingBox, m_x, m_y, m_x + m_width, m_y + m_height ) ;
		return ;
	}

	m_width = m_uCellWidth * m_uLineNumber ;
	if( uCellaCount % m_uLineNumber )
		m_height =  ( uCellaCount / m_uLineNumber + 1 ) * m_uCellHeight ;
	else
		m_height = uCellaCount / m_uLineNumber * m_uCellHeight ; 

	SetRect( &m_rcBoundingBox, m_x, m_y, m_x + m_width, m_y + m_height ) ;
	_RefreshControlBoundingBox() ;
}
//--------------------------------------------------------------------------------------
//	得到Cell的ID ID最小為0
//--------------------------------------------------------------------------------------
sint CDXUTPuzzle::_FindCellNo()
{
	if( ContainsPoint( m_MousePoint ) )
	{
		sint iX = m_MousePoint.x - m_x ;
		sint iY = m_MousePoint.y - m_y ;
		sint index = iY / ( m_uCellHeight ) * m_uLineNumber + iX / m_uCellWidth ; 
		if( index > (sint)m_ControlVector.size() - 1 )
			return -1 ;
		else return index ;
	}
	return -1 ;
}
//--------------------------------------------------------------------------------------
//	增加一個Cell或是順道填入Control
//--------------------------------------------------------------------------------------
void CDXUTPuzzle::AddCell( uint uCellNo , CDXUTControl* pControl , sint iCtrlId )
{
	if( uCellNo + 1 > m_ControlVector.size() )
		m_ControlVector.resize( uCellNo + 1 , 0 ) ;
 
	RemoveControl( uCellNo ) ;

	if( pControl )
	{
		pControl->SetLocation( m_x + uCellNo % m_uLineNumber * m_uCellWidth , m_y + uCellNo / m_uLineNumber * m_uCellHeight ) ;
		pControl->SetSize( m_uCellWidth , m_uCellHeight ) ;
		m_ControlVector[ uCellNo ] = pControl ;
		m_pDialog->AddControl( pControl , iCtrlId , true ) ;
		UpdateRects() ;
	}
}
//--------------------------------------------------------------------------------------
//	移除 ID 為 pCellID 的 Cell裡的 Control
//--------------------------------------------------------------------------------------
void CDXUTPuzzle::RemoveControl( uint uCellNo )
{
	if( uCellNo < m_ControlVector.size() )
	{
		CDXUTControl* pControl = m_ControlVector[ uCellNo ] ;
		if( pControl )
		{
			m_pDialog->RemoveControl( pControl ) ;
			pControl = 0 ;
		}
	}
}
//--------------------------------------------------------------------------------------
//	移除 ID 為 pCellID 的 Cell
//--------------------------------------------------------------------------------------
void CDXUTPuzzle::RomoveCell( uint uCellNo )
{
	if( uCellNo < m_ControlVector.size() )
	{
		CDXUTControl* pControl = m_ControlVector[ uCellNo ] ;
		if( pControl )
		{	
			m_ControlVector.erase( m_ControlVector.begin() + uCellNo ) ;
			m_pDialog->RemoveControl( pControl ) ;
			pControl = 0 ;
			UpdateRects() ;
		}
	}
}
//--------------------------------------------------------------------------------------
//	移除所有 Cell 裡的 Control
//--------------------------------------------------------------------------------------
void CDXUTPuzzle::RemoveAllCell()
{
	for( size_t i = 0 ; i < m_ControlVector.size() ; ++i )
	{
		if( m_ControlVector[ i ] )
		{
			m_pDialog->RemoveControl( m_ControlVector[ i ] ) ;
			m_ControlVector[ i ] = 0 ;
		}
	}
}
//-------------------------------------------------------------------------------------- 
//	當滑鼠從某個 Cell 移到另一個 Cell 時,會送一個 EVENT_PUZZLE_CROSSCELL 事件 , 當有選取 Control 時送一個 EVENT_PUZZLE_SELECTCELL 事件
//--------------------------------------------------------------------------------------
bool CDXUTPuzzle::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    m_MousePoint = pt;
	if( uMsg == WM_MOUSEMOVE )
	{
		m_iMouseOverIndex = _FindCellNo() ;
		if( m_iMouseOverIndex != -1 )
		{
			if( m_iLastMouseOverIndex != m_iMouseOverIndex )
			{
				m_pDialog->SendEvent( EVENT_PUZZLE_CROSSCELL , true , this ) ;	
				m_iLastMouseOverIndex = m_iMouseOverIndex ;
				return true ;
			}
		}
		else
		{
			if( m_iMouseOverIndex != m_iLastMouseOverIndex && m_iLastMouseOverIndex != -1 )
			{
				m_pDialog->SendEvent( EVENT_PUZZLE_CROSSCELL , true , this ) ;
				m_iLastMouseOverIndex = m_iMouseOverIndex ;
				return true ;
			}
		}
	}
	else if( uMsg == WM_LBUTTONUP ) 
	{
		m_iSelectIndex = _FindCellNo() ;
		if( m_iSelectIndex != -1 && m_iSelectIndex != m_iLastSelectIndex )
		{
			if( m_ControlVector[ m_iSelectIndex ] )
			{
				m_pDialog->SendEvent( EVENT_PUZZLE_SELECTCELL, true, this , uMsg );
				m_iLastSelectIndex = m_iSelectIndex ;
				return true ;
			}
		}
	}
	return false ;
}
//--------------------------------------------------------------------------------------
//	取得第 CellID Cell 裡的 Control
//--------------------------------------------------------------------------------------
CDXUTControl* CDXUTPuzzle::GetCellControl( unsigned uCellId )
{
	if( uCellId < m_ControlVector.size() )
		return m_ControlVector[ uCellId ] ;
	return 0 ;
}
//--------------------------------------------------------------------------------------
//	取得滑鼠選取的Control
//--------------------------------------------------------------------------------------
CDXUTControl* CDXUTPuzzle::GetSelectControl()
{
	if( m_iSelectIndex != -1 )
		return m_ControlVector[ m_iSelectIndex ] ;
	return 0 ;
}
//--------------------------------------------------------------------------------------
//	取的上一次選取的 Control
//--------------------------------------------------------------------------------------
CDXUTControl* CDXUTPuzzle::GetLastSelectControl()
{
	if( m_iLastSelectIndex != -1 )
		return m_ControlVector[ m_iLastSelectIndex ] ;
	return 0 ;
}
//--------------------------------------------------------------------------------------
//	取的在游標之下的Control
//--------------------------------------------------------------------------------------
CDXUTControl* CDXUTPuzzle::GetMouseOverControl()
{
	if( m_iMouseOverIndex != -1 )
		return m_ControlVector[ m_iMouseOverIndex ] ;
	return 0 ;
}
//--------------------------------------------------------------------------------------
//	更新 Control 的大小及位置
//--------------------------------------------------------------------------------------
void CDXUTPuzzle::_RefreshControlBoundingBox()
{
	for( size_t i = 0 ; i < m_ControlVector.size() ; ++i )
	{
		CDXUTControl* pControl = m_ControlVector[ i ] ;
		if( pControl )
		{
			pControl->SetLocation( m_x + i % m_uLineNumber * m_uCellWidth , m_y + i / m_uLineNumber * m_uCellHeight ) ;
			pControl->SetSize( m_uCellWidth , m_uCellHeight ) ;
		}
	}
}
//--------------------------------------------------------------------------------------
//	全部隱藏
//--------------------------------------------------------------------------------------
void CDXUTPuzzle::SetVisible( bool bVisible )
{
	_FOREACH( ControlVector , m_ControlVector , it )
	{
		CDXUTControl* pControl = *it ;
		if( pControl )
			pControl->SetVisible( bVisible ) ;
	}
}
//--------------------------------------------------------------------------------------
//	Mouse Out
//--------------------------------------------------------------------------------------
void CDXUTPuzzle::OnMouseLeave()
{
	if( m_bMouseOver == true )
	{
		m_iSelectIndex = m_iLastSelectIndex = -1 ;
		m_iMouseOverIndex = m_iLastMouseOverIndex = -1 ;
		m_pDialog->SendEvent( EVENT_CONTROL_MOUSE_OUT , true , this ) ;
	}
	m_bMouseOver = false; 
} 
//--------------------------------------------------------------------------------------
//	取得滑鼠游標目前所指的Cell的RECT
//--------------------------------------------------------------------------------------
void CDXUTPuzzle::GetSelectedRect( RECT& Rect )
{
	sint index = GetMouseOverIndex() ;
	if( index == -1 )
		SetRectEmpty( &Rect ) ;
	
	sint iCaptionHight = m_pDialog->GetCaptionHeight() ;
	SetRect( &Rect , m_x + index % m_uLineNumber * m_uCellWidth 
				   , m_y + iCaptionHight + index / m_uLineNumber * m_uCellHeight 
				   , m_x + index % m_uLineNumber * ( m_uCellWidth + 1 )
				   , m_y + iCaptionHight + index / m_uLineNumber * ( m_uCellHeight + 1 ) ) ;
}
//--------------------------------------------------------------------------------------
//	取得滑鼠游標目前所指的Cell的螢幕座標RECT
//--------------------------------------------------------------------------------------
void CDXUTPuzzle::GetSelectedScreenRect( RECT& Rect )
{
	GetSelectedRect( Rect ) ;
	POINT pt ;
	GetParent()->GetLocation( pt ) ;
	OffsetRect( &Rect , pt.x , pt.y ) ;
}
//--------------------------------------------------------------------------------------
//	取得Cell在Dialog上的RECT
//--------------------------------------------------------------------------------------
void CDXUTPuzzle::GetCellRect( uint uCellNo , RECT& Rect )
{
	SetRectEmpty( &Rect ) ;
	if( uCellNo < m_ControlVector.size() )
	{
		sint iCaptionHight = m_pDialog->GetCaptionHeight() ;
		SetRect( &Rect , m_x + uCellNo % m_uLineNumber * m_uCellWidth 
			, m_y + iCaptionHight + uCellNo / m_uLineNumber * m_uCellHeight 
			, m_x + uCellNo % m_uLineNumber * ( m_uCellWidth + 1 )
			, m_y + iCaptionHight + uCellNo / m_uLineNumber * ( m_uCellHeight + 1 ) ) ;
	}
}
//--------------------------------------------------------------------------------------
//	取得Cell在螢幕上的RECT
//--------------------------------------------------------------------------------------
void CDXUTPuzzle::GetCellScreenRect( unsigned CellID , RECT& Rect )
{
	GetCellRect( CellID , Rect ) ;
	if( !IsRectEmpty( &Rect ) )
	{
		POINT pt ;
		GetParent()->GetLocation( pt ) ;
		OffsetRect( &Rect , pt.x , pt.y ) ;
	}
}
//--------------------------------------------------------------------------------------