// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif	

//////////////////////								////////////////////////////

#define RESMGR
#define OBJMGR
//使用3d呈像
#define _RENDER_3D

#ifdef RESMGR
#pragma message("資源管理模式啟動") 

#else
#pragma message("資源一般模式啟動") 
#endif

#ifdef OBJMGR
#pragma message("物件管理模式啟動") 

#else
#pragma message("物件一般模式啟動") 
#endif



////////////////////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers



// TODO: reference additional headers your program requires here
#include <windows.h>

//disable the warning : Unknow parama
#pragma warning(disable : 4068)

//disable the warning : 'function': was declared deprecated
#pragma warning(disable : 4996)

//disable the warning : STL needs to have dll-interface to be used by clients of class 
#pragma warning(disable : 4251)

//disable the warning : The file contains a character that cannot be represented in the current code page (950). Save the file in Unicode format to prevent data loss
#pragma warning(disable : 4819)

// Originally declared in CDXUTgui.h.
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <mmsystem.h>
#include <cmath>
#include <d3d9.h>
#include <d3dx9core.h>
#include <D3dx9math.h> 
#include <D3DX9Shader.h>
//#include <vcl.h>
#include <vector>
#include <list>
#include <string>
#include <map>
#include <usp10.h>
#include <dimm.h>
#include <tchar.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "Common.h"
#include "JLSendMessage.h"
#include "PhenixAssert.h"
#include "singleton.h"
#include "CDXUTDef.h"
#include "CDXUTDialog.h"
#include "CDXUTDialogResourceManager.h"
#include "CFDOXML.h"
#include "iniReader.h"
#include "PacketReader.h"
#include "FDOUT.h"
#include "J_SoundManager.h"


/////////////////			ResManage

#ifdef RESMGR

#include "command.h"
#include "ResourceMgr.h"
#include "memorypool.h"

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <set>
#include "Log.h"

using namespace std ;

#endif
