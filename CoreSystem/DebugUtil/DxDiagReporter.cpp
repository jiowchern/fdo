#include "StdAfx.h"
#include <initguid.h>
#include <algorithm>
#include "DxDiagReporter.h"
#include "Log.h"
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#define SAFE_BSTR_FREE(x)    if(x) { SysFreeString( x ); x = NULL; }
#define EXPAND(x)            x, sizeof(x)/sizeof(wchar_t)

DxDiagReporter::DxDiagReporter(void)
: m_bInit(false)
{
	memset(&m_report, 0, sizeof(m_report));
}

DxDiagReporter::~DxDiagReporter(void)
{
}


const DxDiagReport &DxDiagReporter::GetReport()
{
	CollectReport();
	return m_report;
}


void DxDiagReporter::CollectReport()
{
	if(m_bInit) return;
	m_bInit =  true;

	HRESULT       hr;
    hr = CoInitialize( NULL );
	if(SUCCEEDED(hr)){
		IDxDiagProvider*  pDxDiagProvider=NULL;
		IDxDiagContainer* pDxDiagRoot=NULL;
		hr = CoCreateInstance( CLSID_DxDiagProvider,
							   NULL,
							   CLSCTX_INPROC_SERVER,
							   IID_IDxDiagProvider,
							   (LPVOID*) &pDxDiagProvider);	
	
		if(FAILED(hr) || pDxDiagProvider==NULL)
		{		
			_LOG(0, L"!!Get System Info Failed!!");
			goto LCleanup;
		}
		// Fill out a DXDIAG_INIT_PARAMS struct and pass it to IDxDiagContainer::Initialize
		// Passing in TRUE for bAllowWHQLChecks, allows dxdiag to check if drivers are 
		// digital signed as logo'd by WHQL which may connect via internet to update 
		// WHQL certificates.    
		DXDIAG_INIT_PARAMS dxDiagInitParam;
		ZeroMemory( &dxDiagInitParam, sizeof(DXDIAG_INIT_PARAMS) );

		dxDiagInitParam.dwSize                  = sizeof(DXDIAG_INIT_PARAMS);
		dxDiagInitParam.dwDxDiagHeaderVersion   = DXDIAG_DX9_SDK_VERSION;
		dxDiagInitParam.bAllowWHQLChecks        = FALSE;
		dxDiagInitParam.pReserved               = NULL;

		hr = pDxDiagProvider->Initialize( &dxDiagInitParam );
		if( FAILED(hr) )
			goto LCleanup;

		hr = pDxDiagProvider->GetRootContainer( &pDxDiagRoot );
		if( FAILED(hr) )
			goto LCleanup;

		GetSystemInfo(pDxDiagRoot);
		GetDisplayInfo(pDxDiagRoot);

LCleanup:
		SAFE_RELEASE( pDxDiagRoot );
		SAFE_RELEASE( pDxDiagProvider );
		CoUninitialize();
	}
}


HRESULT DxDiagReporter::GetStringValue( IDxDiagContainer* pObject, WCHAR* wstrName, wchar_t* strValue, int nStrLen )
{
    HRESULT hr;
    VARIANT var;
    VariantInit( &var );

    if( FAILED( hr = pObject->GetProp( wstrName, &var ) ) )
        return hr;

    if( var.vt != VT_BSTR )
        return E_INVALIDARG;
    
#ifdef _UNICODE
    wcsncpy_s( strValue, nStrLen, var.bstrVal, nStrLen-1);
#else
    wcstombs( strValue, var.bstrVal, nStrLen );   
#endif
    strValue[nStrLen-1] = TEXT('\0');
    VariantClear( &var );

    return S_OK;
}

