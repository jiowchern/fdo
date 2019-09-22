//
// Mousetrap.h
//
#ifndef __Mousetrap_h__
#define __Mousetrap_h__

class CDXUTDialog;
class CDXUTControl;

class CMousetrap
{
private:
    static CDXUTDialog* s_pDialogTrap;
    static CDXUTControl* s_pControlTrap;
public:
    CMousetrap();
    ~CMousetrap();
    void CaptureMouse(CDXUTDialog* pDialog);
    void CaptureMouse(CDXUTControl* pControl);
    void ReleaseMouse(CDXUTDialog* pDialog);
    void ReleaseMouse(CDXUTControl* pControl);
    bool DialogTrapProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        // Called in UIManager::MsgProc(). Returns bStopProcessing.
    bool ControlTrapProc(UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam);  
        // Called in CDXUTDialog::MsgProc(). Returns bStopProcessing.
};

// There is a global instance of CMousetrap.
extern CMousetrap g_Mousetrap;

#endif // __Mousetrap_h__
