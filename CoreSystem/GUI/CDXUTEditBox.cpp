//
// CDXUTEditBox.cpp
//
#include "stdafx.h"
//#pragma hdrstop
//---------------------------------------------------------------------------
#include <strsafe.h>
#include "CDXUTTimer.h"
#include "FDOUT.h"
#include "PacketReader.h"
#include "JLAnimation2DManager.h"
//#include "HUISystemSet.h"
#include <cmath>
#include "J_SoundManager.h"
#include <stdio.h>
#include "iniReader.h"
#include "CFDOXML.h"
#include "DialogType.h"
#include "CDXUTDialog.h"



using namespace FDO;

#pragma hdrstop
#include "CDXUTEditBox.h"
//--------------------------------------------------------------------------------------
// Static member initialization
//--------------------------------------------------------------------------------------
HINSTANCE CUniBuffer::s_hDll = NULL;
HRESULT (WINAPI *CUniBuffer::_ScriptApplyDigitSubstitution)( const SCRIPT_DIGITSUBSTITUTE*, SCRIPT_CONTROL*, SCRIPT_STATE* ) = Dummy_ScriptApplyDigitSubstitution;
HRESULT (WINAPI *CUniBuffer::_ScriptStringAnalyse)( HDC, const void *, int, int, int, DWORD, int, SCRIPT_CONTROL*, SCRIPT_STATE*, const int*, SCRIPT_TABDEF*, const BYTE*, SCRIPT_STRING_ANALYSIS* ) = Dummy_ScriptStringAnalyse;
HRESULT (WINAPI *CUniBuffer::_ScriptStringCPtoX)( SCRIPT_STRING_ANALYSIS, int, BOOL, int* ) = Dummy_ScriptStringCPtoX;
HRESULT (WINAPI *CUniBuffer::_ScriptStringXtoCP)( SCRIPT_STRING_ANALYSIS, int, int*, int* ) = Dummy_ScriptStringXtoCP;
HRESULT (WINAPI *CUniBuffer::_ScriptStringFree)( SCRIPT_STRING_ANALYSIS* ) = Dummy_ScriptStringFree;
const SCRIPT_LOGATTR* (WINAPI *CUniBuffer::_ScriptString_pLogAttr)( SCRIPT_STRING_ANALYSIS ) = Dummy_ScriptString_pLogAttr;
const int* (WINAPI *CUniBuffer::_ScriptString_pcOutChars)( SCRIPT_STRING_ANALYSIS ) = Dummy_ScriptString_pcOutChars;
bool CDXUTEditBox::s_bHideCaret;   // If true, we don't render the caret.

//--------------------------------------------------------------------------------------
// CDXUTEditBox class
//--------------------------------------------------------------------------------------

// When scrolling, EDITBOX_SCROLLEXTENT is reciprocal of the amount to scroll.
// If EDITBOX_SCROLLEXTENT = 4, then we scroll 1/4 of the control each time.
#define EDITBOX_SCROLLEXTENT 4

//--------------------------------------------------------------------------------------

void CDXUTEditBox::SetDialog( CDXUTDialog *pDialog ) { m_pDialog = pDialog; }

CDXUTEditBox::CDXUTEditBox( CDXUTDialog *pDialog )
: DosKey(10) 
, m_bRecord(false)
, m_bDrawFrame(false)
, mpExr(0)
, m_Buffer(1, &m_InputPlugin)
{
    m_Type = DXUT_CONTROL_EDITBOX;
    m_pDialog = pDialog;

    m_nBorder = 4;  // Default border width
    m_nSpacing = 3;  // Default spacing

    m_bCaretOn = true;
    m_dfBlink = GetCaretBlinkTime() * 0.001f;
    m_dfLastBlink = DXUTGetGlobalTimer()->GetAbsoluteTime();
    s_bHideCaret = false;
    m_nFirstVisible = 0;
    m_TextColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
    m_SelTextColor = D3DCOLOR_ARGB( 255, 0, 0, 0 );
    m_SelBkColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
    m_CaretColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
    m_FrameColor =0xFF000000;
    m_nCaret = m_nSelStart = 0;
    m_bInsertMode = true;

    m_bMouseDrag = false;

    // ming add >>
    m_bPassword = false;
    // ming add <<

	m_nInputCount = 0;
}


