#include "StdAfx.h"
#include "FdoMath.h"

namespace FDO
{

//---------------------------------------------------------------------------
// Given a ray origin (orig) and direction (dir), and three vertices of a triangle, this
// function returns TRUE and the interpolated texture coordinates if the ray intersects
// the triangle
// 給予一個點及方向，和一個三角形的三個點
// 如果一個射線交叉三角形的話會傳回TRUE
// 並且替換材質座標
//---------------------------------------------------------------------------
bool IntersectTriangle( const D3DXVECTOR3& orig, const D3DXVECTOR3& dir,
    const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2,
    FLOAT* t, FLOAT* u, FLOAT* v )
{
    // Find vectors for two edges sharing vert0
    // 找出兩個邊
    D3DXVECTOR3 vEdge1 = v1 - v0;
    D3DXVECTOR3 vEdge2 = v2 - v0;

    // Begin calculating determinant - also used to calculate U parameter
    // 開始計算判定
    D3DXVECTOR3 vPvec;
    D3DXVec3Cross( &vPvec, &dir, &vEdge2 );

    // If determinant is near zero, ray lies in plane of triangle
    float fDet = D3DXVec3Dot( &vEdge1, &vPvec );
    
    // Calculate distance from vert0 to ray origin
    // 假如射線判定接近零的話，此射線是在平面上
    D3DXVECTOR3 vTvec;
    if( fDet > 0.0f )
    {
        vTvec = orig - v0; // 射線從正面穿越三角形
    }
    else
    {
        vTvec = v0 - orig; // 射線從反面穿越三角形
        fDet = -fDet;
    }

    // 假如射線判定接近零的話，此射線是在平面上
    // 所以表示射線沒有穿透三角形
    if( fDet < 0.0001f )
        return false;

    // Calculate U parameter and test bounds
    float fPickU = D3DXVec3Dot( &vTvec, &vPvec );
    if( (fPickU < 0.0f) || (fPickU > fDet) )
        return false;

    // Prepare to test V parameter
    D3DXVECTOR3 vQvec;
    D3DXVec3Cross( &vQvec, &vTvec, &vEdge1 );

    // Calculate V parameter and test bounds
    float fPickV = D3DXVec3Dot( &dir, &vQvec );
    if( (fPickV < 0.0f) || (fPickU + fPickV > fDet) )
        return false;

    return true;
}
//---------------------------------------------------------------------------
TPoint::TPoint()
{
    x = 0;
    y = 0;
}
//---------------------------------------------------------------------------
TPoint::TPoint(int _x, int _y)
{
    x = _x;
    y = _y;
}
//---------------------------------------------------------------------------
TPoint::TPoint(const POINT& pt)
{
    x = pt.x;
    y = pt.y;
}
//---------------------------------------------------------------------------
TPoint::TPoint(const TPoint& pt)
{
    x = pt.x;
    y = pt.y;
}
//---------------------------------------------------------------------------
bool TPoint::operator ==(const TPoint& pt) const
{
    return (x == pt.x) && (y == pt.y);
}
//---------------------------------------------------------------------------
bool TPoint::operator !=(const TPoint& pt) const
{
    return !(pt == *this);
}
//---------------------------------------------------------------------------
TPoint TPoint::operator+(const TPoint &o)
{
    return TPoint(x + o.x, y + o.y);
}
//---------------------------------------------------------------------------
TPoint TPoint::operator-(const TPoint &o)
{
    return TPoint(x - o.x, y - o.y);
}
//---------------------------------------------------------------------------
TPoint& TPoint::operator+=(int n)
{
    x += n;
    y += n;
    return *this;
}
//---------------------------------------------------------------------------
TPoint& TPoint::operator-=(int n)
{
    x -= n;
    y -= n;
    return *this;
}
//---------------------------------------------------------------------------
bool TPoint::IsEmpty()
{
    return ((x == 0) && (y == 0));
}
//---------------------------------------------------------------------------
float TPoint::Distance(POINT &o)
{
    int dx = x - o.x;
    int dy = y - o.y;
    return sqrt((float)(dx*dx + dy*dy));
}
//---------------------------------------------------------------------------
TRect::TRect()
{
    left = 0;
    top = 0;
    right = 0;
    bottom = 0;
}
//---------------------------------------------------------------------------
TRect::TRect(const TPoint& TL, const TPoint& BR)
{
    left = TL.x;
    top = TL.y;
    right = BR.x;
    bottom = BR.y;
}
//---------------------------------------------------------------------------
TRect::TRect(int l, int t, int r, int b)
{
    left = l;
    top = t;
    right = r;
    bottom = b;
}
//---------------------------------------------------------------------------
TRect::TRect(const RECT& r)
{
    left = r.left;
    top = r.top;
    right = r.right;
    bottom = r.bottom;
}
//---------------------------------------------------------------------------
TRect::TRect(const TRect& r)
{
    left = r.left;
    top = r.top;
    right = r.right;
    bottom = r.bottom;
}
//---------------------------------------------------------------------------
int TRect::Width () const
{
    return (right - left);
}
//---------------------------------------------------------------------------
int TRect::Height() const
{
    return (bottom - top);
}
//---------------------------------------------------------------------------
bool TRect::operator ==(const TRect& rc) const
{
   return ((left == rc.left) && (top == rc.top) &&
          (right == rc.right) && (bottom == rc.bottom));
}
//---------------------------------------------------------------------------
bool TRect::operator !=(const TRect& rc) const
{
    return !(rc == (*this));
}
//---------------------------------------------------------------------------
bool TRect::Contains(const TPoint& p) const
{
   return ((p.x >= left) && (p.y >= top) && (p.x < right) && (p.y < bottom));
}

Line::Line()
{
    for (int i = 0; i < count; ++i)
    {
        pts[i] = VEC3_ZERO;
    }
}

Line::Line(const D3DXVECTOR3& v0, const D3DXVECTOR3& v1)
{
    pts[0] = v0;
    pts[1] = v1;
}

float Line::GetLength() const
{
    return D3DXVec3Length(&(pts[0] - pts[1]));
}

RAY::RAY(void)
: Orig(VEC3_ZERO)
, Dir(VEC3_ZERO)
{
}

RAY::RAY(const D3DXVECTOR3 &orig, const D3DXVECTOR3 &dir)
: Orig(orig)
, Dir(dir)
{
}

Triangle::Triangle()
{
    for (int i = 0; i < count; ++i)
    {
        pts[i] = VEC3_ZERO;
    }
}

Triangle::Triangle(const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2)
{
    pts[0] = v0;
    pts[1] = v1;
    pts[2] = v2;
}

bool Triangle::IsRayIntersect(const RAY& ray) const
{
    return IntersectTriangle(ray.Orig, ray.Dir, pts[0], pts[1], pts[2], NULL, NULL, NULL);
}

Plane::Plane()
{
    for (int i = 0; i < count; ++i)
    {
        pts[i] = VEC3_ZERO;
    }
}

Plane::Plane(const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3)
{
    pts[0] = v0;
    pts[1] = v1;
    pts[2] = v2;
    pts[3] = v3;
}

MMBox::MMBox()
: min(VEC3_ZERO)
, max(VEC3_ZERO)
{
}

MMBox::MMBox(const D3DXVECTOR3& _min, const D3DXVECTOR3& _max)
: min(_min)
, max(_max)
{
}

Box::Box()
{
    for (int i = 0; i < count; ++i)
    {
        pts[i] = VEC3_ZERO;
    }
}

Box::Box(const MMBox& box)
{
    pts[0] = D3DXVECTOR3(box.min.x, box.min.y, box.min.z); // xyz
    pts[1] = D3DXVECTOR3(box.min.x, box.max.y, box.min.z); // Xyz
    pts[2] = D3DXVECTOR3(box.max.x, box.max.y, box.min.z); // xYz
    pts[3] = D3DXVECTOR3(box.max.x, box.min.y, box.min.z); // XYz
    pts[4] = D3DXVECTOR3(box.min.x, box.min.y, box.max.z); // xyZ
    pts[5] = D3DXVECTOR3(box.min.x, box.max.y, box.max.z); // XyZ
    pts[6] = D3DXVECTOR3(box.max.x, box.max.y, box.max.z); // xYZ
    pts[7] = D3DXVECTOR3(box.max.x, box.min.y, box.max.z); // XYZ
}

float Box::GetRadius() const
{
    return Line(GetMin(), GetMax()).GetLength() / 2.f;
}

void Box::operator =(const MMBox& box)
{
    pts[0] = D3DXVECTOR3(box.min.x, box.min.y, box.min.z); // xyz
    pts[1] = D3DXVECTOR3(box.min.x, box.max.y, box.min.z); // Xyz
    pts[2] = D3DXVECTOR3(box.max.x, box.max.y, box.min.z); // xYz
    pts[3] = D3DXVECTOR3(box.max.x, box.min.y, box.min.z); // XYz
    pts[4] = D3DXVECTOR3(box.min.x, box.min.y, box.max.z); // xyZ
    pts[5] = D3DXVECTOR3(box.min.x, box.max.y, box.max.z); // XyZ
    pts[6] = D3DXVECTOR3(box.max.x, box.max.y, box.max.z); // xYZ
    pts[7] = D3DXVECTOR3(box.max.x, box.min.y, box.max.z); // XYZ
}

unsigned int Box::indices[36] =
{
    0, 1, 2, // front side
    0, 2, 3,
    4, 6, 5, // back side
    4, 7, 6,
    4, 5, 1, // left side
    4, 1, 0,
    3, 2, 6, // right side
    3, 6, 7,
    1, 5, 6, // top
    1, 6, 2,
    4, 0, 3, // bottom
    4, 3, 7
};

const D3DXVECTOR3& Box::GetMin() const
{
    return pts[0];
}

const D3DXVECTOR3& Box::GetMax() const
{
    return pts[6];
}

bool Box::IsRayIntersect(const RAY& ray, HitInfo& res) const
{
    for (unsigned char n = 0; n < 12; ++n) // 12個面
    {
        Triangle tri
        (
            pts[indices[3 * n + 0]], // v0
            pts[indices[3 * n + 1]], // v1
            pts[indices[3 * n + 2]]  // v2
        );

        if (tri.IsRayIntersect(ray))
        {
            return true;
        }
    }
    return false;
}

double Pt2LineDis( double x, double y, double x1, double y1, double x2, double y2 )
{
    return ( fabs((y1 - y2) * x + (x2 - x1) * y + x1 * y2 - x2 * y1) / hypot( y1-y2, x2-x1 ) );
}

}