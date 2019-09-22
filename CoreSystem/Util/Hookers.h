//
// Hookers.h
//
#ifndef __Hookers_h__
#define __Hookers_h__

#include "singleton.h"
#include <list>

#define g_KeyboardHooker CKeyboardHooker::GetSingleton()

class CDXUTDialog;

typedef bool (*KeyboardHookProc)(
    void* pvUserData, DWORD dwVirtualKeyCode, bool bKeyDown, bool bAltDown, 
    bool bCtrlDown, bool bShiftDown, bool bKeyDownRepeated);

namespace FDO
{

class CKeyboardHooker : public Singleton<CKeyboardHooker>
{
private:
    typedef struct tagDialogHooker {
        CDXUTDialog* m_pDialog;
        KeyboardHookProc m_pKeybProc;
        tagDialogHooker() { m_pDialog = NULL; m_pKeybProc = NULL; }
    } DialogHooker;
    std::list<DialogHooker> m_listDialogMouseOver;
	bool m_bEnable;
public:
    CKeyboardHooker();
    ~CKeyboardHooker();
    bool RegisterDialogMouseOver(CDXUTDialog* pDialog, KeyboardHookProc pKeybProc);
    bool Hook(DWORD dwVirtualKeyCode, bool bKeyDown, bool bAltDown, 
        bool bCtrlDown, bool bShiftDown, bool bKeyDownRepeated);
	//<gino>
	void Enable(bool bTrue){ m_bEnable = bTrue;};
	//</gino>

};

} // namespace FDO

#endif // __Hookers_h__