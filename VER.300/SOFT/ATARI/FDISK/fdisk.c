//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: FDISK.C
//
// MODULE NAME: FDISK
//
// PURPOSE:
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001    6-03-2002 MM     Created
//
//****************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>

#include "plat.h"

#pragma codeseg ("CODE1")

#define EMPTY_STR       "--------"      // 8 characters for Empty Disk Slot

static UINT8            dID = 1;
static BOOL             upd = FALSE;

#ifndef DEBUG
static T_drvinf         drvInfo;
#else
static T_drvinf         drvInfo =
{
  864, 5, 34, 234098, DRV_LBA
};
#endif
#ifndef DEBUG
static T_fsinf          fsInfo;
#else
static T_fsinf          fsInfo =
{
  ISO9660, 4, 2, __SIO2IDE_VER__
};
#endif
#ifndef DEBUG
static T_file           atrTab[ MAX_ATR_ID + 1 ];
#else
static T_file           atrTab[ MAX_ATR_ID + 1 ] =
{
  {{'S','E','S','T','0','0','1',' '}, {125321L, 720, 0xC0}},
  {{'D','W','S','T','0','0','2',' '}, {13621L, 720, 0xC0}},
  {{'G','E','Y','T','0','0','3',' '}, {23221L, 720, 0x84}},
  {{'H','R','U','T','0','0','4',' '}, {43211L, 1024, 0xC0}},
  {{'R','T','O','T','0','1','1',' '}, {123281L, 65535L, 0xC0}},
  {{'Q','Y','S','T','0','1','2',' '}, {13261L, 720, 0xC4}},
  {{'Y','K','S','T','0','1','3',' '}, {23421L, 720, 0x84}},
  {{'T','E','S','T','0','1','4',' '}, {413291L, 1024, 0xC0}},
  {{'T','E','S','T','0','2','1',' '}, {123421L, 720, 0xC0}},
  {{'U','K','D','T','0','2','2',' '}, {130321L, 720, 0xC4}},
  {{'I','S','S','T','0','2','3',' '}, {23321L, 720, 0x84}},
  {{'L','E','S','T','0','2','4',' '}, {41321L, 1024, 0xC4}},
  {{'A','E','S','T','0','3','1',' '}, {512321L, 720, 0xC0}},
  {{'S','A','A','T','0','3','2',' '}, {13621L, 720, 0xC0}},
  {{'Q','D','S','T','0','3','3',' '}, {52321L, 720, 0x80}},
  {{'O','T','S','T','0','3','4',' '}, {40321L, 1024, 0xC4}},
  {{'O','R','S','T','0','4','1',' '}, {120321L, 720, 0xC4}},
  {{'O','E','D','T','0','4','2',' '}, {13215L, 720, 0xC0}},
  {{'T','E','F','T','0','4','3',' '}, {23281L, 720, 0x80}},
  {{'A','E','S','T','0','4','4',' '}, {43231L, 1024, 0xC0}},
  {{'A','Q','S','T','0','5','1',' '}, {121321L, 720, 0xC4}},
  {{'Z','W','S','T','0','5','2',' '}, {102121L, 720, 0xC4}},
  {{'X','E','S','T','0','5','3',' '}, {7521L, 720, 0x84}},
  {{'C','E','F','T','0','5','4',' '}, {9221L, 1024, 0xC0}},
};
#endif
#ifndef DEBUG
static T_file           diskTab[ MAX_FL_ID ];
#else
static T_file           diskTab[ MAX_FL_ID ] =
{
  {{'O','R','S','T','0','4','1',' '}, {120321L, 720, 0xC4}},
  {{'O','E','D','T','0','4','2',' '}, {13215L, 720, 0xC0}},
};
#endif
#ifndef DEBUG
static T_file           dirTab[ MAX_DIR_ID + 1 ];
#else
static T_file           dirTab[ MAX_DIR_ID + 1 ] =
{
  {{'.','.',' ',' ',' ',' ',' ',' '}, {13621L, 720, 0xC2}},
  {{'G','E','Y','T','0','0','3',' '}, {23221L, 720, 0x82}},
  {{'O','E','S','T','0','0','3',' '}, {28321L, 720, 0x82}},
  {{'H','R','U','T','0','0','4',' '}, {43211L, 1024, 0xC2}},
  {{'R','T','O','T','0','1','1',' '}, {123281L, 65535L, 0xC2}},
  {{'Q','Y','S','T','0','1','2',' '}, {13261L, 720, 0xC2}},
  {{'Y','K','S','T','0','1','3',' '}, {23421L, 720, 0x82}},
  {{'T','E','S','T','0','1','4',' '}, {413291L, 1024, 0xC2}},
  {{'T','E','S','T','0','2','1',' '}, {123421L, 720, 0xC2}},
  {{'U','K','D','T','0','2','2',' '}, {130321L, 720, 0xC2}},
  {{'I','S','S','T','0','2','3',' '}, {23321L, 720, 0x82}},
  {{'L','E','S','T','0','2','4',' '}, {41321L, 1024, 0xC2}},
  {{'A','E','S','T','0','3','1',' '}, {512321L, 720, 0xC2}},
  {{'S','A','A','T','0','3','2',' '}, {13621L, 720, 0xC2}},
  {{'Q','D','S','T','0','3','3',' '}, {52321L, 720, 0x82}},
  {{'O','T','S','T','0','3','4',' '}, {40321L, 1024, 0xC2}},
  {{'O','R','S','T','0','4','1',' '}, {120321L, 720, 0xC2}},
  {{'O','E','D','T','0','4','2',' '}, {13215L, 720, 0xC2}},
  {{'T','E','F','T','0','4','3',' '}, {23281L, 720, 0x82}},
};
#endif
#ifndef DEBUG
static T_file   actDir;
#else
static T_file   actDir = {
  {'C',':','(','R','O','O','T',')'}, {23281L, 720, 0x02}
};
#endif
static T_file   selDir;
static UINT16   maxDirs;
static UINT16   maxFiles;

