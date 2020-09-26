//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: platform.h
//
// MODULE NAME: common
//
// PURPOSE:     Platform header file.
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

#ifndef __PLATFORM_H__
  #define __PLATFORM_H__

#ifndef __AVR__
  #define __AVR__
#endif
#define PLAT_NAME    "SIO2IDE"

#ifdef __IAR_SYSTEMS_ICC
  #include <io8515.h>
  #include <ina90.h>
  #include "extiar.h"
  #define  __IAR__
#endif

#ifdef __GNUC__
  #include <io.h>
  #include <interrupt.h>
  #include <sig-avr.h>
  #include <progmem.h>
  #include "extgnu.h"
  #define  __GNU__
#endif

#include "types.h"        // Global types
#include "defs.h"         // Global defs

#include "cfgavr.h"       // Hardware config

#endif  // __PLATFORM_H__