//--------------------------------------------------------------------------------------
CDXUTEditBox::~CDXUTEditBox()
{
}
//////////////////////////////////////////////////////////////////////////
static TdgHandle				ghLengthResponser;
static GameCommand::Executer*	gpExecuter = 0;
void CDXUTEditBox::SetLimitWarn(::GameCommand::Executer* pExr,TdgHandle hCallBack)
{
	ghLengthResponser = hCallBack;
	gpExecuter = pExr;
}
//////////////////////////////////////////////////////////////////////////
void CDXUTEditBox::_OnLimitString(CUniBuffer& uniBuffer)
{
	if(gpExecuter)
	{
		EditBoxLimitWarn eblw;
		eblw.text		= uniBuffer.GetBuffer();
		eblw.nSize		= uniBuffer.GetBufferSize();
		eblw.nLimitSize	= uniBuffer.GetInputLimit();
		eblw.pEditBox	= this;
		gpExecuter->RunLight<const EditBoxLimitWarn& >(ghLengthResponser , eblw);
	}
}

//--------------------------------------------------------------------------------------
// PlaceCaret: Set the caret to a character position, and adjust the scrolling if
//             necessary.
//--------------------------------------------------------------------------------------
void CDXUTEditBox::PlaceCaret( int nCP )
{
    assert( nCP >= 0 && nCP <= m_Buffer.GetTextSize() );
	m_nCaret = nCP;
// 	if( nCP > m_Buffer.GetTextSize() )
// 	{
// 		nCP = m_Buffer.GetTextSize();
// 	}
// 	else if( nCP < 0 )
// 	{
// 		nCP = 0;
// 	}
    

    // Obtain the X offset of the character.
    int nX1st, nX, nX2;
    m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nX1st );  // 1st visible char
    m_Buffer.CPtoX( nCP, FALSE, &nX );  // LEAD
    // If nCP is the NULL terminator, get the leading edge instead of trailing.
    if( nCP == m_Buffer.GetTextSize() )
        nX2 = nX;
    else
        m_Buffer.CPtoX( nCP, TRUE, &nX2 );  // TRAIL

    // If the left edge of the char is smaller than the left edge of the 1st visible char,
    // we need to scroll left until this char is visible.
    if( nX < nX1st )
    {
        // Simply make the first visible character the char at the new caret position.
        m_nFirstVisible = nCP;
    }
    else
    // If the right of the character is bigger than the offset of the control's
    // right edge, we need to scroll right to this character.
    if( nX2 > nX1st + RectWidth( m_rcText ) )
    {
        // Compute the X of the new left-most pixel
        int nXNewLeft = nX2 - RectWidth( m_rcText );

        // Compute the char position of this character
        int nCPNew1st, nNewTrail;
        m_Buffer.XtoCP( nXNewLeft, &nCPNew1st, &nNewTrail );

        // If this coordinate is not on a character border,
        // start from the next character so that the caret
        // position does not fall outside the text rectangle.
        int nXNew1st;
        m_Buffer.CPtoX( nCPNew1st, FALSE, &nXNew1st );
        if( nXNew1st < nXNewLeft )
            ++nCPNew1st;

        m_nFirstVisible = nCPNew1st;
    }
}


//--------------------------------------------------------------------------------------
void CDXUTEditBox::ClearText()
{
    m_Buffer.Clear();
    m_nFirstVisible = 0;
    PlaceCaret( 0 );
    m_nSelStart = 0;
}


//--------------------------------------------------------------------------------------
void CDXUTEditBox::SetText( LPCWSTR wszText, bool bSelected )
{
    Assert( wszText != NULL , "");

    m_Buffer.SetText( wszText, 0 );
    m_nFirstVisible = 0;
    // Move the caret to the end of the text
    PlaceCaret( m_Buffer.GetTextSize() );
    m_nSelStart = bSelected ? 0 : m_nCaret;
}


//--------------------------------------------------------------------------------------
void CDXUTEditBox::SetText( const string &szText, bool bSelected )
{
    SetText( ANSItoUNICODE(szText), bSelected );
}

