//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: bpb.h
//
// MODULE NAME: bpb
//
// PURPOSE:     BIOS Parameters Block header file.
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

#ifndef __BPB_H__
  #define __BPB_H__

/*
 * BIOS Parameter Block (BPB) for DOS 3.3
 */
typedef struct
{
        UINT16          bpbBytesPerSec; /* bytes per sector */
        UINT8           bpbSecPerClust; /* sectors per cluster */
        UINT16          bpbResSectors;  /* number of reserved sectors */
        UINT8           bpbFATs;        /* number of FATs */
        UINT16          bpbRootDirEnts; /* number of root directory entries */
        UINT16          bpbSectors;     /* total number of sectors */
        UINT8           bpbMedia;       /* media descriptor */
        UINT16          bpbFATsecs;     /* number of sectors per FAT */
        UINT16          bpbSecPerTrack; /* sectors per track */
        UINT16          bpbHeads;       /* number of heads */
        UINT16          bpbHiddenSecs;  /* number of hidden sectors */
} T_bpb33;

/*
 * BPB for DOS 5.0 The difference is bpbHiddenSecs is a short for DOS 3.3,
 * and bpbHugeSectors is not in the 3.3 bpb.
 */
typedef struct
{
        UINT16          bpbBytesPerSec; /* bytes per sector */
        UINT8           bpbSecPerClust; /* sectors per cluster */
        UINT16          bpbResSectors;  /* number of reserved sectors */
        UINT8           bpbFATs;        /* number of FATs */
        UINT16          bpbRootDirEnts; /* number of root directory entries */
        UINT16          bpbSectors;     /* total number of sectors */
        UINT8           bpbMedia;       /* media descriptor */
        UINT16          bpbFATsecs;     /* number of sectors per FAT */
        UINT16          bpbSecPerTrack; /* sectors per track */
        UINT16          bpbHeads;       /* number of heads */
        UINT32          bpbHiddenSecs;  /* # of hidden sectors */
        UINT32          bpbHugeSectors; /* # of sectors if bpbSectors == 0 */
} T_bpb50;

/*
 * BPB for DOS 7.10 (FAT32).  This one has a few extensions to bpb50.
 */
typedef struct
{
        UINT16          bpbBytesPerSec; /* bytes per sector */
        UINT8           bpbSecPerClust; /* sectors per cluster */
        UINT16          bpbResSectors;  /* number of reserved sectors */
        UINT8           bpbFATs;        /* number of FATs */
        UINT16          bpbRootDirEnts; /* number of root directory entries */
        UINT16          bpbSectors;     /* total number of sectors */
        UINT8           bpbMedia;       /* media descriptor */
        UINT16          bpbFATsecs;     /* number of sectors per FAT */
        UINT16          bpbSecPerTrack; /* sectors per track */
        UINT16          bpbHeads;       /* number of heads */
        UINT32          bpbHiddenSecs;  /* # of hidden sectors */
        UINT32          bpbHugeSectors; /* # of sectors if bpbSectors == 0 */
        UINT32          bpbBigFATsecs;  /* like bpbFATsecs for FAT32 */
        UINT16          bpbExtFlags;    /* extended flags: */
#define FATNUM          0xf             /* mask for numbering active FAT */
#define FATMIRROR       0x80            /* FAT is mirrored (like it always was) */
        UINT16          bpbFSVers;      /* filesystem version */
#define FSVERS          0               /* currently only 0 is understood */
        UINT32          bpbRootClust;   /* start cluster for root directory */
        UINT16          bpbFSInfo;      /* filesystem info structure sector */
        UINT16          bpbBackup;      /* backup boot sector */
        /* There is a 12 byte filler here, but we ignore it */
} T_bpb710;

/*
 * The following structures represent how the bpb's look on disk.  shorts
 * and longs are just character arrays of the appropriate length.  This is
 * because the compiler forces shorts and longs to align on word or
 * halfword boundaries.
 *
 * XXX The little-endian code here assumes that the processor can access
 * 16-bit and 32-bit quantities on byte boundaries.  If this is not true,
 * use the macros for the big-endian case.
 */
