//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: SIOSRV.H
//
// MODULE NAME: SIOSRV
//
// PURPOSE:     SIO service header.
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

#ifndef __SIOSRV_H__
  #define __SIOSRV_H__

#define FS_SIO2IDE      0x71    // SIO2IDE interface ID base

BOOL    SIO_GetFSInfo       ( UINT8 dev, T_fsinf * fs );
BOOL    SIO_GetDriveInfo    ( UINT8 dev, T_drvinf * drv );
//
BOOL    SIO_GetNextATR      ( UINT8 dev, T_file * file );
BOOL    SIO_GetFirstATR     ( UINT8 dev, T_file * file );
//
BOOL    SIO_PutDisk         ( UINT8 dev, UINT8 disk, T_file * file );
BOOL    SIO_GetDisk         ( UINT8 dev, UINT8 disk, T_file * file );
BOOL    SIO_DiskOff         ( UINT8 dev, UINT8 disk );
//
BOOL    SIO_SaveConfig      ( UINT8 dev );
//
BOOL    SIO_GetFirstDir     ( UINT8 dev, T_file * dir );
BOOL    SIO_GetNextDir      ( UINT8 dev, T_file * dir );
BOOL    SIO_SetCurDir       ( UINT8 dev, T_file * dir );
BOOL    SIO_GetCurDir       ( UINT8 dev, T_file * dir );
BOOL    SIO_InitCurDir      ( UINT8 dev, T_file * dir );
//
BOOL    SIO_PutSect         ( UINT8 dev, UINT32 sect, UINT8 * data );
BOOL    SIO_GetSect         ( UINT8 dev, UINT32 sect, UINT8 * buf );
BOOL    SIO_GetLFN          ( UINT8 dev, UINT32 fsect, UINT8 * buf );

#endif

//      End