//--------------------------------------------------------------------------------------
void CDXUTEditBox::SetText( const int &iText, bool bSelected )
{
    char temp[20];
    _itoa( iText, temp, 10 );
    SetText( temp, bSelected );
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTEditBox::GetTextCopy( LPWSTR strDest, UINT bufferCount )
{
    assert( strDest );

    StringCchCopy( strDest, bufferCount, m_Buffer.GetBuffer() );

    return S_OK;
}


//--------------------------------------------------------------------------------------
void CDXUTEditBox::DeleteSelectionText()
{
    int nFirst = __min( m_nCaret, m_nSelStart );
    int nLast = __max( m_nCaret, m_nSelStart );
    // Update caret and selection
    PlaceCaret( nFirst );
    m_nSelStart = m_nCaret;
    // Remove the characters
    for( int i = nFirst; i < nLast; ++i )
        m_Buffer.RemoveChar( nFirst );
}


//--------------------------------------------------------------------------------------
void CDXUTEditBox::UpdateRects()
{
    CDXUTControl::UpdateRects();

    // Update the text rectangle
    m_rcText = m_rcBoundingBox;
    // First inflate by m_nBorder to compute render rects
    InflateRect( &m_rcText, -m_nBorder, -m_nBorder );

    // Update the render rectangles
    m_rcRender[0] = m_rcText;
    SetRect( &m_rcRender[1], m_rcBoundingBox.left, m_rcBoundingBox.top, m_rcText.left, m_rcText.top );
    SetRect( &m_rcRender[2], m_rcText.left, m_rcBoundingBox.top, m_rcText.right, m_rcText.top );
    SetRect( &m_rcRender[3], m_rcText.right, m_rcBoundingBox.top, m_rcBoundingBox.right, m_rcText.top );
    SetRect( &m_rcRender[4], m_rcBoundingBox.left, m_rcText.top, m_rcText.left, m_rcText.bottom );
    SetRect( &m_rcRender[5], m_rcText.right, m_rcText.top, m_rcBoundingBox.right, m_rcText.bottom );
    SetRect( &m_rcRender[6], m_rcBoundingBox.left, m_rcText.bottom, m_rcText.left, m_rcBoundingBox.bottom );
    SetRect( &m_rcRender[7], m_rcText.left, m_rcText.bottom, m_rcText.right, m_rcBoundingBox.bottom );
    SetRect( &m_rcRender[8], m_rcText.right, m_rcText.bottom, m_rcBoundingBox.right, m_rcBoundingBox.bottom );

    // Inflate further by m_nSpacing
    InflateRect( &m_rcText, -m_nSpacing, -m_nSpacing );
}


void CDXUTEditBox::CopyToClipboard()
{
    // Copy the selection text to the clipboard
    if( m_nCaret != m_nSelStart && OpenClipboard( NULL ) )
    {
        EmptyClipboard();

        HGLOBAL hBlock = GlobalAlloc( GMEM_MOVEABLE, sizeof(WCHAR) * ( m_Buffer.GetTextSize() + 1 ) );
        if( hBlock )
        {
            WCHAR *pwszText = (WCHAR*)GlobalLock( hBlock );
            if( pwszText )
            {
                int nFirst = __min( m_nCaret, m_nSelStart );
                int nLast = __max( m_nCaret, m_nSelStart );
                if( nLast - nFirst > 0 )
                    CopyMemory( pwszText, m_Buffer.GetBuffer() + nFirst, (nLast - nFirst) * sizeof(WCHAR) );
                pwszText[nLast - nFirst] = L'\0';  // Terminate it
                GlobalUnlock( hBlock );
            }
            SetClipboardData( CF_UNICODETEXT, hBlock );
        }
        CloseClipboard();
        // We must not free the object until CloseClipboard is called.
        if( hBlock )
            GlobalFree( hBlock );
    }
}


void CDXUTEditBox::PasteFromClipboard()
{
    DeleteSelectionText();

    if( OpenClipboard( NULL ) )
    {
        HANDLE handle = GetClipboardData( CF_UNICODETEXT );
        if( handle )
        {
            // Convert the ANSI string to Unicode, then
            // insert to our buffer.
            WCHAR* pwszText = (WCHAR*)GlobalLock( handle );
            if( pwszText )
            {
				//過濾
				uint nOldSize = m_Buffer.GetTextSize();
				CUniBuffer::InsertResult cir(m_Buffer.InsertString( m_nCaret, pwszText ));
				if( cir == CUniBuffer::InsertResult::RESULT_OK)
				{
					//uint nOldSize = m_Buffer.GetTextSize();
					StartFilter();
					uint nNewSize = m_Buffer.GetTextSize();
					PlaceCaret( m_nCaret + nNewSize - nOldSize );
				}
				else if(cir == CUniBuffer::InsertResult::RESULT_MAXIUMLENGTH)
				{
					_OnLimitString(m_Buffer);
				}
				
				// Copy all characters up to null.
                
// 				if( m_Buffer.InsertString( m_nCaret, pResult ) )
// 				{
// 					//PlaceCaret( m_nCaret + m_Buffer.GetTextSize() );
//                     PlaceCaret( m_nCaret + lstrlenW( pResult ) );
// 				}
                m_nSelStart = m_nCaret;
                GlobalUnlock( handle );
            }
        }
        CloseClipboard();
    }
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CDXUTEditBox::StartFilter()
{
	uint nOldSize = m_Buffer.GetTextSize();
	wstring inputStr(m_Buffer.GetBuffer());
	m_InputPlugin.RunInputPlugin( inputStr );
	wcsncpy( m_Buffer.GetBuffer2() , inputStr.c_str() , m_Buffer.GetBufferSize() );
	m_Buffer.GetBuffer2()[m_Buffer.GetBufferSize() - 1] =0;
	uint nNewSize = m_Buffer.GetTextSize();
	//PlaceCaret( m_nCaret + nNewSize - nOldSize );
}


//--------------------------------------------------------------------------------------
bool CDXUTEditBox::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    bool bHandled = false;

    switch( uMsg )
    {
        case WM_KEYDOWN:
        {
            switch( wParam )
            {
                case VK_TAB:
                    // We don't process Tab in case keyboard input is enabled and the user
                    // wishes to Tab to other controls.
                    break;

                case VK_HOME:
                    PlaceCaret( 0 );
                    if( GetKeyState( VK_SHIFT ) >= 0 )
                        // Shift is not down. Update selection
                        // start along with the caret.
                        m_nSelStart = m_nCaret;
                    ResetCaretBlink();
                    bHandled = true;
                    break;

                case VK_END:
                    PlaceCaret( m_Buffer.GetTextSize() );
                    if( GetKeyState( VK_SHIFT ) >= 0 )
                        // Shift is not down. Update selection
                        // start along with the caret.
                        m_nSelStart = m_nCaret;
                    ResetCaretBlink();
                    bHandled = true;
                    break;

                case VK_INSERT:
                    if( GetKeyState( VK_CONTROL ) < 0 )
                    {
                        // Control Insert. Copy to clipboard
                        CopyToClipboard();
                        bHandled = true;
                    } else
                    if( GetKeyState( VK_SHIFT ) < 0 )
                    {
                        // Shift Insert. Paste from clipboard
                        PasteFromClipboard();
                        bHandled = true;
                    } else
                    {
                        // Toggle caret insert mode
                        // m_bInsertMode = !m_bInsertMode;
                    }
                    break;

                case VK_DELETE:
                    // Check if there is a text selection.
                    if( m_nCaret != m_nSelStart )
                    {
                        DeleteSelectionText();
						StartFilter();
                        m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
                    }
                    else
                    {
                        // Deleting one character
                        if( m_Buffer.RemoveChar( m_nCaret ) )
						{
							StartFilter();
                            m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
						}
                    }
                    ResetCaretBlink();
                    bHandled = true;
                    break;

                case VK_LEFT:
                    if( GetKeyState( VK_CONTROL ) < 0 )
                    {
                        // Control is down. Move the caret to a new item
                        // instead of a character.
                        m_Buffer.GetPriorItemPos( m_nCaret, &m_nCaret );
                        PlaceCaret( m_nCaret );
                    }
                    else
                    if( m_nCaret > 0 )
                        PlaceCaret( m_nCaret - 1 );
                    if( GetKeyState( VK_SHIFT ) >= 0 )
                        // Shift is not down. Update selection
                        // start along with the caret.
                        m_nSelStart = m_nCaret;
                    ResetCaretBlink();
                    bHandled = true;
                    break;

                case VK_RIGHT:
                    if( GetKeyState( VK_CONTROL ) < 0 )
                    {
                        // Control is down. Move the caret to a new item
                        // instead of a character.
                        m_Buffer.GetNextItemPos( m_nCaret, &m_nCaret );
                        PlaceCaret( m_nCaret );
                    }
                    else
                    if( m_nCaret < m_Buffer.GetTextSize() )
                        PlaceCaret( m_nCaret + 1 );
                    if( GetKeyState( VK_SHIFT ) >= 0 )
                        // Shift is not down. Update selection
                        // start along with the caret.
                        m_nSelStart = m_nCaret;
                    ResetCaretBlink();
                    bHandled = true;
                    break;

				case VK_UP:					
					{
						const WCHAR* str = DosKey::GetPrev();
						if(str && IsRecordable())
							SetText(str);						
						// Trap up and down arrows so that the dialog
						// does not switch focus to another control.
						bHandled = true;
					}
					break;
				case VK_DOWN:					
					{
						const WCHAR* str = DosKey::GetNext();
						if(str && IsRecordable())
							SetText(str);						
						// Trap up and down arrows so that the dialog
						// does not switch focus to another control.
						bHandled = true;
					}
                    
                    break;

                default:
                    bHandled = wParam != VK_ESCAPE;  // Let the application handle Esc.
            }
        }
    }
    return bHandled;
}


//--------------------------------------------------------------------------------------
bool CDXUTEditBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    switch( uMsg )
    {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        {
            if( !m_bHasFocus )
                m_pDialog->RequestFocus( this );

            if( !ContainsPoint( pt ) )
                return false;

            //音效 *** modify
            PlaySoundWAV();  //under control class

            m_bMouseDrag = true;
            SetCapture( DXUTGetHWND() );
            // Determine the character corresponding to the coordinates.
            int nCP, nTrail, nX1st;
            m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nX1st );  // X offset of the 1st visible char
            if( SUCCEEDED( m_Buffer.XtoCP( pt.x - m_rcText.left + nX1st, &nCP, &nTrail ) ) )
            {
                // Cap at the NULL character.
                if( nTrail && nCP < m_Buffer.GetTextSize() )
                    PlaceCaret( nCP + 1 );
                else
                    PlaceCaret( nCP );
                m_nSelStart = m_nCaret;
                ResetCaretBlink();
            }
            return true;
        }

        case WM_LBUTTONUP:
            ReleaseCapture();
            m_bMouseDrag = false;
            break;

        case WM_MOUSEMOVE:
            if( m_bMouseDrag )
            {
                // Determine the character corresponding to the coordinates.
                int nCP, nTrail, nX1st;
                m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nX1st );  // X offset of the 1st visible char
                if( SUCCEEDED( m_Buffer.XtoCP( pt.x - m_rcText.left + nX1st, &nCP, &nTrail ) ) )
                {
                    // Cap at the NULL character.
                    if( nTrail && nCP < m_Buffer.GetTextSize() )
                        PlaceCaret( nCP + 1 );
                    else
                        PlaceCaret( nCP );
                }
            }
            break;
    }

    return false;
}
static bool gbHasEditFocus = false;
//--------------------------------------------------------------------------------------
void CDXUTEditBox::OnFocusOut()
{
	CDXUTControl::OnFocusOut();
	gbHasEditFocus = false;
}
//--------------------------------------------------------------------------------------
void CDXUTEditBox::OnFocusIn()
{	
	gbHasEditFocus = true;
    CDXUTControl::OnFocusIn();
    ResetCaretBlink();
}
//--------------------------------------------------------------------------------------
bool CDXUTEditBox::HasEditFocus()	
{
	return gbHasEditFocus;
}
bool CDXUTEditBox::IsRecordable() const
{
	return m_bRecord;
}
//--------------------------------------------------------------------------------------
void CDXUTEditBox::EnableRecord(bool bEnable)
{
	m_bRecord = bEnable;
}

