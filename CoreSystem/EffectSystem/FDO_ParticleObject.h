//---------------------------------------------------------------------------
#ifndef ParticleObjectH
#define ParticleObjectH

#include "FdoDrawable.h"
#include "ParticlePreRequest.h"
#include "elementdef.h" // ref: ObjFactory

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "FDO_RoleMemoryMgr.h"
using std::vector;
using std::string;
using std::map;
//---------------------------------------------------------------------------
//class R_EmitterPlayer;
//class R_EmitterManager;
//class R_ParticleRender;
//class R_ParticleEmitter;
//class WinTrace;
//class TraceNodeEx;
//---------------------------------------------------------------------------
namespace FDO
{
//---------------------------------------------------------------------------
class ParticleObject : public CDrawable
{
    typedef CDrawable base;

private:
	D3DXVECTOR3                     m_vFixSoundPosition;    // 聲音位置
	bool							m_bFixSoundPosition;	// 微調聲音位置開關
    bool                            m_bControl;
	bool							m_bFlush ;

    void                            InternalSetPosition();
    void                            RecursiveUpdate(R_ParticleEmitterPtr pSrcEmt);
	//<gino>
	static MemDataMgr				sm_MemDataCacheMgr;
	//</gino>

public:
    ParticleObject();
    virtual ~ParticleObject();
    void FrameMoveParticle( float fElapsedTime );

	void SetFixSoundPosition(const D3DXVECTOR3& value);
    void SetFixSoundPosition(float x, float y, float z);
    const D3DXVECTOR3& GetFixSoundPosition() const;

	void SetUseFixSoundPosition(bool bFixSound);
    bool IsUseFixSoundPosition() const;

    void Reset();
    bool CreateParticle( const string& FileName, bool staticResource=false);
    void RenderParticle( float fElapsedTime, const D3DXMATRIX& matInvView  , bool isUpdate = false);
    void Control( const float width , const float height ,
					float fElapsedTime, LPD3DXMATRIX pWorldMatrix ,
					const D3DXVECTOR4 &rectRange , bool isMoster , unsigned int &groupCount , unsigned int &outParticCount);
    bool IsPlayEnd();
    void PlaySound();
    void StopSound();
	bool IsFlush(void) { return m_bFlush ; } 
    //float getRaius() { return 10.0f; }

    unsigned short GetPRenderPoolSize();

	//<gino>
	static void ClearCaches();
	//</gino>

/// <yuming>
private:
    /// D3D裝置
    LPDIRECT3DDEVICE9 m_pd3dDevice;              
    /// 特效播放器
    R_EmitterPlayerPtr m_pEmitterPlayer;
    /// 發射器管理員
    R_EmitterManagerPtr m_pEmitterManager;
    /// particle template
    ParticleTempPtr mParticleTemp;

private:
    typedef ObjFactory<ParticleObject> ParticleFactory;
    static ParticleFactory sParticleFactory;

public: // operator
    void* operator new (size_t);
    void operator delete (void* p);

private:
    void UpdateEmtParas();
    void SaveEmtPara();
    std::vector<D3DXVECTOR3> m_vEmtSclS;
    std::vector<D3DXVECTOR3> m_vEmtPosS;
    std::vector<D3DXVECTOR3> m_vEmtRotS;
    TextureMap mTextureMap;
    MeshMap mMeshMap;
    /// This variable is used to distinguish control function was called or not
    bool mControled;

public:
    bool LoadFromFileInMemory(const BYTE* data, UINT size);
    R_EmitterManagerPtr GetEmitterManager() const;
    R_PRenderManagerPtr GetPRenderManager() const;
    ParticleSystemPtr GetParticleSystem() const;
    LPDIRECT3DDEVICE9 GetD3DDevice() const;
    /// 播放特效
    bool PlayEffect();
    /// 暫停播放
    bool PauseEffect();
    /// 停止播放
    bool StopEffect();
    /// 取得材質
    LPDIRECT3DTEXTURE9 GetTexture(const fstring& fileName, bool staticResource);
    /// 取得網格
    LPD3DXMESH GetMesh(const fstring& fileName, bool staticResource);

public: // CDrawable stuff
    virtual bool Update(const UpdateArgsPtr &args);
    virtual bool Draw();
    virtual void RegisterForDrawing(SCNPtr scn);
    virtual bool IsRayIntersect(const RAY& ray, HitInfo& res) const;
    virtual void DrawHelpers();
    virtual void CleanUp();
    virtual void SetScale(float x, float y, float z);
    virtual void SetScale(const D3DXVECTOR3& value);

protected:
    //virtual void UpdateBox() const;
    virtual void UpdateWorldMatrix() const;
/// </yuming>
};
//---------------------------------------------------------------------------
} // namespace FDO
//---------------------------------------------------------------------------
#endif
