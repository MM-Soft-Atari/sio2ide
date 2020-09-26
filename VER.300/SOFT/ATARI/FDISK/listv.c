//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: LISTV.C
//
// MODULE NAME: LISTV
//
// PURPOSE:     List viewer.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <joystick.h>

#include "plat.h"

#pragma codeseg ("CODE2")

//----------------------------------------------------------------
// Function :   LISTV_Delay
// Notes    :
// History  :
//----------------------------------------------------------------

static void LISTV_Delay( UINT32 i )
{
  while( i-- )
  {
    readjoy( JOY_1 );
  }
}

//----------------------------------------------------------------
// Function :   LISTV_DrawItem
// Notes    :
// History  :
//----------------------------------------------------------------

static void LISTV_DrawItem( UINT8 * item, UINT8 size, BOOL inv )
{
  UINT8        s = size;
  UINT8        buf[41];
  UINT8      * p = buf;

  memset( buf, ((inv == TRUE) ? ' ' + 0x80 : ' '), 40 );
  while( *item && s-- )
  {
    *p++ = (inv == TRUE) ? *item + (*item == KEY_ESC ? 0 : 0x80) : *item;
    item++;
  }
  buf[ size ] = 0;
  SCR_DrawLine( buf, size );
}

//----------------------------------------------------------------
// Function :   LISTV_DrawEmptyItem
// Notes    :
// History  :
//----------------------------------------------------------------

static void LISTV_DrawEmptyItem( UINT8 size )
{
  UINT8        buf[41];

  memset( buf, ' ', 40 );
  buf[ size ] = 0;
  SCR_DrawLine( buf, size );
}

//----------------------------------------------------------------
// Function :   LISTV_Draw
// Notes    :
// History  :
//----------------------------------------------------------------

static void LISTV_Draw( T_listview * listv, INT16 pos, UINT16 maxcap, UINT8 maxx, UINT8 maxy )
{
  UINT8      * itm;
  INT16        i;
  UINT8        x, y;
  UINT8        ad;

  x  = (maxx / listv->itmlen);
  ad = (maxx - (x * listv->itmlen)) / (x - 1);
  i  = (pos / maxcap) * maxcap;
  for( x = 0; (x + listv->itmlen) < listv->lx; x = (x + listv->itmlen + ad) )
  {
    for( y = 0; y < maxy; y++ )
    {
      gotoxy( listv->hx + x + 1, listv->hy + y + 1 );
      if( listv->gitem( &itm, i ) )
      {
        LISTV_DrawItem( itm, listv->itmlen, (i == pos ? TRUE : FALSE) );
      }
      else
      {
        LISTV_DrawEmptyItem( listv->itmlen );
      }
      i++;
    }
  }
}

//----------------------------------------------------------------
// Function :   LISTV_WaitKey
// Notes    :
// History  :
//----------------------------------------------------------------

static UINT8 LISTV_WaitKey( T_listview * listv, UINT8 * k )
{
  UINT8        key;

  for(;;)
  {
    key = 0xFF;
    if( kbhit() )
    {
      key = cgetc();
    }
    //
    switch( readjoy( JOY_1 ) )
    {
      case JOY_UP:
        key = KEY_UP;
      break;
      case JOY_DOWN:
        key = KEY_DOWN;
      break;
      case JOY_LEFT:
        key = KEY_LEFT;
      break;
      case JOY_RIGHT:
        key = KEY_RIGHT;
      break;
      case JOY_FIRE:
        key = KEY_RETURN;
      break;
      default:
      break;
    };
    switch( key )
    {
      case 0:
      case 0xFF:
      break;
      case KEY_RETURN:          // Return
        LISTV_Delay( 500 );
        return KEY_RETURN;
      case KEY_UP:              // Up
      case KEY_UP + 128:
      case '-':
        LISTV_Delay( 500 );
        return KEY_UP;
      case KEY_DOWN:            // Down
      case KEY_DOWN + 128:
      case '=':
        LISTV_Delay( 500 );
        return KEY_DOWN;
      case KEY_LEFT:            // Left
      case KEY_LEFT + 128:
      case '+':
        LISTV_Delay( 500 );
        return KEY_LEFT;
      case KEY_RIGHT:           // Right
      case KEY_RIGHT + 128:
      case '*':
        LISTV_Delay( 500 );
        return KEY_RIGHT;
      default:
        if( strchr( listv->exitkeys, key ) )        return 0xFF;
        if( strchr( listv->actkeys, key ) )
        {
          *k = key;
          return 0xFE;
        }
      break;
    };
  }
}

//----------------------------------------------------------------
// Function :   LISTV_ReFresh
// Notes    :
// History  :
//----------------------------------------------------------------

void LISTV_ReFresh( T_listview * listv )
{
  if( listv->redraw )
  {
    listv->redraw();
  }
  SCR_DrawWindow( listv->title, listv->hx, listv->hy, listv->lx, listv->ly );
}

//----------------------------------------------------------------
// Function :   LISTV_Show
// Notes    :
// History  :
//----------------------------------------------------------------

UINT8 LISTV_Show( T_listview * listv )
{
  INT16        curpos = 0;
  INT16        maxpos = 0;
  INT16        maxcap = 0;
  UINT8        maxx, maxy;
  UINT8        key;

  if( !listv->gcount || !listv->gitem )
  {
    return 0xFF;
  }
  curpos = 0;
  maxpos = listv->gcount();
  maxx   = listv->lx - listv->hx - 1;
  maxy   = listv->ly - listv->hy - 1;
  maxcap = maxy * (maxx / listv->itmlen);
  //
  LISTV_ReFresh( listv );
  //
  for(;;)
  {
    LISTV_Draw( listv, curpos, maxcap, maxx, maxy );
    if( listv->focus )
    {
      listv->focus( curpos );
    }
    switch( LISTV_WaitKey( listv, &key ) )
    {
      case 0xFF:        // Exit
        return 0xFF;
      case KEY_UP:      // Previous
        curpos = (curpos > 0) ? (curpos - 1) : curpos;
      break;
      case KEY_DOWN:    // Next
        curpos = (curpos < (maxpos - 1)) ? (curpos + 1) : curpos;
      break;
      case KEY_LEFT:    // Prev <<
        if( curpos >= maxy )
        {
          curpos -= maxy;
        }
        else
        {
          curpos = 0;
        }
      break;
      case KEY_RIGHT:   // Next >>
        if( (curpos + maxy)  <= (maxpos - 1) )
        {
          curpos += maxy;
        }
        else
        {
          curpos = maxpos - 1;
        }
      break;
      case 0xFE:        // Action
        if( listv->action )
          if( listv->action( curpos, key ) )
          {
            return key;
          }
        LISTV_ReFresh( listv );
      break;
      case KEY_RETURN:  // Select (execute OnSelect action)
        if( listv->select )
          if( listv->select( curpos ) )
          {
            return KEY_RETURN;
          }
        LISTV_ReFresh( listv );
      break;
      default:
      break;
    };
  }
  return 0xFF;
}

//      End