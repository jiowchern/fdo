//
// ImageControls.h
//
#ifndef __ImageControls_h__
#define __ImageControls_h__

#include "CDXUTButton.h"
#include "CDXUTListBox.h"
#include "ListBoxItem.h"
#include "TextObject.h"


//-----------------------------------------------------------------------------
// Image control
//-----------------------------------------------------------------------------
class CDXUTImage : public CDXUTButton
{
	bool	mbSetBlend;
	bool	mbStretch;
public:
    CDXUTImage( CDXUTDialog *pDialog = NULL );
    
    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    
    virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );

    // �]�w��Image������A�@�λPCDXUTControl��SetTexture�ۦP
    inline void SetImage( const string &FileName, RECT* prcTexture=0 ,bool bSetBlend = true,bool bStretch = false)  //���� ***
     { 
		 mbSetBlend = bSetBlend;
		 mbStretch = bStretch;
		 CDXUTControl::SetTexture( FileName, prcTexture ); 
	}
    inline void SetImage( const WCHAR *FileName, RECT* prcTexture=0 ,bool bSetBlend = true,bool bStretch = false)
    { 
		mbSetBlend = bSetBlend;
		mbStretch = bStretch;
			CDXUTControl::SetTexture( FileName, prcTexture ); 
	}

    void SetColor( D3DCOLOR color );

    IDirect3DTexture9* GetTexture( int iTexture ); // ���o��Image������
};

//-----------------------------------------------------------------------------
// ImageListBox control
//-----------------------------------------------------------------------------
struct DXUTImageListBoxItem : public DXUTListBoxItem
{
    auto_ptr<CDXUTImage> Image;
    auto_ptr<CDXUTEditBox> EditBox;
    auto_ptr<CDXUTStatic> Static;
    
    bool m_bStatic; // �O�_��ܼƶq�bImage�k�U��
    bool m_Visible;
};

typedef void (*AddItemCallback)(CDXUTImageListBox* pImageListBox, int iAmount);

