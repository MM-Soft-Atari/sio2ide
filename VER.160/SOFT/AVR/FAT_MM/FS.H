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

#define FS_MAXDEV       15      // Max number of Devices (const)
#define FS_MAXHDP       48      // Max number of Partitions

#define FS_NODISK       0xFF    //
#define FS_SECLEN       256     // Sector data length (const)

#define FS_SIO2IDE      0x71                        // SIO2IDE interface ID base (max 2 devices)
#define FS_S2IM         (FS_SIO2IDE)                // Master
#define FS_S2IS         (FS_SIO2IDE + 1)            // Slave

#define FS_DEVL         0x31                        // Atari drive ID (Min)
#define FS_DEVH         (FS_DEVL + FS_MAXDEV - 1)   // Max
#define FS_DEVO         (FS_DEVL)                   // Optional drive

//
//      SIO commands accepted by SIO2IDE FS
//
#define SIOC_NONE       0x00    //
#define SIOC_GCFG       0x4E    // Fixed Config
#define SIOC_PCFG       0x4F    // Dummy Only ACK
#define SIOC_STAT       0x53    // Fixed Status
#define SIOC_PUT        0x50    // Put data
#define SIOC_PUTV       0x57    // Put data with verify
#define SIOC_GET        0x52    // Get data
#define SIOC_FORMAT     0x21    // Format (always OK)
#define SIOC_GETHDP     0xEC    // Get the HD Partition Table
#define SIOC_PUTHDP     0xED    // Put the HD Partition Table
#define SIOC_GETHDI     0xEE    // Get the HD Information Block
#define SIOC_HS         0x3F    // ? US Doubler

//
//      Interface
//
BOOL   FS_Init          ( VOID );
BOOL   FS_CheckDev      ( UINT8 did, UINT8 cmd );

#endif  // __FS_H__
