#ifndef _CDXUTTabPage_H_20090123_
#define _CDXUTTabPage_H_20090123_
//--------------------------------------------------------------------------------------
#include "CDXUTDef.h"

class CDXUTTabPage : public CDXUTControl
{
public :
	enum Stytle
	{
		BUTTON ,
		NO_BOTTON ,
	} m_Stytle ;		//是否有上下頁按鈕,如果沒有,就是有幾個Item就有幾個頁籤	

	enum ButtonStytle
	{
		AUTO_HIDE ,
		NOT_HIDE 
	} m_ButtonStytle ;	//按下上一頁或下一頁到底,按鈕是否要隱藏

	enum PageStytle
	{
		EMPTY ,
		NO_EMPTY
	} m_PageStytle ;	//是否會出現空的頁籤,舉例說明,如果最大顯示的頁籤數是三個,可能因為Item數量只有兩個,而有空的頁籤 ; 按下上下頁跳動頁籤的數量如果大於1,會自動設為有空的狀態

	CDXUTTabPage( CDXUTDialog* pDialog = NULL ) ;
	~CDXUTTabPage() ;
	void SetTabSize( unsigned pWidth , unsigned pHeight ) ;													//設定頁籤按鈕的大小
	void AddItem( const wchar_t* pStr ) ;																	//新增一個頁籤
	void SetItemSize( unsigned uiItemSize ) ;																//設定頁籤數量
	void SetItemStr( unsigned pIndex , const wchar_t* pStr ) ;												//設定頁籤字串 pIndex = 0 為第一頁	
	void SetButtonSize( unsigned pWidth , unsigned pHeight ) ;												//設定上一頁下一頁按鈕的大小
	void SetOffSetSize( unsigned pSize ) ;																	//設定上下頁跳動的頁籤數量
	void SetPageSize( unsigned pSize ) ;																	//設定顯示的頁籤數量
	void SetButtonOffsetSize( unsigned pSize ) ;															//設定上下頁跟頁籤按鈕的距離
	void SetPageNormalElement( CDXUTElement* Element = NULL ) ;												//設定頁籤 Normal 的樣式
	void SetPageFocusElement( CDXUTElement* Element = NULL ) ;												//設定頁籤 Focus 的樣式
	void SetButtonElement( unsigned pSit , unsigned pElementID , CDXUTElement* pElement ) ;					//設定上下頁按鈕的樣式, pSet = 0|1 表示上頁或下頁 , pElementID = 0|1 表示Normal或是Focus
	void SetSelectedIndex( unsigned pIndex ) ;																//設定選取第幾頁頁籤
	unsigned GetSelectedIndex() ;																			//取得選取的頁籤是第幾個 , 第一個預設是 0
	void SetVisible( bool bVisible ) ;

	void SetID( int pID ) ;
	bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam ) ;
	void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime ) ;
	void SetButtonTexture( const wchar_t *pFileName , RECT *rcTexture , int iElement ) ; 
	void Refreshform() ;

private :
	vector < wstring > m_PageStrVector ;
	vector < CDXUTCheckBox* > m_CheckButtonVector ;
	CDXUTPuzzle* m_Puzzle ;
	CDXUTButton* m_LeftButton ;
	CDXUTButton* m_RightButton ;
	unsigned m_TabWidth ;
	unsigned m_TabHeight ;
	unsigned m_PageSize ;
	unsigned m_ButtonWidth ;
	unsigned m_ButtonHeight ;
	unsigned m_ButtonOffSetSize ;
	
	

	int LeftButtonClk( const UIEventData& data ) ;
	int RightButtonClk( const UIEventData& data ) ;
	int CheckButtonClk( const UIEventData& data ) ;
	int MouseOut( const UIEventData& data ) ;

	unsigned m_OffSetSize ;
	unsigned m_BeginIndex ;
	unsigned m_SelectIndex ;
	int m_iLastSelectedIndex ;
	CDXUTElement m_PageNormalElement ;
	CDXUTElement m_PageFocusElement ;
	CDXUTElement m_LeftButtonElement ;
	CDXUTElement m_RightButtonElement ;
} ;







//--------------------------------------------------------------------------------------
#endif