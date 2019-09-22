//
// CDXUTControl.cpp
//
#include "stdafx.h"
//#pragma hdrstop
//---------------------------------------------------------------------------
#include "CDXUTDialog.h"
#include <strsafe.h>
#include "CDXUTTimer.h"
#include "FDOUT.h"
#include "PacketReader.h"
#include "JLAnimation2DManager.h"
#include "CDXUTDialogResourceManager.h"
//#include "HUISystemSet.h"
#include <cmath>
#include "J_SoundManager.h"
#include <stdio.h>
#include "iniReader.h"
#include "CFDOXML.h"
#include "DialogType.h"
#pragma hdrstop
#include "CDXUTControl.h"
#include "TalkbarImageControl.h"
#include "Tree.h"
#include "ElementDef.h"
#include "RenderComponent.h"
using namespace FDO;


typedef ObjFactory<char16[MAX_PATH], 1000> ControlTextFactory;
ControlTextFactory& gControlTextFactory = *(new ControlTextFactory);
//--------------------------------------------------------------------------------------
// Static variables
//--------------------------------------------------------------------------------------
CDXUTControl* CDXUTControl::s_pDragSource = NULL;
IDirect3DTexture9* CDXUTControl::s_pDragTexture = NULL;
int CDXUTControl::s_SrcIndex = -1;
int CDXUTControl::s_DestIndex = -1;
CDXUTControl* CDXUTControl::s_pOnMouseCtrl = 0;
//--------------------------------------------------------------------------------------
CDXUTControl::CDXUTControl( CDXUTDialog *pDialog )  : 
mDDTarget(this) ,mnAlign(CDXUTControl::ALIGN_NONE) , mpNode(0) 
,m_strText(0),m_strHint(0),m_strHintImgPath(0) , m_ElementsQueryer( this )
{
    m_Type = DXUT_CONTROL_BUTTON;
	m_uCurState = DXUT_STATE_NORMAL ;
    m_pDialog = pDialog;
    m_ID = 0;
    m_Index = 0;
    m_pUserData = NULL;

	m_pRender = NULL ; 

    m_bEnabled = true;
    m_bVisible = true;
    m_bMouseOver = false;
    m_bHasFocus = false;
    m_bIsDefault = false;

    //m_pDialog = NULL;

    m_x = 0;
    m_y = 0;
    m_width = 0;
    m_height = 0;

	ZeroMemory( &m_rcBoundingBox, sizeof( m_rcBoundingBox ) );
// 	ZeroMemory( &m_strText, sizeof(m_strText) );
// 	ZeroMemory( &m_strHint, sizeof(m_strHint) );
// 	ZeroMemory( &m_strHintImgPath, sizeof(m_strHintImgPath) );

	m_cxWordMaxExtent = 25 * 6;
	m_iHintBackAlpha = 180;
	m_FrameColor = D3DCOLOR_ARGB( 0, 0, 0, 0 );
	m_HintTextColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	m_BackgroundColor = D3DCOLOR_ARGB( 180, 0, 0, 0 );
	m_dwTextFormat = DT_CENTER | DT_VCENTER;
	m_bHintTurnRight = false;

   // ming add >>
   m_bShadow = false;
   // ming add <<
}

