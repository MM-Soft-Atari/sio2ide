//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: CDFS.C
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
//              001   22-02-2002 MM     Created
//
//****************************************************************

#include "apps.h"
#include "cdfs.h"

#ifdef __MSDOS__
  #ifdef DEBUG
    #define DEBUG_DISKFS
    #include <bios.h>
    #include <stdio.h>
    #include <ctype.h>
  #endif
#else
  #ifdef DEBUG
    //#define         DEBUG_CD
  #endif
#endif

#define FL_DATA_OFFS    (sizeof(T_atrhdr))

STATIC UINT8   secBuff[2048];
STATIC DATA  T_drvinf   drvInf;         // Drive phisical info

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
#endif
    if( !IDE_SectorGet( (UINT16 *)secBuff, &drvInf, sector ) )
      return FALSE;
    //
    lsect = sector;
  }
  return TRUE;
}

//----------------------------------------------------------------
// Function :   SectorWrite
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL SectorWrite( UINT32 sector )
{
  return FALSE;
}

STATIC   UINT8      name[128];
STATIC   UINT8      dep = 0;

STATIC VOID ShowDir( T_ISO_DIR_REC * cdir )
{
  UINT32              csec;
  UINT8               len;
  UINT8             * ptr;
  T_ISO_DIR_REC     * dir;

  csec = isonum_733( cdir->extent ) << 2;
  SectorRead( csec );
  ptr = secBuff;
  do
  {
    dir = (T_ISO_DIR_REC*)ptr;
    if( (len = isonum_711( dir->name_len )) > 1 )
    {
        memset( name, 0, 128 );
        memset( name, ' ' , dep );
        memcpy( &name[dep], dir->name, len );
        printf( "\n%s", name );

      if( dir->flags[0] & 0x02 )
      {
        //
        if( dep < 0 )
        {
          dep += 4;
          ShowDir( dir );
          dep -= 4;
          SectorRead( csec );
        }
      }
      else
      {
//        printf( "\nfile" );
//        SectorRead( isonum_733( dir->extent ) << 2 );
      }
    }
    ptr += isonum_711( dir->length );
  }while( isonum_711( dir->length ) );
}

//----------------------------------------------------------------
// Function :   CDFS_Init
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL CDFS_Init( VOID )
{
  T_ISO_PRIM_DESC   * pdesc;
  T_ISO_SUP_DESC    * sdesc;
  T_ISO_DIR_REC     * rdir;

  //
  // Initialise IDE interface
  //
  if( !IDE_Init( &drvInf ) )
  {
    return FALSE;
  }

  //
  // Read Primary Volume Descriptor
  //
  SectorRead( 0x10 << 2 );

  pdesc = (T_ISO_PRIM_DESC*)secBuff;
  sdesc = (T_ISO_SUP_DESC*)secBuff;

  if( isonum_711( pdesc->type ) == ISO_VD_PRIMARY )
  {
    if( memcmp( pdesc->id, ISO_STANDARD_ID, 5 ) == 0 )
    {
      rdir = (T_ISO_DIR_REC*)&pdesc->root_directory_record;

      ShowDir( rdir );

      return TRUE;
    }
  }
  return FALSE;
}

//      End
