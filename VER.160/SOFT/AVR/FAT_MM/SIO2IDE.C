//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: sio2ide.c
//
// MODULE NAME: sio2ide
//
// PURPOSE:     SIO2IDE main module.
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001    6-12-2001 MM     Created
//
//****************************************************************

#include "apps.h"

//
//      SW identification string.
//
FLASH UINT8  VerInfo[] PROGMEM = "\n"PLAT_NAME" ver. "__SIO2IDE_VER_TXT__" (c)2001 MMSoft\n";

//----------------------------------------------------------------
// Function :   main
// Notes    :
// History  :
//----------------------------------------------------------------

#ifdef __GNU__
int main( VOID )
#endif
#ifdef __IAR__
VOID main( VOID )
#endif
{
  if( !FS_Init() )              // Initialise File System
  {                             //
    for(;;);                    //  STOP if error
  }                             //

  disable_interrupt();

  SIO_Init();                   // Initialise SIO driver

  enable_interrupt();

  for(;;)                       // Application foreground task
  {                             //
    SIO_Run();                  //   Poll SIO driver
  }                             //
#ifdef __GNU__
  return 1;                     //
#endif
}                               //

//      End