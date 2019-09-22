//
// Mousetrap.cpp
//
#include "stdafx.h"
//#pragma hdrstop
//---------------------------------------------------------------------------
#include "CDXUTDialog.h"
#include "CDXUTControl.h"
#include "Mousetrap.h"
#pragma hdrstop
// There is a global instance of CMousetrap.
CMousetrap g_Mousetrap;

// Static variables.
CDXUTDialog* CMousetrap::s_pDialogTrap = NULL;
CDXUTControl* CMousetrap::s_pControlTrap = NULL;

CMousetrap::CMousetrap()
{
}

CMousetrap::~CMousetrap()
{
}

void CMousetrap::CaptureMouse(CDXUTDialog* pDialog)
{
    s_pDialogTrap = pDialog;
    s_pControlTrap = NULL;
}

void CMousetrap::CaptureMouse(CDXUTControl* pControl)
{
    if (pControl) 
        s_pDialogTrap = pControl->m_pDialog;
    else s_pDialogTrap = NULL;
    s_pControlTrap = pControl;
}

void CMousetrap::ReleaseMouse(CDXUTDialog* pDialog)
{
    if (s_pDialogTrap != pDialog)
        return;
    s_pDialogTrap = NULL;
    s_pControlTrap = NULL;
}

void CMousetrap::ReleaseMouse(CDXUTControl* pControl)
{
    if (s_pControlTrap != pControl)
        return;
    s_pDialogTrap = NULL;
    s_pControlTrap = NULL;
}

bool CMousetrap::DialogTrapProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
// Called in UIManager::MsgProc(). Returns bStopProcessing.
{
    if (s_pDialogTrap)
        return s_pDialogTrap->MsgProc(hWnd, uMsg, wParam, lParam);
    return false;
}

bool CMousetrap::ControlTrapProc(UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam)
// Called in CDXUTDialog::MsgProc(). Returns bStopProcessing.
{
    if (s_pControlTrap)
        return s_pControlTrap->HandleMouse(uMsg, pt, wParam, lParam);
    return false;
}

