//-----------------------------------------------------------------------------
#ifndef _J_SOUND_H_
#define _J_SOUND_H_
//-----------------------------------------------------------------------------

#include <windows.h>
#include <d3dx9math.h>

#ifdef __DLL__ 
	#define DLL_API __declspec(dllexport) 
#else 
	#define DLL_API __declspec(dllimport)
#endif 

// 用typedef自訂函式指標的型別
typedef bool  (*__Initialize)			( HWND hWnd, DWORD dwCoopLevel, short shSoundNum, short sh3DSoundNum );
typedef short (*__Play2DSound)			( LPCSTR pFileName, bool bLooped );
typedef short (*__PlayExistent2DSound)	( LPCSTR pFileName, bool bLooped );
typedef short (*__Play2DSoundInMemory)	( LPCSTR pFileName, BYTE* pMemData, const DWORD dwDataSize, bool bLooped );
typedef void  (*__Stop2DSound)			( short shSound );
typedef void  (*__Set2DSoundVolume)		( const WORD wVolume );
typedef short (*__Play3DSound)			( LPCSTR pFileName, const D3DXVECTOR3& vPosition, bool bLooped );
typedef short (*__PlayExistent3DSound)	( LPCSTR pFileName, const D3DXVECTOR3& vPosition, bool bLooped );
typedef short (*__Play3DSoundInMemory)	( LPCSTR pFileName, BYTE* pMemData, const DWORD dwDataSize, 
													  const D3DXVECTOR3& vPosition, bool bLooped );
typedef void  (*__Stop3DSound)			( short shSound );
typedef void  (*__Set3DSoundVolume)		( const WORD wVolume );
typedef void  (*__SetListenerPos)		( const D3DXVECTOR3& vPosition );
typedef void  (*__SetDistanceFactor)	( FLOAT fDistanceFactor );
typedef void  (*__SetMinMaxDistance)	( FLOAT fMinDistance, FLOAT fMaxDistance );		
typedef bool  (*__PlayMP3)				( LPCSTR pFileName );
typedef bool  (*__PlayMP3InMemory)		( LPCSTR pFileName, BYTE* pMemData, const DWORD dwDataSize ); 
typedef void  (*__StopMP3)				( void );
typedef void  (*__SetMP3Volume)			( const WORD wVolume );
typedef bool  (*__IsMP3Playing)			( void );
//-----------------------------------------------------------------------------
extern "C"
{
	// 初始函式
    DLL_API bool Initialize( HWND hWnd, DWORD dwCoopLevel, short shSoundNum, short sh3DSoundNum );	
	
	// 音效播放函式
	DLL_API short Play2DSound( LPCSTR pFileName, bool bLooped );	
	DLL_API short PlayExistent2DSound( LPCSTR pFileName, bool bLooped );
	DLL_API short Play2DSoundInMemory( LPCSTR pFileName, BYTE* pMemData, const DWORD dwDataSize, bool bLooped );
	DLL_API void  Stop2DSound( short shSound );	
	DLL_API void  Set2DSoundVolume( const WORD wVolume );	

	// 3D音效播放函式
	DLL_API short Play3DSound( LPCSTR pFileName, const D3DXVECTOR3& vPosition, bool bLooped );
	DLL_API short PlayExistent3DSound( LPCSTR pFileName, const D3DXVECTOR3& vPosition, bool bLooped );	
	DLL_API short Play3DSoundInMemory( LPCSTR pFileName, BYTE* pMemData, const DWORD dwDataSize,
									   const D3DXVECTOR3& vPosition, bool bLooped );
	DLL_API void  Stop3DSound( short shSound );	
	DLL_API void  Set3DSoundVolume( const WORD wVolume );	
	DLL_API void  SetListenerPos( const D3DXVECTOR3& vPosition );
	DLL_API void  SetDistanceFactor( FLOAT fDistanceFactor );
	DLL_API void  SetMinMaxDistance( FLOAT fMinDistance, FLOAT fMaxDistance );	

	// MP3播放函式
	DLL_API bool  PlayMP3( LPCSTR pFileName );
	DLL_API bool  PlayMP3InMemory( LPCSTR pFileName, BYTE* pMemData, const DWORD dwDataSize );	
	DLL_API void  StopMP3();
	DLL_API void  SetMP3Volume( const WORD wVolume );
	DLL_API bool  IsMP3Playing();	
};

//-----------------------------------------------------------------------------
#endif // _J_SOUND_H_
