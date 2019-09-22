//---------------------------------------------------------------------------
#ifndef CommonH
#define CommonH

#include "basetype.h"
#include <d3dx9math.h>
#include <ctime>
#include "PhenixAssert.h"
//---------------------------------------------------------------------------
namespace FDO
{

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

#define _CLAMP(n,_min,_max) MIN(MAX(n,_min),_max)
#define _CLAMPI(n,_min,_max) ((INT)MIN(MAX(n,_min),_max))

extern const int BIT_NUM[];

const D3DCOLOR        WHITE( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );   // 白色
const D3DCOLOR        BLACK( D3DCOLOR_ARGB( 255,   0,   0,   0 ) );   // 黑色
const D3DCOLOR         GRAY( D3DCOLOR_ARGB( 255, 100, 100, 100 ) );   // 灰色
const D3DCOLOR          ASH( D3DCOLOR_ARGB( 255, 200, 200, 200 ) );   // 淡灰色
const D3DCOLOR     CHARCOAL( D3DCOLOR_ARGB( 255,  50,  50,  50 ) );   // 深灰色

const D3DCOLOR         ROSY( D3DCOLOR_ARGB( 255, 242, 109, 125 ) );   // 玫瑰紅
const D3DCOLOR         PINK( D3DCOLOR_ARGB( 255, 255, 170, 255 ) );   // 粉紅色
const D3DCOLOR     ROSEPINK( D3DCOLOR_ARGB( 255, 240, 155, 190 ) );   // 淡粉紅色
const D3DCOLOR          RED( D3DCOLOR_ARGB( 255, 255,  34,  53 ) );   // 紅色
const D3DCOLOR      PINKISH( D3DCOLOR_ARGB( 255, 232, 152, 137 ) );   // 桃紅色
const D3DCOLOR      PURERED( D3DCOLOR_ARGB( 255, 255,   0,   0 ) );   // 純紅色

const D3DCOLOR         LIME( D3DCOLOR_ARGB( 255, 109, 242, 125 ) );   // 淡綠色
const D3DCOLOR      EMERALD( D3DCOLOR_ARGB( 255, 167, 255, 137 ) );   // 翠綠色
const D3DCOLOR        GREEN( D3DCOLOR_ARGB( 255, 130, 202, 156 ) );   // 綠色
const D3DCOLOR         TEAL( D3DCOLOR_ARGB( 255,  57,  98,   0 ) );   // 橄欖綠
const D3DCOLOR    PUREGREEN( D3DCOLOR_ARGB( 255,   0, 255,   0 ) );   // 純綠色
const D3DCOLOR    DARKGREEN( D3DCOLOR_ARGB( 255,   0, 120,  50 ) );   // 深綠色

const D3DCOLOR    LIGHTBLUE( D3DCOLOR_ARGB( 255,  66, 255, 255 ) );   // 淡藍色
const D3DCOLOR         AQUA( D3DCOLOR_ARGB( 255, 200, 243, 216 ) );   // 水綠色
const D3DCOLOR         NAVY( D3DCOLOR_ARGB( 255,  64, 140, 163 ) );   // 海藍色
const D3DCOLOR         BLUE( D3DCOLOR_ARGB( 255,   0, 174, 239 ) );   // 藍色
const D3DCOLOR     MAZARINE( D3DCOLOR_ARGB( 255,   0,  50, 200 ) );   // 深藍色
const D3DCOLOR     PUREBLUE( D3DCOLOR_ARGB( 255,   0,   0, 255 ) );   // 純藍色
const D3DCOLOR    BLUEGREEN( D3DCOLOR_ARGB( 255,   0, 255, 255 ) );   // 藍綠色
const D3DCOLOR      SKYBLUE( D3DCOLOR_ARGB( 255,   5, 176, 190 ) );   // 天藍色

const D3DCOLOR  LIGHTYELLOW( D3DCOLOR_ARGB( 255, 255, 244, 104 ) );   // 淡黃色
const D3DCOLOR       YELLOW( D3DCOLOR_ARGB( 255, 255, 235,  41 ) );   // 黃色
const D3DCOLOR  YELLOWBROWN( D3DCOLOR_ARGB( 255, 220, 182,  41 ) );   // 土黃色
const D3DCOLOR   PUREYELLOW( D3DCOLOR_ARGB( 255, 255, 255,   0 ) );   // 純黃色
const D3DCOLOR       SULFUR( D3DCOLOR_ARGB( 255, 182, 205,  20 ) );   // 黃綠色
const D3DCOLOR		   GOLD( D3DCOLOR_ARGB( 255, 200, 200,  43 ) );	  // 黃金色

const D3DCOLOR       ORANGE( D3DCOLOR_ARGB( 255, 243, 152,  26 ) );   // 橙色
const D3DCOLOR       PURPLE( D3DCOLOR_ARGB( 255, 197, 137, 163 ) );   // 紫色
const D3DCOLOR  LIGHTPURPLE( D3DCOLOR_ARGB( 255, 255, 200, 255 ) );   // 淡紫色
const D3DCOLOR       MAROON( D3DCOLOR_ARGB( 255, 230,  90,  30 ) );   // 褐紅色
const D3DCOLOR       COFFEE( D3DCOLOR_ARGB( 255, 170,  90,  30 ) );   // 咖啡色
const D3DCOLOR         SKIN( D3DCOLOR_ARGB( 255, 255, 194, 137 ) );   // 皮膚色
const D3DCOLOR    TANGERINE( D3DCOLOR_ARGB( 255, 255,  98,  19 ) );   // 橘紅色
const D3DCOLOR   PUREPURPLE( D3DCOLOR_ARGB( 255, 255,   0, 255 ) );   // 純紫色

const D3DCOLOR    ROGERBLUE( D3DCOLOR_ARGB( 255, 135, 223, 255 ) );   // Roger藍

const D3DXCOLOR	 BEACH_SAND( D3DCOLOR_XRGB(255, 249, 157) );
const D3DXCOLOR   DARKBROWN( D3DCOLOR_XRGB(115, 100,  87));

const D3DXCOLOR LIGHT_YELLOW_GREEN( D3DCOLOR_XRGB(124, 197, 118) );
const D3DXCOLOR  DARK_YELLOW_GREEN( D3DCOLOR_XRGB( 25, 123,  48) );

const D3DXVECTOR2 VEC2_ZERO         (0.0f, 0.0f);
const D3DXVECTOR2 VEC2_UNIT_SCALE   (1.0f, 1.0f);
const D3DXVECTOR2 VEC2_UNIT_X       (1.0f, 0.0f);
const D3DXVECTOR2 VEC2_UNIT_Y       (0.0f, 1.0f);
const D3DXVECTOR2 VEC2_M_UNIT_X     (-1.0f,  0.0f);
const D3DXVECTOR2 VEC2_M_UNIT_Y     ( 0.0f, -1.0f);

const D3DXVECTOR3 VEC3_ZERO         (0.0f, 0.0f, 0.0f);
const D3DXVECTOR3 VEC3_UNIT_X       (1.0f, 0.0f, 0.0f);
const D3DXVECTOR3 VEC3_UNIT_Y       (0.0f, 1.0f, 0.0f);
const D3DXVECTOR3 VEC3_UNIT_Z       (0.0f, 0.0f, 1.0f);
const D3DXVECTOR3 VEC3_UNIT_SCALE   (1.0f, 1.0f, 1.0f);

const D3DXVECTOR3 VEC3_M_UNIT_X     (-1.0f,  0.0f,  0.0f);
const D3DXVECTOR3 VEC3_M_UNIT_Y     ( 0.0f, -1.0f,  0.0f);
const D3DXVECTOR3 VEC3_M_UNIT_Z     ( 0.0f,  0.0f, -1.0f);

const float RADIAN = D3DX_PI / 180.0f;
const unsigned int sizeof_MATRIX4 = sizeof(float) * 16;
const unsigned int sizeof_D3DLIGHT9 = sizeof(D3DLIGHT9);
const unsigned int sizeof_D3DMATERIAL9 = sizeof(D3DMATERIAL9);
const unsigned int sizeof_BITMAPINFOHEADER = sizeof(BITMAPINFOHEADER);
const unsigned int sizeof_BITMAPFILEHEADER = sizeof(BITMAPFILEHEADER);
const unsigned int sizeof_WORD = sizeof(WORD);
const unsigned int sizeof_int = sizeof(int);
const unsigned int sizeof_UINT = sizeof(UINT);
const unsigned int sizeof_BYTE = sizeof(BYTE);

// 雙碼 <=> 萬國碼
//WCHAR <-> ASNI (Multi-Byte) (傳回內部buffer指標!!!小心使用)
const WCHAR* AnsiToUnicode(const char* szSource);
const char* UnicodeToAnsi(const WCHAR* wszSrc);

const WCHAR* AnsiToUnicodeThread1(const char* szSrc);
const WCHAR* AnsiToUnicodeThread2(const char* szSrc);

// WCHAR <-> UTF8
int WCHARToUTF8(char *utf8, const WCHAR *unicode, int maxoutput);
int UTF8ToWCHAR(WCHAR *unicode, const char *utf8, int maxoutput);

LPCWSTR LangConv(LPCWSTR wszSrc, WORD wLangID);


} // namespace FDO

