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
  #define CONST           const
  #define DATA            tiny
  #define REGISTER        register
  #define PROGMEM
#endif
#ifdef __GNUC__
  #define CONST           const
  #define FLASH           const
  #define DATA
  #define REGISTER        register
#endif
#ifdef __MSDOS__
  #define CONST           const
  #define FLASH           const
  #define DATA
  #define REGISTER        register
  #define PROGMEM
#endif

#define VOID            void

#define STATIC          static
#define EXTERN          extern
#define VOLATILE        volatile

#ifndef NULL
  #define NULL          ((VOID *)0L)
#endif

#define FALSE           0
#define TRUE            !FALSE

#define FOREVER         for(;;)

#define low(a)          ((UINT8)(a & 0xFF))
#define high(a)         ((UINT8)((a >> 8) & 0xFF))

#endif  // __DEFS_H__
