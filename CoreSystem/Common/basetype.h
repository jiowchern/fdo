#ifndef _basetype_h_20090617pm0543_
#define _basetype_h_20090617pm0543_

typedef signed	 __int8		sint8;
typedef unsigned __int8		uint8;
typedef	char				char8;

typedef signed	 __int16	sint16;
typedef unsigned __int16	uint16;
typedef			 wchar_t	char16;


typedef signed __int32		sint32;
typedef unsigned long		uint32;

typedef signed __int64		sint64;
typedef unsigned __int64	uint64;

typedef bool				boole;
typedef signed	int			sint;
typedef unsigned	int		uint;


template<typename T>
struct Rect
{
	T left;
	T top;
	T right;
	T bottom;
};
typedef RECT Rect32;


#endif