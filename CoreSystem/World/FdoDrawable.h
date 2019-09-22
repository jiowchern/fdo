/**
 * @file FdoDrawable.h
 * @author Yuming Ho
 */
#ifndef FdoDrawableH
#define FdoDrawableH

#include "FdoWorldPreRequest.h"

namespace FDO
{

class CDrawable
{
public:
    CDrawable(void);
    virtual ~CDrawable(void);

public: // pure virtual function    
    virtual bool Update(const UpdateArgsPtr &args) = 0;
    virtual bool Draw() = 0;
    virtual void RegisterForDrawing(SCNPtr scn) = 0;
    virtual bool IsRayIntersect(const RAY& ray, HitInfo& res) const;
    //virtual float GetViewDistance() const = 0;    
    //virtual bool HitTest(PICKINFO &pi,HitInfo &res) const = 0;
	//virtual bool SelectionTest(const RECT &rect,VIEW *view) const = 0;
    virtual void DrawHelpers(SCNPtr scn);
	//virtual const BBOX &GetBoundingBox() const = 0;
	//virtual const AABBOX_EX& GetAABBox() const = 0;
	//virtual void DrawImmediate() = 0;
    //virtual BOOL Pick(PICKINFO &pi,HitInfo &res,DWORD nIntersectMask = WORLDITEM_ALL) = 0;
	//virtual BOOL RayIntersect( const D3DXVECTOR3 &start, const D3DXVECTOR3 &dir, FLOAT maxdist, HitInfo &res,DWORD nIntersectMask = WORLDITEM_OBJECTS | WORLDITEM_TERRAIN ) = 0;
    virtual void CleanUp();

public: // attribute
    // get
    virtual const D3DXMATRIX& GetWorldMatrix() const;
    virtual const D3DXVECTOR3& GetPosition() const;
    virtual const D3DXVECTOR3& GetRotation() const;
    virtual const D3DXVECTOR3& GetScale() const;
    virtual const D3DXVECTOR3& GetOffset() const;
    /// this function was used for scene culling
    virtual const D3DXVECTOR3& GetMinBox() const;
    /// this function was used for scene culling
    virtual const D3DXVECTOR3& GetMaxBox() const;
    virtual float GetRadius() const;
    virtual float GetSqDistance() const;
    virtual bool IsVisible() const;
    virtual bool IsControlEnabled(int which) const;
    virtual void GetBoundingBox(D3DXVECTOR3 * pMin, D3DXVECTOR3 * pMax) const;
    // set
    virtual void SetPosition(float x, float y, float z);
    virtual void SetRotation(float x, float y, float z);
    virtual void SetScale(float x, float y, float z);
    virtual void SetOffset(float x, float y, float z);
    virtual void SetPosition(const D3DXVECTOR3& value);
    virtual void SetRotation(const D3DXVECTOR3& value);
    virtual void SetScale(const D3DXVECTOR3& value);
    virtual void SetOffset(const D3DXVECTOR3& value);
    virtual void SetVisible(bool value);
    virtual void SetControlEnabled(int which, bool value);

protected:
    /// world matrix flag
    mutable bool mWorldMatrixOutOfData;
    /// �@�ɯx�}
    mutable D3DXMATRIX mWorldMatrixCache;
    /// ��m
    D3DXVECTOR3 mPosition;
    /// ����
    D3DXVECTOR3 mRotation;
    /// �Y��
    D3DXVECTOR3 mScale;
    /// �첾
    D3DXVECTOR3 mOffset;
    /// min max point
    mutable MMBox mBox;
    mutable bool mBoxOutOfData;
    /// �g�L�ɶ�
    float mElapsedTime;
    ///
    bool mVisible;
    /// distance
    float mSqDistance;
    /// flag
    int mFlag;
    /// �b�|
    mutable float mRadius;
    mutable Box mBoundingBox;
    /// �b�|
    mutable bool mBoundsOutOfData;

protected:
    virtual void UpdateBounds() const;
    virtual void UpdateBox() const;
    virtual void UpdateWorldMatrix() const;
};

} // namespace FDO

#endif // FdoDrawableH