//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: MENU.C
//
// MODULE NAME: MENU
//
// PURPOSE:     Menu service.
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
// Function :   MENU_Delay
// Notes    :
// History  :
//----------------------------------------------------------------

void MENU_Delay( UINT32 i )
{
  while( i-- )
  {
    readjoy( JOY_1 );
  }
}

//----------------------------------------------------------------
// Function :   MENU_DrawItem
// Notes    :
// History  :
//----------------------------------------------------------------

static void MENU_DrawItem( UINT8 * item, UINT8 size, BOOL inv )
{
  UINT8        buf[41];
  UINT8      * p = buf;

  memset( buf, ((inv == TRUE) ? ' ' + 0x80 : ' '), 40 );
  while( *item )
  {
    *p++ = (inv == TRUE) ? *item + (*item == KEY_ESC ? 0 : 0x80) : *item;
    item++;
  }
  buf[ size ] = 0;
  SCR_DrawLine( buf, size );
}

//----------------------------------------------------------------
// Function :   MENU_Draw
// Notes    :
// History  :
//----------------------------------------------------------------

static void MENU_Draw( T_menu * menu, UINT8 max, UINT8 pos )
{
  T_menuitem * item;
  UINT8        size;
  UINT8        i, o;

  size = menu->type == MNU_VERT ? (menu->lx - menu->hx - 1) :
                                  ((menu->lx - menu->hx - 1) / max) - 1;
  i    = 0;
  item = menu->items;
  if( menu->type == MNU_VERT )
  {
    o  = menu->hy + 1;
    while( item->type != MITM_EMPTY )
    {
      gotoxy( menu->hx + 1, o++ );
      MENU_DrawItem( item->name, size, (i == pos ? TRUE : FALSE) );
      item++;
      i++;
    }
  }
  else if( menu->type == MNU_HOR )
  {
    o  = menu->hx + 1;
    while( item->type != MITM_EMPTY )
    {
      gotoxy( o, menu->hy + 1 );
      MENU_DrawItem( item->name, size, (i == pos ? TRUE : FALSE) );
      o += size + 2;
      item++;
      i++;
    }
  }
}

//----------------------------------------------------------------
// Function :   MENU_WaitKey
// Notes    :
// History  :
//----------------------------------------------------------------

static UINT8 MENU_WaitKey( T_menu * menu, UINT8 * pos )
{
  T_menuitem * item;
  UINT8        key;
  UINT8        p;

  for(;;)
  {
    key  = 0xFF;
    if( kbhit() )
    {
      key = cgetc();
    }
    //
    switch( readjoy( JOY_1 ) )
    {
      case JOY_UP:
      case JOY_LEFT:
        key = KEY_UP;
      break;
      case JOY_DOWN:
      case JOY_RIGHT:
        key = KEY_DOWN;
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
        MENU_Delay( 100 );
        return KEY_RETURN;
      case KEY_UP:              // Up
      case KEY_UP + 128:
      case '-':
      case KEY_LEFT:            // Left
      case KEY_LEFT + 128:
      case '+':
        MENU_Delay( 100 );
        return KEY_UP;
      case KEY_DOWN:            // Down
      case KEY_DOWN + 128:
      case '=':
      case KEY_RIGHT:           // Right
      case KEY_RIGHT + 128:
      case '*':
        MENU_Delay( 50 );
        return KEY_DOWN;
      default:
        if( strchr( menu->exitkeys, key ) )        return 0xFF;
        //
        item = menu->items;
        p    = 0;
        while( item->type != MITM_EMPTY )
        {
          if( strchr( item->actkeys, key ) )
          {
            *pos = p;
            return 0xFE;
          }
          item++;
          p++;
        }
      break;
    };
  }
}

//----------------------------------------------------------------
// Function :   MENU_ReFresh
// Notes    :
// History  :
//----------------------------------------------------------------

void MENU_ReFresh( T_menu * menu )
{
  if( menu->redraw )
  {
    menu->redraw();
  }
  SCR_DrawFrame( menu->hx, menu->hy, menu->lx, menu->ly, TRUE );
  gotoxy( menu->hx + 1, menu->hy );
  SCR_LineCenter( menu->title, (menu->lx - menu->hx - 1) );
}

//----------------------------------------------------------------
// Function :   MENU_Show
// Notes    :
// History  :
//----------------------------------------------------------------

UINT8 MENU_Show( T_menu * menu )
{
  T_menuitem * item;
  UINT8        curpos = 0;
  UINT8        maxpos = 0;
  UINT8        pos;
  UINT8        key;

  curpos = 0;
  maxpos = 0;
  item   = menu->items;
  while( item->type != MITM_EMPTY )
  {
    maxpos++;
    item++;
  }
  MENU_ReFresh( menu );
  //
  for(;;)
  {
    MENU_Draw( menu, maxpos, curpos );
    pos = curpos;
    switch( key = MENU_WaitKey( menu, &pos ) )
    {
      case 0xFF:        // Exit
        return curpos;
      case 0xFE:        // Select
        curpos = (pos < maxpos) ? pos : curpos;
      break;
      case KEY_DOWN:    // Next
        curpos = (curpos < (maxpos - 1)) ? (curpos + 1) : curpos;
      break;
      case KEY_UP:      // Previous
        curpos = (curpos > 0) ? (curpos - 1) : curpos;
      break;
      case KEY_RETURN:  // Return (execute item action)
        item   = menu->items + curpos;
        if( item->action )
        {
          if( item->action() )
          {
            MENU_ReFresh( menu );
          }
        }
        if( item->type == MITM_EXIT )
        {
          return curpos;
        }
      break;
      default:
      break;
    };
  }
  return curpos;
}

//      End