/*
#include <machine/endian.h>
#if (BYTE_ORDER == LITTLE_ENDIAN) && defined(UNALIGNED_ACCESS)
#define getushort(x)    *((UINT16    *)(x))
#define getulong(x)     *((UINT32    *)(x))
#define putushort(p, v) (*((UINT16    *)(p)) = (v))
#define putulong(p, v)  (*((UINT32    *)(p)) = (v))
#else
#define getushort(x)    (((UINT8    *)(x))[0] + (((UINT8    *)(x))[1] << 8))
#define getulong(x)     (((UINT8    *)(x))[0] + (((UINT8    *)(x))[1] << 8) \
                         + (((UINT8    *)(x))[2] << 16) \
                         + (((UINT8    *)(x))[3] << 24))
#define putushort(p, v) (((UINT8    *)(p))[0] = (v),    \
                         ((UINT8    *)(p))[1] = (v) >> 8)
#define putulong(p, v)  (((UINT8    *)(p))[0] = (v),    \
                         ((UINT8    *)(p))[1] = (v) >> 8, \
                         ((UINT8    *)(p))[2] = (v) >> 16,\
                         ((UINT8    *)(p))[3] = (v) >> 24)
#endif
*/

/*
 * BIOS Parameter Block (BPB) for DOS 3.3
 */
typedef struct
{
        INT8   bpbBytesPerSec[2];       /* bytes per sector */
        INT8   bpbSecPerClust;          /* sectors per cluster */
        INT8   bpbResSectors[2];        /* number of reserved sectors */
        INT8   bpbFATs;                 /* number of FATs */
        INT8   bpbRootDirEnts[2];       /* number of root directory entries */
        INT8   bpbSectors[2];           /* total number of sectors */
        INT8   bpbMedia;                /* media descriptor */
        INT8   bpbFATsecs[2];           /* number of sectors per FAT */
        INT8   bpbSecPerTrack[2];       /* sectors per track */
        INT8   bpbHeads[2];             /* number of heads */
        INT8   bpbHiddenSecs[2];        /* number of hidden sectors */
} T_byte_bpb33;

/*
 * BPB for DOS 5.0 The difference is bpbHiddenSecs is a short for DOS 3.3,
 * and bpbHugeSectors is not in the 3.3 bpb.
 */
typedef struct
{
        INT8   bpbBytesPerSec[2];       /* bytes per sector */
        INT8   bpbSecPerClust;          /* sectors per cluster */
        INT8   bpbResSectors[2];        /* number of reserved sectors */
        INT8   bpbFATs;                 /* number of FATs */
        INT8   bpbRootDirEnts[2];       /* number of root directory entries */
        INT8   bpbSectors[2];           /* total number of sectors */
        INT8   bpbMedia;                /* media descriptor */
        INT8   bpbFATsecs[2];           /* number of sectors per FAT */
        INT8   bpbSecPerTrack[2];       /* sectors per track */
        INT8   bpbHeads[2];             /* number of heads */
        INT8   bpbHiddenSecs[4];        /* number of hidden sectors */
        INT8   bpbHugeSectors[4];       /* # of sectors if bpbSectors == 0 */
} T_byte_bpb50;

/*
 * BPB for DOS 7.10 (FAT32).  This one has a few extensions to bpb50.
 */
typedef struct
{
        UINT8    bpbBytesPerSec[2];     /* bytes per sector */
        UINT8    bpbSecPerClust;        /* sectors per cluster */
        UINT8    bpbResSectors[2];      /* number of reserved sectors */
        UINT8    bpbFATs;               /* number of FATs */
        UINT8    bpbRootDirEnts[2];     /* number of root directory entries */
        UINT8    bpbSectors[2];         /* total number of sectors */
        UINT8    bpbMedia;              /* media descriptor */
        UINT8    bpbFATsecs[2];         /* number of sectors per FAT */
        UINT8    bpbSecPerTrack[2];     /* sectors per track */
        UINT8    bpbHeads[2];           /* number of heads */
        UINT8    bpbHiddenSecs[4];      /* # of hidden sectors */
        UINT8    bpbHugeSectors[4];     /* # of sectors if bpbSectors == 0 */
        UINT8    bpbBigFATsecs[4];      /* like bpbFATsecs for FAT32 */
        UINT8    bpbExtFlags[2];        /* extended flags: */
        UINT8    bpbFSVers[2];          /* filesystem version */
        UINT8    bpbRootClust[4];       /* start cluster for root directory */
        UINT8    bpbFSInfo[2];          /* filesystem info structure sector */
        UINT8    bpbBackup[2];          /* backup boot sector */
        /* There is a 12 byte filler here, but we ignore it */
} T_byte_bpb710;

/*
 * FAT32 FSInfo block.
 */
typedef struct
{
        UINT8    fsisig1[4];
        UINT8    fsifill1[480];
        UINT8    fsisig2[4];
        UINT8    fsinfree[4];
        UINT8    fsinxtfree[4];
        UINT8    fsifill2[12];
        UINT8    fsisig3[4];
        UINT8    fsifill3[508];
        UINT8    fsisig4[4];
} T_fsinfo;

#endif

//      End
