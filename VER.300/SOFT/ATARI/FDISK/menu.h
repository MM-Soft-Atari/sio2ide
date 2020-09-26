//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: MENU.H
//
// MODULE NAME: MENU
//
// PURPOSE:     Menu header.
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


#ifndef __MENU_H__
  #define __MUNU_H__

typedef BOOL (*T_menuact)( void );
typedef void (*T_menudsk)( void );

typedef struct
{
  UINT8        type;
  #define MITM_EMPTY    0
  #define MITM_NORMAL   1
  #define MITM_EXIT     2
  UINT8      * name;
  UINT8      * actkeys;
  T_menuact    action;
} T_menuitem;

typedef struct
{
  UINT8        type;
  #define MNU_VERT      1
  #define MNU_HOR       2
  UINT8      * title;
  UINT8        hx;
  UINT8        hy;
  UINT8        lx;
  UINT8        ly;
  T_menuitem * items;
  UINT8      * exitkeys;
  T_menudsk    redraw;
} T_menu;

//
UINT8    MENU_Show       ( T_menu * menu );
//
void     MENU_Delay      ( UINT32 i );

#endif

//      End
