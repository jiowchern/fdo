#pragma once
#define INITGUID
#include <dxdiag.h>

struct SysInfo
{
    wchar_t m_szDxDiagVersion[100];               // DxDiag version (not localizable)
    wchar_t m_szProcessorEnglish[200];            // Processor name and speed (english)
    wchar_t m_szPhysicalMemoryEnglish[100];       // Formatted version of physical memory (english)
    wchar_t m_szCSDVersion[200];                  // OS version with CSD info (localized)
    wchar_t m_szDirectXVersionLongEnglish[100];   // long DirectX version (english)
    wchar_t m_szOSExEnglish[100];                 // Formatted version of platform, version, build num (english)
    wchar_t m_szLanguagesEnglish[200];            // Formatted version of m_szLanguage, m_szLanguageRegional (english) 
};

struct DisplayInfo
{
    wchar_t m_szDescription[200];
    wchar_t m_szManufacturer[200];
    wchar_t m_szChipType[100];
    wchar_t m_szDisplayMemoryEnglish[100];
    wchar_t m_szDriverVersion[100];
};

#define MAX_DISPLAYINFO 2

struct DxDiagReport
{
	SysInfo sysinfo;
	DisplayInfo displayInfo[MAX_DISPLAYINFO];
	int numDisplayInfo;
};

class DxDiagReporter
{
	bool m_bInit;
	DxDiagReport m_report;
public:
	DxDiagReporter(void);
	~DxDiagReporter(void);

	void CollectReport();
	const DxDiagReport &GetReport();
	void WriteReport(const wchar_t *fileName);
private:
	void WriteBeginXMLTag(HANDLE fp, const wchar_t *pTag);
	void WriteEndXMLTag(HANDLE fp, const wchar_t *pTag);
	void WriteXMLValue(HANDLE fp, const wchar_t *pTag, const wchar_t *pValue);
	void GetSystemInfo(IDxDiagContainer *);
	void GetDisplayInfo(IDxDiagContainer *);
	HRESULT GetStringValue( IDxDiagContainer* pObject, WCHAR* wstrName, wchar_t* strValue, int nStrLen );
};
