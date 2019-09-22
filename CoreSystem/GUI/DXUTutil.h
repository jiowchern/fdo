#pragma once


LPD3DXFONT GetCachedFont(int iFontNode);
class CDXUTDialogResourceManager;
CDXUTDialogResourceManager* GetDialogResourceManager();
DWORD HexToDWORD(LPCWSTR wszValue);