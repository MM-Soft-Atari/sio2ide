//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: HDRS.H
//
// MODULE NAME: HDRS
//
// PURPOSE:     Files header description - header file.
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001   14-01-2002 MM     Created
//
//****************************************************************

#ifndef __HDRS_H__
  #define __HDRS_H__

//
// ATR file header file
//
typedef struct
{
  UINT16    wMagic;       // $0296 (sum of 'NICKATARI')
  #define NICKATARI   0x0296
  UINT16    wPars;        // size of this disk image, in paragraphs (size/$10)
  UINT16    wSecSize;     // sector size. ($80 or $100) bytes/sector
  UINT8     btParsHigh;   // high part of size, in paragraphs (added by REV 3.00)
  UINT32    dwCRC;        // 32bit CRC of file (added by APE?)
  UINT32    dwUnused;     // unused
  UINT8     btFlags;      // bit 0 (ReadOnly) (added by APE?)
} T_atrhdr;

#endif

//     End
