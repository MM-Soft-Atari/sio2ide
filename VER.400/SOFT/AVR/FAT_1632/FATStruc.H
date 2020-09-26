//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE:
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
//              001   24-01-2002 MM     Created
//
//****************************************************************

#ifndef __FATSTRUC_H__
  #define __FATSTRUC_H__

#define RDIR_NAME       "C:(ROOT)"      // 8 bytes
#define SDIR_NAME       "(SUBDIR)"      // 8 bytes
#define CFL_NAME        "<COMMON>"      // 8 bytes
#define FL_NAME_EXT     "ATR"           // 3 bytes
#define CFG_FL_NAME     "SIO2IDE "      // 8 bytes
#define CFG_NAME_EXT    "CFG"           // 3 bytes
#define CFG_FL_ID       0               // Config file index
#define MAX_FL_ID       9               // Max number of ATR disks (files)
#define COM_FL_ID       (MAX_FL_ID)     // Common ATR disk index
#define LFN_MAX_SIZE    (WIN_CHARS * 3) // Max LFN size

typedef struct
{
  UINT32   flStartSec;   // File Start Sector
  UINT16   flSize;       // File Size (ATARI sectors)
  UINT8    flStat;       // File State (denisity etc.)
  #define FLS_FILEOK    0x80
  #define FLS_SEC256B   0x40
  #define FLS_NOTDEFR   0x20
  #define FLS_HDRERR    0x10
  #define FLS_CFGFILE   0x08
  #define FLS_RDONLY    0x04
  #define FLS_DIROK     0x02
  #define FLS_S2IDIR    0x01
} T_fldet;

typedef struct
{
  UINT8     flName[8];  // File Name
  T_fldet   flDet;      // File details
} T_file;

typedef T_fldet      T_disktab[ MAX_FL_ID + 1 ];

typedef struct
{
  UINT8    prtType;      // Partition Type
  UINT32   fatStartSec;  // FATs Start Sector
  UINT32   dataStartSec; // Data Area Start Sector
  UINT32   rdirStartSec; // Root Directory Start Sector
  UINT32   cwdirStartSec;// Current Working Directory Start Sector
  UINT8    rdirClust;    // Number of Root Directory Clusters (max 255)
  UINT8    secPClust;    // Sectors Per Cluster
} T_drive;

typedef struct
{
  UINT8    prtType;      // Partition Type
  UINT8    secPClust;    // Sectors Per Cluster
  UINT8    curFiles;     // Number of assigned files (Atari disks)
  UINT8    swVer;        // Software version
} T_fsinf;

typedef struct
{
  UINT32       psect;    // Previous sector in cluster chain
  UINT32       csect;    // Current sector in cluster chain
  UINT8        cpos;     // Current position in sector
  UINT32       esect;    // End sector in cluster chain
  T_direntry * cent;     // Current File directory entry
} T_hdfind;

typedef struct
{
  UINT32          csect;    // Current sector in chain (512B)
  UINT16          cpos;     // Current position in sector (512B)
  T_ISO_DIR_REC * cent;     // Current File directory entry
} T_cdfind;

typedef union
{
  T_hdfind     HD;
  T_cdfind     CD;
} T_findfl;

#endif

//      End