void DxDiagReporter::GetSystemInfo(IDxDiagContainer *pDxDiagRoot)
{
    HRESULT           hr;
    IDxDiagContainer* pObject         = NULL;
	
	_LOG(0, L"[System Info]");
	SysInfo* pSysInfo = &this->m_report.sysinfo;

    // Get the IDxDiagContainer object called "DxDiag_SystemInfo".
    // This call may take some time while dxdiag gathers the info.
    hr = pDxDiagRoot->GetChildContainer( L"DxDiag_SystemInfo", &pObject );
    if( FAILED( hr ) || pObject == NULL )
    {
        hr = E_FAIL;
        goto LCleanup;
    }
   
	if( FAILED( hr = GetStringValue( pObject, L"szDxDiagVersion", EXPAND(pSysInfo->m_szDxDiagVersion) ) ) )
        goto LCleanup; 
	_LOG(0, L" DxDiag Version = %s", pSysInfo->m_szDxDiagVersion);
    if( FAILED( hr = GetStringValue( pObject, L"szProcessorEnglish", EXPAND(pSysInfo->m_szProcessorEnglish) ) ) )
        goto LCleanup; 
	_LOG(0, L" Processor = %s", pSysInfo->m_szProcessorEnglish);
    if( FAILED( hr = GetStringValue( pObject, L"szPhysicalMemoryEnglish", EXPAND(pSysInfo->m_szPhysicalMemoryEnglish) ) ) )
        goto LCleanup; 
	_LOG(0, L" Physical Memory = %s", pSysInfo->m_szPhysicalMemoryEnglish);
//    if( FAILED( hr = GetStringValue( pObject, L"szCSDVersion", EXPAND(pSysInfo->m_szCSDVersion) ) ) )
  //      goto LCleanup; 
//	_LOG(0, L"CSD Version = %s", pSysInfo->m_szCSDVersion);
	if( FAILED( hr = GetStringValue( pObject, L"szDirectXVersionLongEnglish", EXPAND(pSysInfo->m_szDirectXVersionLongEnglish) ) ) )
        goto LCleanup; 
	_LOG(0, L" DirectX Version = %s", pSysInfo->m_szDirectXVersionLongEnglish);
    if( FAILED( hr = GetStringValue( pObject, L"szOSExEnglish", EXPAND(pSysInfo->m_szOSExEnglish) ) ) )
        goto LCleanup;       
	_LOG(0, L" OS = %s", pSysInfo->m_szOSExEnglish);
    if( FAILED( hr = GetStringValue( pObject, L"szLanguagesEnglish", EXPAND(pSysInfo->m_szLanguagesEnglish) ) ) )
        goto LCleanup; 
	_LOG(0, L" Languages = %s", pSysInfo->m_szLanguagesEnglish);
LCleanup:
    SAFE_RELEASE( pObject );
}

void DxDiagReporter::GetDisplayInfo(IDxDiagContainer *pDxDiagRoot)
{
   HRESULT           hr;
    WCHAR             wszContainer[256];
    IDxDiagContainer* pContainer      = NULL;
    IDxDiagContainer* pObject         = NULL;
    DWORD             nInstanceCount    = 0;
    DWORD             nItem             = 0;

    // Get the IDxDiagContainer object called "DxDiag_DisplayDevices".
    // This call may take some time while dxdiag gathers the info.
    if( FAILED( hr = pDxDiagRoot->GetChildContainer( L"DxDiag_DisplayDevices", &pContainer ) ) )
        goto LCleanup;
    if( FAILED( hr = pContainer->GetNumberOfChildContainers( &nInstanceCount ) ) )
        goto LCleanup;

	nInstanceCount = std::min<DWORD>(nInstanceCount, MAX_DISPLAYINFO);
	this->m_report.numDisplayInfo = nInstanceCount;
    for( nItem = 0; nItem < nInstanceCount; nItem++ )
    {
		DisplayInfo* pDisplayInfo = &this->m_report.displayInfo[nItem];

        hr = pContainer->EnumChildContainerNames( nItem, wszContainer, 256 );
        if( FAILED( hr ) )
            goto LCleanup;
        hr = pContainer->GetChildContainer( wszContainer, &pObject );
        if( FAILED( hr ) || pObject == NULL )
        {
            if( pObject == NULL )
                hr = E_FAIL;
            goto LCleanup;
        }
		_LOG(0, L"[Display Info %d]", nItem); 
        if( FAILED( hr = GetStringValue( pObject, L"szDescription", EXPAND(pDisplayInfo->m_szDescription) ) ) )
            goto LCleanup; 
		_LOG(0, L" Description = %s", pDisplayInfo->m_szDescription);
        if( FAILED( hr = GetStringValue( pObject, L"szManufacturer", EXPAND(pDisplayInfo->m_szManufacturer) ) ) )
            goto LCleanup; 
		_LOG(0, L" Manufacturer = %s", pDisplayInfo->m_szManufacturer);
		if( FAILED( hr = GetStringValue( pObject, L"szChipType", EXPAND(pDisplayInfo->m_szChipType) ) ) )
            goto LCleanup; 
		_LOG(0, L" ChipType = %s", pDisplayInfo->m_szChipType);
        if( FAILED( hr = GetStringValue( pObject, L"szDisplayMemoryEnglish", EXPAND(pDisplayInfo->m_szDisplayMemoryEnglish) ) ) )
            goto LCleanup; 
		_LOG(0, L" Display Memory = %s", pDisplayInfo->m_szDisplayMemoryEnglish);
        if( FAILED( hr = GetStringValue( pObject, L"szDriverVersion", EXPAND(pDisplayInfo->m_szDriverVersion) ) ) )
            goto LCleanup; 
		_LOG(0, L" Driver Version = %s", pDisplayInfo->m_szDriverVersion);
    }

LCleanup:
    SAFE_RELEASE( pObject );
    SAFE_RELEASE( pContainer );
}