#ifdef __CART__
       void CardWait    ( void );
#endif
static BOOL InitData    ( UINT8 dev );
static BOOL SaveConfig  ( void );

//----------------------------------------------------------------
// Function :   WaitKey
// Notes    :
// History  :
//----------------------------------------------------------------

static void WaitKey( UINT8 * keys )
{
  UINT8  key;

  for(;;)
  {
    key = 0xFF;
    if( kbhit() )
    {
      key = cgetc();
      if( strchr( keys, key ) )        return;
    }
  }
}

//----------------------------------------------------------------
// Function :   WaitKeyFor
// Notes    :
// History  :
//----------------------------------------------------------------
#ifdef __CART__
static UINT8 WaitKeyFor( UINT8 * title, UINT8 * title1, UINT8 * title2, UINT8 * keys, UINT16 time )
{
  UINT8        key;
  UINT16       i;
  T_progress   prog;

  SCR_DrawWindow( title, 6, 7, 33, 16 );
  //
  SCR_ProgressShow( &prog, 9, 9, 20, (time * 10) );
  //
  gotoxy( 6, 13 );
  SCR_LineCenter( title1, 28 );
  gotoxy( 6, 14 );
  SCR_LineCenter( title2, 28 );

  for( i = 0; i < (time * 10); i++ )
  {
    key = 0xFF;
    if( kbhit() )
    {
      key = cgetc();
      if( strchr( keys, key ) )        return key;
    }
    SCR_ProgressUpdate( &prog, i );
    MENU_Delay( 350 );
  }
  return 0;
}
#endif

//----------------------------------------------------------------
// Function :   WaitAnyKey
// Notes    :
// History  :
//----------------------------------------------------------------
/*
static UINT8 WaitAnyKey( void )
{
  for(;;)
  {
    if( kbhit() )
    {
      return cgetc();
    }
  }
}
*/

//
// OK/Error Button
//

//----------------------------------------------------------------
// Function :   ShowCnfButton
// Notes    :
// History  :
//----------------------------------------------------------------

static void ShowCnfButton( UINT8 * name, UINT8 pos )
{
  static T_menuitem       buttonItm[] =
  {
    {MITM_EXIT, " Press 'Return'", "", NULL},
    {MITM_EMPTY,  "", "", NULL}
  };
  static T_menu           buttonCnf =
  {
    MNU_VERT, "", 10, 12, 28, 14, buttonItm, "", NULL
  };

  buttonCnf.title = name;
  buttonCnf.hy    = pos;
  buttonCnf.ly    = pos + 2;
  MENU_Show( &buttonCnf );
}

//----------------------------------------------------------------
// Function :   ShowYesNoButtons
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL ShowYesNoButtons( UINT8 * name, UINT8 pos )
{
  static T_menuitem       buttonItm[] =
  {
    {MITM_EXIT, " 'Y'es", "Yy", NULL},
    {MITM_EXIT, " 'N'o ", "\033Nn", NULL},
    {MITM_EMPTY,  "", "", NULL}
  };
  static T_menu           buttonCnf =
  {
    MNU_HOR, "", 9, 12, 29, 14, buttonItm, "", NULL
  };

  buttonCnf.title = name;
  buttonCnf.hy    = pos;
  buttonCnf.ly    = pos + 2;
  return (MENU_Show( &buttonCnf ) == 0) ? TRUE : FALSE;
}

//
// Directory list Viewer
//

//----------------------------------------------------------------
// Function :   DirCnt
// Notes    :
// History  :
//----------------------------------------------------------------

static UINT16 DirCnt ( void )
{
  return maxDirs;
}

