/*
  Filename    : Callbacks.H
  Author      : Curtis Lo
  Date        : 2003/10/22
  Descirption : Header file of callback function definion
*/
// Avoid recurrence include this file, Start condition compile
#ifndef __Callbacks_H
#define __Callbacks_H

 // Define symbol display error message callback function
 typedef void (*_DemcbFunction)( char * );

 // Define symbol display scan filename callback function
 typedef void (*_DsfcbFunction)( char * );

 // Define symbol display progress text callback function
 typedef void (*_DptcbFunction)( char * );

 // Define symbol display progress percent callback function
 typedef void (*_DppcbFunction)( int );


// End condition compile
#endif

 