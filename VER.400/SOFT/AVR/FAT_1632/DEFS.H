//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: defs.h
//
// MODULE NAME: common
//
// PURPOSE:     Global definitions.
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001    6-12-2001 MM     Created
//
//****************************************************************

#ifndef __DEFS_H__
  #define __DEFS_H__

#ifdef __IAR_SYSTEMS_ICC
  #define FLASH           flash
  #define CONST           flash
  #define DATA            tiny
  #define REGISTER        register
  #define PROGMEM
#elif __GNUC__
  #define CONST           const
  #define FLASH           const
  #define DATA
  #define REGISTER        register
#elif __MSDOS__
  #define CONST           const
  #define FLASH           const
  #define DATA
  #define REGISTER        register
  #define PROGMEM
#else
  #define CONST           const
  #define FLASH           const
  #define DATA
  #define REGISTER        register
  #define PROGMEM
#endif

#define VOID            void

#define STATIC          static
#define EXTERN          extern
#define VOLATILE        // volatile

#ifndef NULL
  #define NULL          0 //((VOID *)0L)
#endif

#define FALSE           0
#define TRUE            !FALSE

#define OFF             0
#define ON              1

#define FOREVER         for(;;)

#define low(a)          ((UINT8)(a & 0xFF))
#define high(a)         ((UINT8)((a >> 8) & 0xFF))

#define min( val1, val2 )               \
(                                       \
  (((val1) < (val2))? (val1) : (val2))  \
)

#ifdef   BIG_ENDIAN
  #define SWAP(x)         ((((x) & 0xFF) << 8) | (((x) >> 8) & 0xFF))
#else
  #define SWAP(x)         (x)
#endif

#endif  // __DEFS_H__
