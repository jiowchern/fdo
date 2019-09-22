#ifndef _CDXUTPuzzle_H_20090117_
#define _CDXUTPuzzle_H_20090117_
//--------------------------------------------------------------------------------------
#include "CDXUTDef.h"
#include "Common.h"
//--------------------------------------------------------------------------------------

class CDXUTPuzzle : public CDXUTControl
{
public :

	CDXUTPuzzle( CDXUTDialog* pDialog = NULL ) ;
	~CDXUTPuzzle() ;
	void SetCellSize( uint uWidth , uint uHeight ) ;
	void SetLineNumber( uint uNumber ) ;
	void UpdateRects() ;
	void AddCell( uint uCellId , CDXUTControl* pControl = NULL , sint iID = 0 ) ;
	void RemoveControl( uint uCellNo ) ;
	void RomoveCell( uint uCellNo ) ;
	void RemoveAllCell() ;
	bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam ) ;
	void OnMouseLeave() ;
	CDXUTControl* GetCellControl( uint uCellId ) ;
	CDXUTControl* GetSelectControl() ;
	CDXUTControl* GetLastSelectControl() ;
	CDXUTControl* GetMouseOverControl() ;
	inline sint GetMouseOverIndex() { return m_iMouseOverIndex ; }
	inline sint GetLastMouseOverIndex() { return m_iLastMouseOverIndex ; }
	inline sint GetSelectedIndex() { return m_iSelectIndex ; }
	inline sint GetLastSelectedIndex() { return m_iLastSelectIndex ; }
	
	void GetSelectedRect( RECT& Rect ) ;
	void GetSelectedScreenRect( RECT& Rect ) ;
	void GetCellRect( unsigned CellID , RECT& Rect ) ;
	void GetCellScreenRect( unsigned CellID , RECT& Rect ) ;
	void SetVisible( bool bVisible ) ;
	
private :

	typedef vector< CDXUTControl* > ControlVector ;
	ControlVector m_ControlVector ;
	uint		  m_uCellWidth ;
	uint	      m_uCellHeight ;
	uint	      m_uLineNumber ;
	sint		  m_iSelectIndex ;
	sint		  m_iLastSelectIndex ;
	sint	      m_iMouseOverIndex ;
	sint	      m_iLastMouseOverIndex ;

	sint _FindCellNo() ;
	void _RefreshControlBoundingBox() ;
} ;
//--------------------------------------------------------------------------------------
#endif
