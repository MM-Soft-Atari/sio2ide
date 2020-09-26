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

#ifndef __TIMERS_H__
  #define __TIMERS_H__

#define T0_FRQ          100             // 100Hz
#define T0_1S           T0_FRQ          // 1sek

EXTERN VOID Timer_Update    ( UINT8 );

       VOID TMRS_Init       ( VOID );
       VOID TMRS_Start      ( VOID );
       VOID TMRS_Stop       ( VOID );

#endif

//      End
