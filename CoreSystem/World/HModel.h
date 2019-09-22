/**
 * @file HModel.h
 * @author Yuming Ho
 */
//---------------------------------------------------------------------------
#ifndef HModelH
#define HModelH
//---------------------------------------------------------------------------
#include "FdoDrawable.h"
#include "elementdef.h" // ref: ObjFactory
//---------------------------------------------------------------------------
namespace FDO
{
//---------------------------------------------------------------------------
class HModel : public CDrawable
{
    typedef CDrawable base;

public:
    HModel();
    virtual ~HModel();

public: // attribute
    void SetAnimation(LPD3DXANIMATIONCONTROLLER value);
    void SetSkinMeshTemp(SkinMeshTempPtr value);
    const D3DXVECTOR3& GetCenter() const;
    /// 設定透明度
    void SetAlphaDest(float value);
    float GetAlphaDest() const;
    bool IsTransparent() const;

private: // helper
    bool IsTransparentable() const;
    bool IsRayIntersectBoundingBox(const RAY& ray) const;
    bool IsRayIntersectSkinMesh(const D3DXMATRIX& viewMatrix, float maxDist, const RAY& ray) const;
    bool IsAlphaChanging() const;

public: // CDrawable stuff
    virtual bool Update(const UpdateArgsPtr &args);
    virtual bool Draw();
    /// over write
    virtual void RegisterForDrawing(SCNPtr scn);
    /// over write
    virtual bool IsVisible() const;
    /// over write
    virtual bool IsRayIntersect(const RAY& ray, HitInfo& res) const;
    /// over write
    virtual void UpdateBox() const;
    /// over write
    virtual void CleanUp();
    /// over write
    virtual float GetSqDistance() const;
    /// over write
    virtual float GetRadius() const;

private:
    typedef ObjFactory<HModel> HModelFactory;
    static HModelFactory sHModelFactory;

public: // operator
    void* operator new (size_t);
    void operator delete (void* p);

private:
    SkinMeshTempPtr mSkinMeshTemp;
    /// 中心點
    mutable D3DXVECTOR3 mCenterCache;
    mutable bool mCenterCacheOutOfData;
    /// animation controller
    LPD3DXANIMATIONCONTROLLER mAnimCtrller;
    /// 透明度
    float mAlphaDest;
    /// 目前透明度
    mutable float mAlphaValue;
};
//---------------------------------------------------------------------------
} // namespace FDO
//---------------------------------------------------------------------------
#endif // HModelH
