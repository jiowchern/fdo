/*
  Filename    : Defines.H
  Author      : Curtis Lo
  Date        : 2003/10/23
  Descirption : Header file of defines
*/
// Avoid recurrence include this file, Start condition compile
#ifndef __Defines_H
#define __Defines_H

 #ifdef __BORLANDC__
  // Set data member's align byte to 1 bytes
  #pragma option push
  #pragma option -a1
 #else
  // Adjust structure alignment to 1 byte ( default is 8 byte )
  #pragma pack( push )
  #pragma pack( 1 )
 #endif

 // Define global symbol directory size
 #define _MaxFilenameSize       256
 #define _MaxPathSize           256

 #ifdef __BORLANDC__
  // Set data member's align byte to 4 bytes
  #pragma option pop
 #else
  // Back to default alignment ( 8 byte )
  #pragma pack( pop )
 #endif

// End condition compile
#endif

 