//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: CDFS.H
//
// MODULE NAME: CDFS
//
// PURPOSE:
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001   27-02-2002 MM     Created
//
//****************************************************************

#ifndef __CDFS_H__
  #define __CDFS_H__

#include "iso.h"

#define CDSECTOR_SIZE   2048            // ISO CD sector size
//#define FL_MAXSEC_SIZE  256             // ATARI max sector size

//
// Interface
//

BOOL    CDFS_Init          ( VOID );
//
//BOOL    FATFS_DiskCheck    ( UINT8 id );
//UINT16  FATFS_DiskSecSize  ( UINT8 id );
//UINT16  FATFS_DiskSize     ( UINT8 id );
//
//BOOL    FATFS_DiskGet      ( UINT8 id, T_file * disk );
//BOOL    FATFS_DiskSet      ( UINT8 id, T_file * disk );
//BOOL    FATFS_DiskOff      ( UINT8 id );
//BOOL    FATFS_SaveConfig   ( VOID );
//VOID    FATFS_GetDrvInfo   ( T_drvinf * buf );
//VOID    FATFS_GetFsInfo    ( T_fsinf * buf );
//
//BOOL    FATFS_GetFirstFile ( T_file * file );
//BOOL    FATFS_GetNextFile  ( T_file * file );
//BOOL    FATFS_GetLFN       ( UINT8 * name, UINT8 * lfn );
//
//BOOL    FATFS_GetFirstDir  ( T_file * dir );
//BOOL    FATFS_GetNextDir   ( T_file * dir );
//BOOL    FATFS_GetCurDir    ( T_file * dir );
//BOOL    FATFS_SetCurDir    ( T_file * dir );
//BOOL    FATFS_InitCurDir   ( T_file * dir );
//
//BOOL    FATFS_GetFileSec   ( UINT8 disk, UINT16 sec, UINT8 * buf );
//BOOL    FATFS_PutFileSec   ( UINT8 disk, UINT16 sec, UINT8 * buf );

#endif

//      End
