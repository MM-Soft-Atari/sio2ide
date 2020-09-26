//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: SCREEN.H
//
// MODULE NAME: SCREEN
//
// PURPOSE:     Screen hedaer.
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001   27-03-2002 MM     Created
//
//****************************************************************

#ifndef __SCREEN_H__
  #define __SCREEN_H__

typedef struct
{
  UINT8      hx;
  UINT8      hy;
  UINT8      len;
  UINT16     max;
} T_progress;

//
void    SCR_Init            ( void );
void    SCR_DeInit          ( void );
//
void    SCR_DrawFrame       ( UINT8 hx, UINT8 hy, UINT8 lx, UINT8 ly, BOOL clr );
void    SCR_DrawWindow      ( UINT8 * title, UINT8 hx, UINT8 hy, UINT8 lx, UINT8 ly );
//
void    SCR_ProgressShow    ( T_progress * prog, UINT8 hx, UINT8 hy, UINT8 len, UINT16 max );
void    SCR_ProgressUpdate  ( T_progress * prog, UINT16 val );
//
void    SCR_LoadShow        ( T_progress * prog, UINT8 hx, UINT8 hy, UINT8 len );
void    SCR_LoadUpdate      ( T_progress * prog, UINT16 val );
//
void    SCR_LineCenter      ( UINT8 * line, UINT8 maxlen );
void    SCR_LineCenterClr   ( UINT8 * line, UINT8 maxlen );
void    SCR_DrawLine        ( UINT8 * line, UINT8 maxlen );

#endif

//      End
