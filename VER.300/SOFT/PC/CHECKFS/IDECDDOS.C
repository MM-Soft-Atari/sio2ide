//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: IDECDDOS.C
//
// MODULE NAME: IDECDDOS
//
// PURPOSE:     IDE CD interface for DOS.
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

#ifdef __MSDOS__
  #ifdef DEBUG
    #define DEBUG_DISKFS
  #endif
#endif

#include <bios.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <platform.h>
#include "ide.h"

#define CF              1

UINT8     dbuf[2048];

STATIC BOOL IDECD_Check( VOID )
{
  struct  REGPACK reg;

  reg.r_ax = 0x150B;
  reg.r_cx = DISK - 0x80 + 2;
  //
  intr( 0x2F, &reg );
  if (reg.r_flags & CF)
  {
    return FALSE;
  }
  if( (reg.r_bx == 0xADAD) && (reg.r_ax != 0) )
  {
    return TRUE;
  }
  return FALSE;
}

BOOL IDECD_Init( VOID )
{
  if( IDECD_Check() )
  {
#ifdef DEBUG_DISKFS
    printf( "\nCD-ROM drive detected." );
#endif
    return TRUE;
  }
  return FALSE;
}

BOOL IDECD_SectorGet( UINT8 *buffer , UINT32 sector )
{
  struct  REGPACK reg;

  reg.r_ax = 0x1508;
  reg.r_cx = DISK - 0x80 + 2;
  reg.r_dx = 1;
  reg.r_si = sector >> 16;
  reg.r_di = sector;
  reg.r_es = FP_SEG(buffer);
  reg.r_bx = FP_OFF(buffer);
  //
  intr( 0x2F, &reg );
  if (reg.r_flags & CF)
  {
    return FALSE;
  }
  return TRUE;
}

//      End
