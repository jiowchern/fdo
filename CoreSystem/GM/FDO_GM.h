//-----------------------------------------------------------------------------
#ifndef _FDO_GM_H_
#define _FDO_GM_H_

//#include <vcl.h>
//-----------------------------------------------------------------------------
namespace FDO
{
// 用typedef自訂函式指標的型別
// typedef void    (__stdcall *__Create)            (TComponent* Owner);
// typedef void    (__stdcall *__Destroy)           (void);
// typedef void    (__stdcall *__Login)             (int Server,int RoleIndex,char *Account,char *Password,char *LocalIP);
// typedef char *  (__stdcall *__RecvData)          (char *TempBuf);
// typedef int     (__stdcall *__GetDataCount)      (void);
// typedef char *  (__stdcall *__GetData)           (void);
// typedef void    (__stdcall *__FreeData)          (char *TempBuf);
// typedef int     (__stdcall *__GetServerVersion)  (void);

	typedef __declspec(dllimport) void	 (*__Create)					(void* Owner);
	typedef __declspec(dllimport) void	 (*__Destroy)					(void);
	typedef __declspec(dllimport) void	 (*__Login)						(int Server,int RoleIndex,char *Account,char *Password,char *LocalIP);
	typedef __declspec(dllimport) char * (*__RecvData)					(char *TempBuf);
	typedef __declspec(dllimport) int	 (*__GetDataCount)				(void);
	typedef __declspec(dllimport) char * (*__GetData)					(void);
	typedef __declspec(dllimport) void	 (*__FreeData)					(char *TempBuf);
	typedef __declspec(dllimport) int	 (*__GetServerVersion)			(void);

//-----------------------------------------------------------------------------
class FDO_GM
{
private:
    HINSTANCE m_hDLL;

public:
    FDO_GM();
    ~FDO_GM();

    bool            InitDLL();

    // 宣告函式指標
    __Create            Create;
    __Destroy           Destroy;
    __Login             Login;
    __RecvData          RecvData;
    __GetDataCount      GetDataCount;
    __GetData           GetData;
    __FreeData          FreeData;
    __GetServerVersion  GetServerVersion;
};

} //namespace FDO

#endif
//-----------------------------------------------------------------------------


