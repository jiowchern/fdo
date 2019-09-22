//-----------------------------------------------------------------------------
#ifndef _J_SOUNDMANAGER_H_
#define _J_SOUNDMANAGER_H_
//-----------------------------------------------------------------------------

#include "singleton.h"
#include "jsound.h"
#include <string>

#define g_SoundManager CSoundManager::GetSingleton()

using namespace std;

//-----------------------------------------------------------------------------
namespace FDO
{

//-----------------------------------------------------------------------------

class CSoundManager : public Singleton<CSoundManager>
{
private:
    float       m_fMaxDistance;
    D3DXVECTOR3 m_vListenerPos;
    string      m_strMP3Name;
	HMODULE		m_hSoundDLL;
	int			m_maxVol;
	int			m_NowVol;			// �ثe���a�]�w���̤j��( �Ω�L����l )
	int			m_maxSoundVolume;//
public:
    CSoundManager();
    ~CSoundManager();

    // ��l�禡
    bool _Initialize( HWND hWnd, DWORD dwCoopLevel, short shSoundNum, short sh3DSoundNum );

	// ���ļ���禡   //���� ***
	short _Play2DSound( const string& strFileName, bool bLooped = false );
	short _Play2DSoundInMemory( const string& strFileName, bool bLooped = false );
	void  _Stop2DSound( short shSound );
	void  _Set2DSoundVolume( const WORD wVolume );

	// 3D���ļ���禡      //���� ***
	short _Play3DSound( const string& strFileName, const D3DXVECTOR3& vPosition, bool bLooped = false );
	short _Play3DSoundInMemory( const string& strFileName, const D3DXVECTOR3& vPosition, bool bLooped = false );
        // ���� *** modify  //no distance
        short _Play3DSoundInMemory(  const string& strFileName, bool bLooped = false );

	void  _Stop3DSound( short shSound );
	void  _Set3DSoundVolume( const WORD wVolume );
	void  _SetListenerPos( const D3DXVECTOR3& vPosition );
	void  _SetDistanceFactor( FLOAT fDistanceFactor );
	void  _SetMinMaxDistance( FLOAT fMinDistance, FLOAT fMaxDistance );

	// MP3����禡           //���� ***
	bool  _PlayMP3( const string& strFileName );
	bool  _PlayMP3InMemory( const string& strFileName );
	void  _StopMP3();
	void  _SetMP3Volume( const WORD wVolume );
	void  _SetSoundVolume( const WORD wVolume );			// �]�w�n�����̤j��( �Ω�L����l )


	void SetMaxVolume(INT vol){ m_maxVol = vol; }
private:
	// Load DLL
	bool InitDLL();
};

}   // namespace FDO

//-----------------------------------------------------------------------------
#endif // _J_SOUNDMANAGER_H_