//
// CDXUTScrollBar.h
//
#ifndef __CDXUTScrollBar_h__
#define __CDXUTScrollBar_h__

#include "CDXUTDef.h"
#include "CDXUTControl.h"
#include "BaseType.h"
#include "LokiLib.h"
#include "Visitor.h"

//-----------------------------------------------------------------------------
// Scrollbar control
//-----------------------------------------------------------------------------
namespace ScrollBarEvent
{
	//////////////////////////////////////////////////////////////////////////
	// bar 可以監聽的事件
	enum
	{
		UPDATE_MAXSIZE,
	};

	struct MaxSize
	{
		uint nMaxSize;
		uint nPageSize;
	};

	struct ScrollAmount
	{
		uint nScrollAmount;		
	};
	
	typedef Visitor<TYPELIST_2(MaxSize,ScrollAmount),DoIVisit>	ControlVisitor;
	//////////////////////////////////////////////////////////////////////////
	// control 可以監聽的事件
	enum
	{
		UPDATE_CURPOS
	};
	struct CurPos
	{
		sint nCurPos;
	};

	typedef Visitor<TYPELIST_1(CurPos),DoIVisit>		BarVisitor;
};

struct DXUTScrollBarVisitor
	: public Visitable<ScrollBarEvent::ControlVisitor,DoIVisit> // 給Control拜訪者拜訪	
	
{	
	void	AcceptMaxSize		(uint nMaxSize,uint nPageSize)
	{
		ScrollBarEvent::MaxSize ms;
		ms.nMaxSize = nMaxSize;
		ms.nPageSize = nPageSize;
		Accept(ms);
	}
	void	AcceptScroll		(sint nScrollAmount)
	{
		ScrollBarEvent::ScrollAmount ms;
		ms.nScrollAmount = nScrollAmount;		
		Accept(ms);
	}
	//virtual void	Visit			(ScrollBarEvent::CurPos& e) = 0;	
private:
	
};

struct DXUTScrollBar 
	: public Visitable<ScrollBarEvent::BarVisitor,DoIVisit> // 給Bar拜訪者拜訪
	
{
	void			AcceptCurPos		(sint nPos)
	{
		ScrollBarEvent::CurPos cp;
		cp.nCurPos = nPos;
		Accept(cp);
	}		
};

class CDXUTScrollBar : public CDXUTControl , public ScrollBarEvent::ControlVisitor	//自己是Control拜訪者	
{
	DXUTScrollBar mDXUTScrollBar; 
public:
    CDXUTScrollBar( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTScrollBar();

    virtual HRESULT OnInit();

    virtual bool    HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool    HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );

    virtual void    Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
    virtual void    UpdateRects();

	void	AddVisitor	(ScrollBarEvent::BarVisitor* pVis);
    void SetTrackRange( int nStart, int nEnd );
    void GetTrackRange( int& nStart, int& nEnd );
	uint GetTrackSize( ) const ;
    inline int GetTrackPos() const { return m_nPosition; }
	void SetTrackPos( int nPosition ) ;
    //void SetTrackPos( int nPosition ) { m_nPosition = nPosition; Cap(); UpdateThumbRect(); }
    inline int GetPageSize() { return m_nPageSize; }
    void SetPageSize( int nPageSize ) { m_nPageSize = nPageSize; Cap(); UpdateThumbRect(); }

    void Scroll( int nDelta );    // Scroll by nDelta items (plus or minus)
    void ShowItem( int nIndex );  // Ensure that item nIndex is displayed, scroll if necessary
    
    bool IsArrowUpHeld();
    bool IsThumbHeld();

	static void ResetDragState(){  sm_DraggedScrollBar = NULL; }

	// ScrollBar 的渲染要素。
    void SetElementTrack(const CDXUTElement& elemSrc);
    void SetElementUpArrow(const CDXUTElement& elemSrc);
    void SetElementDownArrow(const CDXUTElement& elemSrc);
    void SetElementThumb(const CDXUTElement& elemSrc);

	virtual void	Visit			(ScrollBarEvent::MaxSize& e) ;		
	virtual void	Visit			(ScrollBarEvent::ScrollAmount& e) ;		
private:
	bool IsDragged(){ return sm_DraggedScrollBar==this; }
	void SetDragged(CDXUTScrollBar *p){ sm_DraggedScrollBar = p; }
protected:
    // ARROWSTATE indicates the state of the arrow buttons.
    // CLEAR            No arrow is down.
    // CLICKED_UP       Up arrow is clicked.
    // CLICKED_DOWN     Down arrow is clicked.
    // HELD_UP          Up arrow is held down for sustained period.
    // HELD_DOWN        Down arrow is held down for sustained period.
    enum ARROWSTATE { 
        CLEAR, CLICKED_UP, CLICKED_DOWN, HELD_UP, HELD_DOWN };

    void UpdateThumbRect();
    void Cap();  // Clips position at boundaries. Ensures it stays within legal range.

    bool m_bDragMove; // Dragged on thumb (bDrag = true) and mouse moved (WM_MOUSEMOVE).
    bool m_bShowThumb;
    RECT m_rcUpButton;
    RECT m_rcDownButton;
    RECT m_rcTrack;
    RECT m_rcThumb;
    int m_nPosition;  // Position of the first displayed item
    int m_nPageSize;  // How many items are displayable in one page
    int m_nStart;     // First item
    int m_nEnd;       // The index after the last item
    POINT m_LastMouse;// Last mouse position
    ARROWSTATE m_Arrow; // State of the arrows
    double m_dArrowTS;  // Timestamp of last arrow event.

    // ScrollBar 的渲染要素。
    CDXUTElement* m_ElemTrack;
    CDXUTElement* m_ElemUpArrow;
    CDXUTElement* m_ElemDownArrow;
    CDXUTElement* m_ElemThumb;

	static CDXUTScrollBar *sm_DraggedScrollBar;
};

#endif // __CDXUTScrollBar_h__