//----------------------------------------------------------------
// Function :   DirItm
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL DirItm ( UINT8 ** item, UINT16 cnt )
{
  if( cnt < maxDirs )
  {
    *item = dirTab[ cnt ].flName;
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   DirDummySel
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL OnDirSelect ( UINT16 pos )
{
  if( pos < maxDirs )
  {
    if( !SIO_SetCurDir( dID, &dirTab[ pos ] ) )
    {
      ShowCnfButton( "Error", 9 );
      return TRUE;
    }
    selDir = dirTab[ pos ];
  }
  return TRUE;
}

//----------------------------------------------------------------
// Function :   OnDirAction
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL OnDirAction ( UINT16 pos, UINT8 key )
{
  T_file   tmpDir;

  key = key;
  pos = pos;
  //
  if( upd && (fsInfo.prtType != ISO9660) )
  {
    if( ShowYesNoButtons( "Save config ?", 9 ) )
    {
      if( SIO_SaveConfig( dID ) )
      {
        ShowCnfButton( "OK", 9 );

        upd = FALSE;
      }
      else
      {
        ShowCnfButton( "Error", 9 );
        return TRUE;
      }
    }
  }
  //
  SCR_DrawWindow( "Initialising", 9, 5, 29, 12 );
  //
  gotoxy( 10, 7 );
  SCR_LineCenterClr( "Current DIR", 19 );
  //
  if( SIO_InitCurDir( dID, &tmpDir ) )
  {
#ifdef DEBUG
    tmpDir = selDir;
#endif
    if( tmpDir.flDet.flStat & FLS_DIROK )
    {
      if( InitData( dID ) )
      {
        actDir = tmpDir;
        //
        ShowCnfButton( "OK", 13 );
        return TRUE;
      }
    }
  }
  ShowCnfButton( "Error", 9 );
  return TRUE;
}

//----------------------------------------------------------------
// Function :   DirDummyFoc
// Notes    :
// History  :
//----------------------------------------------------------------

static void DirDummyFoc ( UINT16 pos )
{
  pos = pos;
}

//----------------------------------------------------------------
// Function :   DrawDirList
// Notes    :
// History  :
//----------------------------------------------------------------

static void DrawDirList( void )
{
  SCR_DrawFrame( 0, 20, 39, 23, TRUE );
  gotoxy( 1, 21 );
  SCR_LineCenter( "Press 'A' to Activate current DIR", 38 );
  gotoxy( 1, 22 );
  SCR_LineCenter( "Press 'Esc' to close", 38 );
}

//----------------------------------------------------------------
// Function :   CmpDir
// Notes    :
// History  :
//----------------------------------------------------------------

static INT16 CmpDir( INT16 a, INT16 b )
{
  return memcmp( dirTab[a].flName, dirTab[b].flName, 8 );
}

//----------------------------------------------------------------
// Function :   SwpDir
// Notes    :
// History  :
//----------------------------------------------------------------

static void SwpDir( INT16 a, INT16 b )
{
  T_file   tdir;

  tdir = dirTab[a];
  dirTab[a] = dirTab[b];
  dirTab[b] = tdir;
}

//----------------------------------------------------------------
// Function :   ReadDirs
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL ReadDirs( UINT8 dev )
{
  T_file       dir;
  T_progress   prog;

#ifndef DEBUG
  memset( dirTab, 0, sizeof(dirTab) );
#endif

  SCR_DrawWindow( "Reading data", 9, 5, 29, 12 );
  //
  gotoxy( 10, 7 );
  SCR_LineCenterClr( "Dierectories", 19 );
  SCR_LoadShow( &prog, 11, 9, 15 );
  //
  maxDirs = 0;
  //
  if( SIO_GetFirstDir( dev, &dir ) )
  {
#ifndef DEBUG
    do
    {
      if( maxDirs >= MAX_DIR_ID )
      {
        break;
      }
      if( !(dir.flDet.flStat & FLS_DIROK) )
      {
        break;
      }
      //
      if( memcmp( dir.flName, ".       ", 8 ) != 0 )
      {
        dirTab[maxDirs++] = dir;
        //
        SCR_LoadUpdate( &prog, maxDirs );
      }
      //
    }while( SIO_GetNextDir( dev, &dir ) );
    //
    //  Sort Directories
    //
    if( maxDirs > 1 )
    {
      QuickSort( 0, maxDirs - 1, (T_compare)CmpDir, (T_swap)SwpDir );
    }
    //
    return TRUE;
#else
    maxDirs = 19;
    //
    SCR_LoadUpdate( &prog, maxDirs );
    //
    if( maxDirs > 1 )
    {
      QuickSort( 0, maxDirs - 1, (T_compare)CmpDir, (T_swap)SwpDir );
    }
    //
    return TRUE;
#endif
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   ViewDirectories
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL ViewDirectories( void )
{
  UINT8   name[12];
  UINT8   buf[40];
  UINT8   key;

  static T_listview         listDIR =
  {
    0, "", 8, 0, 3, 39, 16, "\033", "Aa", DirCnt, DirItm,
    OnDirSelect, (T_listact)OnDirAction, DirDummyFoc, DrawDirList
  };
  //
  if( !SIO_GetCurDir( dID, &actDir ) )
  {
    ShowCnfButton( "Error", 9 );
    return TRUE;
  }
  if( !(actDir.flDet.flStat & FLS_DIROK) )
  {
    ShowCnfButton( "Error", 9 );
    return TRUE;
  }
  selDir = actDir;
  //
  SCR_DrawFrame( 0, 3, 39, 16, TRUE );
  //
  key = KEY_RETURN;
  do
  {
    SCR_DrawFrame( 0, 17, 39, 19, TRUE );
    gotoxy( 3, 18 );
    memset( name, 0, sizeof(name) );
    memcpy( name, actDir.flName, 8 );
    sprintf( buf, "Active directory : %s", name );
    SCR_LineCenter( buf, 38 );
    //
    if( (key == KEY_RETURN) ? ReadDirs( dID ) : TRUE )
    {
      memset( name, 0, sizeof(name) );
      memcpy( name, selDir.flName, 8 );
      sprintf( buf, "Directory: %s", name );
      //
      listDIR.title = buf;
      key = LISTV_Show( &listDIR );
    }
    else
    {
      ShowCnfButton( "Error", 9 );
    }
  }while( key != 0xFF );
  //
  if( !SIO_SetCurDir( dID, &actDir ) )
  {
    ShowCnfButton( "Error", 9 );
  }
  //
  return TRUE;
}

//
// Atari Disks list Viewer
//

//----------------------------------------------------------------
// Function :   DiskCnt
// Notes    :
// History  :
//----------------------------------------------------------------

static UINT16 DiskCnt ( void )
{
  return MAX_FL_ID;
}

//----------------------------------------------------------------
// Function :   DiskItm
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL DiskItm ( UINT8 ** item, UINT16 cnt )
{
  static UINT8   buf[50];
         UINT8   name[10];

  if( cnt < MAX_FL_ID )
  {
    memset( name, 0, 10 );
    memcpy( name, diskTab[ cnt ].flName, 8 );
    if( diskTab[ cnt ].flDet.flStat & FLS_FILEOK )
    {
      sprintf( buf, " D%d: < %s %s %5u sectors %s",
               cnt + 1,
               name,
               (diskTab[ cnt ].flDet.flStat & FLS_RDONLY) ? "RO" : "RW",
               diskTab[ cnt ].flDet.flSize,
               (diskTab[ cnt ].flDet.flStat & FLS_SEC256B) ? "256B" : "128B" );
    }
    else
    {
      sprintf( buf, " D%d:   %s",
               cnt + 1,
               name );
    }
    *item = buf;
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   DiskDummySel
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL DiskDummySel ( UINT16 pos )
{
  pos = pos;
  return FALSE;
}

//----------------------------------------------------------------
// Function :   DiskDummyAct
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL OnDiskAction ( UINT16 pos, UINT8 key )
{
  T_file   disk;

  if( (pos < MAX_FL_ID) && (diskTab[pos].flDet.flStat & FLS_FILEOK) )
  {
    if( fsInfo.prtType == ISO9660 )
    {
      ShowCnfButton( "NOT Allowed", 7 );
      return FALSE;
    }
    disk = diskTab[pos];
    //
    switch( key )
    {
      case 'r':
      case 'R':
        if( diskTab[pos].flDet.flStat & FLS_RDONLY )
        {
          return FALSE;
        }
        disk.flDet.flStat |= FLS_RDONLY;
      break;
      case 'w':
      case 'W':
        if( !(diskTab[pos].flDet.flStat & FLS_RDONLY) )
        {
          return FALSE;
        }
        disk.flDet.flStat &= ~FLS_RDONLY;
      break;
      default:
      break;
    };
    //
    if( SIO_PutDisk( dID, (pos + 1), &disk ) )
    {
      diskTab[pos] = disk;
      for( pos = 0; pos < MAX_ATR_ID; pos++ )
      {
        if( atrTab[pos].flDet.flStartSec == disk.flDet.flStartSec )
        {
          atrTab[pos] = disk;
          break;
        }
      }
      for( pos = 0; pos < MAX_FL_ID; pos++ )
      {
        if( diskTab[pos].flDet.flStartSec == disk.flDet.flStartSec )
        {
          diskTab[pos] = disk;
        }
      }
      ShowCnfButton( "OK", 7 );

      upd = TRUE;
    }
    else
    {
      ShowCnfButton( "Error", 7 );
    }
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   OnDiskFocus
// Notes    :
// History  :
//----------------------------------------------------------------

static void OnDiskFocus ( UINT16 pos )
{
  UINT8   buf[40];

  SCR_DrawWindow( "Disk details", 0, 13, 39, 18 );
  //
  if( pos < MAX_FL_ID )
  {
    if( diskTab[pos].flDet.flStat & FLS_FILEOK )
    {
      gotoxy( 6, 15 );
      sprintf( buf, "Atari disk     : D%d", (pos + 1) );
      SCR_DrawLine( buf, 40 );
      gotoxy( 6, 16 );
      sprintf( buf, "Start sector   : %lu", diskTab[pos].flDet.flStartSec );
      SCR_DrawLine( buf, 40 );
    }
    else
    {
      gotoxy( 1, 15 );
      sprintf( buf, "DISK SLOT D%d IS EMPTY", (pos + 1) );
      SCR_LineCenter( buf, 38 );
    }
  }
}

//----------------------------------------------------------------
// Function :   DrawDiskList
// Notes    :
// History  :
//----------------------------------------------------------------

static void DrawDiskList( void )
{
  SCR_DrawFrame( 0, 19, 39, 23, TRUE );
  gotoxy( 1, 20 );
  SCR_LineCenter( "Press 'W' to mark Disk as Writable ", 38 );
  gotoxy( 1, 21 );
  SCR_LineCenter( "Press 'R' to mark Disk as Read Only", 38 );
  gotoxy( 1, 22 );
  SCR_LineCenter( "Press 'Esc' to close", 38 );
}

//----------------------------------------------------------------
// Function :   SelectAtariDisk
// Notes    :
// History  :
//----------------------------------------------------------------

static void SelectAtariDisk( T_listsel sel )
{
  static T_listview         listDSK =
  {
    0, "Atari disks", 38, 0, 3, 39, 12, "\033", "RrWw", DiskCnt, (T_getitem)DiskItm,
    DiskDummySel, (T_listact)OnDiskAction, (T_listfoc)OnDiskFocus, DrawDiskList
  };
  //
  listDSK.select = sel;
  LISTV_Show( &listDSK );
}

//
// ATR File list Viewer
//

//----------------------------------------------------------------
// Function :   AtrCnt
// Notes    :
// History  :
//----------------------------------------------------------------

static UINT16 AtrCnt ( void )
{
  return maxFiles;
}

//----------------------------------------------------------------
// Function :   AtrItm
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL AtrItm ( UINT8 ** item, UINT16 cnt )
{
  if( cnt < maxFiles )
  {
    *item = atrTab[ cnt ].flName;
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   AtrDummySel
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL AtrDummySel ( UINT16 pos )
{
  pos = pos;
  return FALSE;
}

//----------------------------------------------------------------
// Function :   AtrDummyAct
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL AtrDummyAct ( UINT16 pos, UINT8 key )
{
  key = key;
  pos = pos;
  return FALSE;
}

//----------------------------------------------------------------
// Function :   OnAtrFocus
// Notes    :
// History  :
//----------------------------------------------------------------

static void OnAtrFocus ( UINT16 pos )
{
  UINT8   buf[40];

  SCR_DrawWindow( "ATR file details", 0, 15, 39, 20 );
  //
  if( pos < maxFiles )
  {
    if( atrTab[pos].flDet.flStat & FLS_FILEOK )
    {
      gotoxy( 2, 17 );
      sprintf( buf, "File nr: %d", (pos + 1) );
      SCR_DrawLine( buf, 40 );
      gotoxy( 2, 18 );
      sprintf( buf, "Mode   : %s", (atrTab[pos].flDet.flStat & FLS_RDONLY) ?
                                   "RO" : "RW" );
      SCR_DrawLine( buf, 40 );
      gotoxy( 16, 17 );
      sprintf( buf, "Size (sectors) : %u", atrTab[pos].flDet.flSize );
      SCR_DrawLine( buf, 40 );
      gotoxy( 16, 18 );
      sprintf( buf, "Sector size    : %s", (atrTab[pos].flDet.flStat & FLS_SEC256B) ?
                                          "256B" : "128B" );
      SCR_DrawLine( buf, 40 );
    }
  }
}

//----------------------------------------------------------------
// Function :   DrawAtrList
// Notes    :
// History  :
//----------------------------------------------------------------

static void DrawAtrList( void )
{
  SCR_DrawFrame( 0, 21, 39, 23, TRUE );
  gotoxy( 1, 22 );
  SCR_LineCenter( "Press 'Esc' to close", 38 );
}

//----------------------------------------------------------------
// Function :   SelectAtrFile
// Notes    :
// History  :
//----------------------------------------------------------------

static void SelectAtrFile( T_listsel sel, UINT8 * title )
{
  static T_listview         listATR =
  {
    0, "", 8, 0, 3, 39, 14, "\033", "", AtrCnt, AtrItm, AtrDummySel,
    AtrDummyAct, (T_listfoc)OnAtrFocus, DrawAtrList
  };
  //
  listATR.select = sel;
  listATR.title  = title;
  LISTV_Show( &listATR );
}

//
// View Parameters screen
//

//----------------------------------------------------------------
// Function :   ViewParameters
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL ViewParameters( void )
{
  UINT8    buf[40];

  SCR_DrawFrame( 0, 21, 39, 23, TRUE );
  gotoxy( 1, 22 );
  SCR_LineCenter( "Press 'Esc' to close", 38 );
  //
  //  Drive parameters
  //
  SCR_DrawWindow( "Disk Drive parameters", 0, 3, 39, 11 );
  gotoxy( 6, 5 );
  sprintf( buf, "HDD mode      : %s", (drvInfo.flg & DRV_LBA) ? "LBA":"CHS" );
  SCR_DrawLine( buf, 40 );
  if( !(drvInfo.flg & DRV_LBA) )
  {
    gotoxy( 6, 6 );
    sprintf( buf, "Heads         : %d", drvInfo.hd );
    SCR_DrawLine( buf, 40 );
    gotoxy( 6, 7 );
    sprintf( buf, "Cylinders     : %d", drvInfo.cyl );
    SCR_DrawLine( buf, 40 );
    gotoxy( 6, 8 );
    sprintf( buf, "Sec per Track : %d", drvInfo.spt );
    SCR_DrawLine( buf, 40 );
    gotoxy( 6, 9 );
  }
  else
  {
    gotoxy( 6, 6 );
  }
  sprintf( buf, "Sectors       : %lu", drvInfo.sec );
  SCR_DrawLine( buf, 40 );
  //
  SCR_DrawWindow( "Interface parameters", 0, 12, 39, 20 );
  gotoxy( 6, 14 );
  sprintf( buf, "Device mode   : %s", (dID == 1) ? "MASTER":"SLAVE" );
  SCR_DrawLine( buf, 40 );
  gotoxy( 6, 15 );
  sprintf( buf, "Soft version  : %d.%d", (fsInfo.swVer>>4), (fsInfo.swVer&0x0F) );
  SCR_DrawLine( buf, 40 );
  gotoxy( 6, 16 );
  sprintf( buf, "ATR files     : %d", maxFiles );
  SCR_DrawLine( buf, 40 );
  //
  gotoxy( 6, 17 );
  sprintf( buf, "Part. type    : " );
  switch( fsInfo.prtType )
  {
    case PARTDOS:
      strcat( buf, "FAT16 small" );
    break;
//    case PARTEXTDOS:
    case PARTBIGDOS:
    case PARTFAT16W:
      strcat( buf, "FAT16 big" );
    break;
    case PARTFAT32:
    case PARTFAT32B:
      strcat( buf, "FAT32 LBA" );
    break;
    case ISO9660:
      strcat( buf, "ISO9660" );
    break;
    default:
      strcat( buf, "Unknown" );
    break;
  };
  SCR_DrawLine( buf, 40 );
  gotoxy( 6, 18 );
  sprintf( buf, "Sec per Clust : %d", fsInfo.secPClust );
  SCR_DrawLine( buf, 40 );

  WaitKey( "\033" );
  return TRUE;
}

//
// View ATR files screen
//

//----------------------------------------------------------------
// Function :   ViewAtrFiles
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL ViewAtrFiles( void )
{
  SelectAtrFile( AtrDummySel, "ATR files viewer" );
  return TRUE;
}

static UINT8    curDisk;        // current disk

//----------------------------------------------------------------
// Function :   AtrSelect
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL AtrSelect ( UINT16 pos )
{
  //
  // Assign selected ATR to Disk
  //
  if( SIO_PutDisk( dID, (curDisk + 1), &atrTab[pos] ) )
  {
    diskTab[curDisk] = atrTab[pos];       // Assign disk
    ShowCnfButton( "OK", 7 );

    upd = TRUE;
  }
  else
  {
    ShowCnfButton( "Error", 7 );
  }
  //
  return TRUE;          // Exit from list viewer
}

//----------------------------------------------------------------
// Function :   DiskSelect
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL DiskSelect ( UINT16 pos )
{
  UINT8   buf[40];

  if( pos < MAX_FL_ID )
  {
    curDisk = pos;      // Save selected Disk number-1
    //
    if( !(diskTab[pos].flDet.flStat & FLS_FILEOK) )
    {
      //
      //  Disk is Empty select new ATR
      //
      sprintf( buf, "Select ATR file for D%d:", (pos + 1) );
      SelectAtrFile( AtrSelect, buf );
    }
    else
    {
      //
      // Ask if Disk should be OFF
      //
      if( ShowYesNoButtons( "Disk OFF ?", 7 ) )
      {
        //
        if( SIO_DiskOff( dID, (pos + 1) ) )
        {
          memset( &diskTab[pos], 0, sizeof(T_file) );     // Disk OFF
          memcpy( diskTab[pos].flName, EMPTY_STR, 8 );
          ShowCnfButton( "OK", 7 );

          upd = TRUE;
        }
        else
        {
          ShowCnfButton( "Error", 7 );
        }
      }
    }
  }
  return FALSE;         // NO exit form list viewer
}

//----------------------------------------------------------------
// Function :   EditAtariDisks
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL EditAtariDisks( void )
{
  SelectAtariDisk( (T_listsel)DiskSelect );
  return TRUE;
}

//----------------------------------------------------------------
// Function :   SaveConig
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL SaveConfig( void )
{
  if( fsInfo.prtType == ISO9660 )
  {
    ShowCnfButton( "NOT Allowed", 14 );
    return TRUE;
  }
  if( SIO_SaveConfig( dID ) )
  {
    ShowCnfButton( "OK", 14 );

    upd = FALSE;
  }
  else
  {
    ShowCnfButton( "Error", 14 );
  }
  return TRUE;
}

//
// Main Menu
//

//----------------------------------------------------------------
// Function :   DrawMainMenu
// Notes    :
// History  :
//----------------------------------------------------------------

void DrawMainMenu( void )
{
  UINT8  name[12];
  UINT8  buf[40];

  SCR_DrawFrame( 0, 3, 39, 17, TRUE );
  //
  SCR_DrawFrame( 0, 18, 39, 20, TRUE );
  gotoxy( 3, 19 );
  memset( name, 0, sizeof(name) );
  memcpy( name, actDir.flName, 8 );
  sprintf( buf, "Active directory : %.8s", name );
  SCR_LineCenter( buf, 38 );
  //
  SCR_DrawFrame( 0, 21, 39, 23, TRUE );
  gotoxy( 1, 22 );
  SCR_LineCenter( "Select option", 38 );
}

//----------------------------------------------------------------
// Function :   MainMenu
// Notes    :
// History  :
//----------------------------------------------------------------

static void MainMenu( void )
{
  static T_menuitem       menuMMenuItm[] =
  {
    {MITM_NORMAL, " P - View parameters", "Pp", (T_menuact)ViewParameters},
    {MITM_NORMAL, " D - Change active DIR", "Dd", (T_menuact)ViewDirectories},
    {MITM_NORMAL, " V - View ATR files", "Vv", (T_menuact)ViewAtrFiles},
    {MITM_NORMAL, " A - Assign Disk", "Aa", (T_menuact)EditAtariDisks},
    {MITM_NORMAL, " S - Save configuration", "Ss", (T_menuact)SaveConfig},
    {MITM_EXIT, " \033 - Exit", "\033", NULL},
    {MITM_EMPTY,  "", "", NULL}
  };
  static T_menu           menuMMenu =
  {
    MNU_VERT, "Main menu", 6, 6, 32, 13, menuMMenuItm, "", (T_menudsk)DrawMainMenu
  };

  MENU_Show( &menuMMenu );
}


//
// Load Data screen
//

//----------------------------------------------------------------
// Function :   CmpATR
// Notes    :
// History  :
//----------------------------------------------------------------

static INT16 CmpATR( INT16 a, INT16 b )
{
  return memcmp( atrTab[a].flName, atrTab[b].flName, 8 );
}

//----------------------------------------------------------------
// Function :   SwpATR
// Notes    :
// History  :
//----------------------------------------------------------------

static void SwpATR( INT16 a, INT16 b )
{
  T_file   tfile;

  tfile = atrTab[a];
  atrTab[a] = atrTab[b];
  atrTab[b] = tfile;
}

//----------------------------------------------------------------
// Function :   InitConfig
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL InitConfig( UINT8 dev )
{
  T_progress   prog;

#ifndef DEBUG
  memset( &drvInfo, 0, sizeof(T_drvinf) );
  memset( &fsInfo, 0, sizeof(T_fsinf) );
#endif
  //
  gotoxy( 10, 7 );
  SCR_LineCenterClr( "Configuration", 19 );
  SCR_ProgressShow( &prog, 11, 9, 15, 2 );
  //
  if( !SIO_GetFSInfo( dev, &fsInfo ) )
    return FALSE;
  SCR_ProgressUpdate( &prog, 1 );
  //
  if( fsInfo.swVer != __SIO2IDE_VER__ )
  {
    return FALSE;
  }
  //
  if( !SIO_GetDriveInfo( dev, &drvInfo ) )
    return FALSE;
  SCR_ProgressUpdate( &prog, 2 );
  //
  return TRUE;
}

//----------------------------------------------------------------
// Function :   InitData
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL InitData( UINT8 dev )
{
  UINT8        id;
  T_file       file;
  T_progress   prog;

#ifndef DEBUG
  memset( diskTab, 0, sizeof(diskTab) );
  memset( atrTab, 0, sizeof(atrTab) );
#endif

#ifdef DEBUG
  MENU_Delay( 2000 );
#endif
  //
  gotoxy( 10, 7 );
  SCR_LineCenterClr( "Atari disks", 19 );
  SCR_ProgressShow( &prog, 11, 9, 15, MAX_FL_ID );
  //
  for( id = 0; id < MAX_FL_ID; id++ )
  {
    if( !SIO_GetDisk( dev, id + 1, &diskTab[id] ) )
      return FALSE;
    //
    SCR_ProgressUpdate( &prog, id + 1 );
    //
    if( !(diskTab[id].flDet.flStat & FLS_FILEOK) )
    {
      memcpy( diskTab[id].flName, EMPTY_STR, 8 );
    }
  }
#ifdef DEBUG
  MENU_Delay( 2000 );
#endif
  //
  maxFiles = 0;
  //
  gotoxy( 10, 7 );
  SCR_LineCenterClr( "ATR files", 19 );
  SCR_LoadShow( &prog, 11, 9, 15 );
  //
  if( SIO_GetFirstATR( dev, &file ) )
  {
#ifndef DEBUG
    do
    {
      if( maxFiles >= MAX_ATR_ID )
      {
        break;
      }
      if( !(file.flDet.flStat & FLS_FILEOK) )
      {
        break;
      }
      //
      atrTab[maxFiles++] = file;
      //
      SCR_LoadUpdate( &prog, maxFiles );
      //
    }while( SIO_GetNextATR( dev, &file ) );
    //
    //  Sort ATR files
    //
    if( maxFiles > 1 )
    {
      QuickSort( 0, maxFiles - 1, (T_compare)CmpATR, (T_swap)SwpATR );
    }
    //
    return TRUE;
#else
    maxFiles = 24;
    //
    SCR_LoadUpdate( &prog, maxFiles );
    //
    if( maxFiles > 1 )
    {
      QuickSort( 0, maxFiles - 1, (T_compare)CmpATR, (T_swap)SwpATR );
    }
    //
    return TRUE;
#endif
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   DrawLoadScreen
// Notes    :
// History  :
//----------------------------------------------------------------

static void DrawLoadScreen( UINT8 dev )
{
  upd = FALSE;

  SCR_DrawFrame( 0, 3, 39, 20, TRUE );
  SCR_DrawWindow( "Reading data", 9, 5, 29, 12 );

  if( InitConfig( dev ) && InitData( dev ) )
  {
    //
    if( SIO_GetCurDir( dev, &actDir ) )
    {
      if( actDir.flDet.flStat & FLS_DIROK )
      {
        ShowCnfButton( "OK", 17 );
        //
        MainMenu();
      }
    }
    //
    if( upd && (fsInfo.prtType != ISO9660) )
    {
      if( ShowYesNoButtons( "Save config ?", 14 ) )
      {
        SaveConfig();
      }
    }
  }
  else
  {
    ShowCnfButton( "Error", 9 );
  }
}

//----------------------------------------------------------------
// Function :   LoadMasterData
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL LoadMasterData( void )
{
  dID = 1;
  DrawLoadScreen( dID );
  return TRUE;
}

//----------------------------------------------------------------
// Function :   LoadSlaveData
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL LoadSlaveData( void )
{
  dID = 2;
  DrawLoadScreen( dID );
  return TRUE;
}

//
// Main Screen
//

//----------------------------------------------------------------
// Function :   DrawMainWindows
// Notes    :
// History  :
//----------------------------------------------------------------

void DrawMainWindows( BOOL mod )
{
  //
  SCR_DrawFrame( 0, 21, 39, 23, TRUE );
  gotoxy( 1, 22 );
#ifdef DEBUG
  #ifdef __DOS__
    SCR_LineCenter( "MMSoft (c) 2002  (DOS demo ver)", 38 );
  #endif
  #ifdef __CART__
    SCR_LineCenter( "MMSoft (c) 2002  (Cart demo ver)", 38 );
  #endif
#else
  #ifdef __DOS__
    SCR_LineCenter( "MMSoft (c) 2002  (DOS version)", 38 );
  #endif
  #ifdef __CART__
    SCR_LineCenter( "MMSoft (c) 2002  (Cartridge version)", 38 );
  #endif
#endif
  //
  SCR_DrawFrame( 0, 0, 39, 2, FALSE );
  gotoxy( 1, 1 );
  if( mod )
  {
    SCR_LineCenter( "SIO2IDE "__SIO2IDE_VER_TXT__" configuration center", 38 );
  }
  else
  {
    SCR_LineCenter( "SIO2IDE "__SIO2IDE_VER_TXT__" BootLoader", 38 );
  }
  SCR_DrawFrame( 0, 3, 39, 20, TRUE );
}

//----------------------------------------------------------------
// Function :   DrawMainScreen
// Notes    :
// History  :
//----------------------------------------------------------------

void DrawMainScreen( void )
{
  DrawMainWindows( TRUE );
}

//----------------------------------------------------------------
// Function :   MainScreen
// Notes    :
// History  :
//----------------------------------------------------------------

static void MainScreen( void )
{
  static T_menuitem       menuMScrItm[] =
  {
    {MITM_NORMAL, " M - Master device", "Mm", LoadMasterData},
    {MITM_NORMAL, " S - Slave device", "Ss", LoadSlaveData},
    {MITM_EXIT, " \033 - Exit", "\033", NULL},
    {MITM_EMPTY,  "", "", NULL}
  };
  static T_menu           menuMScr =
  {
    MNU_VERT, "Select interface", 9, 8, 29, 12, menuMScrItm, "", DrawMainScreen
  };

#ifdef __DOS__
  do
  {
#endif

    MENU_Show( &menuMScr );

#ifdef __DOS__
  }while( !ShowYesNoButtons( "Are you sure ?", 17 ) );
#endif
}

//
// Application Main
//

//----------------------------------------------------------------
// Function :   main
// Notes    :
// History  :
//----------------------------------------------------------------

UINT16 main ( void )
{
  SCR_Init();

#ifdef __DOS__
  MainScreen();
#endif

#ifdef __CART__
  DrawMainWindows( FALSE );
  switch( WaitKeyFor( "Wait for disk BOOT",
                      "†≈Û„†  - SIO2IDE setup",
                      "”·„Â  - skip setup   ",
                      "\033\040", 5 )
        )
  {
    case KEY_ESC:
    {
      MainScreen();

      DrawMainScreen();
      if( !ShowYesNoButtons( "Leave CART ON ?", 10 ) )
      {
        SCR_DrawFrame( 7, 10, 31, 12, TRUE );
        gotoxy( 1, 11 );
        SCR_LineCenter( "Switch OFF the CART", 38 );
        CardWait();
      }
    }
    break;
    case 0x20:
    default:
    break;
  };
#endif

  SCR_DeInit();

  return EXIT_SUCCESS;
}

//      End
