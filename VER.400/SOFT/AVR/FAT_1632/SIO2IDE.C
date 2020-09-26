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

#include "hal4usb.h"
#include "usb_reg.h"

//
//      SW identification string.
//
FLASH UINT8  VerInfo[] PROGMEM =
"\n"PLAT_NAME" ver. "__SIO2IDE_VER_TXT__" (c)2004 MMSoft for ATMEL ATmega"AT_MEGA"\n";

#if USB==1
STATIC T_drvinf   DrvInfo;
#endif
STATIC volatile UINT16     DTimer = 0;
STATIC volatile UINT16     WTimer = 0;

//----------------------------------------------------------------
// Function :   Timer_Update
// Notes    :
// History  :
//----------------------------------------------------------------

VOID Timer_Update( UINT8 msec )
{
#if SIO==1
  SIO_TimerUpd();
#endif
#if USB==1
  MSBOT_TimerUpd();
#endif
  DTimer += msec;
  WTimer += msec;
}

//----------------------------------------------------------------
// Function :   ErrorBlink
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID ErrorBlink( UINT16 spd )
{
  if( DTimer > spd )
  {
    SIO_RDYL_TGL();
    DTimer = 0;
  }
}

//----------------------------------------------------------------
// Function :   Wait
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID Wait( UINT16 spd )
{
  WTimer = 0;
  while( WTimer < spd );
}

//----------------------------------------------------------------
// Function :   Reset
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID Reset( VOID )
{
  // Init Watchdog (1.9 sek)
  WDTCR = ((1<<WDE) | 0x07);
  for(;;);
}

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
restart:
  //
  TMRS_Init();
  IO_Init();
  //
  // Check if MODE jumper is ON or OFF
  // ON  - IDE2USB
  // OFF - SIO2IDE
  //
  if( !FS_MOD_ON() )
  {
#if SIO==1
    //
    // SIO2IDE mode
    //
    if( !FS_Init() )              // Initialise File System
    {                             //
      for(;;)                     //  STOP if error
      {                           //
        ErrorBlink( 200 );        //  Blink BSY_LED
      }                           //
    }                             //

    disable_interrupt();

    SIO_Init();                   // Initialise SIO driver

    enable_interrupt();

    for(;;)                       // Application foreground task
    {                             //
      SIO_Run();                  //   Poll SIO driver
    }                             //
#else
    Reset();     // RESET
#endif
  }
  else
  {
#if USB==1
#ifdef DEBUG
    init_comm();
    Printf( "\nStart" );
#endif

//
// TEST mode procedure
//
    SIO_RDYL_ON();
    WTimer = 0;
    enable_interrupt();
    do
    {
      if( !FS_MOD_ON() )
      {
        SIO_RDYL_OFF();
        Wait( 100 );
        SIO_RDYL_ON();
        Wait( 100 );
        SIO_RDYL_OFF();
        //
        disable_interrupt();
        //
        SIO_Init();
        //
        enable_interrupt();
        //
        FATFS_Test();
        //
        for(;;)                                 //  STOP if test mode
        {                                       //
          ErrorBlink( 40 );                     //  Blink BSY_LED
          if( FS_MOD_ON() )
          {
            Wait( 100 );
            if( FS_MOD_ON() )
            {
              Printf( "\nRestart" );
              goto restart;
            }
          }
        }                                       //  test END
      }
    }while( WTimer < 3000 );                    // wait 3 sec
    //
    SIO_RDYL_OFF();                             // Test not entered
    disable_interrupt();

//
// IDE2USB mode
//
    hal4usb_init();

    TST1_INIT();
    TST2_INIT();
    //
    if( IDE_Init( &DrvInfo ) != IDE_HD )        // Initialise File System
    {                                           //
      for(;;)                                   //  STOP if error
      {                                         //
        ErrorBlink( 200 );                      //  Blink BSY_LED
      }                                         //
    }                                           //
#ifdef DEBUG
    Printf( "\nHDD" );
    Printf( "\nH=%i", DrvInfo.hd );
    Printf( "\nC=%i", DrvInfo.cyl );
    Printf( "\nS=%i", DrvInfo.spt );
#endif

    disable_interrupt();

    USB_Init();                                 // USB driver
    RBC_Init( &DrvInfo );

    enable_interrupt();

    for(;;)
    {
      usb_node_handler();

      MSBOT_Handler();

      if( DTimer > 500 )
      {
        SIO_RDYL_TGL();
        DTimer = 0;
      }
    }
#else
    Reset();     // RESET
#endif
  }

#ifdef __GNU__
  return 1;                     //
#endif
}                               //

//      End
