//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: IDEDOS.C
//
// MODULE NAME: IDEDOS
//
// PURPOSE:     IDE driver for DOS.
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

#ifdef __MSDOS__
  #ifdef DEBUG
    #define DEBUG_DISKFS
  #endif
#endif

#include <bios.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <platform.h>
#include "iso.h"
#include "ide.h"

UINT16      DISK = 0x80;

#define CF              1

UINT16      acc = 0;
UINT16      sBuf[256];

typedef struct
{
  UINT8         spt;
  UINT8         cyl;
  UINT8         ndrv;
  UINT8         hd;
} T_hddbiospar;

typedef struct
{
  UINT16        bSize;
  UINT16        iFlags;
  UINT32        nCyl;
  UINT32        nHd;
  UINT32        nSpT;
  UINT32        nSecL;
  UINT32        nSecH;
  UINT16        nBpS;
} T_hddpar;

#ifdef __MSDOS__

EXTERN UINT16 Printf( UINT8 *format, ... );

#endif

STATIC BOOL IDE_SectorGetCD( UINT16 *buffer, UINT32 sector, UINT16 offs );

STATIC BOOL IDE_GetExtParams( UINT32 * drv )
{
  struct  REGPACK reg;
  T_hddpar        par;

  reg.r_ax = 0x4100;
  reg.r_dx = DISK;
  reg.r_bx = 0x55AA;
  intr( 0x13, &reg );
  if (reg.r_flags & CF)
  {
    return FALSE;
  }
  par.bSize = 0x1A;
  reg.r_ax = 0x4800;
  reg.r_dx = DISK;
  reg.r_ds = FP_SEG(&par);
  reg.r_si = FP_OFF(&par);
  intr( 0x13, &reg );
  if (reg.r_flags & CF)
  {
    return FALSE;
  }
  *drv = par.nSecL;
  return TRUE;
}

STATIC BOOL IDE_CheckCD( VOID )
{
  struct  REGPACK reg;

  reg.r_ax = 0x150B;
  reg.r_cx = DISK - 0x80 + 2;
  //
  intr( 0x2F, &reg );
  if (reg.r_flags & CF)
  {
    return FALSE;
  }
  if( (reg.r_bx == 0xADAD) && (reg.r_ax != 0) )
  {
    return TRUE;
  }
  return FALSE;
}

UINT8 IDE_Init( T_drvinf * drv )
{
  UINT32              sec;
  T_hddbiospar        par;
  T_ISO_PRIM_DESC   * pdesc;

  if( IDE_CheckCD() )
  {
#ifdef DEBUG_DISKFS
    Printf( "\nCD ROM drive detected." );
#endif

    //
    // Read Primary Volume Descriptor
    //
    if( IDE_SectorGetCD( sBuf, (0x10 << 2), 0 ) )
    {
      pdesc = (T_ISO_PRIM_DESC*)sBuf;
      if( isonum_711( pdesc->type ) == ISO_VD_PRIMARY )
      {
        if( memcmp( pdesc->id, ISO_STANDARD_ID, 5 ) == 0 )
        {
          drv->flg    = DRV_CD;
          drv->spt    = 0;
          drv->cyl    = 0;
          drv->hd     = 0;
          drv->sec    = isonum_733( pdesc->volume_space_size ) << 2;
#ifdef DEBUG_DISKFS
          Printf( "\nCD ROM parameters: Sectors (512B)  = %lu", drv->sec );
          Printf( "\n                   Total Capacity  = %luMB",
                  ((drv->sec * 512) / 1024 / 1024) );
#endif
          return IDE_CD;
        }
      }
    }
    return IDE_NONE;
  }
  else if( !biosdisk( 8, DISK, 0, 0, 0, 0, &par ) )
  {
#ifdef DEBUG_DISKFS
    Printf( "\nHD drive detected." );
#endif

    drv->spt    = (par.spt & 0x3F);
    drv->cyl    = ((UINT16)(par.spt & 0xC0) << 2) + par.cyl + 1;
    drv->hd     = par.hd + 1;
    drv->sec    = (UINT32)drv->spt * (UINT32)(drv->cyl+1) * (UINT32)drv->hd;

#ifdef DEBUG_DISKFS
    Printf( "\nHDD parameters   : Heads           = %d", drv->hd );
    Printf( "\n                   Sec per Track   = %d", drv->spt );
    Printf( "\n                   Cylinders       = %d", drv->cyl );
#endif
    if( IDE_GetExtParams( &sec ) )
    {
      drv->flg    = DRV_LBA;
      if( sec != drv->sec )
      {
#ifdef DEBUG_DISKFS
        Printf( " (WARNING: Max = %d)", sec / drv->hd / drv->spt);
        Printf( "\n                   Total Sectors   = %lu (WARNING: Max(LBA) = %lu)", drv->sec, sec );
#endif
      }
#ifdef DEBUG_DISKFS
      else
        Printf( "\n                   Total Sectors   = %lu", drv->sec );
#endif
    }
#ifdef DEBUG_DISKFS
    else
      Printf( "\n                   Total Sectors   = %lu", drv->sec );
#endif
    return IDE_HD;
  }
  return IDE_NONE;
}

