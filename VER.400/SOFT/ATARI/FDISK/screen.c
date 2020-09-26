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

#pragma codeseg ("CODE0")

//----------------------------------------------------------------
// Function :   WaitKey
// Notes    :
// History  :
//----------------------------------------------------------------

void WaitKey( UINT8 * keys )
{
  UINT8  key;

  for(;;)
  {
    key = 0xFF;
    if( kbhit() )
    {
      key = cgetc();
      if( strchr( keys, key ) )        return;
    }
  }
}

//----------------------------------------------------------------
// Function :   WaitKeyFor
// Notes    :
// History  :
//----------------------------------------------------------------
#ifdef __CART__
UINT8 WaitKeyFor( UINT8 * title, UINT8 * title1, UINT8 * title2, UINT8 * keys, UINT16 time )
{
  UINT8        key;
  UINT16       i;
  T_progress   prog;

  SCR_DrawWindow( title, 6, 7, 33, 16 );
  //
  SCR_ProgressShow( &prog, 9, 9, 20, (time * 10) );
  //
  gotoxy( 6, 13 );
  SCR_LineCenter( title1, 28 );
  gotoxy( 6, 14 );
  SCR_LineCenter( title2, 28 );

  for( i = 0; i < (time * 10); i++ )
  {
    key = 0xFF;
    if( kbhit() )
    {
      key = cgetc();
      if( strchr( keys, key ) )        return key;
    }
    SCR_ProgressUpdate( &prog, i );
    MENU_Delay( 350 );
  }
  return 0;
}
#endif

//----------------------------------------------------------------
// Function :   WaitAnyKey
// Notes    :
// History  :
//----------------------------------------------------------------
/*
UINT8 WaitAnyKey( void )
{
  for(;;)
  {
    if( kbhit() )
    {
      return cgetc();
    }
  }
}
*/

//
// OK/Error Button
//

//----------------------------------------------------------------
// Function :   ShowCnfButton
// Notes    :
// History  :
//----------------------------------------------------------------

void ShowCnfButton( UINT8 * name, UINT8 pos )
{
  static T_menuitem       buttonItm[] =
  {
    {MITM_EXIT, " Press 'Return'", "", NULL},
    {MITM_EMPTY,  "", "", NULL}
  };
  static T_menu           buttonCnf =
  {
    MNU_VERT, "", 10, 12, 28, 14, buttonItm, "", NULL
  };

  buttonCnf.title = name;
  buttonCnf.hy    = pos;
  buttonCnf.ly    = pos + 2;
  MENU_Show( &buttonCnf );
}

//----------------------------------------------------------------
// Function :   ShowYesNoButtons
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL ShowYesNoButtons( UINT8 * name, UINT8 pos )
{
  static T_menuitem       buttonItm[] =
  {
    {MITM_EXIT, " 'Y'es", "Yy", NULL},
    {MITM_EXIT, " 'N'o ", "\033Nn", NULL},
    {MITM_EMPTY,  "", "", NULL}
  };
  static T_menu           buttonCnf =
  {
    MNU_HOR, "", 9, 12, 29, 14, buttonItm, "", NULL
  };

  buttonCnf.title = name;
  buttonCnf.hy    = pos;
  buttonCnf.ly    = pos + 2;
  return (MENU_Show( &buttonCnf ) == 0) ? TRUE : FALSE;
}

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
  SCR_LineCenter( "0%", prog->len );
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
