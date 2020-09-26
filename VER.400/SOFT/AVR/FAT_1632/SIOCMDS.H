//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: SIOCMD.H
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
//              001    8-03-2002 MM     Created
//
//****************************************************************

#ifndef __SIOCMDS_H__
  #define __SIOCMDS_H__

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
#define SIOC_HS         0x3F    // ? US Doubler
//
#define SIOC_GETDSK     0x11    // Get the Disk description (aux1)
#define SIOC_PUTDSK     0x12    // Put the Disk description (aux1)
#define SIOC_FATR       0x13    // Get First ATR file
#define SIOC_NATR       0x14    // Get Next ATR file
#define SIOC_OFFDSK     0x15    // Switch Off selected Disk (aux1)
#define SIOC_SCFG       0x16    // Save Config file to HDD
#define SIOC_GETDRV     0x17    // Get Drive parameters
#define SIOC_GETFSI     0x18    // Get FS info
#define SIOC_FDIR       0x19    // Get First Dir entry (from current dir)
#define SIOC_NDIR       0x1A    // Get Next Dir entry (from current dir)
#define SIOC_SCDIR      0x1B    // Set Current working Dir
#define SIOC_GCDIR      0x1C    // Get Current working Dir
#define SIOC_ICDIR      0x1D    // Initialise Current working Dir
//
#define SIOC_SSECT      0x61    // Set Phisical Disk Sector Number
                                // or File Start Sect for SIOC_GLFN command
#define SIOC_GSECT      0x62    // Get Phisical Disk Sector
#define SIOC_PSECT      0x63    // Put Phisical Disk Sector
#define SIOC_GLFN       0x64    // Get Long File Name (first use SIOC_SSECT)
#define SIOC_CDRV       0x65    // Change Current Drive (0x00=Master, 0x10=Slave)

#endif  // __FS_H__
