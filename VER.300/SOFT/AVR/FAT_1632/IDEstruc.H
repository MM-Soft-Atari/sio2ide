//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: IDESTRUC.H
//
// MODULE NAME: IDESTRUC
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

#ifndef __IDESTRUC_H__
  #define __IDESTRUC_H__

#define IDE_NONE        0
#define IDE_HD          1
#define IDE_CD          2

//
//      IDE Drive Information structure
//
typedef struct
{
  UINT16     cyl;       // Number of cylinders
  UINT8      hd;        // Number of heads
  UINT16     spt;       // Sectors/Track
  UINT32     sec;       // Number of Sectors
  UINT8      flg;       // Drive flags
  #define DRV_LBA  0x80 // LBA mode
  #define DRV_CD   0x40 // CD ROM mode
} T_drvinf;

#endif  // __IDE_H__