////////////////////////////////////////////////////////////////////////////////
//設定要限制的字數
////////////////////////////////////////////////////////////////////////////////
void CDXUTEditBox::SetInputCount(int nCount)
{
	m_Buffer.SetInputLimit( nCount );
}


//--------------------------------------------------------------------------------------
bool CDXUTEditBox::MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    switch( uMsg )
    {
        case WM_CHAR:
        {
            switch( (WCHAR)wParam )
            {
				
				
				
				
                // Backspace
                case VK_BACK:
                {

                    // If there's a selection, treat this
                    // like a delete key.
                    if( m_nCaret != m_nSelStart )
                    {
                        DeleteSelectionText();
						StartFilter();
                        m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
                    }
                    else
                    if( m_nCaret > 0 )
                    {
                        // Move the caret, then delete the char.
                        PlaceCaret( m_nCaret - 1 );
                        m_nSelStart = m_nCaret;
                        m_Buffer.RemoveChar( m_nCaret );
						StartFilter();
                        m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
                    }
                    ResetCaretBlink();
                    break;
                }

                case 24:        // Ctrl-X Cut
                case VK_CANCEL: // Ctrl-C Copy
                {
                    CopyToClipboard();

                    // If the key is Ctrl-X, delete the selection too.
                    if( (WCHAR)wParam == 24 )
                    {
                        DeleteSelectionText();
                        m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
                    }

                    break;
                }

                // Ctrl-V Paste
                case 22:
                {
                    PasteFromClipboard();
                    m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
                    break;
                }

                // Ctrl-A Select All
                case 1:
                    if( m_nSelStart == m_nCaret )
                    {
                        m_nSelStart = 0;
                        PlaceCaret( m_Buffer.GetTextSize() );
                    }
                    break;

                case VK_RETURN:
                    // Invoke the callback when the user presses Enter.
					if(IsRecordable() )
						DosKey::Push( GetText() ) ;
                    m_pDialog->SendEvent( EVENT_EDITBOX_STRING, true, this );
					
                    break;

                // Junk characters we don't want in the string
                case 26:  // Ctrl Z
                case 2:   // Ctrl B
                case 14:  // Ctrl N
                case 19:  // Ctrl S
                case 4:   // Ctrl D
                case 6:   // Ctrl F
                case 7:   // Ctrl G
                case 10:  // Ctrl J
                case 11:  // Ctrl K
                case 12:  // Ctrl L
                case 17:  // Ctrl Q
                case 23:  // Ctrl W
                case 5:   // Ctrl E
                case 18:  // Ctrl R
                case 20:  // Ctrl T
                case 25:  // Ctrl Y
                case 21:  // Ctrl U
                case 9:   // Ctrl I
                case 15:  // Ctrl O
                case 16:  // Ctrl P
                case 27:  // Ctrl [
                case 29:  // Ctrl ]
                case 28:  // Ctrl \ 
                    break;

			
				//在這裡擋輸入
                default:
                {
					if(mpExr)
					{
						EditBoxCheckParam ebcp;
						ebcp.text = m_Buffer.GetBuffer();
						ebcp.nSize = m_Buffer.GetBufferSize();
						ebcp.newChar = (WCHAR)wParam;
						ebcp.bSuccess = true;
						mpExr->RunLight<EditBoxCheckParam*>(mhChecker , &ebcp);
						if( ebcp.bSuccess == false)
						{
							return false;
						}
					}
                    // If there's a selection and the user
                    // starts to type, the selection should
                    // be deleted.
                    if( m_nCaret != m_nSelStart )
					{
                        DeleteSelectionText();	
					}
										
                    // If we are in overwrite mode and there is already
                    // a char at the caret's position, simply replace it.
                    // Otherwise, we insert the char as normal.
                    if( !m_bInsertMode && m_nCaret < m_Buffer.GetTextSize() )
                    {
                        m_Buffer[m_nCaret] = (WCHAR)wParam;
                        PlaceCaret( m_nCaret + 1 );
                        m_nSelStart = m_nCaret;
                    } 					
					else
                    {
                        // Insert the char
						uint nOldSize = m_Buffer.GetTextSize();
						CUniBuffer::InsertResult cir(m_Buffer.InsertChar( m_nCaret, (WCHAR)wParam, m_InputPlugin ));

                        if( cir == CUniBuffer::InsertResult::RESULT_OK )
                        {
							StartFilter();
							uint nNewSize = m_Buffer.GetTextSize();
							PlaceCaret( m_nCaret + nNewSize - nOldSize );
							m_nSelStart = m_nCaret;
						}
						else if(cir == CUniBuffer::InsertResult::RESULT_MAXIUMLENGTH)
						{
							_OnLimitString(m_Buffer);
						}
                    }

                    ResetCaretBlink();
                    m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
                }
            }
            return true;
        }

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            if( wParam == VK_RETURN )
            {
                DWORD dwMask = (1 << 29);
                if( (lParam & dwMask) != 0 ) // 判斷Alt是否被按下
                {
                    m_pDialog->SendEvent( EVENT_EDITBOX_ALT_ENTER, true, this );
                    return true;
                }
                else if( GetKeyState(VK_CONTROL) & 0x8000 ) // 判斷Ctrl是否被按下
                {
                    m_pDialog->SendEvent( EVENT_EDITBOX_CTRL_ENTER, true, this );
                    return true;
                }
                else if( GetKeyState(VK_SHIFT) & 0x8000 ) // 判斷Shift是否被按下
                {
                    m_pDialog->SendEvent( EVENT_EDITBOX_SHIFT_ENTER, true, this );
                    return true;
                }
            }
            break;
        }
    }
    return false;
}