void DxDiagReporter::WriteBeginXMLTag(HANDLE fp, const wchar_t *pTag)
{
	wchar_t msg[256];
	wsprintf(msg, L"<%s>\r\n", pTag);
	char strText[512];
    WideCharToMultiByte ( CP_ACP, 0, msg, -1, strText, 510, NULL, NULL );

	DWORD dwNumWritten = 0;
	::WriteFile(fp, strText, (DWORD)strlen(strText)*sizeof(char), &dwNumWritten, NULL); 
}

void DxDiagReporter::WriteEndXMLTag(HANDLE fp, const wchar_t *pTag)
{
	wchar_t msg[256];
	wsprintf(msg, L"</%s>\r\n", pTag);

	char strText[512];
    WideCharToMultiByte ( CP_ACP, 0, msg, -1, strText, 510, NULL, NULL );

	DWORD dwNumWritten = 0;
	::WriteFile(fp, strText, (DWORD)strlen(strText)*sizeof(char), &dwNumWritten, NULL); 
}

void DxDiagReporter::WriteXMLValue(HANDLE fp, const wchar_t *pTag, const wchar_t *pValue)
{
	wchar_t msg[256];
	wsprintf(msg, L"   <%s>%s</%s>\r\n", pTag, pValue, pTag);
	
	char strText[512];
    WideCharToMultiByte ( CP_ACP, 0, msg, -1, strText, 510, NULL, NULL );

	DWORD dwNumWritten = 0;
	::WriteFile(fp, strText, (DWORD)strlen(strText)*sizeof(char), &dwNumWritten, NULL); 
}

void DxDiagReporter::WriteReport(const wchar_t *logFile)
{
	CollectReport();

	HANDLE fp = CreateFile(logFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if(fp!=NULL){
		WriteBeginXMLTag(fp, _T("SystemInfo"));
		WriteXMLValue(fp, _T("DxDiagVersion"), this->m_report.sysinfo.m_szDxDiagVersion);
		WriteXMLValue(fp, _T("Processor"), this->m_report.sysinfo.m_szProcessorEnglish);
		WriteXMLValue(fp, _T("PhysicalMemory"), this->m_report.sysinfo.m_szPhysicalMemoryEnglish);
		WriteXMLValue(fp, _T("OS"), this->m_report.sysinfo.m_szOSExEnglish);
		WriteXMLValue(fp, _T("CSDVersion"), this->m_report.sysinfo.m_szCSDVersion);
		WriteXMLValue(fp, _T("DirectXVersion"), this->m_report.sysinfo.m_szDirectXVersionLongEnglish);
		WriteXMLValue(fp, _T("Languages"), this->m_report.sysinfo.m_szLanguagesEnglish);
		WriteEndXMLTag(fp, _T("SystemInfo"));

		wchar_t *gpuInfo[]={_T("GPUInfo_1"), _T("GPUInfo_2")};
		for(int i=0; i< this->m_report.numDisplayInfo; i++){
			WriteBeginXMLTag(fp, gpuInfo[i]);
			WriteXMLValue(fp, _T("Description"), this->m_report.displayInfo[i].m_szDescription);
			WriteXMLValue(fp, _T("Manufacturer"), this->m_report.displayInfo[i].m_szManufacturer);
			WriteXMLValue(fp, _T("ChipType"), this->m_report.displayInfo[i].m_szChipType);
			WriteXMLValue(fp, _T("DisplayMemory"), this->m_report.displayInfo[i].m_szDisplayMemoryEnglish);
			WriteXMLValue(fp, _T("DriverVersion"), this->m_report.displayInfo[i].m_szDriverVersion);
			WriteEndXMLTag(fp, gpuInfo[i]);
		}

		CloseHandle(fp);
	}
}

