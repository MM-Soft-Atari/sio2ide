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
//              001    8-08-2002 MM     Created
//
//****************************************************************

#include <platform.h>

#ifndef BIG_ENDIAN
/* ----- reverse UINT16 ----------------------------------------------- */

UINT16  rev_word( UINT16 w )
{
  return ( UINT16 )((( w >> 8 ) & 0xFF ) | (( w << 8 ) & 0xFF00 ));
}

/* ----- reverse UINT32 ------------------------------------------- */

UINT32  rev_longword( UINT32 l )
{
  return ( UINT32 )((( l >> 24L ) & 0x000000FFL )
          | (( l >> 8L ) & 0x0000FF00L )
          | (( l << 8L ) & 0x00FF0000L )
          | (( l << 24L ) & 0xFF000000L ));
}
#endif  /* BIG_ENDIAN */


/*      Del for Cnt msek                                                */
/*      Cnt=1   =>      0.15 ms                                         */
VOID   Delay(UINT16 Cnt)
{
  STATIC  UINT16 n,m;

  for ( n=0;n<Cnt;n++ )
  {
    for ( m=0;m<60;m++ )
    {};
  }
}

/*      END                                                             */