//--------------------------------------------------------------------------------------
void CDXUTEditBox::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    if( m_bVisible == false )
        return;

    int nSelStartX = 0, nCaretX = 0;  // Left and right X cordinates of the selection region

    CDXUTElement* pElement = GetElement( 0 );
    if( pElement )
    {
        m_Buffer.SetFontNode( m_pDialog->GetFont( pElement->iFont ) );
        PlaceCaret( m_nCaret );  // Call PlaceCaret now that we have the font info (node),
                                 // so that scrolling can be handled.
    }

    if (m_bDrawFrame) {
		RECT rcFrame = m_rcText;							//Darkness Add
		InflateRect( &rcFrame, 3, 0 );						//Darkness Add
        m_pDialog->DrawFrameRect(&rcFrame, m_FrameColor);	//Darkness Add  m_rcText
    } // end if

    // Render the control graphics
    for( int e = 0; e < 9; ++e )
    {
        pElement = GetElement( e ) ;
		pElement->TextureColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );
        m_pDialog->DrawSprite( pElement, &m_rcRender[e] );
    }

    //
    // Compute the X coordinates of the first visible character.
    //
    int nXFirst;
    m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nXFirst );

    //
    // Compute the X coordinates of the selection rectangle
    //
    m_Buffer.CPtoX( m_nCaret, FALSE, &nCaretX );
    if( m_nCaret != m_nSelStart )
        m_Buffer.CPtoX( m_nSelStart, FALSE, &nSelStartX );
    else
        nSelStartX = nCaretX;

    //
    // Render the selection rectangle
    //
    RECT rcSelection;  // Make this available for rendering selected text
    if( m_nCaret != m_nSelStart )
    {
        int nSelLeftX = nCaretX, nSelRightX = nSelStartX;
        // Swap if left is bigger than right
        if( nSelLeftX > nSelRightX )
            { int nTemp = nSelLeftX; nSelLeftX = nSelRightX; nSelRightX = nTemp; }

        SetRect( &rcSelection, nSelLeftX, m_rcText.top, nSelRightX, m_rcText.bottom );
        OffsetRect( &rcSelection, m_rcText.left - nXFirst, 0 );
        IntersectRect( &rcSelection, &m_rcText, &rcSelection );
        m_pDialog->DrawRect( &rcSelection, m_SelBkColor );
    }

    //
    // Render the text
    //
    // Element 0 for text
	GetElement( 0 )->FontColor.Current = m_TextColor;

    // ming edit >> 把密碼的字串換成STAR畫出來
    LPCWSTR strText;
    strText = m_Buffer.GetBuffer() + m_nFirstVisible;

    WCHAR wszOutput[256] = { '\0' };
    if( m_bPassword )
    {
        wszOutput[0] = '\0';
        int iLen = wcslen( strText );
        for( int i=0; i<iLen; ++i )
            StringCchCat( wszOutput, 256, L"*" );

        strText = wszOutput;
    }
	m_pDialog->DrawPlainText( strText, GetElement( 0 ), &m_rcText );

    // Render the selected text
    if( m_nCaret != m_nSelStart )
    {
        int nFirstToRender = __max( m_nFirstVisible, __min( m_nSelStart, m_nCaret ) );
        int nNumChatToRender = __max( m_nSelStart, m_nCaret ) - nFirstToRender;
		GetElement( 0 )->FontColor.Current = m_SelTextColor;

        // ming edit >> 把密碼的字串換成STAR畫出來
        strText = m_Buffer.GetBuffer() + nFirstToRender;
        if( m_bPassword )
        {
            wszOutput[0] = '\0';
            int iLen = wcslen( strText );
            for( int i=0; i<iLen; ++i )
                StringCchCat( wszOutput, 256, L"*" );

            strText = wszOutput;
        }
		 m_pDialog->DrawPlainText(strText, GetElement( 0 ), &rcSelection, m_bShadow, nNumChatToRender);
    }

    //
    // Blink the caret
    //
    if( DXUTGetGlobalTimer()->GetAbsoluteTime() - m_dfLastBlink >= m_dfBlink )
    {
        m_bCaretOn = !m_bCaretOn;
        m_dfLastBlink = DXUTGetGlobalTimer()->GetAbsoluteTime();
    }

    //
    // Render the caret if this control has the focus
    //
    if( m_bHasFocus && m_bCaretOn && !s_bHideCaret )
    {
        // Start the rectangle with insert mode caret
        RECT rcCaret = { m_rcText.left - nXFirst + nCaretX - 1, m_rcText.top,
                         m_rcText.left - nXFirst + nCaretX + 1, m_rcText.bottom };

        // If we are in overwrite mode, adjust the caret rectangle
        // to fill the entire character.
        if( !m_bInsertMode )
        {
            // Obtain the right edge X coord of the current character
            int nRightEdgeX;
            m_Buffer.CPtoX( m_nCaret, TRUE, &nRightEdgeX );
            rcCaret.right = m_rcText.left - nXFirst + nRightEdgeX;
        }

        m_pDialog->DrawRect( &rcCaret, m_CaretColor );
    }
}


