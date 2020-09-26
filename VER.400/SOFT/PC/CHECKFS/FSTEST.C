//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: FSTEST.C
//
// MODULE NAME: FSTEST
//
// PURPOSE:     CHECKFS main module.
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include <platform.h>
#include <cfg.h>
#include "ide.h"
#include "cdfs.h"
#include "fatfs.h"

EXTERN VOID       GetFindStruct ( T_findfl * fnd );
EXTERN VOID       PutFindStruct ( T_findfl * fnd );

STATIC UINT8      buf[512];
STATIC UINT8      level = 2;
STATIC FILE     * outf;

//----------------------------------------------------------------
// Function :   Printf
// Notes    :
// History  :
//----------------------------------------------------------------

UINT16 Printf( UINT8 FLASH * format, ... )
{
  va_list        ap;
  UINT16         nr_of_chars;

  if( outf )
  {
    va_start (ap, format);      /* Variable argument begin */
    nr_of_chars = vfprintf( outf, format, ap );
    va_end (ap);                /* Variable argument end */
  }
  va_start (ap, format);      /* Variable argument begin */
  nr_of_chars = vprintf( format, ap );
  va_end (ap);                /* Variable argument end */

  return (nr_of_chars);       /* According to ANSI */
}

//----------------------------------------------------------------
// Function :   DBG_Open
// Notes    :
// History  :
//----------------------------------------------------------------

VOID DBG_Open( VOID )
{
  outf = fopen( "checkfs.log", "wt" );
}

//----------------------------------------------------------------
// Function :   DBG_Close
// Notes    :
// History  :
//----------------------------------------------------------------

VOID DBG_Close( VOID )
{
  fclose( outf );
}

//----------------------------------------------------------------
// Function :   CheckAllDirs
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID CheckAllDirs( T_file  * dir, BOOL sub )
{
  T_findfl  find;

  FATFS_SetCurDir( dir );
  if( FATFS_InitCurDir( dir ) )
  {
    Printf( "\nOK" );
  }
  //
  if( !sub )  return;
  //
  if( FATFS_GetFirstDir( dir ) )
  {
    do
    {
      if( dir->flName[0] == 0 )
      {
        break;
      }
      if( (dir->flDet.flStat & FLS_DIROK)              &&
          (strncmp( dir->flName, ".       ", 8 ) != 0) &&
          (strncmp( dir->flName, "..      ", 8 ) != 0)
        )
      {
        memset( buf, ' ', 128 );
        buf[0] = '\n';
        sprintf( buf+level+1, "%.8s", &dir->flName );
        buf[strlen(buf)] = ' ';
        buf[40] = 0;
        Printf( "%s", buf );

        GetFindStruct( &find );
        level += 2;

        CheckAllDirs( dir, sub );

        PutFindStruct( &find );
        level -= 2;
      }
    }while( FATFS_GetNextDir( dir ) );
  }
}

//----------------------------------------------------------------
// Function :   main
// Notes    :
// History  :
//----------------------------------------------------------------

VOID main( int argc, char *argv[] )
{
  T_file    dir;

  DBG_Open();

  Printf( "\nATARI FAT16/FAT32 filesystem utility (ver. "__SIO2IDE_VER_TXT__")  MMSoft (c) 2004\n" );

  DISK = 0x80;
  if( argc >= 2 )
  {
    if( strcmp( argv[1], "C:" ) == 0 )       DISK = 0x80;
    else if( strcmp( argv[1], "D:" ) == 0 )  DISK = 0x81;
    else if( strcmp( argv[1], "E:" ) == 0 )  DISK = 0x82;
    else if( strcmp( argv[1], "F:" ) == 0 )  DISK = 0x83;
    else if( strcmp( argv[1], "G:" ) == 0 )  DISK = 0x84;
    else if( strcmp( argv[1], "H:" ) == 0 )  DISK = 0x85;
    else
    {
      printf( "\nWrong drive specified." );
      printf( "\nUsage: FSTEST.EXE <drive> [S]" );
      printf( "\n       drive - (C: - H:)" );
      printf( "\n       S     - scan subdirectories" );
      printf( "\n" );
      goto exit;
    }
  }
  else
  {
    printf( "\nPlease specify drive." );
    printf( "\nUsage: FSTEST.EXE <drive> [S]" );
    printf( "\n       drive - (C: - H:)" );
    printf( "\n       S     - scan subdirectories" );
    printf( "\n" );
    goto exit;
  }

  Printf( "\n*****************************************************************************" );
  if( FATFS_Init() )
  {
    Printf( "\n*****************************************************************************" );
    memset( buf, ' ', 128 );
    sprintf( buf, "\n%s\\", argv[1] );
    buf[strlen(buf)] = ' ';
    buf[40] = 0;
    Printf( "%s", buf );
    //
    FATFS_GetCurDir( &dir );
    //
    if( strcmp( argv[2], "S" ) == 0 )
    {
      CheckAllDirs( &dir, TRUE );
    }
    else
    {
      CheckAllDirs( &dir, FALSE );
/*
      FATFS_DiskGet( 3, &dir );
      FATFS_GetLFN( dir.flDet.flStartSec, buf );
      Printf( "\n%s", buf );
*/
/*
      FATFS_GetFileSec( 1, 1, buf );
      FATFS_GetFileSec( 1, 2, buf );
      FATFS_GetFileSec( 1, 3, buf );
      FATFS_GetFileSec( 1, 4, buf );
      FATFS_GetFileSec( 1, 5, buf );
      FATFS_GetFileSec( 1, 65533, buf );
      FATFS_GetFileSec( 1, 65534, buf );
      FATFS_GetFileSec( 1, 65535, buf );
*/
    }
  }
  Printf( "\n*****************************************************************************" );

  Printf( "\nDisk accessed %u times.", acc );

exit:
  Printf( "\nFinished\n" );

  DBG_Close();
}

//      End
