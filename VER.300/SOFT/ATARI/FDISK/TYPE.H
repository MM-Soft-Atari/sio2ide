//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: TYPE.H
//
// MODULE NAME: TYPE
//
// PURPOSE:     Common types.
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

#ifndef __TYPES
  #define __TYPES

typedef char            INT8;
typedef short           INT16;
typedef long            INT32;
typedef unsigned char   UINT8;
typedef unsigned int    UINT16;
typedef unsigned long   UINT32;
typedef unsigned char   BOOL;

#ifndef NULL
  #define NULL          ((VOID *)0)
#endif

#define FALSE           0
#define TRUE            !FALSE

#define FOREVER         for(;;)

#define LOW(a)          ((UINT8)(a & 0xFF))
#define HIGH(a)         ((UINT8)((a >> 8) & 0xFF))

#endif  /* __TYPES */
