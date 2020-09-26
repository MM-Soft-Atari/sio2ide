//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: FATFS.C
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
//              001   22-02-2002 MM     Created
//
//****************************************************************

#include "apps.h"
#include "iso.h"

#ifdef __MSDOS__
  #ifdef DEBUG
    #define DEBUG_DISKFS
    #include <bios.h>
    #include <stdio.h>
    #include <ctype.h>
  #endif
#else
  #ifdef DEBUG
    #define         DEBUG_FAT
  #endif
#endif

#define FL_DATA_OFFS    (sizeof(T_atrhdr))

#define PRT_START_SEC   0

#define SecToClust( drv, sec )                                          \
(                                                                       \
  (sec > drv.dataStartSec) ?                                            \
  (UINT32)(((sec - drv.dataStartSec) / drv.secPClust) + 2) :            \
  (UINT32)(2)                                                           \
)

#define ClustToSec( drv, clu )                                          \
(                                                                       \
  (clu > 1) ?                                                           \
  (UINT32)(((clu - 2) * drv.secPClust) + drv.dataStartSec) :            \
  (UINT32)(drv.rdirStartSec)                                            \
)

typedef BOOL (*T_filechk)( T_file * );

//
// Static data
//
STATIC FLASH UINT8      flNameExt[] PROGMEM   = FL_NAME_EXT;
STATIC FLASH UINT8      cfgFileName[] PROGMEM = CFG_FL_NAME;
STATIC FLASH UINT8      cfgNameExt[] PROGMEM  = CFG_NAME_EXT;
STATIC FLASH UINT8      cfgTokenDel[] PROGMEM = " \t\r\n";
STATIC FLASH UINT8      rdirName[] PROGMEM    = RDIR_NAME;
STATIC FLASH UINT8      sdirName[] PROGMEM    = SDIR_NAME;
STATIC FLASH UINT8      cflName[] PROGMEM     = CFL_NAME;
STATIC FLASH UINT8      isoID[] PROGMEM       = ISO_STANDARD_ID;
//
STATIC       UINT8      secBuff[ SECTOR_SIZE ];   // Sector buffer
STATIC       T_disktab  diskTab;                  // ATR table
STATIC       T_drive    cDrive;                   // Drive logical info
             T_drvinf   drvInf;                   // Drive phisical info
STATIC       T_findfl   find;                     // Find file structure

STATIC       UINT32     saveLoc[2];

#define GETSAVELOC( ms )        (UINT32)((ms == IDE_MASTER) ? (saveLoc[0]) : (saveLoc[1]))
#define SETSAVELOC( ms, loc )   (((ms) == IDE_MASTER) ? (saveLoc[0] = (loc)) : (saveLoc[1] = (loc)))

//
// Implementation
//

#ifdef __MSDOS__

EXTERN UINT16 Printf( UINT8 *format, ... );

VOID GetFindStruct( T_findfl * fnd )
{
  *fnd = find;
}

VOID PutFindStruct( T_findfl * fnd )
{
  find = *fnd;
}
#endif

#ifdef DEBUG_FAT
STATIC VOID Dump( UINT8 * p, UINT16 s )
{
  Printf( "\n" );
  while( s-- )
  {
    Printf( "%X ", *p++ );
  }
  Printf( "\n" );
}
#endif

/*
       rdirSects   = bpbSectors / (bpbRootDirEnts * 32)
       fatSects    = bpbFATs * bpbFATsecs
       dataStart   = bpbResSectors + fatSects + rdirSects
       absSector   = dataStart + ((AnyClusterNumber-2) * sectorsPerCluster)
*/

//----------------------------------------------------------------
// Function :   SectorRead
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL SectorRead( UINT32 sector )
{
  STATIC UINT32      lsect  = 0xFFFFFFFFL;

  if( sector != lsect )
  {
#ifdef DEBUG_DISKFS
    acc++;
//    printf( "\nS=%i", (UINT16)sector );
#endif
#ifdef DEBUG_FAT
//    Printf( "\nS=%i", (UINT16)sector );
#endif
    if( !IDE_SectorGet( (UINT16 *)secBuff, &drvInf, sector ) )
      return FALSE;
    //
    lsect = sector;
  }
  return TRUE;
}

//----------------------------------------------------------------
// Function :   SectorReadAt
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL SectorReadAt( UINT32 sector, UINT16 offs )
{
#ifdef DEBUG_DISKFS
  acc++;
//  printf( "\nS=%i O=%i", (UINT16)sector, (UINT16)offs );
#endif
#ifdef DEBUG_FAT
//  Printf( "\nS=%i O=%i", (UINT16)sector, (UINT16)offs );
#endif
  return IDE_SectorGetAt( (UINT16 *)secBuff, &drvInf, sector, offs );
}

//----------------------------------------------------------------
// Function :   SectorWrite
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL SectorWrite( UINT32 sector )
{
  return IDE_SectorPut( (UINT16 *)secBuff, &drvInf, sector );
}

//----------------------------------------------------------------
// Function :   GetNextFAT16Clust
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC UINT32 GetNextFAT16Clust( UINT32 clust )
{
  UINT32      csect;
  T_fat16     fat16;

  csect = (((clust) * 2) / SECTOR_SIZE) + cDrive.fatStartSec;
  if( !SectorRead( csect ) )
    return 0;
  fat16 = (T_fat16)secBuff + (((clust * 2) % SECTOR_SIZE) / 2);
  return (*fat16 & FAT16_MASK);
}

//----------------------------------------------------------------
// Function :   GetNextFAT32Clust
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC UINT32 GetNextFAT32Clust( UINT32 clust )
{
  UINT32      csect;
  T_fat32     fat32;

  csect = (((clust) * 4) / SECTOR_SIZE) + cDrive.fatStartSec;
  if( !SectorRead( csect ) )
    return 0;
  fat32 = (T_fat32)secBuff + (((clust * 4) % SECTOR_SIZE) / 4);
  return (*fat32 & FAT32_MASK);
}

//----------------------------------------------------------------
// Function :   TestFileStruct
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC UINT16 TestFileStruct( T_fldet * fdet )
{
  UINT32      cclust = SecToClust( cDrive, fdet->flStartSec );
  UINT32      nclust;
  UINT16      clucnt = 0;

  //
  // Check file structure
  //
  switch( cDrive.prtType )
  {
    case PARTBIGDOS:
    case PARTDOS:
    case PARTFAT16W:
      do
      {
        //
        // Read FAT16 file entries (check if file is defragmented)
        //
        if( (nclust = GetNextFAT16Clust( cclust )) == 0 )
        {
          return 0;
        }
        //
        clucnt++;
        if( nclust != (CLUST_EOFE & FAT16_MASK) )
        {
          if( nclust != (cclust + 1) )
          {
            fdet->flStat |= FLS_NOTDEFR;
          }
        }
        cclust = nclust;
      }while( nclust != (CLUST_EOFE & FAT16_MASK) );
      return clucnt;
    case PARTFAT32:     // 0x0C
    case PARTFAT32B:    // 0x0B
      do
      {
        //
        // Read FAT32 file entries (check if file is defragmented)
        //
        if( (nclust = GetNextFAT32Clust( cclust )) == 0 )
        {
          return 0;
        }
        //
        clucnt++;
        if( nclust != (CLUST_EOFE & FAT32_MASK) )
        {
          if( nclust != (cclust + 1) )
          {
            fdet->flStat |= FLS_NOTDEFR;
          }
        }
        cclust = nclust;
      }while( nclust != (CLUST_EOFE & FAT32_MASK) );
      return clucnt;
    case ISO9660:
      return 1;
    default:
    break;
  }
  //
  return 0;
}

