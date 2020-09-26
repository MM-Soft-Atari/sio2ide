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

#ifdef ATmega163
  #define AT_MEGA    "163"
#endif
#ifdef ATmega323
  #define AT_MEGA    "323"
#endif

#ifdef __IAR_SYSTEMS_ICC

  #define ENABLE_BIT_DEFINITIONS

  #ifdef ATmega163
    #include <iom163.h>
  #endif
  #ifdef ATmega323
    #include <iom323.h>
  #endif
  #include <ina90.h>
  #include "extiar.h"
  #define  __IAR__
#endif

#ifdef __GNUC__
  #include <io.h>
  #include <interrupt.h>
  #include <sig-avr.h>
  #include <progmem.h>
  #include <eeprom.h>
  #include "extgnu.h"
  #define  __GNU__
#endif

#include "types.h"        // Global types
#include "defs.h"         // Global defs

#include "cfgavr.h"       // Hardware config

#include "extmemf.h"      // Flash mem ext.
#include "eeserv.h"       // EE services

#include "utils.h"        // Utility
#include "asrt.h"         // ASSERT

#include "io.h"           // General IO

#include "timers.h"       // System Timers

#include "usbdrv.h"       // USBN9603 driver
#include "usb.h"          // USB driver
#include "usbxfer.h"      // USB Data Xfer
#include "rbccmd.h"       // SCSI RBC
#include "msbot.h"        // Mass Stora Bulk Only Transport

#ifndef __MSDOS__
  #include "extio.h"      // IO ext.
  #include "extstr.h"     // String ext.
#endif

#ifdef DEBUG
  #ifndef __MSDOS__
    #include "uartdrv.h"  // UART services
  #endif
#endif

#include <string.h>

#endif  // __PLATFORM_H__