extern LPDIRECT3DDEVICE9 g_pD3DDevice;
// TalkBar
enum eTalkKind
{
	Talk_Total  = 0,
	Talk_Team   = 1,
	Talk_Guild  = 2,
	Talk_Secret = 3,
	Talk_GameMaster = 4,
	Talk_System = 5,
	Talk_Map = 6,
};

//--------------------------------------------------------------------------------------

// gino add : 
// some additional declaration and inline functions for rapid porting from BCB6 to VC2005
// Not in FDO namespace
typedef std::string String;
typedef std::vector< std::string > StringList;

class TPersistent{
public:
	virtual ~TPersistent(){};
};

class TComponent{
};

inline void randomize()
{
	srand((unsigned)time(NULL));
}

inline int random(int max)
{
	return (max*rand())/(RAND_MAX+1);
}

inline int ToInt(const String &s)
{
	return atoi(s.c_str());
}

inline int ToInt(const char *s)
{
	return atoi(s);
}

inline int ToInt(const wchar_t *s)
{
	return _wtoi(s);
}

inline double ToDouble(const String &s)
{
	return atof(s.c_str());
}

inline String ToString(double v)
{
	char buffer[_CVTBUFSIZE];
	_gcvt( v, 12, buffer );
	return String(buffer);
}

