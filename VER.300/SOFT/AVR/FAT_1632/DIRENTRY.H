//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: direntry.h
//
// MODULE NAME: direntry
//
// PURPOSE:     PC FS direntry header file.
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

#ifndef __DIRENTRY_H__
  #define __DIRENTRY_H__

/*
 * Structure of a dos directory entry.
 */
typedef struct
{
        UINT8           deName[8];      /* filename, blank filled */
#define SLOT_EMPTY      0x00            /* slot has never been used */
#define SLOT_E5         0x05            /* the real value is 0xe5 */
#define SLOT_DELETED    0xe5            /* file in this slot deleted */
        UINT8           deExtension[3]; /* extension, blank filled */
        UINT8           deAttributes;   /* file attributes */
#define ATTR_NORMAL     0x00            /* normal file */
#define ATTR_READONLY   0x01            /* file is readonly */
#define ATTR_HIDDEN     0x02            /* file is hidden */
#define ATTR_SYSTEM     0x04            /* file is a system file */
#define ATTR_VOLUME     0x08            /* entry is a volume label */
#define ATTR_DIRECTORY  0x10            /* entry is a directory name */
#define ATTR_ARCHIVE    0x20            /* file is new or modified */
        UINT8           deLowerCase;    /* case for base and extension */
#define CASE_LOWER_BASE 0x08            /* base is lower case */
#define CASE_LOWER_EXT  0x10            /* extension is lower case */
        UINT8           deCTimeHundredth; /* create time, 1/100th of a sec */
        UINT8           deCTime[2];     /* create time */
        UINT8           deCDate[2];     /* create date */
        UINT8           deADate[2];     /* access date */
//        UINT8           deHighClust[2]; /* high byte of cluster number */
        UINT16          deHighClust;    /* high byte of cluster number */
        UINT8           deMTime[2];     /* last update time */
        UINT8           deMDate[2];     /* last update date */
//        UINT8           deStartCluster[2]; /* starting cluster of file */
        UINT16          deStartCluster; /* starting cluster of file */
//        UINT8           deFileSize[4];  /* size of file in bytes */
        UINT32          deFileSize;     /* size of file in bytes */
} T_direntry;

/*
 * Structure of a Win95 long name directory entry
 */
typedef struct
{
        UINT8           weCnt;
#define WIN_LAST        0x40
#define WIN_CNT         0x3f
        UINT8           wePart1[10];
        UINT8           weAttributes;
#define ATTR_WIN95      0x0f
        UINT8           weReserved1;
        UINT8           weChksum;
        UINT8           wePart2[12];
        UINT16          weReserved2;
        UINT8           wePart3[4];
} T_winentry;

#define WIN_CHARS       13      /* Number of chars per winentry */

/*
 * This is the format of the contents of the deTime field in the direntry
 * structure.
 */
#define DT_2SECONDS_MASK        0x1F    /* seconds divided by 2 */
#define DT_2SECONDS_SHIFT       0
#define DT_MINUTES_MASK         0x7E0   /* minutes */
#define DT_MINUTES_SHIFT        5
#define DT_HOURS_MASK           0xF800  /* hours */
#define DT_HOURS_SHIFT          11

/*
 * This is the format of the contents of the deDate field in the direntry
 * structure.
 */
#define DD_DAY_MASK             0x1F    /* day of month */
#define DD_DAY_SHIFT            0
#define DD_MONTH_MASK           0x1E0   /* month */
#define DD_MONTH_SHIFT          5
#define DD_YEAR_MASK            0xFE00  /* year - 1980 */
#define DD_YEAR_SHIFT           9

#endif

//      End