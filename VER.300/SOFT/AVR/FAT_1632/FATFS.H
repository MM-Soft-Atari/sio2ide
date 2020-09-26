//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: FATFS.H
//
// MODULE NAME: FATFS
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

#ifndef __FATFS_H__
  #define __FATFS_H__

#include "ptable.h"
#include "bootsect.h"
#include "bpb.h"
#include "fat.h"
#include "hdrs.h"
#include "direntry.h"

#include "iso.h"

#include "fatstruc.h"

#define SECTOR_SIZE     512             // HDD sector size
#define FL_MAXSEC_SIZE  256             // ATARI max sector size

//
// Interface
//

BOOL    FATFS_Init         ( VOID );
BOOL    FATFS_Test         ( VOID );
//
BOOL    FATFS_DiskCheck    ( UINT8 id );
UINT16  FATFS_DiskSecSize  ( UINT8 id );
UINT16  FATFS_DiskSize     ( UINT8 id );
//
BOOL    FATFS_DiskGet      ( UINT8 id, T_file * disk );
BOOL    FATFS_DiskSet      ( UINT8 id, T_file * disk );
BOOL    FATFS_DiskOff      ( UINT8 id );
BOOL    FATFS_SaveConfig   ( VOID );
VOID    FATFS_GetDrvInfo   ( T_drvinf * buf );
VOID    FATFS_GetFsInfo    ( T_fsinf * buf );
//
BOOL    FATFS_GetFirstFile ( T_file * file );
BOOL    FATFS_GetNextFile  ( T_file * file );
BOOL    FATFS_GetLFN       ( UINT8 * name, UINT8 * lfn );
//
BOOL    FATFS_GetFirstDir  ( T_file * dir );
BOOL    FATFS_GetNextDir   ( T_file * dir );
BOOL    FATFS_GetCurDir    ( T_file * dir );
BOOL    FATFS_SetCurDir    ( T_file * dir );
BOOL    FATFS_InitCurDir   ( T_file * dir );
BOOL    FATFS_SaveCurDir   ( VOID );
//
BOOL    FATFS_GetFileSec   ( UINT8 disk, UINT16 sec, UINT8 * buf );
BOOL    FATFS_PutFileSec   ( UINT8 disk, UINT16 sec, UINT8 * buf );

#endif

//      End