inline String ToString(int v)
{
	char str[64];
	_itoa(v, str, 10);
	return String(str);
}

struct ITextDatabase
{
	virtual std::wstring GetText(int nText) const = 0;
};


template<typename TCharType,int (*fn)(TCharType*,size_t ,const TCharType*,va_list)>
void fnStringFormat(std::basic_string<TCharType, std::char_traits<TCharType>,std::allocator<TCharType>>& str, const TCharType* fmt, ...)   
{   
	//typedef std::basic_string<TCharType, char_traits<TCharType>,allocator<TCharType> > tstr;
	va_list   argptr;   
	va_start(argptr,   fmt);   
	int   bufsize   =   fn(NULL,   0,   fmt,   argptr)   ;   
	str.resize(bufsize);
	if(bufsize)
	{
		TCharType*   buf = &*str.begin();
		// 		TCharType*   buf   =   new   TCharType[bufsize];   
		fn(buf,   bufsize,   fmt,   argptr);    		
	}	
	// 		delete[]   buf;       
	va_end(argptr);     
}
#pragma warning(disable : 4995)
#define StringFormat16 fnStringFormat<wchar_t,_vsnwprintf> 
#define StringFormat8 fnStringFormat<char,vsnprintf>

/// <yuming> ref: typhoon
#define _FOREACH(stlType, stlObj, itr) \
	for (stlType::iterator itr(stlObj.begin()); itr != stlObj.end(); ++itr)

#define _FOREACH_CONST(stlType, stlObj, itr) \
	for(stlType::const_iterator itr(stlObj.begin()); itr != stlObj.end(); ++itr)

#define _RFOREACH(stlType, stlObj, itr) \
	for (stlType::reverse_iterator itr(stlObj.rbegin()); itr != stlObj.rend(); ++itr)

#define _RFOREACH_CONST(stlType, stlObj, itr) \
	for(stlType::const_reverse_iterator itr(stlObj.rbegin()); itr != stlObj.rend(); ++itr)

#define _LOOP(val, i) for(int i = 0; i < val; ++i)
#define _ULOOP(val, i) for(unsigned int i = 0; i < val; ++i)

#define F_RETURN(x)    { if (!(x)) { return false; } }
#define T_RETURN(x)    { if (x) { return true; } }
#define V_RETURN(x)    { hr = (x); if (FAILED(hr)) { return false; } }
#define F_CONTINUE(x)  { if (!(x)) { continue; } }

template <class T>
inline void SafeDelete(T& p)
{
    delete p;
	p = 0;
}

template <class T>
inline void SafeDeleteArray(T& p)
{
    delete[] p;
    p = 0;
}

template <class T>
inline void SafeRelease(T& p)
{
	if (p != 0)
    {
        p->Release();
		p = 0;
    }
}

template<typename T>
inline T Lerp(float t, const T& a, const T& b)
{
    return a + t * (b - a);
}

template<typename T>
inline T D3DLerp(T a, T b, T t)
{
	return ( a - (a*t) + (b*t) );
}

//template<class T>
//inline const T& max(const T& a, const T& b)
//{
//    return a > b ? a : b; 
//}
//
//template<class T>
//inline const T& min(const T& a, const T& b)
//{ 
//    return a < b ? a : b;
//}
#define max(a, b)  (((a) > (b)) ? (a) : (b))
#define min(a, b)  (((a) < (b)) ? (a) : (b))

const float ALPHA_OPAQUE = 1.f;
const float ALPHA_TRANSPARENT = 0.f;
const float ALPHA_FADE_TIME = 1.f;

const float Epsilon0 = 0.000001f;
const float Epsilon1 = 0.999999f;

/// </yuming>

// void fnGameDebugMessageA(const char8*);
// void fnGameDebugMessageW();

#define GameDebugMessageA fnGameDebugMessageA
#define GameDebugMessageW fnGameDebugMessageW

template<typename T>
struct TDGArray
{
	template<typename TArrayType , size_t nCnt>
	TDGArray(TArrayType(&arr)[nCnt]) : pObject(arr),nCount(nCnt)
	{

	}
	T*			pObject;
	uint		nCount;
	T&	operator[] (const int i);
	
};
template<typename T> 
T&	TDGArray<T>::operator[] (const int i)
{
	return pObject[i];
}


bool IsBit(void* pBuffer, sint offsetBit);

#endif
