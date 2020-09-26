//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: ide.h
//
// MODULE NAME: ide
//
// PURPOSE:     IDE driver (header file).
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


#ifndef __IDEDRV_H__
  #define __IDEDRV_H__

#define IDE_TOUT        10000

//
//      Return values for IDE_WriteSectorData
//
#define IDE_WR_ERR      0x00
#define IDE_WR_OK       0x01
#define IDE_WR_BSY      0x02
//
//      Return values for IDE_ReadSectorData
//
#define IDE_RD_ERR      0x00
#define IDE_RD_OK       0x01
#define IDE_RD_BSY      0x02

//
//      IDE Drive Information structure
//
typedef struct
{
  UINT16     cyl;       // Number of cylinders
  UINT8      hd;        // Number of heads
  UINT16     spt;       // Sectors/Track
  UINT32     sec;       // Number of Sectors
} T_DRVINF;

//
//      Interface
//
BOOL  IDE_Init                  ( T_DRVINF * DrvInfo );
BOOL  IDE_ReadSectorInit        ( T_DRVINF * DrvInfo, UINT32 sn );
UINT8 IDE_ReadSectorData        ( UINT16 * dat );
BOOL  IDE_WriteSectorInit       ( T_DRVINF * DrvInfo, UINT32 sn );
UINT8 IDE_WriteSectorData       ( UINT16 dat );

#endif  // __IDE_H__
