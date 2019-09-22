//---------------------------------------------------------------------------
#ifndef __PhenixAssert_H_
#define __PhenixAssert_H_
//---------------------------------------------------------------------------
/* 
  modify from http://dn.codegear.com/tw/article/28432
*/

int HandleAsserting(const char *testStr, const char *msgStr, const char *fileStr, int line);
int HandleAsserting(const char *testStr, const wchar_t *msgStr, const char *fileStr, int line);
#define Assert(test,msg)  { \
	if (!(test)) { \
        if (HandleAsserting(#test,msg,__FILE__,__LINE__)) \
			{ _asm { int 3 } } \
	}\
}
/*
#if defined _DEBUG

    #include <assert.h>

    #define Assert( exp, description ) assert( (exp) && (description) )  
#else
    #define Assert( exp, description )
#endif
*/
//---------------------------------------------------------------------------
#endif
