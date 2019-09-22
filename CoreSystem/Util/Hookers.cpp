//
// Hookers.cpp
//
#include "stdafx.h"
//#pragma hdrstop
//---------------------------------------------------------------------------
#include "Hookers.h"
#include "CDXUTDialog.h"
#include "CDXUTDialog.h"
//#include "FDO_RoleManager.h"
#pragma hdrstop
using namespace FDO;
using namespace std;

// The singleton instance of CKeyboardHooker.
CKeyboardHooker _KeyboardHooker;

CKeyboardHooker::CKeyboardHooker()
: m_bEnable(false)
{
}

CKeyboardHooker::~CKeyboardHooker()
{
}

bool CKeyboardHooker::RegisterDialogMouseOver(CDXUTDialog* pDialog, KeyboardHookProc pKeybProc)
{
    if (!pDialog || !pKeybProc)
        return false;
    DialogHooker hooker;
    hooker.m_pDialog = pDialog;
    hooker.m_pKeybProc = pKeybProc;
    m_listDialogMouseOver.push_back(hooker);
    return true;
}

bool CKeyboardHooker::Hook(DWORD dwVirtualKeyCode, bool bKeyDown, bool bAltDown, 
    bool bCtrlDown, bool bShiftDown, bool bKeyDownRepeated)
{
    if (false == m_bEnable)//g_RoleManager.GetIsGameStart())
        return false;

    // m_listDialogMouseOver
    if (false == m_listDialogMouseOver.empty()) {
        list<DialogHooker>::iterator it = m_listDialogMouseOver.begin();
        for (; it != m_listDialogMouseOver.end(); ++it) {
            DialogHooker hooker = *it;
            if (!hooker.m_pDialog || !hooker.m_pKeybProc) continue;
            if (CDXUTDialog::MouseDialog() != hooker.m_pDialog) continue;
            return (*hooker.m_pKeybProc)(hooker.m_pDialog, 
                dwVirtualKeyCode, bKeyDown, bAltDown, 
                bCtrlDown, bShiftDown, bKeyDownRepeated);
        } // end for
    } // end if
    
    return false;
}

