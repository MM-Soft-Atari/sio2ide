//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE:
//
// MODULE NAME:
//
// PURPOSE:
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001   26-08-2002 MM     Created
//
//****************************************************************

#ifndef __IO_H__
  #define __IO_H__

//
//      SIO LED
//
//   Port (PD)
#define SRDYL                   (1<<7)
//
#define SIO_RDYL_INIT()         __outp( DDRD, (__inp(DDRD) | SRDYL) )
#define SIO_RDYL_ON()           __outp( PORTD, (__inp(PORTD) & ~SRDYL) )
#define SIO_RDYL_OFF()          __outp( PORTD, (__inp(PORTD) | SRDYL) )
#define SIO_RDYL_TGL()          __outp( PORTD, (__inp(PORTD) ^ SRDYL) )
//
//      RESET_OUT MODE_IN
//
//   Port (PD)
#define FRESO                   (1<<3)
//
#define FS_MOD_ON()             (__port_and(DDRD,~FRESO),\
                                (BOOL)((__inp(PIND) & FRESO) ? FALSE : TRUE) )
#define FS_RES_LO()             __port_or(DDRD,FRESO);__port_and(PORTD,~FRESO)
#define FS_RES_HI()             __port_or(DDRD,FRESO);__port_or(PORTD,FRESO)

//
//      Test IO (used only in USB mode)
//
#if (SIO==0) && (USB==1) && !defined(DEBUG)
#define TST1                    (1<<1)
#define TST1_INIT()             __outp( DDRD, (__inp(DDRD) | TST1) )
#define TST1_OFF()              __outp( PORTD, (__inp(PORTD) & ~TST1) )
#define TST1_ON()               __outp( PORTD, (__inp(PORTD) | TST1) )
#define TST1_TGL()              __outp( PORTD, (__inp(PORTD) ^ TST1) )
#define TST2                    (1<<2)
#define TST2_INIT()             __outp( DDRD, (__inp(DDRD) | TST2) )
#define TST2_OFF()              __outp( PORTD, (__inp(PORTD) & ~TST2) )
#define TST2_ON()               __outp( PORTD, (__inp(PORTD) | TST2) )
#define TST2_TGL()              __outp( PORTD, (__inp(PORTD) ^ TST2) )
#else
#define TST1_INIT()
#define TST1_OFF()
#define TST1_ON()
#define TST1_TGL()
#define TST2_INIT()
#define TST2_OFF()
#define TST2_ON()
#define TST2_TGL()
#endif

//----------------------------------------------------------------
// Macro    :   IO_Init
// Notes    :
// History  :
//----------------------------------------------------------------

#define IO_Init()               \
{                               \
  SIO_RDYL_INIT();              \
}

#endif

//      End
