#include "stdafx.h"
  int HandleAsserting(const char *testStr,const char *msgStr,const char *fileStr,int line)
  {
    // assert message & and return flag regarding aborting:
    // callFlag set if repeating forbidden
    static char s_text[255]=""; // don't assign dynamically in 
                                // case of 'out of memory' error
    sprintf(s_text, "Assertion failed! \n"
					"File : %s\n"
					"Line : %d\n\n"
					"Expression : (%s)\n"
                    "Message : %s\n\n"
					"(Press Retry to debug the application - JIT must be enabled)",
                    fileStr, line, testStr, msgStr);
    switch(::MessageBoxA(NULL,s_text,"Assertion Error",MB_ABORTRETRYIGNORE|MB_ICONERROR) )
    {
      case IDIGNORE: 
		  return 0;
      case IDABORT:  
          abort();    // abort/break
	  default:
		  return 1;
    }
  }

  int HandleAsserting(const char *testStr,const wchar_t *msgStr,const char *fileStr,int line)
  {
	  return HandleAsserting(testStr, FDO::UnicodeToAnsi(msgStr), fileStr, line);
  }
