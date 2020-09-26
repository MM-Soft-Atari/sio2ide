//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: ptable.c
//
// MODULE NAME: ptable
//
// PURPOSE:     PC Disk Partitions Table header file.
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

#ifndef __PTABLE_H__
  #define __PTABLE_H__

/*
 * Format of a disk partitions table
 */
typedef struct
{
        UINT8           pActive;                /* Partition Active or Not */
#define PARTACT         0x80
#define PARTINACT       0x00
        UINT8           pStartHead;             /* Partition start: Head */
        UINT8           pStartCS[2];            /* Partition start: Cyl, Sect */
        UINT8           pType;                  /* Partition Type */
#define PARTNU          0x00
#define PARTDOS         0x04
//#define PARTEXTDOS      0x05
#define PARTFAT16W      0x0E
#define PARTBIGDOS      0x06
#define PARTFAT32B      0x0B
#define PARTFAT32       0x0C
#define ISO9660         0x7F
        UINT8           pEndHead;               /* Partition end: Head */
        UINT8           pEndCS[2];              /* Partition end: Cyl, Sect */
        UINT32          pStartSect;             /* Partition start: relative sector */
        UINT32          pSize;                  /* Partition size: sectors */
} T_partition;

/*
 * Format of a disk Master Boot Record.
 */
typedef struct
{
        UINT8           mbrLoader[0x1BE];       /* Loader */
        T_partition     mbrPTable[4];           /* Partitions Table */
        UINT8           mbrEnd1;                 /* MBR end marker */
        UINT8           mbrEnd2;                 /* MBR end marker */
#define MBRENDMARK1     0x55
#define MBRENDMARK2     0xAA
} T_mbr;

#endif

//      End
