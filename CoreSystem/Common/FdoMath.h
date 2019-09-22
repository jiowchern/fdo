//---------------------------------------------------------------------------
#ifndef FdoMathH
#define FdoMathH

#include <d3dx9.h>

namespace FDO
{

struct HitInfo 
{
    //WorldItemObjectID oid;
    bool isbbox;
    D3DXVECTOR3 pos;
    /// the surface normal of the object we hit.
    D3DXVECTOR3 normal;
    float dist;
    bool gizmo;
    union Data
    {
        UINT axis;
        DWORD part;
    } data;
};

struct RAY
{
    RAY();
    RAY(const D3DXVECTOR3 &orig, const D3DXVECTOR3 &dir);
    D3DXVECTOR3 Orig;
	D3DXVECTOR3 Dir;
};

#pragma pack(push,1)
typedef struct TPoint : public POINT
{
public:
    TPoint();
    TPoint(int _x, int _y);
    TPoint(const POINT& pt);
    TPoint(const TPoint& pt);

    bool operator ==(const TPoint& pt) const;
    bool operator !=(const TPoint& pt) const;

    TPoint operator+(const TPoint &o);
    TPoint operator-(const TPoint &o);
    TPoint& operator+=(int n);
    TPoint& operator-=(int n);

    bool IsEmpty();
    float Distance(POINT &o);
} *PTPOINT;
//---------------------------------------------------------------------------
typedef struct TRect : public RECT
{
public:
    TRect();
    TRect(const TPoint& TL, const TPoint& BR);
    TRect(int l, int t, int r, int b);
    TRect(const RECT& r);
    TRect(const TRect& r);

    int Width () const;
    int Height() const;

    bool operator ==(const TRect& rc) const;
    bool operator !=(const TRect& rc) const;

    bool Contains(const TPoint& p) const;
} *PTRECT;
//---------------------------------------------------------------------------
struct Line
{
    enum { count = 2 };
    Line();
    Line(const D3DXVECTOR3& v0, const D3DXVECTOR3& v1);
    D3DXVECTOR3 pts[count];
    float GetLength() const;
};

struct Triangle
{
    enum { count = 3 };
    Triangle();
    Triangle(const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2);
    D3DXVECTOR3 pts[count];
    bool IsRayIntersect(const RAY& ray) const;
};

struct Plane
{
    enum { count = 4 };
    Plane();
    Plane(const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3);
    D3DXVECTOR3 pts[count];
};

struct MMBox
{
    MMBox();
    MMBox(const D3DXVECTOR3& _min, const D3DXVECTOR3& _max);
    D3DXVECTOR3 min;
    D3DXVECTOR3 max;
};

struct Box
{
    enum { count = 8 };
    Box();
    Box(const MMBox& box);
    D3DXVECTOR3 pts[count];
    bool IsRayIntersect(const RAY& ray, HitInfo& res) const;
    const D3DXVECTOR3& GetMin() const;
    const D3DXVECTOR3& GetMax() const;
    void operator =(const MMBox& box);
    float GetRadius() const;
    static unsigned int indices[36];
};
//---------------------------------------------------------------------------
#pragma pack(pop)

double Pt2LineDis( double x, double y, double x1, double y1, double x2, double y2 );

//bool IntersectTriangle( const D3DXVECTOR3& orig, const D3DXVECTOR3& dir,
//    const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2,
//    FLOAT* t, FLOAT* u, FLOAT* v );

//bool IntersectTriangle(const RAY& ray, const D3DXVECTOR3& v0, const D3DXVECTOR3& v1,
//     const D3DXVECTOR3& v2, FLOAT* t, FLOAT* u, FLOAT* v);

//bool IntersectTriangle(const RAY& ray, const Triangle& tri);

} // namespace FDO

#endif // FdoMathH