//----------------------------------------------------------------
// Function :   GetDiskSector
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC UINT32 GetDiskSector( T_fldet * fdet, UINT32 sSec, UINT16 relSec )
{
  UINT32      cclust;
  UINT32      nclust;
  UINT32      csec;

  if( !(fdet->flStat & FLS_NOTDEFR) )
  {
    return sSec + relSec;
  }
  //
  cclust = SecToClust( cDrive, sSec );
  csec   = ClustToSec( cDrive, cclust );
  //
  if( sSec > csec )
  {
    relSec += (sSec - csec);
  }
  //
  csec = 0;
  //
  // Check file structure
  //
  switch( cDrive.prtType )
  {
    case PARTBIGDOS:
    case PARTDOS:
    case PARTFAT16W:
      do
      {
        csec += cDrive.secPClust;
        //
        if( relSec < csec )
        {
          return ClustToSec( cDrive, cclust ) + (relSec % cDrive.secPClust);
        }
        //
        // Read FAT16 file entries (check if file is defragmented)
        //
        if( (nclust = GetNextFAT16Clust( cclust )) == 0 )
        {
          return 0;
        }
        //
        cclust = nclust;
      }while( nclust != (CLUST_EOFE & FAT16_MASK) );
      //
    case PARTFAT32:     // 0x0C
    case PARTFAT32B:    // 0x0B
      do
      {
        csec += cDrive.secPClust;
        //
        if( relSec < csec )
        {
          return ClustToSec( cDrive, cclust ) + (relSec % cDrive.secPClust);
        }
        //
        // Read FAT32 file entries (check if file is defragmented)
        //
        if( (nclust = GetNextFAT32Clust( cclust )) == 0 )
        {
          return 0;
        }
        //
        cclust = nclust;
      }while( nclust != (CLUST_EOFE & FAT32_MASK) );
      //
    case ISO9660:
      return sSec + relSec;
    default:
    break;
  }
  return 0;
}

#define FINDFL_ERR      0
#define FINDFL_OK       1
#define FINDFL_BSY      2

//----------------------------------------------------------------
// Function :   CheckNext_HDClu
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC UINT8 CheckNext_HDClu( T_file * file, T_filechk filechk )
{
  //
  // Check all Root Dir sectors in cur cluster
  //
  while( find.HD.csect < find.HD.esect )
  {
    find.HD.cent = (T_direntry*)secBuff + find.HD.cpos;
    //
    // Check all dir entries in the sector
    //
    while( find.HD.cpos < (SECTOR_SIZE / sizeof(T_direntry)) )
    {
      if( !SectorRead( find.HD.csect ) )
        return FINDFL_ERR;
      find.HD.cpos++;
      //
      if( filechk( file ) )
        return FINDFL_OK;
      //
      find.HD.cent++;
    }
    find.HD.cpos = 0;
    find.HD.psect = find.HD.csect;
    find.HD.csect++;
  }
  return FINDFL_BSY;
}

//----------------------------------------------------------------
// Function :   GetNextFile_Fat16RootDir
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL GetNextFile_Fat16RootDir( T_file * file, T_filechk filechk )
{
  //
  // Check all Root Dir clusters
  //
  switch( CheckNext_HDClu( file, filechk ) )
  {
    case FINDFL_OK:
      return TRUE;
    case FINDFL_ERR:
      return FALSE;
    case FINDFL_BSY:
    default:
      return FALSE;
  };
}

//----------------------------------------------------------------
// Function :   GetFirstFile_Fat16RootDir
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL GetFirstFile_Fat16RootDir( T_file * file, T_filechk filechk )
{
  find.HD.csect = cDrive.rdirStartSec;
  find.HD.psect = find.HD.csect;
  find.HD.esect = find.HD.csect + (cDrive.rdirClust * cDrive.secPClust);
  find.HD.cpos  = 0;
  //
  return GetNextFile_Fat16RootDir( file, filechk );
}

//----------------------------------------------------------------
// Function :   GetNextFile_Fat16Dir
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL GetNextFile_Fat16Dir( T_file * file, T_filechk filechk )
{
  UINT32        cclu = SecToClust( cDrive, find.HD.csect );
  //
  // Check all Root Dir clusters
  //
  do
  {
    switch( CheckNext_HDClu( file, filechk ) )
    {
      case FINDFL_OK:
        return TRUE;
      case FINDFL_ERR:
        return FALSE;
      case FINDFL_BSY:
      default:
        break;
    };
    cclu = GetNextFAT16Clust( cclu );
    find.HD.csect = ClustToSec( cDrive, cclu );
    find.HD.esect = find.HD.csect + cDrive.secPClust;
  }while( cclu != (CLUST_EOFE & FAT16_MASK) );
  //
  return FALSE;
}

//----------------------------------------------------------------
// Function :   GetFirstFile_Fat16Dir
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL GetFirstFile_Fat16Dir( T_file * file, T_filechk filechk )
{
  find.HD.csect = cDrive.cwdirStartSec;
  find.HD.psect = find.HD.csect;
  find.HD.esect = find.HD.csect + cDrive.secPClust;
  find.HD.cpos  = 0;
  //
  return GetNextFile_Fat16Dir( file, filechk );
}

//----------------------------------------------------------------
// Function :   GetNextFile_Fat32Dir
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL GetNextFile_Fat32Dir( T_file * file, T_filechk filechk )
{
  UINT32        cclu = SecToClust( cDrive, find.HD.csect );
  //
  // Check all Root Dir clusters
  //
  do
  {
    switch( CheckNext_HDClu( file, filechk ) )
    {
      case FINDFL_OK:
        return TRUE;
      case FINDFL_ERR:
        return FALSE;
      case FINDFL_BSY:
      default:
        break;
    };
    cclu = GetNextFAT32Clust( cclu );
    find.HD.csect = ClustToSec( cDrive, cclu );
    find.HD.esect = find.HD.csect + cDrive.secPClust;
  }while( cclu != (CLUST_EOFE & FAT32_MASK) );
  //
  return FALSE;
}

//----------------------------------------------------------------
// Function :   GetFirstFile_Fat32Dir
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL GetFirstFile_Fat32Dir( T_file * file, T_filechk filechk )
{
  find.HD.csect = cDrive.cwdirStartSec;
  find.HD.psect = find.HD.csect;
  find.HD.esect = find.HD.csect + cDrive.secPClust;
  find.HD.cpos  = 0;
  //
  return GetNextFile_Fat32Dir( file, filechk );
}

//----------------------------------------------------------------
// Function :   GetNextFile_CDDir
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL GetNextFile_CDDir( T_file * file, T_filechk filechk )
{
  //
  // Check all Dir entries in cur dir sectors
  //
  do
  {
    if( !SectorRead( find.CD.csect ) )                  return FALSE;
    //
    find.CD.cent = (T_ISO_DIR_REC*)((UINT8*)secBuff + find.CD.cpos);
    //
    if( isonum_711( find.CD.cent->length ) == 0 )       return FALSE;
    //
    if( (find.CD.cpos + isonum_711( find.CD.cent->length )) >= SECTOR_SIZE )
    {
      if( !SectorReadAt( find.CD.csect, find.CD.cpos ) )  return FALSE;
      //
      find.CD.cent = (T_ISO_DIR_REC*)secBuff;
      //
      if( isonum_711( find.CD.cent->length ) == 0 )       return FALSE;
      //
      find.CD.cpos += isonum_711( find.CD.cent->length );
      find.CD.cpos %= SECTOR_SIZE;
      find.CD.csect++;
    }
    else
    {
      find.CD.cpos += isonum_711( find.CD.cent->length );
    }
    //
  }while( !filechk( file ) );
  //
  return TRUE;
}

//----------------------------------------------------------------
// Function :   GetFirstFile_CDDir
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL GetFirstFile_CDDir( T_file * file, T_filechk filechk )
{
  find.CD.csect = cDrive.cwdirStartSec;
  find.CD.cpos  = 0;
  //
  return GetNextFile_CDDir( file, filechk );
}

//----------------------------------------------------------------
// Function :   GetFirstFile_Virtual
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL GetFirstFile_Virtual( T_file * file, T_filechk filechk )
{
  switch( cDrive.prtType )
  {
    case PARTBIGDOS:
    case PARTDOS:
    case PARTFAT16W:
      if( cDrive.cwdirStartSec == cDrive.rdirStartSec )
        return GetFirstFile_Fat16RootDir( file, filechk );
      else
        return GetFirstFile_Fat16Dir( file, filechk );
    case PARTFAT32:
    case PARTFAT32B:
      return GetFirstFile_Fat32Dir( file, filechk );
    case ISO9660:
      return GetFirstFile_CDDir( file, filechk );
    default:
      return FALSE;
  };
}

//----------------------------------------------------------------
// Function :   GetNextFile_Virtual
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL GetNextFile_Virtual( T_file * file, T_filechk filechk )
{
  switch( cDrive.prtType )
  {
    case PARTBIGDOS:
    case PARTDOS:
    case PARTFAT16W:
      if( cDrive.cwdirStartSec == cDrive.rdirStartSec )
        return GetNextFile_Fat16RootDir( file, filechk );
      else
        return GetNextFile_Fat16Dir( file, filechk );
    case PARTFAT32:
    case PARTFAT32B:
      return GetNextFile_Fat32Dir( file, filechk );
    case ISO9660:
      return GetNextFile_CDDir( file, filechk );
    default:
      return FALSE;
  };
}

