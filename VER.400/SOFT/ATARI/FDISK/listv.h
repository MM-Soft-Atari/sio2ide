//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: LISTV.H
//
// MODULE NAME: LISTV
//
// PURPOSE:     List viewer header.
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

#ifndef __LISTV_H__
  #define __LISTV_H__

typedef BOOL   (*T_getitem)( UINT8 ** item, UINT16 cnt );
typedef UINT16 (*T_getcount)( void );
typedef void   (*T_listdsk)( void );
typedef BOOL   (*T_listsel)( UINT16 pos );
typedef BOOL   (*T_listact)( UINT16 * pos, UINT8 key, BOOL * refr );
typedef void   (*T_listfoc)( UINT16 pos );

typedef struct
{
  UINT8        type;
  #define LST_VERT      1
  #define LST_HOR       2
  UINT8      * title;
  UINT8        itmlen;
  UINT8        hx;
  UINT8        hy;
  UINT8        lx;
  UINT8        ly;
  UINT8      * exitkeys;
  UINT8      * actkeys;
  T_getcount   gcount;
  T_getitem    gitem;
  T_listsel    select;
  T_listact    action;
  T_listfoc    focus;
  T_listdsk    redraw;
} T_listview;

//
UINT8   LISTV_Show       ( T_listview * listv );

#endif

//      End