#define IN_FLOAT_CHARSET( c ) \
    ( (c) == L'-' || (c) == L'.' || ( (c) >= L'0' && (c) <= L'9' ) )

void CDXUTEditBox::ParseFloatArray( float *pNumbers, int nCount )
{
    int nWritten = 0;  // Number of floats written
    const WCHAR *pToken, *pEnd;
    WCHAR wszToken[60];

    pToken = m_Buffer.GetBuffer();
    while( nWritten < nCount && *pToken != L'\0' )
    {
        // Skip leading spaces
        while( *pToken == L' ' )
            ++pToken;

        if( *pToken == L'\0' )
            break;

        // Locate the end of number
        pEnd = pToken;
        while( IN_FLOAT_CHARSET( *pEnd ) )
            ++pEnd;

        // Copy the token to our buffer
        int nTokenLen = __min( sizeof(wszToken) / sizeof(wszToken[0]) - 1, int(pEnd - pToken) );
        StringCchCopy( wszToken, nTokenLen, pToken );
        *pNumbers = (float)wcstod( wszToken, NULL );
        ++nWritten;
        ++pNumbers;
        pToken = pEnd;
    }
}


void CDXUTEditBox::SetTextFloatArray( const float *pNumbers, int nCount )
{
    WCHAR wszBuffer[512] = {0};
    WCHAR wszTmp[64];
    
    if( pNumbers == NULL )
        return;
        
    for( int i = 0; i < nCount; ++i )
    {
        StringCchPrintf( wszTmp, 64, L"%.4f ", pNumbers[i] );
        StringCchCat( wszBuffer, 512, wszTmp );
    }

    // Don't want the last space
    if( nCount > 0 && wcslen(wszBuffer) > 0 )
        wszBuffer[wcslen(wszBuffer)-1] = 0;

    SetText( wszBuffer );
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CDXUTEditBox::SetInputMode(EditBoxPlugin::InputMode mode, sint nMaxNum, sint nMinNum)
{ 
	m_InputPlugin.SetInputMode( mode, nMaxNum, nMinNum ); 
}

string CDXUTEditBox::GetTextA()
{
    return UNICODEtoANSI( GetText() );
}

wstring CDXUTEditBox::GetTextAW()
{
    return GetText();
}

int CDXUTEditBox::GetTextI()
{
    return atoi( GetTextA().c_str() );
}

void CDXUTEditBox::ClearTexture()
{
    for( int i=0; i<9; ++i )
    {
        SetTexture( "", 0, i );
    }
}

////////////////////////////////////////////////////////////////////////////////
// 設定輸入的字串使用星號顯示
////////////////////////////////////////////////////////////////////////////////
void CDXUTEditBox::SetStarText(bool bPassword)
{
	m_bPassword = bPassword;  
}


//---------------------------------------------------------------------------
DosKey::DosKey(int nStackCount) : mnStackCount(nStackCount),mCurString(mStrs.begin())
{

}
//////////////////////////////////////////////////////////////////////////
DosKey::~DosKey()
{

}
//////////////////////////////////////////////////////////////////////////
void DosKey::Push(const WCHAR* str)
{
	if(wcslen(str))
	{
		if( std::find( mStrs.begin(), mStrs.end(), std::wstring( str ) ) == mStrs.end() )
		{
			mStrs.push_front(str);
			while((int)mStrs.size() > mnStackCount)
			{
				mStrs.pop_back();
			}

			mCurString = mStrs.begin();
		}
	}	
}
//////////////////////////////////////////////////////////////////////////
const WCHAR* DosKey::GetPrev()
{
	if(mCurString != mStrs.end())
	{
		const WCHAR* str = (*mCurString).c_str();
		++mCurString;
		return str;
	}
	
	return 0;
}
//////////////////////////////////////////////////////////////////////////
const WCHAR* DosKey::GetNext()
{
	const WCHAR* str = 0; 
	if(mCurString != mStrs.end())
		 str = (*mCurString).c_str();
	if(mCurString != mStrs.begin())
	{		
		--mCurString;		
	}
	return str;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CUniBuffer::InsertResult::InsertResult(const CUniBuffer::InsertResult& nCode)
: mnCode(nCode.mnCode)
{

}
//////////////////////////////////////////////////////////////////////////
CUniBuffer::InsertResult::InsertResult(CUniBuffer::InsertResult::Result nCode)
: mnCode(nCode)
{

}
//////////////////////////////////////////////////////////////////////////
CUniBuffer::InsertResult::~InsertResult()
{

}
//////////////////////////////////////////////////////////////////////////
bool CUniBuffer::InsertResult::operator==(const CUniBuffer::InsertResult& nCode)const
{
	return nCode.mnCode == mnCode;
}
//////////////////////////////////////////////////////////////////////////
bool CUniBuffer::InsertResult::operator==(const Result& nCode)const
{
	return mnCode == nCode;
}
//////////////////////////////////////////////////////////////////////////
void CUniBuffer::InsertResult::operator=(const CUniBuffer::InsertResult& nCode)
{
	mnCode = nCode.mnCode ;
}
	

