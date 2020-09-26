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

//#define IDE_INITRW    // Use complex RW mode

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

#include "idestruc.h"

//
//      Interface
//
UINT8 IDE_Init                  ( T_drvinf * DrvInfo );
//
#ifdef IDE_INITRW
BOOL  IDE_ReadSectorInit        ( T_drvinf * DrvInfo, UINT32 sn );
UINT8 IDE_ReadSectorData        ( UINT16 * dat );
BOOL  IDE_WriteSectorInit       ( T_drvinf * DrvInfo, UINT32 sn );
UINT8 IDE_WriteSectorData       ( UINT16 dat );
#endif
//
BOOL  IDE_SectorGet             ( UINT16 *buf, T_drvinf * drv, UINT32 sec );
BOOL  IDE_SectorPut             ( UINT16 *buf, T_drvinf * drv, UINT32 sec );
BOOL  IDE_SectorGetAt           ( UINT16 *buf, T_drvinf * drv, UINT32 sec, UINT16 offs );
//
BOOL  IDE_ATAPILoadCD           ( T_drvinf * drv  );
BOOL  IDE_ATAPIEjectCD          ( T_drvinf * drv  );
//
VOID  IDE_ErrorBlink            ( VOID );

#endif  // __IDE_H__
