//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: IDE.H
//
// MODULE NAME: IDE
//
// PURPOSE:     IDE header.
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001   27-03-2002 MM     Created
//
//****************************************************************

#ifndef __IDE_H__
  #define __IDE_H__

#include <idestruc.h>

EXTERN UINT16      acc;
EXTERN UINT16      DISK;

UINT8 IDE_Init           ( T_drvinf * drv );
BOOL  IDE_SectorGet      ( UINT16 *buffer , T_drvinf * drv, UINT32 sector );
BOOL  IDE_SectorPut      ( UINT16 *buffer , T_drvinf * drv, UINT32 sector );
BOOL  IDE_SectorGetAt    ( UINT16 *buffer , T_drvinf * drv, UINT32 sector, UINT16 offs );

#endif

//      End

