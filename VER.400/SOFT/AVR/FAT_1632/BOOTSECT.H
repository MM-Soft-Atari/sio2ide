//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: bootsect.h
//
// MODULE NAME: bootsect
//
// PURPOSE:     PC Disk Boot Sector header file.
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001   20-12-2001 MM     Created
//
//****************************************************************

#ifndef __BOOTSECT_H__
  #define __BOOTSECT_H__

/*
 * Format of a boot sector.  This is the first sector on a DOS floppy disk
 * or the fist sector of a partition on a hard disk.  But, it is not the
 * first sector of a partitioned hard disk.
 */
typedef struct
{
        UINT8           bsJump[3];              /* jump inst E9xxxx or EBxx90 */
        INT8            bsOemName[8];           /* OEM name and version */
        INT8            bsBPB[19];              /* BIOS parameter block */
        INT8            bsDriveNumber;          /* drive number (0x80) */
        INT8            bsBootCode[479];        /* pad so struct is 512b */
        UINT8           bsBootSectSig0;
        UINT8           bsBootSectSig1;
#define BOOTSIG0        0x55
#define BOOTSIG1        0xaa
} T_bootsector33;

typedef struct
{
        INT8            exDriveNumber;          /* drive number (0x80) */
        INT8            exReserved1;            /* reserved */
        INT8            exBootSignature;        /* ext. boot signature (0x29) */
#define EXBOOTSIG       0x29
        INT8            exVolumeID[4];          /* volume ID number */
        INT8            exVolumeLabel[11];      /* volume label */
        INT8            exFileSysType[8];       /* fs type (FAT12 or FAT16) */
} T_extboot;

typedef struct
{
        UINT8           bsJump[3];              /* jump inst E9xxxx or EBxx90 */
        INT8            bsOemName[8];           /* OEM name and version */
        INT8            bsBPB[25];              /* BIOS parameter block */
        INT8            bsExt[26];              /* Bootsector Extension */
        INT8            bsBootCode[448];        /* pad so structure is 512b */
        UINT8           bsBootSectSig0;
        UINT8           bsBootSectSig1;
#define BOOTSIG0        0x55
#define BOOTSIG1        0xaa
} T_bootsector50;

typedef struct
{
        UINT8           bsJump[3];              /* jump inst E9xxxx or EBxx90 */
        INT8            bsOEMName[8];           /* OEM name and version */
        INT8            bsBPB[53];              /* BIOS parameter block */
        INT8            bsExt[26];              /* Bootsector Extension */
        INT8            bsBootCode[418];        /* pad so structure is 512b */
        UINT8           bsBootSectSig2;         /* 2 & 3 are only defined for FAT32? */
        UINT8           bsBootSectSig3;
        UINT8           bsBootSectSig0;
        UINT8           bsBootSectSig1;
#define BOOTSIG0        0x55
#define BOOTSIG1        0xaa
#define BOOTSIG2        0
#define BOOTSIG3        0
} T_bootsector710;

typedef union
{
        T_bootsector33  bs33;
        T_bootsector50  bs50;
        T_bootsector710 bs710;
} T_bootsector;

/*
 * Shorthand for fields in the bpb.
 */
#define bsBytesPerSec   bsBPB.bpbBytesPerSec
#define bsSectPerClust  bsBPB.bpbSectPerClust
#define bsResSectors    bsBPB.bpbResSectors
#define bsFATS          bsBPB.bpbFATS
#define bsRootDirEnts   bsBPB.bpbRootDirEnts
#define bsSectors       bsBPB.bpbSectors
#define bsMedia         bsBPB.bpbMedia
#define bsFATsecs       bsBPB.bpbFATsecs
#define bsSectPerTrack  bsBPB.bpbSectPerTrack
#define bsHeads         bsBPB.bpbHeads
#define bsHiddenSecs    bsBPB.bpbHiddenSecs
#define bsHugeSectors   bsBPB.bpbHugeSectors

#endif

//      End
