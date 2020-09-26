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
//              001   28-08-2002 MM     Created
//
//****************************************************************

#include "platform.h"

//
//      Timer/Counter0 clock divider
//
#define CLK0_DIV        5        // 1-CK/1, 2-CK/8, 3-CK/64, 4-CK/256, 5-CK/1024
//
#if (CLK0_DIV==1)
  #define CLK0_FACT     1
#elif (CLK0_DIV==2)
  #define CLK0_FACT     8
#elif (CLK0_DIV==3)
  #define CLK0_FACT     64
#elif (CLK0_DIV==4)
  #define CLK0_FACT     256
#elif (CLK0_DIV==5)
  #define CLK0_FACT     1024
#endif

#define T0_FACT         (0xFF - (FOSC / CLK0_FACT / T0_FRQ))

//----------------------------------------------------------------
// Function :   TMRS_Init
// Notes    :
// History  :
//----------------------------------------------------------------

VOID TMRS_Init( VOID )
{
  __outp( TCNT0, T0_FACT );
  __outp( TCCR0, CLK0_DIV );
  __port_or( TIMSK, (1<<TOIE0) );                 // Timer0 Ovf Int Enabled
}

//----------------------------------------------------------------
// Function :   TMRS_Stop
// Notes    :
// History  :
//----------------------------------------------------------------

VOID TMRS_Stop( VOID )
{
  __port_and( TIMSK, ~(1<<TOIE0) );             // Timer0 Ovf Int Disable
}

//----------------------------------------------------------------
// Function :   TMRS_Start
// Notes    :
// History  :
//----------------------------------------------------------------

VOID TMRS_Start( VOID )
{
  __port_or( TIMSK, (1<<TOIE0) );               // Timer0 Ovf Int Enable
}

//----------------------------------------------------------------
// Function :   TC0_Ovf
// Notes    :
// History  :
//----------------------------------------------------------------

#ifdef __IAR__
interrupt [TIMER0_OVF_vect] VOID TC0_Ovf ( VOID )
#endif
#ifdef __GNU__
SIGNAL( SIG_OVERFLOW0 )
#endif
{
  Timer_Update( (1000 / T0_FACT) );

  __outp( TCNT0, T0_FACT );
}

//      End