CDXUTControl::~CDXUTControl()
{
	_FOREACH( RenderComponentVector , m_RenderComponents , it )
	{
		delete *it ;
	}
	m_RenderComponents.clear() ;
	_FOREACH( ElementMap , m_Elements , it )
	{
		delete it->second ;
	}
	if(mpNode)
	{
		using namespace Tree;
		TreeFactory::fnDestoryNode<CDXUTControl::Node>(mpNode);
		mpNode = 0;
	}
	if(this == CDXUTControl::s_pDragSource)
		CDXUTControl::s_pDragSource = NULL;
	
	if(this == CDXUTControl::s_pOnMouseCtrl)
		CDXUTControl::s_pOnMouseCtrl = 0;
	if(m_strText)
		gControlTextFactory.Destroy(m_strText);
	if(m_strHint)
		gControlTextFactory.Destroy(m_strHint);
	if(m_strHintImgPath)
		gControlTextFactory.Destroy(m_strHintImgPath);
}
//--------------------------------------------------------------------------------------
RenderComponent* CDXUTControl::AddRenderComponent( uint uComponentType )
{
	switch( uComponentType )
	{
	case Component_Jiugong_Frame :
		m_RenderComponents.push_back( new RenderJiugongFrame( &m_ElementsQueryer ) ) ;
		break ;

	case Component_Jiugong_ColorFrame :
		m_RenderComponents.push_back( new RenderJiugongFrame( &m_ElementsQueryer , RenderJiugongFrame::Render_Color ) ) ;
		break ;

	case Component_Centered_Image :
		m_RenderComponents.push_back( new RenderCenteredImage( &m_ElementsQueryer ) ) ;
		break ;

	case Component_Crop_Image :
		m_RenderComponents.push_back( new RenderCropImage( &m_ElementsQueryer ) ) ;
		break ;

	case Component_Flash_Color :
		m_RenderComponents.push_back( new RenderFlashColor( &m_ElementsQueryer ) ) ;
		break ;

	case Component_Flash_Frame :
		m_RenderComponents.push_back( new RenderFlashFrame( &m_ElementsQueryer ) ) ;
		break ;

	case Component_Mask_Image :
		m_RenderComponents.push_back( new RenderMaskImage( &m_ElementsQueryer ) ) ;
		break ;

	case Component_ColdDown :
		m_RenderComponents.push_back( new RenderColdDown( &m_ElementsQueryer ) ) ;
		break ;

	case Component_Run_Frame :
		m_RenderComponents.push_back( new RenderRunFrame( &m_ElementsQueryer ) ) ;
		break ;

	default :
		assert( 0 ) ;
		break ;
	} ;
	m_RenderComponents.back()->UpdataBoundingBox() ;
	return m_RenderComponents.back() ;
}
//--------------------------------------------------------------------------------------
void CDXUTControl::RemoveRenderComponent( uint uIndex )
{
	if( uIndex > m_RenderComponents.size() - 1 )
		return ;

	RenderComponentVector::iterator it = m_RenderComponents.begin() ;
	for( uint i = 0 ; i < uIndex ; ++i , ++it ) ;

	RenderComponent* pComponent = *it ;
	m_RenderComponents.erase( it ) ;
	delete pComponent ;
}
//--------------------------------------------------------------------------------------
void CDXUTControl::RemoveRenderComponent( RenderComponent* pRenderComponent )
{
	_FOREACH( RenderComponentVector , m_RenderComponents , it )
	{
		RenderComponent* pConponent = *it ;
		if( pConponent == pRenderComponent )
		{
			m_RenderComponents.erase( it ) ;
			delete pConponent ;
			pConponent = 0 ;
			break ;
		}
	}
}
//--------------------------------------------------------------------------------------
void CDXUTControl::ClearRenderComponent()
{
	_FOREACH( RenderComponentVector , m_RenderComponents , it )
	{
		delete *it ;
	}
	m_RenderComponents.clear() ;
}
//--------------------------------------------------------------------------------------
RenderComponent* CDXUTControl::GetRenderComponent( uint uIndex )
{
	if( uIndex > m_RenderComponents.size() - 1 )
		return 0 ;
	return m_RenderComponents[ uIndex ] ;
}
//--------------------------------------------------------------------------------------
void CDXUTControl::UpdateComponentRects()
{
	_FOREACH( RenderComponentVector , m_RenderComponents , it )
	{
		( *it )->UpdataBoundingBox() ;
	}
}
//--------------------------------------------------------------------------------------
void CDXUTControl::RenderControl( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
	Render( pd3dDevice , fElapsedTime ) ;
	_FOREACH( RenderComponentVector , m_RenderComponents , it )
	{
		if( ( *it )->GetVisible() )
		{
			( *it )->Render( m_uCurState , pd3dDevice , fElapsedTime ) ;
			( *it )->Render( m_uCurState , m_pDialog , fElapsedTime ) ;
		}
	}
}
//--------------------------------------------------------------------------------------
void CDXUTControl::SetTextColor( D3DCOLOR Color, UINT iElement )
{
	ElementMap::iterator it = m_Elements.find( iElement ) ;
	if( it != m_Elements.end() )
	{
		CDXUTElement* pElement = it->second ;

		if( pElement )
			pElement->FontColor.States[ DXUT_STATE_NORMAL ] = Color ;
	}
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTControl::SetElement( UINT iElement, CDXUTElement* pElement )
{
    HRESULT hr = S_OK ;

    if( pElement == NULL )
        return E_INVALIDARG ;

	ElementMap::iterator it = m_Elements.find( iElement ) ;
	if( it != m_Elements.end() )
	{
		*( it->second ) = *pElement ;
	}
	else
	{
		pair< ElementMap::iterator , bool > result = m_Elements.insert( make_pair( iElement , new CDXUTElement ) ) ;
		if( result.second )
		{
			*( result.first->second ) = *pElement ;
		}
		else
		{
			return E_OUTOFMEMORY ;
		}
	}
    return S_OK;
}
//--------------------------------------------------------------------------------------
//音效 *** modify
//1. Play nothing without any string from strText or strHint
//2. Play nothing with any string "Mute" from strText or strHint
//3. Play Default sound with any string from strText or strHint which can't match the mapping on SoundMap.ini
//4. Play the right sound with any string from strText or strHint which can match the mapping on SoundMap.ini

//Any object on Dialog can set the sound playing by SoundMap.ini and setting SetHint() or SetText()
//in order to set string on m_strText or m_strHint

void CDXUTControl::PlaySoundWAV()
{
	
	string mText = UNICODEtoANSI(m_strText ? m_strText : L"Mute");
	string mHint = UNICODEtoANSI(m_strHint ? m_strHint : L"Mute");


//     if(strcmp(m_strText, "")==0)
     if(mText != "" )
     {
        string m_SoundFile = GetSoundMap(mText);

        if(m_SoundFile == "" )
           m_SoundFile=GetSoundMap("Dialog->Default");
        else if(m_SoundFile == "Mute")
           return;
        //water edit 2006/12/22*********************************
        g_SoundManager._Play2DSoundInMemory(m_SoundFile );
        //water edit 2006/12/22*********************************        

      }
      else if(mHint != "")
      {
        string m_SoundFile = GetSoundMap(mHint);

        if(m_SoundFile == "" )
           m_SoundFile=GetSoundMap("Dialog->Default");
        else if(m_SoundFile == "Mute")
           return;
        //water edit 2006/12/22*********************************
        g_SoundManager._Play2DSoundInMemory(m_SoundFile );
        //water edit 2006/12/22*********************************
      }
}

//--------------------------------------------------------------------------------------
void CDXUTControl::SetTexture( const WCHAR *FileName, RECT* prcTexture, int iElement )
{
    SetTexture( UNICODEtoANSI(FileName), prcTexture, iElement );
}

//--------------------------------------------------------------------------------------
void CDXUTControl::SetTexture( const string &FileName, RECT* prcTexture, int iElement )
{
	ElementMap::iterator it = m_Elements.find( iElement ) ;
	if( it == m_Elements.end() )
	{
		pair< ElementMap::iterator , bool > result = m_Elements.insert( make_pair( iElement , new CDXUTElement ) ) ;
		if( result.second )
		{
			CDXUTElement* pElement = result.first->second ;
			// 取得資源管理員並增加材質
			int iTexture = m_pDialog->GetManager()->AddTexture( FileName ) ;
			pElement->SetTexture( iTexture , prcTexture ) ;
		}
		else
		{
			assert( 0 ) ;
		}
	}
	else
	{
		CDXUTElement* pElement = it->second ;
		// 取得資源管理員並增加材質
		int iTexture = m_pDialog->GetManager()->AddTexture( FileName ) ;
		pElement->SetTexture( iTexture , prcTexture ) ;
	}

}
//--------------------------------------------------------------------------------------
void CDXUTControl::SetTexture( int iTexture, RECT* prcTexture, int iElement )
{
	ElementMap::iterator it = m_Elements.find( iElement ) ;
	if( it != m_Elements.end() )
	{
		CDXUTElement* pElement = it->second ;
		pElement->SetTexture( iTexture , prcTexture ) ;
	}
	else
	{
		pair< ElementMap::iterator , bool > result = m_Elements.insert( make_pair( iElement , new CDXUTElement ) ) ;
		if( result.second )
		{
			CDXUTElement* pElement = result.first->second ;
			pElement->SetTexture( iTexture , prcTexture ) ;
		}
		else
		{
			assert( 0 ) ;
		}
	}
}
//--------------------------------------------------------------------------------------
void CDXUTControl::SetFont( const string &strFaceName, LONG height, LONG weight, int iElement )
{
    SetFont( ANSItoUNICODE(strFaceName), height, weight, iElement );
}

//--------------------------------------------------------------------------------------
void CDXUTControl::SetFont( LPCWSTR strFaceName, LONG height, LONG weight, int iElement )
{
	ElementMap::iterator it = m_Elements.find( iElement ) ;
	if( it == m_Elements.end() )
	{
		pair< ElementMap::iterator , bool > result = m_Elements.insert( make_pair( iElement , new CDXUTElement ) ) ;
		if( result.second )
		{
			CDXUTElement* pElement = result.first->second ;
			int iFont = m_pDialog->GetManager()->AddFont( strFaceName, height, weight ) ;
			pElement->SetFont( iFont ) ;
		}
		else
		{
			assert( 0 ) ;
		}
	}
	else
	{
		CDXUTElement* pElement = it->second ;
		int iFont = m_pDialog->GetManager()->AddFont( strFaceName, height, weight ) ;
		pElement->SetFont( iFont ) ;
	}
}

//--------------------------------------------------------------------------------------
void CDXUTControl::SetTextFormat( DWORD dwTextFormat, int iElement )
{
	ElementMap::iterator it = m_Elements.find( iElement ) ;
	if( it == m_Elements.end() )
	{
		pair< ElementMap::iterator , bool> result = m_Elements.insert( make_pair( iElement , new CDXUTElement ) ) ;
		if( result.second )
		{
			CDXUTElement* pElement = result.first->second ;
			pElement->dwTextFormat = dwTextFormat ;
		}
		else
		{
			assert( 0 ) ;
		}
	}
	else
	{
		CDXUTElement* pElement = it->second ;
		pElement->dwTextFormat = dwTextFormat ;
	}
}
//--------------------------------------------------------------------------------------
HRESULT OnInit()
{ 
	return S_OK ;
}
//--------------------------------------------------------------------------------------
void CDXUTControl::Refresh()
{
    m_bMouseOver = false;
    m_bHasFocus = false;

    _FOREACH( ElementMap , m_Elements , it )
	{
		CDXUTElement* pElement = it->second ;
		pElement->Refresh() ;
	}
}

//--------------------------------------------------------------------------------------
void CDXUTControl::UpdateRects()
{
    SetRect( &m_rcBoundingBox, m_x, m_y, m_x + m_width, m_y + m_height );
}
//--------------------------------------------------------------------------------------
void CDXUTControl::SetAlign(int nAlign)
{
	mnAlign = nAlign;
}
//--------------------------------------------------------------------------------------
void CDXUTControl::SetParentNode(Node* pNode)
{
	assert(mpNode == 0);
	mpNode = pNode;
}
//--------------------------------------------------------------------------------------
CDXUTControl::Node* CDXUTControl::GetParentNode()
{
	return mpNode;
}
//--------------------------------------------------------------------------------------
void CDXUTControl::SetHintBackgroundAlpha( int iAlpha )
{
	if( iAlpha > 255 )
		m_iHintBackAlpha = 255;
	else if( iAlpha < 0 )
		m_iHintBackAlpha = 0;
	else
		m_iHintBackAlpha = iAlpha;

	int iRed   = ( m_BackgroundColor<<8  & 0xFF );
	int iGreen = ( m_BackgroundColor<<16 & 0xFF );
	int iBlue  = ( m_BackgroundColor<<24 & 0xFF );

	D3DCOLOR color = D3DCOLOR_ARGB( m_iHintBackAlpha, iRed, iGreen, iBlue );
	m_BackgroundColor = color;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTControl::SetHint( LPCWSTR strHint, LPCWSTR strHintImgPath )
{
	_SetHint(strHint? strHint : L"");
	_SetHintImagePath(strHintImgPath? strHintImgPath : L"");

    
    return S_OK;
}
//--------------------------------------------------------------------------------------
HRESULT CDXUTControl::SetHint( const string &strHint, const string &strHintImgPath )
{
    WCHAR wHint[512], wImgPath[512];

    ZeroMemory( wHint, sizeof( wHint ) );

    ZeroMemory( wImgPath, sizeof( wImgPath ) );

    ANSItoUNICODE( wHint, strHint );

    ANSItoUNICODE( wImgPath, strHintImgPath );

    return SetHint( wHint, wImgPath );
}
//--------------------------------------------------------------------------------------
HRESULT CDXUTControl::SetMultipleHint( LPCWSTR strHint, LPCWSTR strHintImgPath, int Index )
{
    if( strHint == NULL )
    {
        MultipleHint[Index].m_strHint[0] = 0;
    }
    else
    {
        StringCchCopy( MultipleHint[Index].m_strHint, MAX_PATH, strHint );
    }

    if( strHintImgPath == NULL )
    {
        MultipleHint[Index].m_strHintImgPath[0] = 0;
    }
    else
    {
        StringCchCopy( MultipleHint[Index].m_strHintImgPath, MAX_PATH, strHintImgPath );
    }

    return S_OK;
}
//--------------------------------------------------------------------------------------
HRESULT CDXUTControl::SetMultipleHint( const string &strHint, const string &strHintImgPath, int Index )
{
    WCHAR wHint[512], wImgPath[512];

    ZeroMemory( wHint, sizeof( wHint ) );

    ZeroMemory( wImgPath, sizeof( wImgPath ) );

    ANSItoUNICODE( wHint, strHint );

    ANSItoUNICODE( wImgPath, strHintImgPath );

    return SetMultipleHint( wHint, wImgPath, Index );
}
//---------------------------------------------------------------------------------------
void CDXUTControl::SetMultipleHintBufferSize( int iHintBufferSize )
{
	MultipleHint.resize( iHintBufferSize );
}
//---------------------------------------------------------------------------------------
void CDXUTControl::RemoveFirst_SlashN( WCHAR *temp )
{
	WCHAR temp2[MAX_PATH] ;
	ZeroMemory( &temp2, sizeof( temp2 ) );
	StringCchCopy( temp2, MAX_PATH, temp );
	int count = 0; 
	for(int i=1;count < MAX_PATH;i++)
	{
		if(i==1 && temp2[count] == L'\n')
		{
			for(int j=count;j < MAX_PATH;j++)
			{
				temp2[j] = temp2[j+1];
			}
			i = 0;
		} 
		else if(i==12 || temp2[count] == L'\n' )	
			i = 0;

		count++;
	}
	StringCchCopy( temp, MAX_PATH, temp2 );
}
CDXUTElement* CDXUTControl::GetElement( UINT iElement ) 
{ 
	ElementMap::iterator it = m_Elements.find( iElement ) ;
	if( it != m_Elements.end() )
	{
		return it->second ;
	}
	return 0 ;
}
//////////////////////////////////////////////////////////////////////////
void CDXUTControl::OnDrag() 
{
	if(m_pDialog)
		m_pDialog->SendEvent(EVENT_CONTROL_DRAG,true,this);
}
//////////////////////////////////////////////////////////////////////////
void CDXUTControl::OnDrop() 
{
	if(m_pDialog)
		m_pDialog->SendEvent(EVENT_CONTROL_DROP,true,this);
}
//////////////////////////////////////////////////////////////////////////
void CDXUTControl::OnMouseEnter() 
{ 
	m_uCurState = DXUT_STATE_MOUSEOVER ;
	if(m_pDialog && m_bMouseOver == false)
	{
		s_pOnMouseCtrl = this;
		m_pDialog->SendEvent(EVENT_CONTROL_MOUSE_IN ,true,this);
		
	}
	m_bMouseOver = true; 
	
}
//////////////////////////////////////////////////////////////////////////
void CDXUTControl::OnMouseLeave() 
{ 
	m_uCurState = DXUT_STATE_NORMAL ;
	if(m_pDialog&& (s_pOnMouseCtrl == 0 || s_pOnMouseCtrl->GetParent() == m_pDialog ) && m_bMouseOver == true)
	{
		m_bMouseOver = false; 
		s_pOnMouseCtrl = 0;
		m_pDialog->SendEvent(EVENT_CONTROL_MOUSE_OUT ,true,this);		
	}
	
}
//////////////////////////////////////////////////////////////////////////
void CDXUTControl::SetEnabled( bool bEnabled ) 
{
	m_bEnabled = bEnabled ;
	
	if( m_bEnabled )
		m_uCurState = DXUT_STATE_NORMAL ;
	else
		m_uCurState = DXUT_STATE_DISABLED ;
}

void CDXUTControl::SetVisible( bool bVisible )
{ 
	m_bVisible = bVisible; 
	if( m_bVisible )
		m_uCurState = DXUT_STATE_NORMAL ;
	else
	{
		m_uCurState = DXUT_STATE_HIDDEN ;
		m_bMouseOver = false;
	}

	
}
//////////////////////////////////////////////////////////////////////////
void CDXUTControl::fnGetScreenRect(RECT& outrc,CDXUTControl* pCtrl)
{
	RECT rcCtrl = pCtrl->m_rcBoundingBox;
	RECT rcDlg;
	pCtrl->m_pDialog->GetRect(rcDlg);
	
	OffsetRect(&rcCtrl,rcDlg.left,rcDlg.top + pCtrl->m_pDialog->GetCaptionHeight());
	outrc = rcCtrl;
}
//////////////////////////////////////////////////////////////////////////
void CDXUTControl::GetScreenOffset(POINT& ptOut,const POINT& ptScreen) 
{
	CDXUTControl::fnGetScreenOffset(ptOut,ptScreen,this);
}
//////////////////////////////////////////////////////////////////////////

void CDXUTControl::fnGetScreenOffset(POINT& ptOut,const POINT& ptScreen,CDXUTControl* pCtrl)
{
	RECT rcClient;
	rcClient = pCtrl->GetBoundingBox();
	POINT ptDialog;
	pCtrl->GetParent()->GetLocation(ptDialog);							
	
	POINT ptControlItem;							
	ptControlItem.x = ptDialog.x;
	ptControlItem.y = ptDialog.y;							
	ptControlItem.x += rcClient.left;
	ptControlItem.y += rcClient.top ; 							
	ptOut = ptScreen;
	ptOut.x -= ptControlItem.x;
	ptOut.y -= (ptControlItem.y+ pCtrl->GetParent()->GetCaptionHeight());
	ptOut.x =  0 - (ptOut.x);
	ptOut.y =  0 - (ptOut.y);
}
//////////////////////////////////////////////////////////////////////////
void CDXUTControl::OnFocusIn() 
{
	m_uCurState = DXUT_STATE_FOCUS ;
	m_bHasFocus = true; 
	if(m_pDialog)
		m_pDialog->SendEvent(EVENT_CONTROL_FOCUS_IN , true , this );
	
}
void CDXUTControl::OnFocusOut() 
{ 
	m_uCurState = DXUT_STATE_NORMAL ;
	m_bHasFocus = false; 
	if(m_pDialog)
		m_pDialog->SendEvent(EVENT_CONTROL_FOCUS_OUT , true , this );
	
}
//////////////////////////////////////////////////////////////////////////
int  CDXUTControl::GetID() const 
{ 
	return m_ID; 
}
//////////////////////////////////////////////////////////////////////////
void CDXUTControl::SetID( int ID ) 
{ 	
	// change id
// 	if (m_pDialog)
// 	{
// 		m_pDialog->RemoveControl(m_ID);				
// 		m_pDialog->AddControl(this,ID,false);
// 	}
	m_ID = ID; 
}
//////////////////////////////////////////////////////////////////////////
void CDXUTControl::_SetText(const char16* text)
{
	if(text[0] == 0)
	{
		if(m_strText)
			gControlTextFactory.Destroy(m_strText);
		m_strText = 0;
	}
	else
	{
		if(m_strText == 0)
			m_strText = (char16*)gControlTextFactory.Create();
		wcsncpy(m_strText,text,MAX_PATH-1);
		m_strText[MAX_PATH-1] = 0;
	}
	
}
//////////////////////////////////////////////////////////////////////////
const char16* CDXUTControl::_GetText() const
{
	return m_strText ? m_strText : L"";
}
//////////////////////////////////////////////////////////////////////////
void CDXUTControl::_SetHint(const char16* text)
{
	if(text[0] == 0)
	{
		if(m_strHint)
			gControlTextFactory.Destroy(m_strHint);
		m_strHint = 0;
	}
	else
	{
		if(m_strHint== 0)
			m_strHint = (char16*)gControlTextFactory.Create();
		wcsncpy(m_strHint,text,MAX_PATH-1);
		m_strHint[MAX_PATH-1] = 0;
	}
	
}
//////////////////////////////////////////////////////////////////////////
const char16* CDXUTControl::_GetHint() const
{
	return m_strHint ? m_strHint : L"" ;
}
//////////////////////////////////////////////////////////////////////////
void CDXUTControl::_SetHintImagePath(const char16* text)
{
	if(text[0] == 0)
	{
		if(m_strHintImgPath)
			gControlTextFactory.Destroy(m_strHintImgPath);
		m_strHintImgPath = 0;
	}
	else
	{
		if(m_strHintImgPath == 0)
		{
			m_strHintImgPath = (char16*)gControlTextFactory.Create();
		}
		
		wcsncpy(m_strHintImgPath,text,MAX_PATH-1);
		m_strHintImgPath[MAX_PATH-1] = 0;
	}
}
//////////////////////////////////////////////////////////////////////////
const char16* CDXUTControl::_GetHintImagePath()	const
{
	return m_strHintImgPath ? m_strHintImgPath : L"";
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CDXUTDropTarget::CDXUTDropTarget(CDXUTControl* pOwner) : mpOwner(pOwner)
{

}
//////////////////////////////////////////////////////////////////////////
CDXUTDropTarget::~CDXUTDropTarget()
{

}
//////////////////////////////////////////////////////////////////////////
void CDXUTDropTarget::OnDrop(DragDropInfo* pInfo) 
{
	mpOwner->GetParent()->SendEvent(EVENT_CONTROL_DROP,true,mpOwner,(UINT)pInfo);
}
//////////////////////////////////////////////////////////////////////////
void CDXUTDropTarget::OnMouseOver(DragDropInfo* pInfo) 
{	
}
//////////////////////////////////////////////////////////////////////////
void CDXUTDropTarget::OnDropNotTarget(DragDropInfo* pInfo) 
{
	mpOwner->GetParent()->SendEvent(EVENT_CONTROL_DROP_NOT_TARGET,true,mpOwner,(UINT)pInfo);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CDXUTSubDialog::CDXUTSubDialog(CDXUTDialog* pDlg) : CDXUTControl(pDlg) , mbNeedDelete(false)
{
	m_Type = DXUT_CONTROL_SUBDIALOG;
}
//////////////////////////////////////////////////////////////////////////
CDXUTSubDialog::~CDXUTSubDialog()
{
	CDXUTDialog* pDlg = CDXUTDialog::Query(mhHnd);
	if(pDlg)
	{
		pDlg->SetVisible(false);
		if(mbNeedDelete)
		{
			pDlg->SeparateDialogList();
			delete pDlg;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CDXUTSubDialog::OnMouseEnter() 
{
	CDXUTControl::OnMouseEnter();	
}
//////////////////////////////////////////////////////////////////////////
bool CDXUTSubDialog::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	
	return false;
}
//////////////////////////////////////////////////////////////////////////
void CDXUTSubDialog::_ResetLocation()
{
	CDXUTDialog* pDlg = CDXUTDialog::Query(mhHnd);
	if(pDlg)
	{
		POINT ptParent;
		m_pDialog->GetLocation(ptParent);
		pDlg->SetLocation(ptParent.x + m_x,ptParent.y + m_y,false);
		//pDlg->SetSize(m_width,m_height);

	}
}
//////////////////////////////////////////////////////////////////////////
void CDXUTSubDialog::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime ) 
{
	
	_ResetLocation();
}
//////////////////////////////////////////////////////////////////////////
void CDXUTSubDialog::UpdateRects()
{
	CDXUTDialog* pDlg = CDXUTDialog::Query(mhHnd);
	if(pDlg)
	{
		//pDlg->SetSize(m_width,m_height);
	}
	CDXUTControl::UpdateRects();
}
//////////////////////////////////////////////////////////////////////////
void CDXUTSubDialog::SetDialog(CDXUTDialog* pDlg, bool bAutoDelete)
{
	
	if(mbNeedDelete)
	{
		CDXUTDialog* pDlg = CDXUTDialog::Query(mhHnd);
		if(pDlg)
		{
			pDlg->SeparateDialogList();
			pDlg->SetParent(0);
		}
		delete pDlg;
	}

	if(pDlg)
	{
		pDlg->SetParent(this);
		
		mhHnd = pDlg->GetHandle();
	}
	mbNeedDelete = bAutoDelete;

	_ResetLocation();
}
//////////////////////////////////////////////////////////////////////////
CDXUTDialog* CDXUTSubDialog::GetDialog() const
{	
	return CDXUTDialog::Query(mhHnd);
	
}
//////////////////////////////////////////////////////////////////////////
void CDXUTSubDialog::SetVisible( bool bVisible ) 
{
	CDXUTControl::SetVisible(bVisible);
	CDXUTDialog* pDlg = CDXUTDialog::Query(mhHnd);
	if(pDlg)
		pDlg->SetVisible(bVisible);
	
	
}
//////////////////////////////////////////////////////////////////////////
void CDXUTSubDialog::Raise(bool bSet /*= true*/)
{
	CDXUTDialog* pDlg = CDXUTDialog::Query(mhHnd);
	if(pDlg)
	{
		if(bSet)
			pDlg->push_front();
		else
			pDlg->push_back();
	}
}

ElementsQueryer::ElementsQueryer( CDXUTControl* pControl ) : m_pControl( pControl ) 
{
	m_pElementMap = &pControl->m_Elements ;
	m_uQueryNo = 0 ;
}

ElementsQueryer::~ElementsQueryer()
{
}

void ElementsQueryer::InitElement( CDXUTElement* pElement )
{
	SetRectEmpty( &pElement->rcTexture ) ;
	pElement->iTexture = -1 ;
	pElement->iFont = -1 ;
	pElement->TextureColor.States[ DXUT_STATE_NORMAL ]    = 0xffffffff ;
	pElement->TextureColor.States[ DXUT_STATE_DISABLED ]  = 0xc8808080 ;
	pElement->TextureColor.States[ DXUT_STATE_HIDDEN ]    = 0x00000000 ;
	pElement->TextureColor.States[ DXUT_STATE_FOCUS ]     = 0xff000000 ;
	pElement->TextureColor.States[ DXUT_STATE_MOUSEOVER ] = 0xffffffff ;
	pElement->TextureColor.States[ DXUT_STATE_PRESSED]    = 0xffffffff ;
	pElement->TextureColor.States[ DXUT_STATE_UNFOCUS]    = 0xc8c8c8c8 ;
	pElement->TextureColor.Current = pElement->TextureColor.States[ DXUT_STATE_HIDDEN ] ;
}

CDXUTElement* ElementsQueryer::QueryElement( uint uAdress )
{
	CDXUTElement* pElement = 0 ;
	ElementMap::iterator it = m_pElementMap->find( m_uQueryNo ) ;
	if( it == m_pElementMap->end() )
	{
		pElement = new CDXUTElement ;
		InitElement( pElement ) ;
		m_pElementMap->insert( make_pair( m_uQueryNo , pElement ) ) ;
		m_Table.insert( make_pair( uAdress , m_uQueryNo++ ) ) ;
	}
	else
	{
/*		if( !it->second )
		{
			it->second = new CDXUTElement ;
			InitElement( pElement ) ;
		}
		pElement = it->second ;*/	
		m_uQueryNo++ ;
		pElement = QueryElement( uAdress ) ;
	}
	return pElement ;
}

void ElementsQueryer::ClearElement( uint uAdress ) 
{
	pair< QueryTable::iterator , QueryTable::iterator > it = m_Table.equal_range( uAdress ) ;
	while( it.first != it.second )
	{
		ElementMap::iterator itor = m_pElementMap->find( it.first->second ) ;
		if( itor != m_pElementMap->end() )
			m_pElementMap->erase( itor ) ;
		++it.first ;
	}
	m_Table.erase( uAdress ) ;
}


RECT ElementsQueryer::GetControlBoundingBox()
{
	return m_pControl->GetBoundingBox() ;
}

RECT ElementsQueryer::GetControlScreenBoundingBox()
{
	RECT rcScreen = GetControlBoundingBox();
	CDXUTDialog* pDialog = m_pControl->m_pDialog ;
	POINT ptLocation ;
	pDialog->GetLocation( ptLocation ) ;
	assert(ptLocation.x != 0 && ptLocation.y != 0);
	OffsetRect( &rcScreen , ptLocation.x , ptLocation.y ) ;
	return rcScreen ;
}