class CDXUTImageListBox : public CDXUTListBox
{
public:
    CDXUTImageListBox( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTImageListBox();

    //virtual HRESULT OnInit() { return m_pDialog->InitControl( &m_ScrollBar ); }
    //virtual bool    CanHaveFocus() { return (m_bVisible && m_bEnabled); }
    //virtual bool    HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool    HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void    Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
    virtual void    RenderHint( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
    virtual void    UpdateRects();

	void OnMouseLeave() ;

    virtual void SetSpace( int nSpace ) { m_nSpace = nSpace; UpdateRects(); }// �]�wItem���Z

    HRESULT AddItem( DXUTImageListBoxItem *pItem, int iAmount=-1, int Price=0 );
    HRESULT AddItem( const WCHAR *ImageFile, const WCHAR *wszText, D3DXCOLOR TextColor, void *pData, int Amount=0, int Price=0 );
    HRESULT AddItem( const string &ImageFile, const string &szText, D3DXCOLOR TextColor, void *pData, int Amount=0, int Price=0 );
    // �Ϥ��Ĥ@��βĤG�檺�r��
    HRESULT AddItem( const string &ImageFile, const string &szText1st, const string &szText2nd, D3DXCOLOR TextColor, void *pData, int Amount=0, int Price=0 );
    //water add 2007/07/16
    HRESULT InsertItem( int nIndex, const WCHAR *ImageFile, const WCHAR *wszText, D3DXCOLOR TextColor, void *pData, int Amount=0, int Price=0 );
    // �Ϥ��Ĥ@��βĤG�檺�r��
    HRESULT AddItem( const string &ImageFile, const string &szText1st, const int &szText2nd, D3DXCOLOR TextColor, void *pData, int Amount=0, int Price=0 );
    // AddItemCallback
	HRESULT ResetItem( int pIndex , const wchar_t* pImageFile , const wchar_t* pWText, D3DXCOLOR pTextColor , void* pData = 0 , int pAmount = 0  , int pPrice = 0 ) ;
    void SetAddItemCallback(AddItemCallback pFunc) { m_pAddItemCallback = pFunc; }

    void RemoveItem( const int &nIndex );
    void RemoveItem( DXUTImageListBoxItem *pItem ) { RemoveItem( m_Items.IndexOf(pItem) ); }
    void RemoveAllItems();
    void RemoveItemPointer( const int &nIndex );
    void RemoveItemPointer( DXUTImageListBoxItem *pItem ) { RemoveItemPointer( m_Items.IndexOf(pItem) ); }

    int GetItemIndexByData( void *pData );
    DXUTImageListBoxItem *GetItem( int nIndex );
    DXUTImageListBoxItem *GetSelectedItem( int nPreviousSelected = -1 ) { return GetItem( GetSelectedIndex( nPreviousSelected ) ); }
    DXUTImageListBoxItem *GetItemByData( void *pData ) { return GetItem( GetItemIndexByData(pData) ); }

    virtual int GetSize() const { return m_Items.GetSize(); }
    
    // �]�w�i�_�즲
    void SetEnableEdit( bool bEnableEdit ) { m_bEnableEdit = bEnableEdit; }
    void SetDragIcon( bool bDragIcon ) { m_bDragIcon = bDragIcon; }
    bool GetDragIcon() { return m_bDragIcon; }

    // per-item EditBox
    void SetEditVisible( const bool &bEditVisible ) { m_bEditVisible = bEditVisible; }
    void SetEditEnabled( const bool &bEditEnabled ) { m_bEditEnabled = bEditEnabled; }
    void SetEditOffsetPos( int iX, int iY ) { m_iEditOffsetX = iX; m_iEditOffsetY = iY; }
    void SetEditSize(int cxEdit, int cyEdit);
    void GetEditSize(int& cxEdit, int& cyEdit);
    bool GetEditDrawFrame();
    void SetEditDrawFrame(bool bDraw);
    void SetEditFrameColor(D3DCOLOR Color);
    // per-item EditBox icon
    CDXUTElement m_EditBoxIconElement;

    // �]�w����Ѽ�
    void SetItemHeight( int iHeight ) { m_iItemHeight = iHeight; }
    void SetStaticWidth( int iWidth ) { m_iStaticWidth = iWidth; }
    void SetDrawText( bool bDraw ) { m_bDrawText = bDraw; }

    // �]�wScrollBar����List�̫�
    void TrackLast() { m_ScrollBar.SetTrackPos( m_Items.GetSize() ); }
    
    // ��r�C�� text-format
    DWORD GetTextFormatLine0();
    DWORD GetTextFormatLine1();
    void SetTextFormatLine0(DWORD dwTextFormat);
    void SetTextFormatLine1(DWORD dwTextFormat);

	int GetCheck() { return m_iMouseOverIndex ; }		
    
protected:
    void DrawItemText(DXUTImageListBoxItem* pItem, CDXUTElement* pElement, RECT* pDestRect);
    virtual void DrawItemTextBkgnd(DXUTImageListBoxItem* pItem, CDXUTElement* pElement, RECT* pDestRect);
    // ��r�C�� text-format
    void InitTextFormats();
    // AddItemCallback
    AddItemCallback m_pAddItemCallback;

protected:
    CGrowableArray< DXUTImageListBoxItem* > m_Items;

    bool m_bEditVisible; // �O�_���EditBox
    bool m_bEditEnabled; // �O�_���EditBox
    bool m_bDragIcon; // �i�_�즲
    int  m_iItemHeight;  // Item������
    int  m_iStaticWidth; // Static���e��
    bool m_bDrawText;    // �O�_ø�s��r
    bool m_bEnableEdit;
    POINT m_NowPt;
    // ��r�C�� text-format
    DWORD m_dwTextFormatLine0;
    DWORD m_dwTextFormatLine1;
	int   m_iMouseOverIndex ;

    // per-item EditBox
    int  m_iEditOffsetX; // EditBox �����qX
    int  m_iEditOffsetY; // EditBox �����qY
    int  m_iEditWidth;
    int  m_iEditHeight;
	int	 m_iEditWords;	 // EditBox �r��
	int	 m_iEditWordWidth; // EditBox ��r�e��
    bool m_bEditDrawFrame;
    D3DCOLOR m_EditFrameColor;

    CDXUTElement pTextureElement;
};

//-----------------------------------------------------------------------------
// ImageBox control
//-----------------------------------------------------------------------------
class CDXUTImageBox : public CDXUTImageListBox
{
public:
    CDXUTImageBox( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTImageBox();

    virtual void    RenderHint( IDirect3DDevice9* pd3dDevice, float fElapsedTime ); // �e�X���󴣥�
    virtual bool    HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void    Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );

	void AddTextureItem( const string &ImageFile, RECT* prcTexture = 0, void *pData = 0, int iAmount=0, int iPrice=0 );
	void AddTextureItem( const WCHAR *ImageFile, RECT* prcTexture = 0, void *pData = 0, int iAmount=0, int iPrice=0 );
	void SetRenderBackground( bool bVisible )	{ m_bRenderBackground = bVisible; }
                   
    bool HasItem() { return ( m_Items.GetSize() > 0 ) ? true : false; }

protected:
    virtual void UpdateRects();
	bool  m_bRenderBackground;
};

//--------------------------------------------------------------------------------------
// CDXUTBtnImageListBox
//--------------------------------------------------------------------------------------
class CDXUTBtnImageListBox : public CDXUTImageListBox
{
    public:
    CDXUTBtnImageListBox( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTBtnImageListBox();

    CDXUTButton *pBtn[2];

    bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );

    virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );

    HRESULT CDXUTBtnImageListBox::AddItem( int iStaticNum, int iWidth1, int iHeight1,
                                           const string& strEditText, int iWidth2, int iHeight2,
                                           const string& strImageFile, void* pData = NULL );

};

//-----------------------------------------------------------------------------
// ImageList control ������
//-----------------------------------------------------------------------------
class CDXUTImageBoxList : public CDXUTListBox
{
public:
    CDXUTImageBoxList( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTImageBoxList();

    //virtual HRESULT OnInit() { return m_pDialog->InitControl( &m_ScrollBar ); }
    //virtual bool    CanHaveFocus() { return (m_bVisible && m_bEnabled); }
    //virtual bool    HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    //virtual bool    HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );

    virtual void    Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
    //virtual void    UpdateRects();

protected:
    CGrowableArray< CDXUTImageBox* > m_ImageBoxes;

    int m_numCellsPerRow;
    int m_numCellsPerCol;
};

#endif // __ImageControls_h__