//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: SCREEN.C
//
// MODULE NAME: SCREEN
//
// PURPOSE:     Screen service.
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

#include <stdio.h>
#include <string.h>
#include <conio.h>

#include "plat.h"

#pragma codeseg ("CODE2")

//----------------------------------------------------------------
// Function :   SCR_Init
// Notes    :   Install new DisplayLists
// History  :
//----------------------------------------------------------------

void    SCR_Init( void )
{
  cursor( 0 );
  clrscr();
}

//----------------------------------------------------------------
// Function :   SCR_DeInit
// Notes    :
// History  :
//----------------------------------------------------------------

void    SCR_DeInit( void )
{
  clrscr();
}

//----------------------------------------------------------------
// Function :   SCR_DrawFrame
// Notes    :
// History  :
//----------------------------------------------------------------

void SCR_DrawFrame( UINT8 hx, UINT8 hy, UINT8 lx, UINT8 ly, BOOL clr )
{
  UINT8  i, j;

  if( clr )
  {
    for( i = hx + 1; i < lx; i++ )
    {
      for( j = hy + 1; j < ly; j++ )
      {
        cputcxy( i, j, ' ' );
      }
    }
  }
  cputcxy( hx, hy, 17 );
  cputcxy( hx, ly, 26 );
  for( i = hx + 1; i < lx; i++ )
  {
    cputcxy( i, hy, 18 );
    cputcxy( i, ly, 18 );
  }
  cputcxy( lx, hy, 5 );
  cputcxy( lx, ly, 3 );
  for( i = hy + 1; i < ly; i++ )
  {
    cputcxy( hx, i, 124 );
    cputcxy( lx, i, 124 );
  }
}

//----------------------------------------------------------------
// Function :   SCR_LineCenter
// Notes    :
// History  :
//----------------------------------------------------------------

void SCR_LineCenter( UINT8 * line, UINT8 maxlen )
{
  UINT8   i;

  i = strlen( line ) > maxlen ? 0 : (maxlen - strlen( line )) / 2;
  gotoxy( wherex() + i, wherey() );
  while( *line && maxlen-- )
  {
    cputc( *line++ );
  }
}

//----------------------------------------------------------------
// Function :   SCR_LineCenterClr
// Notes    :
// History  :
//----------------------------------------------------------------

void SCR_LineCenterClr( UINT8 * line, UINT8 maxlen )
{
  UINT8   buf[40];
  UINT8   i, x, y;

  x = wherex();
  y = wherey();
  memset( buf, ' ' , 40 );
  SCR_DrawLine( buf, maxlen );
  //
  i = strlen( line ) > maxlen ? 0 : (maxlen - strlen( line )) / 2;
  gotoxy( x + i, y );
  while( *line && maxlen-- )
  {
    cputc( *line++ );
  }
}

//----------------------------------------------------------------
// Function :   SCR_LineCenterBuf
// Notes    :
// History  :
//----------------------------------------------------------------

static void SCR_LineCenterBuf( UINT8 * buf, UINT8 * line, UINT8 maxlen )
{
  buf += strlen( line ) > maxlen ? 0 : (maxlen - strlen( line )) / 2;
  while( *line && maxlen-- )
  {
    *buf++ = *line++;
  }
}

//----------------------------------------------------------------
// Function :   SCR_DrawLine
// Notes    :
// History  :
//----------------------------------------------------------------

void SCR_DrawLine( UINT8 * line, UINT8 maxlen )
{
  while( *line && maxlen-- )
  {
    cputc( *line++ );
  }
}

//----------------------------------------------------------------
// Function :   SCR_DrawWindow
// Notes    :
// History  :
//----------------------------------------------------------------

void SCR_DrawWindow( UINT8 * title, UINT8 hx, UINT8 hy, UINT8 lx, UINT8 ly )
{
  UINT8  buf[40];
  UINT8  i, j;

  memset( buf, 18, 40 );
  for( i = hx + 1; i < lx; i++ )
  {
    for( j = hy + 1; j < ly; j++ )
    {
      cputcxy( i, j, ' ' );
    }
  }
  cputcxy( hx, hy, 17 );
  cputcxy( hx, ly, 26 );
  for( i = hx + 1; i < lx; i++ )
  {
    cputcxy( i, ly, 18 );
  }
  cputcxy( lx, hy, 5 );
  cputcxy( lx, ly, 3 );
  for( i = hy + 1; i < ly; i++ )
  {
    cputcxy( hx, i, 124 );
    cputcxy( lx, i, 124 );
  }
  //
  cputcxy( hx + 1, hy, 4 );
  cputcxy( hx + 2, hy, 20 );
  cputcxy( hx + 3, hy, 1 );
  //
  SCR_LineCenterBuf( buf, title, (lx - hx - 4) );
  gotoxy( hx + 4, hy );
  SCR_DrawLine( buf, (lx - hx - 4) );
}

//----------------------------------------------------------------
// Function :   SCR_ProgressShow
// Notes    :
// History  :
//----------------------------------------------------------------

void SCR_ProgressShow( T_progress * prog, UINT8 hx, UINT8 hy, UINT8 len, UINT16 max )
{
  prog->hx  = hx;
  prog->hy  = hy;
  prog->len = len;
  prog->max = max;
  //
  SCR_DrawFrame( hx, hy, hx + len + 1, hy + 2, TRUE );
  cputcxy( ++hx, hy + 1, 1 );
  while( --len )
  {
    cputcxy( ++hx, hy + 1, 18 );
  }
  gotoxy( prog->hx + 1, prog->hy );
  SCR_LineCenter( "0%%", prog->len );
}

//----------------------------------------------------------------
// Function :   SCR_ProgressUpdate
// Notes    :
// History  :
//----------------------------------------------------------------

void SCR_ProgressUpdate( T_progress * prog, UINT16 val )
{
  UINT8   buf[40];
  UINT16  i, x;

  if( val > prog->max )
  {
    val = prog->max;
  }
  i = (prog->len * val) / prog->max;
  x = prog->hx + 1;
  while( i-- )
  {
    cputcxy( x++, prog->hy + 1, 160 );
  }
  gotoxy( prog->hx + 1, prog->hy );
  i = (val * 100) / prog->max;
  sprintf( buf, "%d%%", (UINT16)i );
  SCR_LineCenter( buf, prog->len );
}

//----------------------------------------------------------------
// Function :   SCR_LoadShow
// Notes    :
// History  :
//----------------------------------------------------------------

void SCR_LoadShow( T_progress * prog, UINT8 hx, UINT8 hy, UINT8 len )
{
  prog->hx  = hx;
  prog->hy  = hy;
  prog->len = len;
  prog->max = 0;
  //
  SCR_DrawFrame( hx, hy, hx + len + 1, hy + 2, TRUE );
  gotoxy( prog->hx + 1, prog->hy + 1 );
  SCR_LineCenter( "Loaded: 0", prog->len );
}

//----------------------------------------------------------------
// Function :   SCR_LoadUpdate
// Notes    :
// History  :
//----------------------------------------------------------------

void SCR_LoadUpdate( T_progress * prog, UINT16 val )
{
  UINT8   buf[40];

  gotoxy( prog->hx + 1, prog->hy + 1 );
  sprintf( buf, "Loaded: %u", (UINT16)val );
  SCR_LineCenter( buf, prog->len );
}

//      End
