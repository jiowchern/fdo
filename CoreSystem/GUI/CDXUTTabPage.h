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
	} m_Stytle ;		//�O�_���W�U�����s,�p�G�S��,�N�O���X��Item�N���X�ӭ���	

	enum ButtonStytle
	{
		AUTO_HIDE ,
		NOT_HIDE 
	} m_ButtonStytle ;	//���U�W�@���ΤU�@���쩳,���s�O�_�n����

	enum PageStytle
	{
		EMPTY ,
		NO_EMPTY
	} m_PageStytle ;	//�O�_�|�X�{�Ū�����,�|�һ���,�p�G�̤j��ܪ����ҼƬO�T��,�i��]��Item�ƶq�u�����,�Ӧ��Ū����� ; ���U�W�U�����ʭ��Ҫ��ƶq�p�G�j��1,�|�۰ʳ]�����Ū����A

	CDXUTTabPage( CDXUTDialog* pDialog = NULL ) ;
	~CDXUTTabPage() ;
	void SetTabSize( unsigned pWidth , unsigned pHeight ) ;													//�]�w���ҫ��s���j�p
	void AddItem( const wchar_t* pStr ) ;																	//�s�W�@�ӭ���
	void SetItemSize( unsigned uiItemSize ) ;																//�]�w���Ҽƶq
	void SetItemStr( unsigned pIndex , const wchar_t* pStr ) ;												//�]�w���Ҧr�� pIndex = 0 ���Ĥ@��	
	void SetButtonSize( unsigned pWidth , unsigned pHeight ) ;												//�]�w�W�@���U�@�����s���j�p
	void SetOffSetSize( unsigned pSize ) ;																	//�]�w�W�U�����ʪ����Ҽƶq
	void SetPageSize( unsigned pSize ) ;																	//�]�w��ܪ����Ҽƶq
	void SetButtonOffsetSize( unsigned pSize ) ;															//�]�w�W�U�����ҫ��s���Z��
	void SetPageNormalElement( CDXUTElement* Element = NULL ) ;												//�]�w���� Normal ���˦�
	void SetPageFocusElement( CDXUTElement* Element = NULL ) ;												//�]�w���� Focus ���˦�
	void SetButtonElement( unsigned pSit , unsigned pElementID , CDXUTElement* pElement ) ;					//�]�w�W�U�����s���˦�, pSet = 0|1 ��ܤW���ΤU�� , pElementID = 0|1 ���Normal�άOFocus
	void SetSelectedIndex( unsigned pIndex ) ;																//�]�w����ĴX������
	unsigned GetSelectedIndex() ;																			//���o��������ҬO�ĴX�� , �Ĥ@�ӹw�]�O 0
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