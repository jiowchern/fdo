#include "stdafx.h"
#pragma hdrstop
//-----------------------------------------------------------------------------
#include "J_SoundManager.h"
#include "PacketReader.h"
//#include "HUISystemSet.h"

//////////////////////								////////////////////////////


#ifdef RESMGR
#pragma message("管理模式啟動") 
#else
#pragma message("一般模式啟動") 
#endif

////////////////////////////////////////////////////////////////////////////////////

namespace FDO
{

//-----------------------------------------------------------------------------

__Initialize Sound_Initialize = NULL;
__Play2DSound Sound_Play2DSound = NULL;
__PlayExistent2DSound Sound_PlayExistent2DSound = NULL;
__Play2DSoundInMemory Sound_Play2DSoundInMemory = NULL;
__Stop2DSound Sound_Stop2DSound = NULL;
__Set2DSoundVolume Sound_Set2DSoundVolume = NULL;
__Play3DSound Sound_Play3DSound = NULL;
__PlayExistent3DSound Sound_PlayExistent3DSound = NULL;
__Play3DSoundInMemory Sound_Play3DSoundInMemory = NULL;
__Stop3DSound Sound_Stop3DSound = NULL;
__Set3DSoundVolume Sound_Set3DSoundVolume = NULL;
__SetListenerPos Sound_SetListenerPos = NULL;
__SetDistanceFactor Sound_SetDistanceFactor = NULL;
__SetMinMaxDistance Sound_SetMinMaxDistance = NULL;
__PlayMP3 Sound_PlayMP3 = NULL;
__PlayMP3InMemory Sound_PlayMP3InMemory = NULL; 
__StopMP3 Sound_StopMP3 = NULL;
__SetMP3Volume Sound_SetMP3Volume = NULL;
__IsMP3Playing Sound_IsMP3Playing = NULL;

CSoundManager::CSoundManager()
{
    m_fMaxDistance = 600.0f;
    m_vListenerPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_strMP3Name   = "";
	m_hSoundDLL = 0;
	m_maxVol = 80;
	m_NowVol = 80;
	m_maxSoundVolume = 100;
}

//-----------------------------------------------------------------------------
CSoundManager::~CSoundManager()
{    
	if(m_hSoundDLL!=0){
		_StopMP3();
		//Note by gino : Error when call FreeLibrary. Maybe there are some bugs in the DLL
		//::FreeLibrary(m_hSoundDLL);
		m_hSoundDLL = 0;
	}
}

//-----------------------------------------------------------------------------
bool CSoundManager::InitDLL()
{
	m_hSoundDLL = ::LoadLibrary(L"jsound.dll");
	if(m_hSoundDLL==0) return false; //cannot load jsound.dll

	//goto is here. just goto forward, don't goto backward!
	Sound_Initialize = (__Initialize)::GetProcAddress(m_hSoundDLL, "Initialize");
	if(Sound_Initialize==0) goto init_failed;

	Sound_Play2DSound = (__Play2DSound)::GetProcAddress(m_hSoundDLL, "Play2DSound");
	if(Sound_Play2DSound==0) goto init_failed;

	Sound_PlayExistent2DSound = (__PlayExistent2DSound)::GetProcAddress(m_hSoundDLL, "PlayExistent2DSound");
	if(Sound_PlayExistent2DSound==0) goto init_failed;

	Sound_Play2DSoundInMemory = (__Play2DSoundInMemory)::GetProcAddress(m_hSoundDLL, "Play2DSoundInMemory");
	if(Sound_Play2DSoundInMemory==0) goto init_failed;

	Sound_Stop2DSound = (__Stop2DSound)::GetProcAddress(m_hSoundDLL, "Stop2DSound");
	if(Sound_Stop2DSound==0) goto init_failed;

	Sound_Set2DSoundVolume = (__Set2DSoundVolume)::GetProcAddress(m_hSoundDLL, "Set2DSoundVolume");
	if(Sound_Set2DSoundVolume==0) goto init_failed;

	Sound_Play3DSound = (__Play3DSound)::GetProcAddress(m_hSoundDLL, "Play3DSound");
	if(Sound_Play3DSound==0) goto init_failed;

	Sound_PlayExistent3DSound = (__PlayExistent3DSound)::GetProcAddress(m_hSoundDLL, "PlayExistent3DSound");
	if(Sound_PlayExistent3DSound==0) goto init_failed;

	Sound_Play3DSoundInMemory = (__Play3DSoundInMemory)::GetProcAddress(m_hSoundDLL, "Play3DSoundInMemory");
	if(Sound_Play3DSoundInMemory==0) goto init_failed;

	Sound_Stop3DSound = (__Stop3DSound)::GetProcAddress(m_hSoundDLL, "Stop3DSound");
	if(Sound_Stop3DSound==0) goto init_failed;

	Sound_Set3DSoundVolume = (__Set3DSoundVolume)::GetProcAddress(m_hSoundDLL, "Set3DSoundVolume");
	if(Sound_Set3DSoundVolume==0) goto init_failed;

	Sound_SetListenerPos = (__SetListenerPos)::GetProcAddress(m_hSoundDLL, "SetListenerPos");
	if(Sound_SetListenerPos==0) goto init_failed;

	Sound_SetDistanceFactor = (__SetDistanceFactor)::GetProcAddress(m_hSoundDLL, "SetDistanceFactor");
	if(Sound_SetDistanceFactor==0) goto init_failed;

	Sound_SetMinMaxDistance = (__SetMinMaxDistance)::GetProcAddress(m_hSoundDLL, "SetMinMaxDistance");
	if(Sound_SetMinMaxDistance==0) goto init_failed;

	Sound_PlayMP3 = (__PlayMP3)::GetProcAddress(m_hSoundDLL, "PlayMP3");
	if(Sound_PlayMP3==0) goto init_failed;

	Sound_PlayMP3InMemory = (__PlayMP3InMemory)::GetProcAddress(m_hSoundDLL, "PlayMP3InMemory");
	if(Sound_PlayMP3InMemory==0) goto init_failed;

	Sound_StopMP3 = (__StopMP3)::GetProcAddress(m_hSoundDLL, "StopMP3");
	if(Sound_StopMP3==0) goto init_failed;

	Sound_SetMP3Volume = (__SetMP3Volume)::GetProcAddress(m_hSoundDLL, "SetMP3Volume");
	if(Sound_SetMP3Volume==0) goto init_failed;

	Sound_IsMP3Playing = (__IsMP3Playing)::GetProcAddress(m_hSoundDLL, "IsMP3Playing");
	if(Sound_IsMP3Playing==0) goto init_failed;

	return true;
	//failed area
init_failed:
	::FreeLibrary(m_hSoundDLL);
	m_hSoundDLL = 0;
	return false;
}

bool CSoundManager::_Initialize( HWND hWnd, DWORD dwCoopLevel, short shSoundNum, short sh3DSoundNum )
{
	if(InitDLL())
		return Sound_Initialize( hWnd, dwCoopLevel, shSoundNum, sh3DSoundNum );
	else
		return false;
}

//-----------------------------------------------------------------------------
short CSoundManager::_Play2DSound( const string& strFileName, bool bLooped )
{
	if(m_hSoundDLL==0) return 0;	
	return Sound_Play2DSound( strFileName.c_str(), bLooped );   //音效　＊＊＊
}

//-----------------------------------------------------------------------------
short CSoundManager::_Play2DSoundInMemory( const string& strFileName, bool bLooped )
{
	if(m_hSoundDLL==0) return -1;
    //water edit 2007/02/09 ***************************
    if(strcmp((const char *)strFileName.c_str() ,"")==0)return -1;
    //water edit 2007/02/09 ***************************
    short shChannelNum = Sound_PlayExistent2DSound( strFileName.c_str(), bLooped );   //音效　＊＊＊

    if( shChannelNum == -1 )
    {

#ifdef RESMGR
        memPack* pMem = LoadFromPacket( strFileName );
        if( pMem->Data == NULL )
            return -1;

		BYTE* pData = pMem->Data;

	    return Sound_Play2DSoundInMemory( strFileName.c_str(), pData , pMem->DataSize, bLooped );  //音效　＊＊＊

#else
        memPack* pMem = LoadFromPacket( strFileName );
        if( pMem == NULL )
            return -1;

	    return Sound_Play2DSoundInMemory( strFileName.c_str(), pMem->Data, pMem->DataSize, bLooped );  //音效　＊＊＊

#endif



    }

    return shChannelNum;
}

//-----------------------------------------------------------------------------
void CSoundManager::_Stop2DSound( short shSound )	
{
	if(m_hSoundDLL) 
		Sound_Stop2DSound( shSound );
}

//-----------------------------------------------------------------------------
void CSoundManager::_Set2DSoundVolume( const WORD wVolume )
{
	assert(wVolume>=0 && wVolume<=100);
	DWORD volume = (m_maxSoundVolume * wVolume)/100;
	if(m_hSoundDLL) 
		Sound_Set2DSoundVolume( volume );
}

//-----------------------------------------------------------------------------
short CSoundManager::_Play3DSound( const string& strFileName, const D3DXVECTOR3& vPosition, bool bLooped )
{
	if(m_hSoundDLL==0) return -1;

    // 判斷是否超過最大距離
    D3DXVECTOR3 vLength = vPosition - m_vListenerPos;
    if( D3DXVec3LengthSq( &vLength ) > m_fMaxDistance )
        return -1;

	return Sound_Play3DSound( strFileName.c_str(), vPosition, bLooped );   //音效　＊＊＊
}

//-----------------------------------------------------------------------------
short CSoundManager::_Play3DSoundInMemory( const string& strFileName, const D3DXVECTOR3& vPosition, bool bLooped )
{
	if(m_hSoundDLL==0) return -1;

    // 判斷是否超過最大距離
    D3DXVECTOR3 vLength = vPosition - m_vListenerPos;
    if( D3DXVec3LengthSq( &vLength ) > m_fMaxDistance )
        return -1;

    short sh3DChannelNum = Sound_PlayExistent3DSound( strFileName.c_str(), vPosition, bLooped );   //音效 ***

    if( sh3DChannelNum == -1 )
    {
#ifdef RESMGR


        memPack* pMem = LoadFromPacket( strFileName );
        if( pMem->Data == NULL )
            return -1;

		BYTE *pdata = pMem->Data;

	    return Sound_Play3DSoundInMemory( strFileName.c_str(), pdata , pMem->DataSize, vPosition, bLooped );

#else
        memPack* pMem = LoadFromPacket( strFileName );
        if( pMem == NULL )
            return -1;

	    return Sound_Play3DSoundInMemory( strFileName.c_str(), pMem->Data, pMem->DataSize, vPosition, bLooped );


#endif
    }

    return sh3DChannelNum;
}

//--------------------------------------------------------------------------------------
//音效 *** modify
//Play sound in any place without distance checking
short CSoundManager::_Play3DSoundInMemory(  const string& strFileName, bool bLooped )
{
	if(m_hSoundDLL==0) return -1;

    if(strcmp((const char *)strFileName.c_str() ,"")==0)return -1;

    D3DXVECTOR3 vPosition   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

    _SetListenerPos( vPosition );

    short sh3DChannelNum = Sound_PlayExistent3DSound( strFileName.c_str(), vPosition, bLooped );   //音效 ***

    if( sh3DChannelNum == -1 )
    {

#ifdef RESMGR


    memPack* pMem = LoadFromPacket( strFileName );
    
    if( pMem->Data == NULL ) return -1;
        

    BYTE* pData = pMem->Data;

	return Sound_Play3DSoundInMemory( strFileName.c_str(), pData , pMem->DataSize, vPosition, bLooped );

#else

    memPack* pMem = LoadFromPacket( strFileName );
        if( pMem == NULL )
            return -1;

	return Sound_Play3DSoundInMemory( strFileName.c_str(), pMem->Data, pMem->DataSize, vPosition, bLooped );


#endif



    }

    return sh3DChannelNum;
}


//-----------------------------------------------------------------------------
void CSoundManager::_Stop3DSound( short shSound )
{
	if(m_hSoundDLL)
		Sound_Stop3DSound( shSound );    //音效　＊＊＊
}

//-----------------------------------------------------------------------------
void CSoundManager::_Set3DSoundVolume( const WORD wVolume )  //音效 *** ***
{
	assert(wVolume>=0 && wVolume<=100);
	DWORD volume = (m_maxSoundVolume * wVolume)/100;
	if(m_hSoundDLL)
		Sound_Set3DSoundVolume( volume );
}

//-----------------------------------------------------------------------------
void CSoundManager::_SetListenerPos( const D3DXVECTOR3& vPosition ) //音效 *** ***
{
    m_vListenerPos = vPosition;

	if(m_hSoundDLL)
		Sound_SetListenerPos( vPosition );
}

//-----------------------------------------------------------------------------
void CSoundManager::_SetDistanceFactor( FLOAT fDistanceFactor )
{
	if(m_hSoundDLL)
		Sound_SetDistanceFactor( fDistanceFactor );
}

//-----------------------------------------------------------------------------
void CSoundManager::_SetMinMaxDistance( FLOAT fMinDistance, FLOAT fMaxDistance )
{
    m_fMaxDistance = fMaxDistance * fMaxDistance;

	if(m_hSoundDLL)
		Sound_SetMinMaxDistance( fMinDistance, fMaxDistance );
}

//-----------------------------------------------------------------------------
bool CSoundManager::_PlayMP3( const string& strFileName )
{
	if(m_hSoundDLL==0) return false;

	if( Sound_IsMP3Playing() && ( m_strMP3Name == strFileName ) )
		return true;

	if( Sound_PlayMP3( strFileName.c_str() ) ) //音效　＊＊＊
	{
		m_strMP3Name = strFileName;
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
bool CSoundManager::_PlayMP3InMemory( const string& strFileName )
{
	if(m_hSoundDLL==0) return false;

	if(m_strMP3Name != strFileName)
	{
		if(Sound_IsMP3Playing())
		{
			_StopMP3() ; 
		}
	}
	else
	{
		if(Sound_IsMP3Playing())
			return true ; 
	}

#ifdef RESMGR

  
    memPack* pMem = LoadFromPacket( strFileName );
    
    if( pMem->Data == NULL ) return false;
        

    BYTE* pData = pMem->Data;

     //音效　＊＊＊
    if( Sound_PlayMP3InMemory( strFileName.c_str(), pData , pMem->DataSize ) )
    {
        m_strMP3Name = strFileName;

		_SetMP3Volume( m_NowVol );

		return true;
    }

#else

    memPack* pMem = LoadFromPacket( strFileName );
    if( pMem == NULL )
        return false;

     //音效　＊＊＊
    if( Sound_PlayMP3InMemory( strFileName.c_str(), pMem->Data, pMem->DataSize ) )
    {
        m_strMP3Name = strFileName;
		if( m_NowVol >= 10 )
		{
			for ( unsigned short i = 10; i <= m_NowVol; i++ )
			{
				_SetMP3Volume( i );//將音樂漸升
				Sleep( 5 );
			}
		}
		else
		{
			_SetMP3Volume( m_NowVol );//將音樂漸升
			Sleep( 5 );
		}
	return true;
    }

#endif

    return false;
}

//-----------------------------------------------------------------------------
void CSoundManager::_StopMP3()
{
	if(m_hSoundDLL)
		Sound_StopMP3();
}

//-----------------------------------------------------------------------------
void CSoundManager::_SetMP3Volume( const WORD wVolume )
{
	if(m_hSoundDLL)
	{
		Sound_SetMP3Volume( wVolume );
		m_maxVol = wVolume;
	}
}

//-----------------------------------------------------------------------------
void CSoundManager::_SetSoundVolume( const WORD wVolume )
{
	if(m_hSoundDLL)
		m_NowVol = wVolume;
}

//----------------------------------------------------------------------------
}   // namespace FDO