STATIC BOOL IDE_GetFullCDSector( UINT16 *buffer, UINT32 sector )
{
  struct  REGPACK reg;

  reg.r_ax = 0x1508;
  reg.r_cx = DISK - 0x80 + 2;
  reg.r_dx = 1;
  reg.r_si = sector >> 16;
  reg.r_di = sector;
  reg.r_es = FP_SEG(buffer);
  reg.r_bx = FP_OFF(buffer);
  //
  intr( 0x2F, &reg );
  if (reg.r_flags & CF)
  {
    return FALSE;
  }
  return TRUE;
}

STATIC BOOL IDE_SectorGetCD( UINT16 *buffer, UINT32 sector, UINT16 offs )
{
  UINT8  buff[512 * 4];

  if( offs >= 512 )    return FALSE;
  //
  if( !IDE_GetFullCDSector( (UINT16*)buff, (sector >> 2) ) )     return FALSE;
  //
  if( (offs > 0) && ((sector % 4) == 3) )
  {
    memcpy( buffer, &buff[((sector % 4) * 512) + offs], (512 - offs) );
    //
    if( !IDE_GetFullCDSector( (UINT16*)buff, (sector >> 2) + 1 ) )     return FALSE;
    //
    memcpy( buffer, &buff[(sector % 4) * 512], offs );
  }
  else
  {
    memcpy( buffer, &buff[((sector % 4) * 512) + offs], 512 );
  }
  //
  return TRUE;
}

BOOL IDE_SectorGetAt( UINT16 *buffer , T_drvinf * drv, UINT32 sector, UINT16 offs )
{
  if( drv->flg & DRV_CD )
  {
    if( sector >= drv->sec )
    {
      return FALSE;
    }
    return IDE_SectorGetCD( buffer, sector, offs );
  }
  return FALSE;
}

BOOL IDE_SectorGet( UINT16 *buffer , T_drvinf * drv, UINT32 sector )
{
  struct  diskinfo_t blk;
  UINT8              head;
  UINT8              sec;
  UINT16             trk;

  if( drv->flg & DRV_CD )
  {
    if( sector >= drv->sec )
    {
      return FALSE;
    }
    return IDE_SectorGetCD( buffer, sector, 0 );
  }

  sec  = 1 + (sector  %  drv->spt);
  head = (sector / drv->spt)  %  drv->hd;
  trk  = sector / (drv->spt * drv->hd);

  if( trk > drv->cyl )
    return FALSE;

  blk.drive  = DISK;
  blk.track  = trk;
  blk.sector = sec;
  blk.head   = head;
  blk.nsectors = 1;
  blk.buffer = buffer;
  _bios_disk( _DISK_READ, &blk );
  if( (_bios_disk( _DISK_STATUS, &blk ) >> 8) == 0 )
  {
    return TRUE;
  }
  return FALSE;
}

BOOL IDE_SectorPut( UINT16 *buffer , T_drvinf * drv, UINT32 sector )
{
  struct  diskinfo_t blk;
  UINT8              head;
  UINT8              sec;
  UINT16             trk;

  if( drv->flg & DRV_CD )
  {
    return FALSE;
  }

  sec  = 1 + (sector  %  drv->spt);
  head = (sector / drv->spt)  %  drv->hd;
  trk  = sector / (drv->spt * drv->hd);

  if( trk > drv->cyl )
    return FALSE;

  blk.drive  = DISK;
  blk.track  = trk;
  blk.sector = sec;
  blk.head   = head;
  blk.nsectors = 1;
  blk.buffer = buffer;
  _bios_disk( _DISK_WRITE, &blk );
  if( (_bios_disk( _DISK_STATUS, &blk ) >> 8) == 0 )
  {
    return TRUE;
  }
  return FALSE;
}

//      End