//----------------------------------------------------------------
// Function :   GetISOFileName
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL GetISOFileName( T_ISO_DIR_REC * dir, UINT8 * name, UINT8 * ext )
{
  UINT8   * ptr, * e_ptr;
  UINT8     len, n_len, e_len;

  len = dir->name_len[0];
  memset( name, ' ', 8 );
  memset( ext, ' ', 3 );
  //
  if( len && (len <= 14) && (dir->name[0] > 1) )
  {
    //
    n_len = len;
    e_ptr = dir->name;
    e_len = 0;
    //
    if( (ptr = strchr( dir->name, ';' )) != NULL )
    {
      if( ((UINT16)ptr - (UINT16)dir->name) < len )
      {
        len = ((UINT16)ptr - (UINT16)dir->name);
      }
    }
    //
    if( len > 12 )      return FALSE;
    //
    if( (ptr = strchr( dir->name, '.' )) != NULL )
    {
      if( ((UINT16)ptr - (UINT16)dir->name) < len )
      {
        n_len = ((UINT16)ptr - (UINT16)dir->name);
        e_ptr = (ptr + 1);
        e_len = len - n_len - 1;
      }
    }
    //
    if( n_len > 8 )     return FALSE;
    if( e_len > 3 )     return FALSE;
    //
    memcpy( name, dir->name, n_len );
    memcpy( ext, e_ptr, e_len );
    //
//    printf( "\n  %.8s.%.3s", name, ext );
    //
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   CheckATR
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC UINT16 CheckATR( T_file * file, UINT32 sec )
{
  T_atrhdr  * hdr;

  if( !SectorRead( sec ) )
    return 0;
  //
  hdr = (T_atrhdr*)secBuff;
  if( hdr->wMagic != NICKATARI )
  {
    file->flDet.flStat |= FLS_HDRERR;
//    return 0;
  }
  //
  if( hdr->wSecSize == FL_MAXSEC_SIZE )
    file->flDet.flStat |= FLS_SEC256B;
  else if( hdr->wSecSize != (FL_MAXSEC_SIZE / 2) )
  {
    file->flDet.flStat |= FLS_HDRERR;
//    return 0;
  }
  //
  if( !SectorRead( find.HD.csect ) )
    return 0;
  //
  return file->flDet.flStat & FLS_SEC256B ? 256 : 128;
}

//----------------------------------------------------------------
// Function :   TestATRFile
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL TestATRFile( T_file * file )
{
  UINT8       ext[3];
  UINT32      sclust;
  UINT16      ssize;

  memset( (UINT8*)file, 0x00, sizeof(T_file) );
  //
  // ISO9660
  //
  if( cDrive.prtType == ISO9660 )
  {
    //
    // Check file size
    //
    if( isonum_733( find.CD.cent->size ) == 0 )
      return FALSE;
    //
    if( isonum_711( find.CD.cent->ext_attr_length ) != 0 )
      return FALSE;
    //
    if( !GetISOFileName( find.CD.cent, file->flName, ext ) )
      return FALSE;
    //
    // Check if NOT directory
    //
    if( find.CD.cent->flags[0] & 0x02 )
      return FALSE;
    //
    file->flDet.flStat |= FLS_RDONLY;
    //
    // Check file name (CFG extension)
    //
    if( memcmpf( ext, cfgNameExt, 3 ) == 0 )
    {
      //
      if( isonum_733( find.CD.cent->size ) > SECTOR_SIZE )
        return FALSE;
      //
      file->flDet.flStat     = FLS_CFGFILE;
      file->flDet.flSize     = 0;
      file->flDet.flStartSec = isonum_733( find.CD.cent->extent ) << 2;
      //
      return TRUE;
    }
    //
    // Check if ATR extension
    //
    if( memcmpf( ext, flNameExt, 3 ) != 0 )
      return FALSE;
    //
    //
    // Check file header (ATR file only)
    //
    if( (ssize = CheckATR( file, isonum_733( find.CD.cent->extent ) )) == 0 )
      return FALSE;
    //
    if( (((isonum_733( find.CD.cent->size ) - FL_DATA_OFFS) % (UINT32)ssize) != 0) &&
        (isonum_733( find.CD.cent->size ) > FL_MAXSEC_SIZE)
      )
      return FALSE;
    //
    file->flDet.flSize     = ((isonum_733( find.CD.cent->size ) - FL_DATA_OFFS) / (UINT32)ssize);
    file->flDet.flStartSec = isonum_733( find.CD.cent->extent ) << 2;
    //
    return TRUE;
  }
  //
  // FAT 16/32
  //
  sclust = (UINT32)find.HD.cent->deStartCluster | ((UINT32)find.HD.cent->deHighClust << 16);
  //
  // Check if file not deleted and normal/archive/redonly
  //
  if( (find.HD.cent->deName[0] == SLOT_DELETED) ||
      (find.HD.cent->deName[0] == SLOT_E5) ||
      (find.HD.cent->deName[0] == 0) ||
      (find.HD.cent->deAttributes & ~(ATTR_ARCHIVE | ATTR_READONLY))
    )
    return FALSE;
  //
  // Check file name (CFG extension)
  //
  if( memcmpf( find.HD.cent->deExtension, cfgNameExt, 3 ) == 0 )
  {
    //
    if( find.HD.cent->deFileSize > SECTOR_SIZE )
      return FALSE;
    //
    file->flDet.flStat     = FLS_CFGFILE;
    file->flDet.flSize     = 0;
    file->flDet.flStartSec = ClustToSec( cDrive, sclust );
    //
    memcpy( file->flName, find.HD.cent->deName, 8 );
    //
    return TRUE;
  }
  //
  // Check file size
  //
  if( find.HD.cent->deFileSize == 0 )
    return FALSE;
  //
  if( memcmpf( find.HD.cent->deExtension, flNameExt, 3 ) != 0 )
    return FALSE;
  //
  // Check if ReadOnly File
  //
  if( find.HD.cent->deAttributes & ATTR_READONLY )
  {
    file->flDet.flStat |= FLS_RDONLY;
  }
  //
  // Check file header (ATR file only)
  //
  if( (ssize = CheckATR( file, ClustToSec( cDrive, sclust ) )) == 0 )
    return FALSE;
  //
  if( (((find.HD.cent->deFileSize - FL_DATA_OFFS - (UINT32)(3 * 128)) % (UINT32)ssize) != 0) &&
      (find.HD.cent->deFileSize > FL_MAXSEC_SIZE)
    )
    return FALSE;
  //
  file->flDet.flSize     = ((find.HD.cent->deFileSize - FL_DATA_OFFS - (UINT32)(3 * 128)) / (UINT32)ssize) + 3;
  file->flDet.flStartSec = ClustToSec( cDrive, sclust );
  memcpy( file->flName, find.HD.cent->deName, 8 );

  return TRUE;
}

//----------------------------------------------------------------
// Function :   FindFileNameF
// Notes    :
// History  :
//----------------------------------------------------------------
#ifdef __GNU__
STATIC BOOL FindFileNameF( T_file * file, CONST prog_char * name )
#endif
#if defined(__IAR__) || defined(__MSDOS__)
STATIC BOOL FindFileNameF( T_file * file, UINT8 FLASH * name )
#endif
{
  if( GetFirstFile_Virtual( file, TestATRFile ) )
  {
    do
    {
      if( memcmpf( file->flName, name, 8 ) == 0 )
      {
        return TRUE;
      }
    }while( GetNextFile_Virtual( file, TestATRFile ) );
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   FindFileName
// Notes    :
// History  :
//----------------------------------------------------------------
/*
STATIC BOOL FindFileName( T_file * file, UINT8 * name )
{
  if( GetFirstFile_Virtual( file, TestATRFile ) )
  {
    do
    {
      if( memcmp( file->flName, name, 8 ) == 0 )
      {
        return TRUE;
      }
    }while( GetNextFile_Virtual( file, TestATRFile ) );
  }
  return FALSE;
}
*/

//----------------------------------------------------------------
// Function :   FindFileNameR
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL FindFileNameR( T_file * file, UINT8 * name )
{
  if( GetFirstFile_Virtual( file, TestATRFile ) )
  {
    do
    {
      if( memcmp( file->flName, name, 8 ) == 0 )
      {
        return TRUE;
      }
    }while( GetNextFile_Virtual( file, TestATRFile ) );
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   FindFileSect
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL FindFileSect( T_file * file, UINT32 sect )
{
  if( GetFirstFile_Virtual( file, TestATRFile ) )
  {
    do
    {
      if( file->flDet.flStartSec == sect )
      {
        return TRUE;
      }
    }while( GetNextFile_Virtual( file, TestATRFile ) );
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   GetHDDriveParams
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL GetHDDriveParams( BOOL debug )
{
  T_mbr         * mbr;
  T_partition   * prt;
  T_bootsector  * boot;
  T_bpb50       * bpb50;
  T_bpb710      * bpb710;
  T_file          file;
  UINT16          tmp;

  if( debug ) Printf( "\nTry to Init File System:" );

  if( !SectorRead( PRT_START_SEC ) )
    return FALSE;
  mbr = (T_mbr*)secBuff;

  if( debug ) Printf(    "\n  MBR mark       : 0x%x%x", mbr->mbrEnd1, mbr->mbrEnd2 );

  if( (mbr->mbrEnd1 == MBRENDMARK1) && (mbr->mbrEnd2 == MBRENDMARK2) )
  {
    prt = &mbr->mbrPTable[0];

//    if( prt->pActive == PARTACT )
    {
      cDrive.prtType = prt->pType;

     if( debug ) Printf( "\n  Partition Type : 0x%x", cDrive.prtType );

      //
      // Read Boot sector
      //
      if( !SectorRead( PRT_START_SEC + prt->pStartSect ) )
        return FALSE;
      //
      boot = (T_bootsector*)secBuff;

#ifdef DEBUG_DISKFS
      Printf( "\nOperating system : %.8s", boot->bs33.bsOemName );
#endif

      switch( cDrive.prtType )
      {
        case PARTBIGDOS:
        case PARTDOS:
        case PARTFAT16W:

          if( debug ) Printf( " FAT16" );

#ifdef DEBUG_FAT
          Printf( "\nFAT16" );
#endif
#ifdef DEBUG_DISKFS
          Printf( "\nFile system      : FAT16" );
#endif
          bpb50 = (T_bpb50*)&boot->bs50.bsBPB[0];

          //
          cDrive.fatStartSec  = PRT_START_SEC + bpb50->bpbResSectors +
                                bpb50->bpbHiddenSecs;
          tmp                 = bpb50->bpbFATs * bpb50->bpbFATsecs; // FAT sects
          //
          cDrive.secPClust    = bpb50->bpbSecPerClust;
          //
          cDrive.rdirStartSec = PRT_START_SEC + bpb50->bpbResSectors +
                                tmp + bpb50->bpbHiddenSecs;
          cDrive.cwdirStartSec= cDrive.rdirStartSec;
          //
          cDrive.rdirClust    = bpb50->bpbRootDirEnts *
                                sizeof(T_direntry) / SECTOR_SIZE /
                                cDrive.secPClust;
          cDrive.dataStartSec = cDrive.rdirStartSec +
                                (cDrive.rdirClust * cDrive.secPClust);

#ifdef DEBUG_FAT
          Dump( (UINT8*)&cDrive, sizeof(cDrive) );
#endif
        break;
        case PARTFAT32:
        case PARTFAT32B:

          if( debug ) Printf( " FAT32" );

#ifdef DEBUG_FAT
          Printf( "\nFAT32" );
#endif
#ifdef DEBUG_DISKFS
          Printf( "\nFile system      : FAT32" );
#endif
          bpb710 = (T_bpb710*)&boot->bs710.bsBPB[0];
          //
          cDrive.fatStartSec  = PRT_START_SEC + bpb710->bpbResSectors +
                                bpb710->bpbHiddenSecs;
          tmp                 = bpb710->bpbFATs * bpb710->bpbBigFATsecs; // FAT sects
          //
          cDrive.secPClust    = bpb710->bpbSecPerClust;
          //
          cDrive.dataStartSec = PRT_START_SEC + bpb710->bpbResSectors +
                                tmp + bpb710->bpbHiddenSecs;
          //
          cDrive.rdirClust    = 1;      // Default (1 cluster)
          cDrive.rdirStartSec = ClustToSec( cDrive, bpb710->bpbRootClust );
          cDrive.cwdirStartSec= cDrive.rdirStartSec;
          //
          // Check if Root Dir is defragmented
          //
          file.flDet.flStartSec = cDrive.rdirStartSec;
          file.flDet.flSize     = 0;
          if( (tmp = TestFileStruct( &file.flDet )) == 0 )
          {
#ifdef DEBUG_DISKFS
            Printf( " (WARNING: Root Dir is NOT defragmented.)" );
#endif
          }
          else
          {
            cDrive.rdirClust = tmp;
          }
        break;
        default:

          if( debug ) Printf( " Unknown" );

#ifdef DEBUG_DISKFS
          Printf( "\nERROR: This Disk Drive is not supported (id=0x%02X)", cDrive.prtType );
#endif
          return FALSE;
      };
#ifdef DEBUG_DISKFS
      Printf( "\n                   Sec per Cluster = %d", cDrive.secPClust );
#endif
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   GetCDDriveParams
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL GetCDDriveParams( VOID )
{
  T_ISO_PRIM_DESC   * pdesc;
  T_ISO_DIR_REC     * rdir;

  if( !SectorRead( 0x10 << 2 ) )
    return FALSE;

  pdesc = (T_ISO_PRIM_DESC*)secBuff;
  //
  if( isonum_711( pdesc->type ) == ISO_VD_PRIMARY )
  {
    if( memcmpf( pdesc->id, isoID, 5 ) == 0 )
    {
#ifdef DEBUG_FAT
      Printf( "\nISO9660" );
#endif
      cDrive.prtType = ISO9660;
      //
      rdir = (T_ISO_DIR_REC*)&pdesc->root_directory_record;
      //
      cDrive.rdirStartSec  = isonum_733( rdir->extent ) << 2;
      cDrive.cwdirStartSec = cDrive.rdirStartSec;
      //
      drvInf.sec = isonum_733( pdesc->volume_space_size ) << 2;

      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   FATFS_CountDisks
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC UINT8 FATFS_CountDisks( VOID )
{
  UINT8    id, fl;

  fl = 0;
  for( id = 1; id <= MAX_FL_ID; id++ )
  {
    if( diskTab[ id ].flStat & FLS_FILEOK )
    {
      if( (id != COM_FL_ID) ||
          ((id == COM_FL_ID) && (cDrive.cwdirStartSec == cDrive.rdirStartSec))
        )
      {
        fl++;
      }
    }
  }
  return fl;
}

//----------------------------------------------------------------
// Function :   TestDir
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL TestDir( T_file * file )
{
  UINT8       ext[3];
  UINT32      sclust;

  memset( (UINT8*)file, 0x00, sizeof(T_file) );
  //
  // ISO9660
  //
  if( cDrive.prtType == ISO9660 )
  {
    //
    // Check file size
    //
    if( isonum_733( find.CD.cent->size ) != 0x800 )
      return FALSE;
    //
    if( isonum_711( find.CD.cent->ext_attr_length ) != 0 )
      return FALSE;
    //
    if( !GetISOFileName( find.CD.cent, file->flName, ext ) )
      return FALSE;
    //
    // Check if Directory
    //
    if( !(find.CD.cent->flags[0] & 0x02) )
      return FALSE;
    //
    file->flDet.flStat |= FLS_RDONLY;
    //
    file->flDet.flSize     = 0;
    file->flDet.flStartSec = isonum_733( find.CD.cent->extent ) << 2;
    //
    return TRUE;
  }
  //
  // FAT 16/32
  //
  sclust = (UINT32)find.HD.cent->deStartCluster | ((UINT32)find.HD.cent->deHighClust << 16);
  //
  // Check if file not deleted and archive/directory
  //
  if( (find.HD.cent->deName[0] == SLOT_DELETED) ||
      (find.HD.cent->deName[0] == SLOT_E5) ||
      (find.HD.cent->deName[0] == 0) ||
      !(find.HD.cent->deAttributes & ATTR_DIRECTORY) )
    return FALSE;
  //
  // Check file size
  //
  if( find.HD.cent->deFileSize != 0 )
    return FALSE;
  //
  file->flDet.flSize     = 0;
  file->flDet.flStartSec = ClustToSec( cDrive, sclust );
  memcpy( file->flName, find.HD.cent->deName, 8 );

  return TRUE;
}

//----------------------------------------------------------------
// Function :   FATFS_IsS2IDir
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL FATFS_IsS2IDir( T_file * dir )
{
  T_file     file;
  T_findfl   curFind;
  UINT32     curDSec;
  BOOL       stat = FALSE;

  curFind = find;
  curDSec = cDrive.cwdirStartSec;
  cDrive.cwdirStartSec = dir->flDet.flStartSec;
  //
  if( FindFileNameF( &file, cfgFileName ) )
  {
    if( file.flDet.flSize == 0 )
    {
      dir->flDet.flStat |= FLS_S2IDIR;
      stat = TRUE;
    }
  }
  cDrive.cwdirStartSec = curDSec;
  find = curFind;
  return stat;
}

//
// Interface
//

//----------------------------------------------------------------
// Function :   FATFS_GetFirstDir
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL FATFS_GetFirstDir( T_file * dir )
{
  if( GetFirstFile_Virtual( dir, TestDir ) )
  {
    dir->flDet.flStat |= FLS_DIROK;
    FATFS_IsS2IDir( dir );
    return TRUE;
  }
  memset( (UINT8*)dir, 0x00, sizeof(T_file) );
  //
  return TRUE;
}

//----------------------------------------------------------------
// Function :   FATFS_GetNextDir
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL FATFS_GetNextDir( T_file * dir )
{
  T_file  file;

  if( GetNextFile_Virtual( dir, TestDir ) )
  {
    dir->flDet.flStat |= FLS_DIROK;
    FATFS_IsS2IDir( dir );
    return TRUE;
  }
  memset( (UINT8*)dir, 0x00, sizeof(T_file) );
  //
  return TRUE;
}

//----------------------------------------------------------------
// Function :   FATFS_SetCurDir
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL FATFS_SetCurDir( T_file * dir )
{
  if( dir->flDet.flStat & FLS_DIROK )
  {
    cDrive.cwdirStartSec = dir->flDet.flStartSec;
    return TRUE;
  }
  //
  return FALSE;
}

//----------------------------------------------------------------
// Function :   FATFS_GetCurDir
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL FATFS_GetCurDir( T_file * dir )
{
  T_file  file;

  memset( (UINT8*)dir, 0x00, sizeof(T_file) );
  //
  if( cDrive.cwdirStartSec == cDrive.rdirStartSec )
  {
    memcpyf( dir->flName, rdirName, 8 );
  }
  else
  {
    memcpyf( dir->flName, sdirName, 8 );
  }
  //
  dir->flDet.flStartSec  = cDrive.cwdirStartSec;
  dir->flDet.flStat     |= FLS_DIROK;
  //
  FATFS_IsS2IDir( dir );
  return TRUE;
}

//----------------------------------------------------------------
// Function :   FATFS_SaveCurDir
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL FATFS_SaveCurDir( VOID )
{
  SETSAVELOC( IDE_GetDrv(), cDrive.cwdirStartSec );
  return EE_PutSavedDat( (UINT8*)saveLoc, sizeof(saveLoc), SAVE_POS1 );
}

//----------------------------------------------------------------
// Function :   FATFS_GetFirstFile
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL FATFS_GetFirstFile( T_file * file )
{
  if( GetFirstFile_Virtual( file, TestATRFile ) )
  {
    do
    {
      if( TestFileStruct( &file->flDet ) )
      {
        if( memcmpf( file->flName, cfgFileName, 8 ) != 0 )
        {
          file->flDet.flStat |= FLS_FILEOK;
          return TRUE;
        }
      }
    }while( GetNextFile_Virtual( file, TestATRFile ) );
  }
  memset( (UINT8*)file, 0x00, sizeof(T_file) );
  //
  return TRUE;
}

//----------------------------------------------------------------
// Function :   FATFS_GetNextFile
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL FATFS_GetNextFile( T_file * file )
{
  while( GetNextFile_Virtual( file, TestATRFile ) )
  {
    if( TestFileStruct( &file->flDet ) )
    {
      if( memcmpf( file->flName, cfgFileName, 8 ) != 0 )
      {
        file->flDet.flStat |= FLS_FILEOK;
        return TRUE;
      }
    }
  }
  memset( (UINT8*)file, 0x00, sizeof(T_file) );
  //
  return TRUE;
}

//----------------------------------------------------------------
// Function :   FATFS_DiskGet
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL FATFS_DiskGet( UINT8 id, T_file * disk )
{
  memset( (UINT8*)disk, 0, sizeof(T_file) );
  //
  if( (id > 0) && (id <= MAX_FL_ID) )
  {
    if( diskTab[ CFG_FL_ID ].flStat == FLS_CFGFILE )
    {
      if( (id == COM_FL_ID) && (cDrive.cwdirStartSec != cDrive.rdirStartSec) )
      {
        if( diskTab[ id ].flStat & FLS_FILEOK )
        {
          memcpy( &disk->flDet, &diskTab[ COM_FL_ID ], sizeof(T_fldet) );
          memcpyf( disk->flName, cflName, 8 );
        }
        return TRUE;
      }
      if( diskTab[ id ].flStat & FLS_FILEOK )
      {
        if( FindFileSect( disk, diskTab[ id ].flStartSec ) )
        {
          disk->flDet.flStat |= FLS_FILEOK;
        }
      }
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   FATFS_DiskSet
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL FATFS_DiskSet( UINT8 id, T_file * disk )
{
  T_file     file;
  UINT32     dsec;
  UINT8      pos;

  if( (id > 0) && (id <= MAX_FL_ID) )
  {
    if( diskTab[ CFG_FL_ID ].flStat == FLS_CFGFILE )
    {
      if( (id != COM_FL_ID) ||
          ((id == COM_FL_ID) && (cDrive.cwdirStartSec == cDrive.rdirStartSec))
        )
      {
        if( FindFileSect( &file, disk->flDet.flStartSec ) )
        {
          dsec = find.HD.csect;
          //
          if( TestFileStruct( &file.flDet ) )
          {
            if( cDrive.prtType == ISO9660 )
            {
              //
              diskTab[ id ]         = file.flDet;
              diskTab[ id ].flStat |= FLS_FILEOK;
              diskTab[ id ].flStat |= FLS_RDONLY;
              //
              for( pos = 1; pos <= MAX_FL_ID; pos++ )
              {
                if( (diskTab[ pos ].flStartSec == diskTab[ id ].flStartSec) &&
                    (pos != id) )
                {
                  diskTab[ pos ] = diskTab[ id ];
                }
              }
              return TRUE;
            }
            //
            if( !SectorRead( dsec ) )
              return FALSE;
            //
            if( disk->flDet.flStat & FLS_RDONLY )
            {
              find.HD.cent->deAttributes |= ATTR_READONLY;     // Read Only
            }
            else
            {
              find.HD.cent->deAttributes &= ~ATTR_READONLY;    // Normal
            }
            //
            if( !SectorWrite( dsec ) )
              return FALSE;
            //
            diskTab[ id ]         = file.flDet;
            diskTab[ id ].flStat |= FLS_FILEOK;
            //
            if( disk->flDet.flStat & FLS_RDONLY )
            {
              diskTab[ id ].flStat    |= FLS_RDONLY;
            }
            else
            {
              diskTab[ id ].flStat    &= ~FLS_RDONLY;
            }
            //
            for( pos = 1; pos <= MAX_FL_ID; pos++ )
            {
              if( (diskTab[ pos ].flStartSec == diskTab[ id ].flStartSec) &&
                  (pos != id) )
              {
                diskTab[ pos ] = diskTab[ id ];
              }
            }
            //
            if( id == COM_FL_ID )
            {
              EE_PutSavedDat( (UINT8*)&diskTab[ id ], sizeof(T_fldet), SAVE_POS2 );
            }
            //
            return TRUE;
          }
        }
      }
      else
      {
        return TRUE;    // Common Disk in subdir (nop action)
      }
    }
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   FATFS_DiskOff
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL FATFS_DiskOff( UINT8 id )
{
  if( (id > 0) && (id <= MAX_FL_ID) )
  {
    if( diskTab[ CFG_FL_ID ].flStat == FLS_CFGFILE )
    {
      if( (id != COM_FL_ID) ||
          ((id == COM_FL_ID) && (cDrive.cwdirStartSec == cDrive.rdirStartSec))
        )
      {
        memset( &diskTab[ id ], 0, sizeof(T_fldet) );
        //
        if( id == COM_FL_ID )
        {
          EE_PutSavedDat( (UINT8*)&diskTab[ id ], sizeof(T_fldet), SAVE_POS2 );
        }
        //
        return TRUE;
      }
    }
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   FATFS_DiskCheck
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL FATFS_DiskCheck( UINT8 id )
{
  if( (id > 0) && (id <= MAX_FL_ID) )
  {
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   FATFS_DiskSecSize
// Notes    :
// History  :
//----------------------------------------------------------------

UINT16 FATFS_DiskSecSize( UINT8 id )
{
  if( (id > 0) && (id <= MAX_FL_ID) )
  {
    if( diskTab[ id ].flStat & FLS_FILEOK )
    {
      return diskTab[ id ].flStat & FLS_SEC256B ? 256 : 128;
    }
  }
  return 0;
}

//----------------------------------------------------------------
// Function :   FATFS_DiskSize
// Notes    :
// History  :
//----------------------------------------------------------------

UINT16 FATFS_DiskSize( UINT8 id )
{
  if( (id > 0) && (id <= MAX_FL_ID) )
  {
    if( diskTab[ id ].flStat & FLS_FILEOK )
    {
      return diskTab[ id ].flSize;
    }
  }
  return 0;
}

//----------------------------------------------------------------
// Function :   FATFS_SaveConfig
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL FATFS_SaveConfig( VOID )
{
  T_file           file;
  UINT16           size;
  UINT32           cfgsec;
  UINT8            id;
  //
  // ISO9660
  //
  if( cDrive.prtType == ISO9660 )
  {
    return FALSE;
  }
  //
  // HD FAT 16/32
  //
  size = (FATFS_CountDisks() * (3 + 8 + 2));
  //
  if( FindFileNameF( &file, cfgFileName ) )
  {
    //
    // New file size
    //
    find.HD.cent->deFileSize = size;
    if( !SectorWrite( find.HD.csect ) )
      return FALSE;
    //
    // new file data
    //
    cfgsec = file.flDet.flStartSec;
    size = 0;
    for( id = 1; id <= MAX_FL_ID; id++ )
    {
      if( (id != COM_FL_ID) ||
          ((id == COM_FL_ID) && (cDrive.cwdirStartSec == cDrive.rdirStartSec))
        )
      {
        //
        if( diskTab[ id ].flStat & FLS_FILEOK )
        {
          if( FindFileSect( &file, diskTab[ id ].flStartSec ) )
          {
            //
            if( !SectorRead( cfgsec ) )
              return FALSE;
            //
            secBuff[size++] = 'D';
            secBuff[size++] = id + 0x30;
            secBuff[size++] = '=';
            memcpy( &secBuff[size], file.flName, 8 );
            size += 8;
            secBuff[size++] = '\r';
            secBuff[size++] = '\n';
            secBuff[size]   = 0;

#ifdef DEBUG_FAT
            Printf( "\nSvd: %s", &secBuff[size-12] );
#endif
            //
            if( !SectorWrite( cfgsec ) )
              return FALSE;
          }
        }
      }
    }
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   FATFS_GetDrvInfo
// Notes    :
// History  :
//----------------------------------------------------------------

VOID    FATFS_GetDrvInfo( T_drvinf * buf )
{
  *buf = drvInf;
}

//----------------------------------------------------------------
// Function :   FATFS_GetFsInfo
// Notes    :
// History  :
//----------------------------------------------------------------

VOID    FATFS_GetFsInfo( T_fsinf * buf )
{
  //
  buf->prtType   = cDrive.prtType;      // Partition Type
  buf->secPClust = cDrive.secPClust;    // Sectors Per Cluster
  buf->curFiles  = FATFS_CountDisks();  //
  buf->swVer     = __SIO2IDE_VER__;     // Software version
}

//----------------------------------------------------------------
// Function :   FATFS_GetLFN
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL FATFS_GetLFN( UINT32 sect, UINT8 * lfn )
{
  T_winentry     * went;
  UINT8          * ptr = lfn;
  UINT8            pos;

  memset( lfn, 0, LFN_MAX_SIZE );
  //
  // ISO9660
  //
  if( cDrive.prtType == ISO9660 )
  {
    return FALSE;
  }
  //
  // FAT16 FAT32
  //
  if( FindFileSect( (T_file*)lfn, sect ) )
  {
    memset( lfn, 0, LFN_MAX_SIZE );
    //
    if( (find.HD.cent->deName[5] == '~')
        ||
        (find.HD.cent->deName[6] == '~')
      )
    {
      went = (T_winentry*)find.HD.cent;
      pos  = 0;
      //
      do
      {
        if( (--find.HD.cpos) > 0 )
        {
          went--;
        }
        else
        {
          // Previous DIR sector
          if( !SectorRead( find.HD.psect ) )
            return FALSE;
          //
          find.HD.cpos = (SECTOR_SIZE / sizeof(T_winentry)) - 1;
          went = (T_winentry*)secBuff + find.HD.cpos;
        }
        if( went->weAttributes == ATTR_WIN95 )
        {
          pos   += WIN_CHARS;
          if( pos > LFN_MAX_SIZE )
          {
            goto lfn_error;
          }
          //
          *lfn++ = (went->wePart1[1] == 0) ? went->wePart1[0] : ' ';
          *lfn++ = (went->wePart1[3] == 0) ? went->wePart1[2] : ' ';
          *lfn++ = (went->wePart1[5] == 0) ? went->wePart1[4] : ' ';
          *lfn++ = (went->wePart1[7] == 0) ? went->wePart1[6] : ' ';
          *lfn++ = (went->wePart1[9] == 0) ? went->wePart1[8] : ' ';
          //
          *lfn++ = (went->wePart2[1] == 0) ? went->wePart2[0] : ' ';
          *lfn++ = (went->wePart2[3] == 0) ? went->wePart2[2] : ' ';
          *lfn++ = (went->wePart2[5] == 0) ? went->wePart2[4] : ' ';
          *lfn++ = (went->wePart2[7] == 0) ? went->wePart2[6] : ' ';
          *lfn++ = (went->wePart2[9] == 0) ? went->wePart2[8] : ' ';
          *lfn++ = (went->wePart2[11] == 0) ? went->wePart2[10] : ' ';
          //
          *lfn++ = (went->wePart3[1] == 0) ? went->wePart3[0] : ' ';
          *lfn++ = (went->wePart3[3] == 0) ? went->wePart3[2] : ' ';
          *lfn   = 0;
          //
        }
        else
        {
          goto lfn_error;
        }
      }while( went->weCnt < WIN_LAST );
      //
      return TRUE;
    }
    //
  lfn_error:
    if( !SectorRead( find.HD.csect ) )
      return FALSE;
    //
    memcpy( ptr, find.HD.cent->deName, 8 );
    ptr[8] = '.';
    memcpy( (ptr + 9), find.HD.cent->deExtension, 3 );
    ptr[12] = 0;
    //
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   FATFS_GetFileSec
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL FATFS_GetFileSec( UINT8 disk, UINT16 sec, UINT8 * buf )
{
  UINT32    snum;
  UINT16    offs;
  UINT16    sps;
  UINT16    ssize;

  if( (disk > 0) && (disk <= MAX_FL_ID) )
  {
    if( (diskTab[ disk ].flStat & FLS_FILEOK) &&
        (diskTab[ disk ].flSize >= sec) &&
        (sec > 0)
      )
    {
      ssize = diskTab[ disk ].flStat & FLS_SEC256B ? 256 : 128;
      //
      if( (ssize == 128) || (sec >= 1 && sec <= 3) )
      {
        //
        // 128B sectors & Header (sectors: 1,2,3)
        //
        sps   = SECTOR_SIZE / 128;
        snum  = GetDiskSector( &diskTab[ disk ], diskTab[ disk ].flStartSec, (((UINT32)sec - 1) / (UINT32)sps) );
        if( snum == 0 )
          return FALSE;
        //
        if( (sec % sps) != 0 )
        {
          if( !SectorRead( snum ) )
            return FALSE;
          offs = FL_DATA_OFFS + (((sec % sps) - 1) * 128 );
          memcpy( buf, (UINT8*)(secBuff + offs), 128 );
        }
        else
        {
          if( !SectorRead( snum ) )
            return FALSE;
          offs = SECTOR_SIZE - (128 - FL_DATA_OFFS);
          memcpy( buf, (UINT8*)(secBuff + offs), (128 - FL_DATA_OFFS) );
          //
          snum  = GetDiskSector( &diskTab[ disk ], snum, 1 );
          if( snum == 0 )
            return FALSE;
          //
          if( !SectorRead( snum ) )
            return FALSE;
          offs = (128 - FL_DATA_OFFS);
          memcpy( (UINT8*)(buf + offs), (UINT8*)secBuff, FL_DATA_OFFS );
        }
      }
      else if( ssize == 256 )
      {
        //
        // 256B sectors (sectors >= 4)
        //
        sps   = SECTOR_SIZE / 256;
        snum  = GetDiskSector( &diskTab[ disk ], diskTab[ disk ].flStartSec, (((UINT32)sec - 3) / (UINT32)sps) );
        if( snum == 0 )
          return FALSE;
        //
        if( (sec % sps) != 0 )
        {
          if( !SectorRead( snum ) )
            return FALSE;
          offs = FL_DATA_OFFS + 128;
          memcpy( buf, (UINT8*)(secBuff + offs), 256 );
        }
        else
        {
          if( !SectorRead( snum ) )
            return FALSE;
          offs = SECTOR_SIZE - (128 - FL_DATA_OFFS);
          memcpy( buf, (UINT8*)(secBuff + offs), (128 - FL_DATA_OFFS) );
          //
          snum  = GetDiskSector( &diskTab[ disk ], snum ,1 );
          if( snum == 0 )
            return FALSE;
          //
          if( !SectorRead( snum ) )
            return FALSE;
          offs = (128 - FL_DATA_OFFS);
          memcpy( (UINT8*)(buf + offs), (UINT8*)secBuff, (128 + FL_DATA_OFFS) );
        }
      }
      //
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   FATFS_PutFileSec
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL FATFS_PutFileSec( UINT8 disk, UINT16 sec, UINT8 * buf )
{
  UINT32    snum;
  UINT16    offs;
  UINT16    sps;
  UINT16    ssize;
  //
  // ISO9660
  //
  if( cDrive.prtType == ISO9660 )
  {
    return FALSE;
  }
  //
  // HD FAT 16/32
  //
  if( (disk > 0) && (disk <= MAX_FL_ID) )
  {
    if( (diskTab[ disk ].flStat & FLS_FILEOK) &&
        !(diskTab[ disk ].flStat & FLS_RDONLY) &&
        (diskTab[ disk ].flSize >= sec) &&
        (sec > 0)
      )
    {
      ssize = diskTab[ disk ].flStat & FLS_SEC256B ? 256 : 128;
      //
      if( (ssize == 128) || (sec >= 1 && sec <= 3) )
      {
        //
        // 128B sectors & Header (sectors: 1,2,3)
        //
        sps   = SECTOR_SIZE / 128;
        snum  = GetDiskSector( &diskTab[ disk ], diskTab[ disk ].flStartSec, (((UINT32)sec - 1) / (UINT32)sps) );
        if( snum == 0 )
          return FALSE;
        //
        if( (sec % sps) != 0 )
        {
          if( !SectorRead( snum ) )
            return FALSE;
          offs = FL_DATA_OFFS + (((sec % sps) - 1) * 128 );
          memcpy( (UINT8*)(secBuff + offs), buf, 128 );
          if( !SectorWrite( snum ) )
            return FALSE;
        }
        else
        {
          if( !SectorRead( snum ) )
            return FALSE;
          offs = SECTOR_SIZE - (128 - FL_DATA_OFFS);
          memcpy( (UINT8*)(secBuff + offs), buf, (128 - FL_DATA_OFFS) );
          if( !SectorWrite( snum ) )
            return FALSE;
          //
          snum  = GetDiskSector( &diskTab[ disk ], snum, 1 );
          if( snum == 0 )
            return FALSE;
          //
          offs = (128 - FL_DATA_OFFS);
          if( !SectorRead( snum ) )
            return FALSE;
          memcpy( (UINT8*)secBuff, (UINT8*)(buf + offs), FL_DATA_OFFS );
          if( !SectorWrite( snum ) )
            return FALSE;
        }
      }
      else if( ssize == 256 )
      {
        //
        // 256B sectors (sectors >= 4)
        //
        sps   = SECTOR_SIZE / 256;
        snum  = GetDiskSector( &diskTab[ disk ], diskTab[ disk ].flStartSec, (((UINT32)sec - 3) / (UINT32)sps) );
        if( snum == 0 )
          return FALSE;
        //
        if( (sec % sps) != 0 )
        {
          if( !SectorRead( snum ) )
            return FALSE;
          offs = FL_DATA_OFFS + 128;
          memcpy( (UINT8*)(secBuff + offs), buf, 256 );
          if( !SectorWrite( snum ) )
            return FALSE;
        }
        else
        {
          if( !SectorRead( snum ) )
            return FALSE;
          offs = SECTOR_SIZE - (128 - FL_DATA_OFFS);
          memcpy( (UINT8*)(secBuff + offs), buf, (128 - FL_DATA_OFFS) );
          if( !SectorWrite( snum ) )
            return FALSE;
          //
          snum  = GetDiskSector( &diskTab[ disk ], snum, 1 );
          if( snum == 0 )
            return FALSE;
          //
          offs = (128 - FL_DATA_OFFS);
          if( !SectorRead( snum ) )
            return FALSE;
          memcpy( (UINT8*)secBuff, (UINT8*)(buf + offs), (128 + FL_DATA_OFFS) );
          if( !SectorWrite( snum ) )
            return FALSE;
        }
      }
      //
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   FATFS_InitCurDir
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL FATFS_InitCurDir( T_file * file )
{
  UINT8          * tok;
  UINT8          * ptr;
  UINT8            name[8];
  UINT8            id;

#ifdef DEBUG_FAT
  Printf( "\nDIR" );
#endif
  //
  // Find Configuration File
  //
  if( FindFileNameF( file, cfgFileName ) )
  {
    if( file->flDet.flSize == 0 )
    {
      //
      memset( diskTab, 0x00, sizeof( T_disktab ) );
      //
      diskTab[ CFG_FL_ID ] = file->flDet;
      //
      // Read and Check Config (text file)
      //
      if( !SectorRead( diskTab[ CFG_FL_ID ].flStartSec ) )
        return FALSE;
      //
      if( (ptr = strtokf( secBuff, cfgTokenDel, &tok )) != NULL )
      {
        do
        {
          if( (ptr[0] == 'D') && (ptr[2] == '=') )
          {
            id = ptr[1] - 0x30;
            if( (id > CFG_FL_ID) && (id <= MAX_FL_ID) )
            {
              if( (id != COM_FL_ID) ||
                  ((id == COM_FL_ID) && (cDrive.cwdirStartSec == cDrive.rdirStartSec))
                )
              {
                if( strlen( &ptr[3] ) <= 8 )
                {
                  memset( name, ' ', 8 );
                  memcpy( name, &ptr[3], strlen( &ptr[3] ) );
                  //
#ifdef DEBUG_FAT
                  Printf( "\n%8s ", name );
#endif
                  if( FindFileNameR( file, name ) )
                  {
                    if( TestFileStruct( &file->flDet ) )
                    {
#ifdef DEBUG_FAT
                      Printf( " D=%d OK", id );
                      if( file->flDet.flStat & FLS_NOTDEFR )
                      {
                        Printf( " (NOT defragmented)" );
                      }
#endif
                      file->flDet.flStat |= FLS_FILEOK;
                    }
                    diskTab[ id ] = file->flDet;
                  }
                }
              }
            }
          }
          //
          if( !SectorRead( diskTab[ CFG_FL_ID ].flStartSec ) )
            return FALSE;
          //
        }while( (ptr = strtokf( NULL, cfgTokenDel, &tok )) != NULL );
#ifndef DEBUG_DISKFS
        //
        // Set-up Common disk (D1<>D9)
        //
        if( (diskTab[ COM_FL_ID ].flStat & FLS_FILEOK) &&
            (cDrive.cwdirStartSec == cDrive.rdirStartSec)
          )
        {
          EE_PutSavedDat( (UINT8*)&diskTab[ COM_FL_ID ], sizeof(T_fldet), SAVE_POS2 );
#ifdef DEBUG_FAT
          Printf( "\nD9: saved" );
#endif
        }
        else
        {
#ifdef DEBUG_FAT
          Printf( "\nD9: restored" );
#endif
          memset( &diskTab[ COM_FL_ID ], 0x00, sizeof(T_fldet) );
          //
          if( EE_GetSavedDat( (UINT8*)&diskTab[ COM_FL_ID ], sizeof(T_fldet), SAVE_POS2 ) )
          {
            if( !TestFileStruct( &diskTab[ COM_FL_ID ] ) )
            {
#ifdef DEBUG_FAT
              Printf( " ERR" );
#endif
              // Wrong File
              //
              memset( &diskTab[ COM_FL_ID ], 0x00, sizeof(T_fldet) );
              EE_PutSavedDat( (UINT8*)&diskTab[ COM_FL_ID ], sizeof(T_fldet), SAVE_POS2 );
            }
#ifdef DEBUG_FAT
            else
              Printf( " OK" );
#endif
          }
#ifdef DEBUG_FAT
          else
            Printf( " NO data" );
#endif
        }
#endif
//        diskTab[ COM_FL_ID ].flStat |= FLS_RDONLY;      // Read Only disk

#ifdef DEBUG_DISKFS
        Printf( "\n-----------------------------------------------------------------------------" );
        Printf( "\nDisk      File          Size (secs)   R/W    Status" );
        Printf( "\n-----------------------------------------------------------------------------" );

        for( id = 1; id <= MAX_FL_ID; id++ )
        {
          FindFileSect( file, diskTab[ id ].flStartSec );

          if( id == COM_FL_ID )
            Printf( "\nD1:  " );
          else
            Printf( "\nD%d:  ", id );
          if( diskTab[ id ].flStat == 0x00 )
          {
            Printf( "     --------.---                       Disk NOT used." );
          }
          else if( diskTab[ id ].flStat & FLS_FILEOK )
          {
            Printf( "<<<  %.8s.ATR  %05u x %sB  %s     Disk OK.",
                    file->flName,
                    diskTab[ id ].flSize,
                    diskTab[ id ].flStat & FLS_SEC256B ? "256" : "128",
                    diskTab[ id ].flStat & FLS_RDONLY ? "RO" : "RW" );
            if( diskTab[ id ].flStat & FLS_NOTDEFR )
            {
              Printf( " (NOT defragmented)" );
            }
          }
          else if( diskTab[ id ].flStat & FLS_HDRERR )
          {
            Printf( "     %.8s.ATR  ", file->flName );
            Printf( "                     Wrong ATR file header." );
          }
          else
          {
            Printf( "     %.8s.ATR  ", file->flName );
            Printf( "                     File NOT found or damaged." );
          }
        }
        Printf( "\n-----------------------------------------------------------------------------" );
        Printf( "\nAvailable for ATARI = %u disk(s)" , FATFS_CountDisks() );
#endif
#ifdef DEBUG_FAT
        Printf( "\n4" );
        Printf( "\nFiles = %i disk(s)" , FATFS_CountDisks() );
#endif
        //
        return TRUE;
      }
    }
#ifdef DEBUG_DISKFS
    Printf( " SIO2IDE config file is corrupted." );
#endif
#ifdef DEBUG_FAT
    Printf( "\nCFG ERR" );
#endif
  }
  else
  {
#ifdef DEBUG_DISKFS
    Printf( " SIO2IDE config file is missing." );
#endif
#ifdef DEBUG_FAT
    Printf( "\nNO CFG" );
#endif
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   FATFS_Init
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL FATFS_Init( VOID )
{
#ifndef __MSDOS__
  T_file           file;
#endif

  //
  memset( &cDrive, 0x00, sizeof( T_drive ) );
  //
  memset( diskTab, 0x00, sizeof( T_disktab ) );
  //
  // Initialise IDE interface
  //
  switch( IDE_Init( &drvInf ) )
  {
    case IDE_HD:
      //
      // Get HD Drive Parameters
      //
      if( !GetHDDriveParams( FALSE ) )
      {
      #ifdef DEBUG_DISKFS
        Printf( "\nERROR: HD Drive can not be initialised." );
      #endif
        return FALSE;
      }
      break;
    case IDE_CD:
      //
      // Get CD ROM Drive Parameters
      //
      if( !GetCDDriveParams() )
      {
      #ifdef DEBUG_DISKFS
        Printf( "\nERROR: CD Drive can not be initialised." );
      #endif
        return FALSE;
      }
      break;
    case IDE_NONE:
    default:
    #ifdef DEBUG_DISKFS
      Printf( "\nERROR: This Disk can not be initialised (Non data disk)." );
    #endif
      return FALSE;
  };
#ifdef __MSDOS__
  return TRUE;
#else
  //
  // Initialise Saved directory (if any)
  //
  if( EE_GetSavedDat( (UINT8*)saveLoc, sizeof(saveLoc), SAVE_POS1 ) )
  {
    cDrive.cwdirStartSec = GETSAVELOC( IDE_GetDrv() );
    //
#ifdef DEBUG_FAT
    Printf( "\nIS" );
#endif
    if( FATFS_InitCurDir( &file ) )
    {
      return TRUE;
    }
  }
  //
  // Initialise ROOT directory
  //
#ifdef DEBUG_FAT
  Printf( "\nIR" );
#endif
  cDrive.cwdirStartSec = cDrive.rdirStartSec;
  FATFS_SaveCurDir();
  //
  if( FATFS_InitCurDir( &file ) )
  {
    return TRUE;
  }
  return FALSE;
#endif
}

//----------------------------------------------------------------
// Function :   FATFS_Test
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL FATFS_Test( VOID )
{
  T_file         file;
  UINT8          name[12];
  UINT8          i;
  //
  Printf( "\nTest start (please wait)" );
  //
  memset( &cDrive, 0x00, sizeof( T_drive ) );
  //
  memset( diskTab, 0x00, sizeof( T_disktab ) );
  //
  // Initialise IDE interface
  //
  switch( IDE_Init( &drvInf ) )
  {
    case IDE_HD:
      Printf( "\nHDD parameters:" );
      if( drvInf.flg & DRV_LBA )
      {
        Printf( "\n  Mode      = LBA " );
        Printf( "\n  Sectors   = %l", drvInf.sec );
      }
      else
      {
        Printf( "\n  Mode      = CHS" );
        Printf( "\n  Cylinders = %i", drvInf.cyl );
        Printf( "\n  Heads     = %i", drvInf.hd );
        Printf( "\n  Sec/Track = %i", drvInf.spt );
      }
      //
      // Get HD Drive Parameters
      //
      if( !GetHDDriveParams( TRUE ) )
      {
        Printf( "\nHD Drive can not be initialised. Possibly:" );
        Printf( "\n  - No MBR" );
        Printf( "\n  - Wrong partition type" );
        goto fat_err;
      }
      Printf( "\nHD Drive initialised OK." );
      break;
    case IDE_CD:
      //
      // Get CD ROM Drive Parameters
      //
      if( !GetCDDriveParams() )
      {
        Printf( "\nCD Drive can not be initialised." );
        goto fat_err;
      }
      Printf( "\nCD Drive initialised OK." );
      break;
    case IDE_NONE:
    default:
      Printf( "\nThis Disk can not be initialised." );
      Printf( "\nWrong disk format or disk not attached." );
      goto fat_err;
  };
  //
  // Initialise Saved directory (if any)
  //
  if( EE_GetSavedDat( (UINT8*)&cDrive.cwdirStartSec, sizeof(UINT32), SAVE_POS1 ) )
  {
    Printf( "\nTry to Init Saved Dir:" );
    if( FATFS_InitCurDir( &file ) )
    {
      Printf( "\n  Saved Dir initialised OK" );
      goto fat_ok;
    }
    Printf( "\n  Saved Dir can not be initialised." );
    Printf( "\n  SIO2IDE.CFG not found." );
  }
  EE_PutSavedDat( (UINT8*)&cDrive.rdirStartSec, sizeof(UINT32), SAVE_POS1 );
  //
  // Initialise ROOT directory
  //
  Printf( "\nTry to Init Root Dir:" );
  //
  cDrive.cwdirStartSec = cDrive.rdirStartSec;
  if( FATFS_InitCurDir( &file ) )
  {
    Printf( "\n  Root Dir initialised OK" );
    goto fat_ok;
  }
  Printf( "\n  Root Dir can not be initialised." );
  Printf( "\n  SIO2IDE.CFG not found." );
//
fat_err:
  //
  Printf( "\nTest ERROR" );
  return FALSE;
  //
fat_ok:
  //
  Printf( "\nDisks available for ATARI:" );
  //
  for( i = 1; i <= MAX_FL_ID; i++ )
  {
    if( FATFS_DiskGet( i, &file ) )
    {
      if( file.flDet.flStat & FLS_FILEOK )
      {
        memset( name, 0, 12 );
        memcpy( name, file.flName, 8 );
        Printf( "\n  D%d: > %s.ATR", i, name );
      }
      else
      {
        Printf( "\n  D%d: > --------.---", i );
      }
    }
  }
  Printf( "\nTest OK" );
  return TRUE;
}

//      End
