//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: IDEINFO.C
//
// MODULE NAME: IDEINFO
//
// PURPOSE:
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001   25-02-2002 MM     Created
//
//****************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>

#include <cfg.h>
#include "types.h"

//#define DEBUG

#define FS_MAXDEV       15      // Max No of Devices (const)
#define FS_MAXHDP       99      // Absolute Max No of Partitions (depends on SIO2IDE)
#define FS_NODISK       0xFF

#define FS_SIO2IDE      0x71    // SIO2IDE interface ID base

// Supported Commands
#define SIOC_GETHDP     0xEC    // Get the HD Partition Table
#define SIOC_PUTHDP     0xED    // Put the HD Partition Table
#define SIOC_GETHDI     0xEE    // Get the HD Information Block

extern UINT8 siov( void );

/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/

//
// Disk Information
//
typedef struct
{
  UINT16     cyl;       // Number of cylinders
  UINT8      hd;        // Number of heads
  UINT16     spt;       // Sectors/Track
  UINT32     sec;       // Number of Sectors
} T_DRVINF;
typedef struct
{
  UINT16   id;
  UINT8    swVer;
  UINT8    maxHDP;
} T_S2IINFO;
#define HDINF_SIZE      (sizeof(T_DRVINF)+sizeof(T_S2IINFO))
//
#ifdef DEBUG
static T_DRVINF    hdInfo = { 762, 8, 39, 273441 };
static T_S2IINFO   s2iInfo = { 0x1234, 0x10, FS_MAXHDP };
#else
static T_DRVINF    hdInfo = { 0, 0, 0, 0 };
static T_S2IINFO   s2iInfo = { 0, 0, 0 };
#endif
//
// Disk Table
//
static UINT8           hdIDTab[FS_MAXDEV] =
{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static UINT8           hdIDTabTmp[FS_MAXDEV] =
{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
//
// Partitions Table
//
typedef struct
{
//  UINT32    sSec;
  UINT16    nSec;
} T_HDPART;
static     T_HDPART        hdPTab[FS_MAXHDP];
static     T_HDPART        hdPTabTmp[FS_MAXHDP];
#define HDPTAB_SIZE        (sizeof(hdIDTab) + (s2iInfo.maxHDP * sizeof(T_HDPART)))

static UINT8       prgInfo[] =
{ 'M'+0x78, 'M'+0x78, 's'+0x78,
  'o'+0x78, 'f'+0x78, 't'+0x78,
  ' '+0x78, '('+0x78, 'c'+0x78,
  ')'+0x78, ' '+0x78, '2'+0x78,
  '0'+0x78, '0'+0x78, '1'+0x78,
  0x00
};

static UINT8       hdpos = 0;
static UINT8       did;
static UINT8       rxtxBuff[128];

/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/

//----------------------------------------------------------------
// Function :   getint
// Notes    :
// History  :
//----------------------------------------------------------------

static UINT32 getint( UINT8 mlen, BOOL echo )
{
  UINT32  i = 0;
  UINT8   c, s, len = 0;

  cursor( 1 );
  while( (c = cgetc()) != 0x9B )
  {
    s = wherex();
    if( isdigit( c ) && (len < mlen) && (s < 40) )
    {
      i *= 10;
      i += (c - 0x30);
      if( echo )
      {
        printf("%c", c);
      }
      len++;
    }
    else if( (c == 0x7E) && len )
    {
      i /= 10;
      if( echo )
      {
        gotoxy( wherex()-1, wherey() );
        printf(" ");
        gotoxy( wherex()-1, wherey() );
      }
      len--;
    }
  }
  cursor( 0 );
  return i;
}

//----------------------------------------------------------------
// Function :   ShowStat
// Notes    :
// History  :
//----------------------------------------------------------------

static void ShowStat( UINT8 err )
{
  gotoxy( 1, 11 );
  switch( err )
  {
    case 1:
      textcolor( COLOR_BLACK );
      printf("      Error: SIO2IDE Not Ready");
    break;
    case 2:
      textcolor( COLOR_BLACK );
      printf("       Error: Table NOT Saved");
    break;
    case 3:
      printf("          Table Saved OK");
    break;
    case 4:
      textcolor( COLOR_BLACK );
      printf("      Error: Can't get HD info");
    break;
    case 5:
      textcolor( COLOR_BLACK );
      printf("     Error: Wrong size (>65535)");
    break;
    case 6:
      textcolor( COLOR_BLACK );
      printf("    Error: Wrong size (HD is full)");
    break;
    case 7:
      textcolor( COLOR_BLACK );
      printf("     Error: Wrong Partition ID");
    break;
    case 8:
      textcolor( COLOR_BLACK );
      printf("       Error: Wrong Disk ID");
    break;
    case 9:
      printf(" Warning: Selected Partition is Empty");
    break;
    default:
    break;
  };
}

//----------------------------------------------------------------
// Function :   WaitKey
// Notes    :
// History  :
//----------------------------------------------------------------

static void WaitKey( void )
{
  gotoxy( 12, 22 );
  printf("Press any key");
  while( !kbhit() );
  cgetc();
  textcolor( COLOR_WHITE );
}

//----------------------------------------------------------------
// Function :   HD_GetInfo
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL HD_GetInfo( UINT8 dev )
{
  UINT8  s;
  UINT8 *d;
  UINT8 *p = rxtxBuff;

#ifdef DEBUG
  return 1;
#endif

  DCB->device   = FS_SIO2IDE;            // SIO2IDE
  DCB->unit     = dev;
  DCB->command  = SIOC_GETHDI;           //
  DCB->status   = 0x40;                  // READ
  DCB->buffer   = rxtxBuff;              //
  DCB->timeout  = 0x40;                  // TimeOut
  DCB->xfersize = HDINF_SIZE;            // Data Size

  if( (s = siov()) != 0 )
  {
    return 0;
  }
  // Copy Data
  s = sizeof(T_DRVINF);
  d = (UINT8*)&hdInfo;
  while( s-- )
  {
    *d++ = *p++;
  }
  s = sizeof(T_S2IINFO);
  d = (UINT8*)&s2iInfo;
  while( s-- )
  {
    *d++ = *p++;
  }
  if( s2iInfo.maxHDP > FS_MAXHDP )
  {
    s2iInfo.maxHDP = 0;
    return 0;
  }
  return 1;
}

//----------------------------------------------------------------
// Function :   HD_ShowInfo
// Notes    :
// History  :
//----------------------------------------------------------------

void HD_ShowInfo( void )
{
  printf( "\n\n             HD Parameters" );
  printf( "\n   ---------------------------------" );
  //
  printf( "\n       Heads             : %u", hdInfo.hd );
  printf( "\n       Cylinders         : %u", hdInfo.cyl );
  printf( "\n       Sectors per Track : %u", hdInfo.spt );
  printf( "\n       Total Sectors     : %lu", hdInfo.sec );
  printf( "\n\n\n         Interface Parameters" );
  printf( "\n   ---------------------------------" );
  printf( "\n       Interface Type    : SIO2IDE");
  if( did == 1 )
  {
  printf( "\n       Master Mode       :");
  }
  else
  {
  printf( "\n       Slave Mode        :");
  }
  printf( " ver %u.%u", (s2iInfo.swVer>>4), (s2iInfo.swVer&0x0F) );
  printf( "\n       Partitions (max)  : %u\n", s2iInfo.maxHDP );
}

//----------------------------------------------------------------
// Function :   HD_GetPTab
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL HD_GetPTab( UINT8 dev )
{
  UINT8  s;
  UINT8 *d;
  UINT8 *p = rxtxBuff;

#ifdef DEBUG
  return 1;
#endif

  DCB->device   = FS_SIO2IDE;            // SIO2IDE
  DCB->unit     = dev;
  DCB->command  = SIOC_GETHDP;           //
  DCB->status   = 0x40;                  // READ
  DCB->buffer   = rxtxBuff;              //
  DCB->timeout  = 0x40;                  // TimeOut
  DCB->xfersize = HDPTAB_SIZE;           // Data Size

  if( (s = siov()) != 0 )
  {
    return 0;
  }
  // Copy Data
  s = sizeof(hdIDTab);
  d = (UINT8*)&hdIDTab;
  while( s-- )
  {
    *d++ = *p++;
  }
  s = (s2iInfo.maxHDP * 2);
  d = (UINT8*)&hdPTab;
  while( s-- )
  {
    *d++ = *p++;
  }
  return 1;
}

//----------------------------------------------------------------
// Function :   HD_PutPTab
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL HD_PutPTab( UINT8 dev )
{
  UINT8  s;
  UINT8  *d;
  UINT8  *p = rxtxBuff;

#ifdef DEBUG
  return 1;
#endif

  // Copy Data
  s = sizeof(hdIDTabTmp);
  d = (UINT8*)&hdIDTabTmp;
  while( s-- )
  {
    *p++ = *d++;
  }
  s = (s2iInfo.maxHDP * 2);
  d = (UINT8*)&hdPTabTmp;
  while( s-- )
  {
    *p++ = *d++;
  }

  DCB->device   = FS_SIO2IDE;            // SIO2IDE
  DCB->unit     = dev;
  DCB->command  = SIOC_PUTHDP;           //
  DCB->status   = 0x80;                  // WRITE
  DCB->buffer   = rxtxBuff;              //
  DCB->timeout  = 0x40;                  // TimeOut
  DCB->xfersize = HDPTAB_SIZE;           // Data Size

  if( (s = siov()) != 0 )
  {
    return 0;
  }
  return 1;
}

//----------------------------------------------------------------
// Function :   HD_ShowPTab
// Notes    :
// History  :
//----------------------------------------------------------------

void HD_ShowPTab( T_HDPART * hdp, UINT8 start )
{
  T_HDPART * hd = hdp + start;
  UINT8      stat, stop, y, ys;
  UINT32     sec = 1;

  //
  printf( "\n" );
  printf( "\n No  Start   Length  No  Start   Length" );
  printf( "\n --------------------------------------" );
  ys = y = wherey() + 1;
  stop = (start + 8) <= s2iInfo.maxHDP ? (start + 8) : s2iInfo.maxHDP;
  for( stat = start; stat < stop; stat++ )
  {
    if( hd->nSec )
    {
      printf( "\n P%u", (stat+1) );
      gotoxy( 5, wherey() );
      printf( "%07lu %05u", sec, hd->nSec );
      sec += (UINT32)hd->nSec;
    }
    else
    {
      printf( "\n P%u", (stat+1) );
      gotoxy( 5, wherey() );
      printf( "------- -----" );
    }
    hd++;
  }
  stop = (start + 16) <= s2iInfo.maxHDP ? (start + 16) : s2iInfo.maxHDP;
  for( stat = (start + 8); stat < stop; stat++ )
  {
    gotoxy( 20, y++ );
    if( hd->nSec )
    {
      printf( " P%u", (stat+1) );
      gotoxy( 25, wherey() );
      printf( "%07lu %05u", sec, hd->nSec );
      sec += (UINT32)hd->nSec;
    }
    else
    {
      printf( " P%u", (stat+1) );
      gotoxy( 25, wherey() );
      printf( "------- -----" );
    }
    hd++;
  }
  sec = 1;
  for( stat = 0; stat < s2iInfo.maxHDP; stat++ )
  {
    sec += (UINT32)hdp->nSec;
    hdp++;
  }
  gotoxy( 1, ys + 8 );
  printf( "--------------------------------------" );
  if( hdInfo.sec > sec )
  {
    printf( "\n       Free space = %06lu sect", hdInfo.sec - sec );
  }
  else
  {
    printf( "\n       Free space = 000000 sect" );
  }
}

//----------------------------------------------------------------
// Function :   HD_ShowIDTab
// Notes    :
// History  :
//----------------------------------------------------------------

void HD_ShowIDTab( UINT8 * id )
{
  UINT8   ys, y, stat;

  //
  printf( "\n" );
  printf( "\n  Disk      HD Prt.   Disk      HD Prt." );
  printf( "\n --------------------------------------" );
  ys = y = wherey() + 1;
  for( stat = 0; stat < ((FS_MAXDEV+1)/2); stat++ )
  {
    if( *id == FS_NODISK )
    {
      printf( "\n  D%u", stat+1 );
      gotoxy( 14, wherey() );
      printf( "--" );
    }
    else
    {
      printf( "\n  D%u", stat+1, (*id + 1) );
      gotoxy( 9, wherey() );
      printf( "<-   P%u", (*id + 1) );
    }
    id++;
  }
  for( stat = ((FS_MAXDEV+1)/2); stat < FS_MAXDEV; stat++ )
  {
    gotoxy( 20, y++ );
    if( *id == FS_NODISK )
    {
      printf( "  D%u", stat+1 );
      gotoxy( 34, wherey() );
      printf( "--" );
    }
    else
    {
      printf( "  D%u", stat+1 );
      gotoxy( 29, wherey() );
      printf( "<-   P%u", (*id + 1) );
    }
    id++;
  }
  gotoxy( 1, ys + ((FS_MAXDEV+1)/2) );
  printf( "--------------------------------------" );
}

//----------------------------------------------------------------
// Function :   ShowHDInfo
// Notes    :
// History  :
//----------------------------------------------------------------

static void ShowHDInfo( void )
{
  if( !HD_GetInfo( did ) )
  {
    ShowStat( 1 );
  }
  else
  {
    HD_ShowInfo();
  }
  WaitKey();
}

//----------------------------------------------------------------
// Function :   ShowHDPTab
// Notes    :
// History  :
//----------------------------------------------------------------

static void ShowHDPTab( void )
{
  UINT8  itm;
  BOOL   act = 1;

  if( !HD_GetPTab( did ) )
  {
    ShowStat( 1 );
  }
  else
  {
    do
    {
        clrscr();

        printf( "\n      HD Partitions Table Viever" );
        HD_ShowPTab( (T_HDPART*)&hdPTabTmp, hdpos );

        printf("\n\n   N -   Next Partitions");
        printf("\n   X -   Exit");
        printf("\n\n            Select Option");

        do
        {
          itm = cgetc();
        }while( !strchr( "NnXx", itm ) );

        clrscr();

        switch( itm )
        {
          case 'N':
          case 'n':
            hdpos += 16;
            if( hdpos > s2iInfo.maxHDP )  hdpos = 0;
            hdpos %= s2iInfo.maxHDP;
          break;
          case 'X':
          case 'x':
            act = 0;
          break;
          default:
          break;
        };
    }while( act );
  }
}

//----------------------------------------------------------------
// Function :   ShowIDTab
// Notes    :
// History  :
//----------------------------------------------------------------

static void ShowIDTab( void )
{
  if( !HD_GetPTab( did ) )
  {
    ShowStat( 1 );
  }
  else
  {
    printf( "\n         Disk Sequence Viewer" );
    HD_ShowIDTab( (UINT8*)&hdIDTab );
  }
  WaitKey();
}

//----------------------------------------------------------------
// Function :   CheckSec
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL CheckSec( UINT32 sec )
{
  UINT32   sn = 0;
  UINT8    d;

  if( HD_GetInfo( did ) )
  {
    for( d = 0; d < s2iInfo.maxHDP; d++ )
    {
      sn += hdPTabTmp[d].nSec;
    }
    sn += sec;
    if( sn >= hdInfo.sec )
    {
      sn++;
      sn -= sec;
      ShowStat( 6 );
      if( hdInfo.sec > sn )
      {
        printf("\n       Max partition size = %lu", (hdInfo.sec - sn) );
      }
      WaitKey();
      return 0;
    }
  }
  else
  {
    ShowStat( 4 );
    WaitKey();
    return 0;
  }
  if( sec > 65535L )
  {
    ShowStat( 5 );
    WaitKey();
    return 0;
  }
  return 1;
}

//----------------------------------------------------------------
// Function :   EditHDPEntry
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL EditHDPEntry( void )
{
  UINT32 sec;
  UINT8  itm;

  clrscr();

  printf( "\n      HD Partitions Table Editor" );
  HD_ShowPTab( (T_HDPART*)&hdPTabTmp, hdpos );

  printf( "\n\nEnter Partition ID    = " );
  itm = getint( 2, 1 );
  if( !((itm >= 1) && (itm <= s2iInfo.maxHDP)) )
  {
    clrscr();
    ShowStat( 7 );
    WaitKey();
    return 0;
  }
  printf("\nEnter new size (sectors) = ");
  sec = getint( 5, 1 );
  //
  clrscr();
  if( !CheckSec( sec ) )
  {
    return 0;
  }
  hdPTabTmp[itm-1].nSec = sec;
  return 1;
}

//----------------------------------------------------------------
// Function :   EditHDPTab
// Notes    :
// History  :
//----------------------------------------------------------------

static void EditHDPTab( void )
{
  UINT8  itm;
  BOOL   act = 1;

  if( !HD_GetPTab( did ) )
  {
    ShowStat( 1 );
    WaitKey();
    return;
  }
  else
  {
    for( itm = 0; itm < s2iInfo.maxHDP; itm++ )
    {
      hdPTabTmp[itm].nSec = hdPTab[itm].nSec;
//      hdPTabTmp[itm].sSec = hdPTab[itm].sSec;
    }
    for( itm = 0; itm < FS_MAXDEV; itm++ )
    {
      hdIDTabTmp[itm] = hdIDTab[itm];
    }
    do
    {
        clrscr();

        printf( "\n      HD Partitions Table Editor" );
        HD_ShowPTab( (T_HDPART*)&hdPTabTmp, hdpos );

        printf("\n\n   N -   Next Partitions");
        printf("\n   E -   Edit Partition");
        printf("\n   S -   Save and Exit");
        printf("\n   X -   Exit");
        printf("\n\n            Select Option");

        do
        {
          itm = cgetc();
        }while( !strchr( "NnEeSsXx", itm ) );

        clrscr();

        switch( itm )
        {
          case 'N':
          case 'n':
            hdpos += 16;
            if( hdpos > s2iInfo.maxHDP )  hdpos = 0;
            hdpos %= s2iInfo.maxHDP;
          break;
          case 'E':
          case 'e':
            EditHDPEntry();
          break;
          case 'S':
          case 's':
            if( !HD_PutPTab( did ) )
            {
              ShowStat( 2 );
            }
            else
            {
              ShowStat( 3 );
              act = 0;
            }
            WaitKey();
          break;
          case 'X':
          case 'x':
            act = 0;
          break;
          default:
          break;
        };
    }while( act );
  }
}

//----------------------------------------------------------------
// Function :   EditIDEntry
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL EditIDEntry( void )
{
  UINT8  itm, pid;

  clrscr();

  printf( "\n         Disk Sequence Editor" );
  HD_ShowIDTab( (UINT8*)&hdIDTabTmp );

  printf( "\n\nEnter Disk ID   = " );
  itm = getint( 2, 1 );
  if( !((itm >= 1) && (itm <= FS_MAXDEV)) )
  {
    clrscr();
    ShowStat( 8 );
    WaitKey();
    return 0;
  }
  printf("\nEnter Partition ID = ");
  pid = getint( 2, 1 );
  if( pid > s2iInfo.maxHDP )
  {
    clrscr();
    ShowStat( 7 );
    WaitKey();
    return 0;
  }
  //
  if( pid == 0 )        pid = FS_NODISK;
  else                  pid--;
  if( pid != FS_NODISK )
  {
    if( hdPTab[pid].nSec == 0 )
    {
      clrscr();
      ShowStat( 9 );
      WaitKey();
    }
  }
  //
  hdIDTabTmp[itm-1] = pid;
  return 1;
}

//----------------------------------------------------------------
// Function :   EditIDTab
// Notes    :
// History  :
//----------------------------------------------------------------

static void EditIDTab( void )
{
  UINT8  itm;
  BOOL   act = 1;

  if( !HD_GetPTab( did ) )
  {
    ShowStat( 1 );
    WaitKey();
    return;
  }
  else
  {
    for( itm = 0; itm < s2iInfo.maxHDP; itm++ )
    {
      hdPTabTmp[itm].nSec = hdPTab[itm].nSec;
//      hdPTabTmp[itm].sSec = hdPTab[itm].sSec;
    }
    for( itm = 0; itm < FS_MAXDEV; itm++ )
    {
      hdIDTabTmp[itm] = hdIDTab[itm];
    }
    do
    {
        clrscr();

        printf( "\n         Disk Sequence Editor" );
        HD_ShowIDTab( (UINT8*)&hdIDTabTmp );

        printf("\n\n   E -   Edit Disk");
        printf("\n   S -   Save and Exit");
        printf("\n   X -   Exit");
        printf("\n\n            Select Option");

        do
        {
          itm = cgetc();
        }while( !strchr( "EeSsXx", itm ) );

        clrscr();

        switch( itm )
        {
          case 'E':
          case 'e':
            EditIDEntry();
          break;
          case 'S':
          case 's':
            if( !HD_PutPTab( did ) )
            {
              ShowStat( 2 );
            }
            else
            {
              ShowStat( 3 );
              act = 0;
            }
            WaitKey();
          break;
          case 'X':
          case 'x':
            act = 0;
          break;
          default:
          break;
        };
    }while( act );
  }
}

//----------------------------------------------------------------
// Function :   MainMenu
// Notes    :
// History  :
//----------------------------------------------------------------

void MainMenu( void )
{
  BOOL   run2 = 1;
  UINT8  itm;

  do
  {
    clrscr();
    printf("\n\n               SIO2IDE\n\n       Configuration Center "__SIO2IDE_VER_TXT__"\n\n");

    printf("\n   P -   View HD & Interface Params");
    printf("\n   V -   View HD Partitions Table");
    printf("\n   W -   View Disk Sequence");
    printf("\n   T -   Edit HD Partitions Table");
    printf("\n   S -   Edit Disk Sequence");
    printf("\n   X -   Exit");
    printf("\n\n\n            Select Option\n\n\n");
    printf("\n\n\n\n           %s", prgInfo );

    do
    {
      itm = cgetc();
    }while( !strchr( "PpVvWwTtSsXx", itm ) );

    clrscr();

    switch( itm )
    {
      case 'P':
      case 'p':
        ShowHDInfo();
      break;
      case 'V':
      case 'v':
        ShowHDPTab();
      break;
      case 'W':
      case 'w':
        ShowIDTab();
      break;
      case 'T':
      case 't':
        EditHDPTab();
      break;
      case 'S':
      case 's':
        EditIDTab();
      break;
      case 'X':
      case 'x':
        run2 = 0;
      break;
      default:
      break;
    };
  }while( run2 );
}

//----------------------------------------------------------------
// Function :   StartMenu
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL StartMenu( void )
{
  BOOL   run1 = 1;
  UINT8  itm;

  do
  {
    clrscr();
#ifndef DEBUG
    printf("\n\n               SIO2IDE\n\n       Configuration Center "__SIO2IDE_VER_TXT__"\n\n");
#else
    printf("\n\n               SIO2IDE\n\n    Configuration Center "__SIO2IDE_VER_TXT__" (demo)\n\n");
#endif

    printf("\n   M -   Edit Master SIO2IDE");
    printf("\n   S -   Edit Slave SIO2IDE");
    printf("\n   X -   Exit");
    printf("\n\n\n            Select Option");
    printf("\n\n\n\n\n\n\n\n\n\n           %s", prgInfo );

    do
    {
      itm = cgetc();
    }while( !strchr( "MmSsXx", itm ) );

    clrscr();

    switch( itm )
    {
      case 'M':
      case 'm':
        did = 1;
        if( !HD_GetInfo( did ) )
        {
          ShowStat( 1 );
          WaitKey();
        }
        else
        {
          run1 = 0;
        }
      break;
      case 'S':
      case 's':
        did = 2;
        if( !HD_GetInfo( did ) )
        {
          ShowStat( 1 );
          WaitKey();
        }
        else
        {
          run1 = 0;
        }
      break;
      case 'X':
      case 'x':
        did = 0;
        return 0;
      break;
      default:
      break;
    };
  }while( run1 );
  return 1;
}

//----------------------------------------------------------------
// Function :   main
// Notes    :
// History  :
//----------------------------------------------------------------

UINT16 main( void )
{
  BOOL   run = 1;
  UINT8 *p;

  p = prgInfo;
  while( *p )
  {
    *p -= 0x78;
    p++;
  }

  clrscr();
  textcolor( _gtia_mkcolor(HUE_YELLOW,4) );
  bgcolor( _gtia_mkcolor(HUE_BLUE,0) );

  do
  {
    if( (run = StartMenu()) == 1 )
    {
      MainMenu();
    }
  }while( run );

  clrscr ();

  return EXIT_SUCCESS;
}

//      End


