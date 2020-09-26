//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: fat.c
//
// MODULE NAME: fat
//
// PURPOSE:     PC Disk FAT header file.
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

#ifndef __FAT_H__
  #define __FAT_H__

typedef UINT16    * T_fat16;
typedef UINT32    * T_fat32;

/*
 * Some useful cluster numbers.
 */
#define MSDOSFSROOT     0               /* cluster 0 means the root dir */
#define CLUST_FREE      0               /* cluster 0 also means a free cluster */
#define MSDOSFSFREE     CLUST_FREE
#define CLUST_FIRST     2               /* first legal cluster number */
#define CLUST_RSRVD     0xfffffff6      /* reserved cluster range */
#define CLUST_BAD       0xfffffff7      /* a cluster with a defect */
#define CLUST_EOFS      0xfffffff8      /* start of eof cluster range */
#define CLUST_EOFE      0xffffffff      /* end of eof cluster range */

#define FAT12_MASK      0x00000fff      /* mask for 12 bit cluster numbers */
#define FAT16_MASK      0x0000ffff      /* mask for 16 bit cluster numbers */
#define FAT32_MASK      0x0fffffff      /* mask for FAT32 cluster numbers */

/*
 * MSDOSFS:
 * Return true if filesystem uses 12 bit fats. Microsoft Programmer's
 * Reference says if the maximum cluster number in a filesystem is greater
 * than 4078 ((CLUST_RSRVS - CLUST_FIRST) & FAT12_MASK) then we've got a
 * 16 bit fat filesystem. While mounting, the result of this test is stored
 * in pm_fatentrysize.
 * GEMDOS-flavour (atari):
 * If the filesystem is on floppy we've got a 12 bit fat filesystem, otherwise
 * 16 bit. We check the d_type field in the disklabel struct while mounting
 * and store the result in the pm_fatentrysize. Note that this kind of
 * detection gets flakey when mounting a vnd-device.
 */
#define FAT12(pmp)      (pmp->pm_fatmask == FAT12_MASK)
#define FAT16(pmp)      (pmp->pm_fatmask == FAT16_MASK)
#define FAT32(pmp)      (pmp->pm_fatmask == FAT32_MASK)

#define MSDOSFSEOF(pmp, cn)     ((((cn) | ~(pmp)->pm_fatmask) & CLUST_EOFS) == CLUST_EOFS)


#endif

//      End
