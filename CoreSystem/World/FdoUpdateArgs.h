/**
 * @file FdoUpdateArg.h
 * @author Yuming Ho
 */
#ifndef FdoUpdateArgsH
#define FdoUpdateArgsH

namespace FDO
{

class CUpdateArgs
{
public:
    CUpdateArgs(void);
    virtual ~CUpdateArgs(void);

public: // helper
    /// This culling function was used for model
    bool IsInBounding(const D3DXVECTOR3& min, const D3DXVECTOR3& max);
    bool IsInFieldOfView(const D3DXVECTOR3& center, float radius);
    /// This culling function was used for particle
    bool IsInRectRange(float x, float z) const;
    /// This culling function was used for particle(far)
    bool IsInGoViewRect(float x, float z) const;

public: // attribute
    void SetElapsedTime(float value);
    float GetElapsedTime() const;
    void SetViewRect(const D3DXVECTOR4& value);
    void SetRectRange(const D3DXVECTOR4& value);
    void SetRay(const RAY& ray);
    const RAY& GetRay() const;
    /// view matrix
    void SetViewMatrix(const D3DXMATRIX& value);
    const D3DXMATRIX& GetViewMatrix() const;
    /// inverse view matrix
    //void SetInvViewMatrix(const D3DXMATRIX& value);
    //const D3DXMATRIX& GeInvtViewMatrix() const;
    /// distance from camera to target
    float GetViewDistance() const;
    void SetViewDistance(float value);
    /// width
    void SetWidth(float value);
    float GetWidth() const;
    /// height
    void SetHeight(float value);
    float GetHeight() const;

private:
    float mElapsedTime;
    /// This variable was used for model culling
    D3DXVECTOR4 mViewRect;
    /// This variable was used for particle culling
    D3DXVECTOR4 mRectRange;
    /// This variable was used for particle culling
    D3DXVECTOR4 mGoViewRect;
    RAY mRay;
    D3DXMATRIX mViewMatrix;
    //D3DXMATRIX mInvViewMatrix;
    float mViewDistance;
    float mWidth;
    float mHeight;
    bool mMonster;
};

} // namespace FDO

#endif // FdoUpdateArgH