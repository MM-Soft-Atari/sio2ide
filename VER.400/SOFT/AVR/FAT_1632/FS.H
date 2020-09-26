//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: fs.h
//
// MODULE NAME: fs
//
// PURPOSE:     SIO2IDE File System (header file).
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

#ifndef __FS_H__
  #define __FS_H__

#define FS_MAXDEV       8       // Max number of Devices (const)

#define FS_SIO2IDE      0x71                        // SIO2IDE interface ID base (max 2 devices)
#define FS_S2IM         (FS_SIO2IDE)                // Master
#define FS_S2IS         (FS_SIO2IDE + 1)            // Slave

#define FS_DEVL         0x31                        // Atari drive ID (Min)
#define FS_DEVH         (FS_DEVL + FS_MAXDEV - 1)   // Max device ID
#define FS_DEVO         (FS_DEVL)                   // Optional device
#define FS_DSW1         (FS_DEVL)                   // Swap device 1
#define FS_DSW2         (FS_DEVL + 1)               // Swap device 2

#include "siocmds.h"

//
//      Interface
//
BOOL   FS_Init          ( VOID );
BOOL   FS_CheckDev      ( UINT8 * did, UINT8 cmd );

#endif  // __FS_H__
