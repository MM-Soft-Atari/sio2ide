//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: fs.h
//
// MODULE NAME: fs
//
// PURPOSE:     SIO2IDE File System module.
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001    6-12-2001 MM     Created
//
//****************************************************************

#include "apps.h"

//****************************************************************
//
// Sector 0 Data
//
//****************************************************************

//
// SIO2IDE Info (const)
//
typedef struct
{
  UINT16   id;
  UINT8    swVer;
  UINT8    maxHDP;
} T_S2IINFO;
STATIC  DATA T_S2IINFO       hdS2IInfo =
{
  __SIO2IDE_ID__,
  __SIO2IDE_VER__,
  FS_MAXHDP
};
//
// Disk Table
//
STATIC  DATA UINT8           hdIDTab[FS_MAXDEV] =
{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
//
// Partitions Table
//
typedef struct
{
//  UINT32    sSec;     // Partition Start
  UINT16    nSec;     // Partition Size
} T_HDPART;
STATIC       T_HDPART        hdPart[FS_MAXHDP] =
{
  {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
  {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
  {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
  {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
  {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
  {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}
//  {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0},
//  {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}
};

//
// Static data
//
STATIC  DATA T_DRVINF        hdInfo;    // Drive Info
STATIC  DATA BOOL            fsRdy;     // FS ready flag
STATIC  DATA UINT8           fsCDrv;    // Current Drive nember
STATIC  DATA UINT32          fsCSec;    // Current Sector number

//
// SIO Command Handlers
//
STATIC BOOL  FS_PutSetup    ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_PutData     ( UINT8 * buf );
STATIC BOOL  FS_GetSetup    ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_GetData     ( UINT8 * buf );
STATIC BOOL  FS_StatSetup   ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_StatData    ( UINT8 * buf );
STATIC BOOL  FS_GHDPSetup   ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_GHDPData    ( UINT8 * buf );
STATIC BOOL  FS_PHDPSetup   ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_PHDPData    ( UINT8 * buf );
STATIC BOOL  FS_GHDISetup   ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_GHDIData    ( UINT8 * buf );
STATIC BOOL  FS_GCfgSetup   ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_GCfgData    ( UINT8 * buf );
STATIC BOOL  FS_PCfgSetup   ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_PCfgData    ( UINT8 * buf );
STATIC BOOL  FS_FrmSetup    ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_FrmData     ( UINT8 * buf );
STATIC BOOL  FS_HSSetup     ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_HSData      ( UINT8 * buf );

//
// SIO Handlers Table
//
FLASH TF_SIOHNDL          sioHndl[] =
{
 {SIOC_STAT,   (T_CSETUP)FS_StatSetup,SIOS_TXSETUP,(T_RXTXDATA)FS_StatData },
 {SIOC_PUT,    (T_CSETUP)FS_PutSetup, SIOS_RXSETUP,(T_RXTXDATA)FS_PutData },
 {SIOC_PUTV,   (T_CSETUP)FS_PutSetup, SIOS_RXSETUP,(T_RXTXDATA)FS_PutData },
 {SIOC_GET,    (T_CSETUP)FS_GetSetup, SIOS_TXSETUP,(T_RXTXDATA)FS_GetData },
 {SIOC_PUTHDP, (T_CSETUP)FS_PHDPSetup,SIOS_RXSETUP,(T_RXTXDATA)FS_PHDPData },
 {SIOC_GETHDP, (T_CSETUP)FS_GHDPSetup,SIOS_TXSETUP,(T_RXTXDATA)FS_GHDPData },
 {SIOC_GETHDI, (T_CSETUP)FS_GHDISetup,SIOS_TXSETUP,(T_RXTXDATA)FS_GHDIData },
 {SIOC_GCFG,   (T_CSETUP)FS_GCfgSetup,SIOS_TXSETUP,(T_RXTXDATA)FS_GCfgData },
 {SIOC_PCFG,   (T_CSETUP)FS_PCfgSetup,SIOS_RXSETUP,(T_RXTXDATA)FS_PCfgData },
 {SIOC_FORMAT, (T_CSETUP)FS_FrmSetup, SIOS_TXSETUP,(T_RXTXDATA)FS_FrmData },
 {SIOC_HS,     (T_CSETUP)FS_HSSetup,  SIOS_TXSETUP,(T_RXTXDATA)FS_HSData },
 {SIOC_NONE,   (T_CSETUP)NULL,        SIOS_IDLE,   (T_RXTXDATA)NULL }
};
//
// SIO2IDE RESET_OUT
//
#define FRESO                   (1<<3)
#define FS_RES_LO()             __port_or(DDRD,FRESO);__port_and(PORTD,~FRESO)
#define FS_RES_HI()             __port_or(DDRD,FRESO);__port_or(PORTD,FRESO)
//
// FS CFG jumpers
//
#define FCFG3                   (1<<5)          // Master/Slave
#define FCFG1                   (1<<6)          // Init FS
#define FCFG2                   (1<<7)          // HD1
#define FS_CFG_INIT()           __port_and(DDRD,~(FCFG1|FCFG2|FCFG3));\
                                __port_or(PORTD,(FCFG1|FCFG2|FCFG3))
#define FS_CFG1()               (BOOL)((__inp(PIND) & FCFG1) ? FALSE : TRUE)
#define FS_CFG2()               (BOOL)((__inp(PIND) & FCFG2) ? FALSE : TRUE)
#define FS_CFG3()               (BOOL)((__inp(PIND) & FCFG3) ? FALSE : TRUE)

//****************************************************************
//
// FS Implementation
//
//****************************************************************

//----------------------------------------------------------------
// Function :   FS_PutHDP
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL FS_PutHDP( VOID )
{
  UINT16 d;
  UINT8  s;
  UINT8  i = sizeof( hdS2IInfo );
  UINT8 *p = (UINT8*)&hdS2IInfo;
  UINT8  j = sizeof( hdIDTab );
  UINT8 *r = (UINT8*)&hdIDTab;
  UINT8  k = sizeof( hdPart );
  UINT8 *q = (UINT8*)&hdPart;

  if( IDE_WriteSectorInit( &hdInfo, 0 ) )
  {
    do
    {
      if( i )
      {
        d = *p++;
        i--;
      }
      else
      {
        if( j )
        {
          d = *r++;
          j--;
        }
        else
        {
          if( k )
          {
            d = *q++;
            k--;
          }
          else
          {
            d = 0x00;
          }
        }
      }
    }while( (s = IDE_WriteSectorData( d )) == IDE_WR_BSY );
    if( s == IDE_WR_OK )
    {
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   FS_GetHDP
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL FS_GetHDP( VOID )
{
  BOOL   err = FALSE;
  UINT16 d;
  UINT8  s;
  UINT8  i = sizeof( hdS2IInfo );
  UINT8 *p = (UINT8*)&hdS2IInfo;
  UINT8  j = sizeof( hdIDTab );
  UINT8 *r = (UINT8*)&hdIDTab;
  UINT8  k = sizeof( hdPart );
  UINT8 *q = (UINT8*)&hdPart;

  if( IDE_ReadSectorInit( &hdInfo, 0 ) )
  {
    while( (s = IDE_ReadSectorData( &d )) == IDE_RD_BSY )
    {
      if( i )
      {
        i--;
        if( low( d ) != *p++ )  err = TRUE;
      }
      else
      {
        if( j )
        {
          j--;
          *r++ = d;
        }
        else
        {
          if( k )
          {
            k--;
            *q++ = d;
          }
          else
          {
            if( low( d ) != 0x00 )  err = TRUE;
          }
        }
      }
    }
    if( (s != IDE_RD_OK) || err )
    {
      return FALSE;
    }
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   FS_InitHDP
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID FS_InitHDP( VOID )
{
  UINT8  i = sizeof( hdPart );
  UINT8 *p = (UINT8*)&hdPart;
  UINT8  dev = 0;
  UINT32 sec = 1;               // Sector 0 reserved

  while( i-- )
  {
    *p++ = 0;
  }
  while( dev < FS_MAXHDP )
  {
//    hdPart[ dev ].sSec = sec;
    //
    sec += (UINT32)65535;
    if( sec <  hdInfo.sec )
    {
      hdPart[ dev ].nSec = 65535;
    }
    else
    {
      sec -= (UINT32)65535;
      hdPart[ dev ].nSec = hdInfo.sec - sec;
      return;
    }
    dev++;
  }
}

//----------------------------------------------------------------
// Function :   FS_Init
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL FS_Init( VOID )
{
  FS_CFG_INIT();
  //
  if( FS_CFG2() )
  {
    FS_RES_LO();        // Reset active if HD 1 present
  }
  else
  {
    FS_RES_HI();        // Else Reset inactive
  }
  //
  fsRdy = FALSE;
  if( !IDE_Init( &hdInfo ) )
  {
    return FALSE;
  }
  if( FS_CFG1() )
  {
    FS_InitHDP();
    if( !FS_PutHDP() )
    {
      return FALSE;
    }
  }
  if( !FS_GetHDP() )
  {
    return FALSE;
  }

  if( FS_CFG2() )   FS_RES_HI();        // Reset inactive HD 1 Ready

  fsRdy = TRUE;
  return TRUE;
}

//----------------------------------------------------------------
// Function :   FS_CheckDev
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL FS_CheckDev( UINT8 did, UINT8 cmd )
{
  // Device ID (SIO2IDE)
  if( ((did == FS_S2IM) && FS_CFG3())           // Master
      ||
      ((did == FS_S2IS) && !FS_CFG3())          // Slave
    )
  {
    if( (cmd == SIOC_GETHDP)    // Only SIO2IDE Extra commands
        ||
        (cmd == SIOC_PUTHDP)
        ||
        (cmd == SIOC_GETHDI)
      )
    {
      return TRUE;
    }
    return FALSE;
  }
  // Device ID (Disk)
  if( !((did >= FS_DEVL)
      &&
      (did <= FS_DEVH))
    )
  {
    return FALSE;
  }
  // ON/OFF Optional Device
  if( !FS_CFG2() && (did == FS_DEVO) )
  {
    return FALSE;
  }
  did = hdIDTab[ did - FS_DEVL ];
  //
  if( did == FS_NODISK )        return FALSE;
  //
  return (hdPart[ did ].nSec > 0) ? TRUE : FALSE;
}

//----------------------------------------------------------------
// Function :   FS_CalcSN
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC UINT32 FS_CalcSN( UINT8 dev, UINT16 sn )
{
  UINT8  d = 0;
  UINT32 sec = 1;               // Sector 0 reserved (sn >= 1)

  sn--;
  while( d < dev )
  {
    sec += (UINT32)hdPart[ d ].nSec;
    d++;
  }
  sec += (UINT32)sn;

  return sec;
}

//****************************************************************
//
// FS Handlers
//
//****************************************************************

//----------------------------------------------------------------
// Function :   FS_PutSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_PutSetup( T_SIOCMD * cmd, UINT16 * len )
{
  UINT16   sn;
  UINT8    d;

  if( !fsRdy )  return FALSE;

  d  = hdIDTab[ cmd->did - FS_DEVL ];
  sn = (UINT16)(((UINT16)cmd->aux2 << 8) | cmd->aux1);

  if( (sn > hdPart[ d ].nSec)
      ||
      (sn == 0)
    )
    return FALSE;

  fsCSec = FS_CalcSN( d, sn );

  if( (sn >= 1) && (sn <= 3) )  *len = 128;
  else                          *len = FS_SECLEN;
  return TRUE;
}

//----------------------------------------------------------------
// Function :   FS_PutData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_PutData( UINT8 * buf )
{
  UINT8   s;

  if( !IDE_WriteSectorInit( &hdInfo, fsCSec ) )  return FALSE;

  while( (s = IDE_WriteSectorData( (UINT16)*buf )) == IDE_WR_BSY )
  {
    buf++;
  }
  if( s == IDE_WR_OK )  return fsRdy;
  else                  return FALSE;
}

//----------------------------------------------------------------
// Function :   FS_GetSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GetSetup( T_SIOCMD * cmd, UINT16 * len )
{
  UINT16   sn;
  UINT8    d;

  if( !fsRdy )  return FALSE;

  d  = hdIDTab[ cmd->did - FS_DEVL ];
  sn = (UINT16)(((UINT16)cmd->aux2 << 8) | cmd->aux1);

  if( (sn > hdPart[ d ].nSec)
      ||
      (sn == 0)
    )
    return FALSE;

  fsCSec = FS_CalcSN( d, sn );

  if( (sn >= 1) && (sn <= 3) )  *len = 128;
  else                          *len = FS_SECLEN;
  return TRUE;
}

//----------------------------------------------------------------
// Function :   FS_GetData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GetData( UINT8 * buf )
{
  UINT16  d;
  UINT8   s;

  if( !IDE_ReadSectorInit( &hdInfo, fsCSec ) )  return FALSE;

  while( (s = IDE_ReadSectorData( &d )) == IDE_RD_BSY )
  {
    *buf++ = (UINT8)d;
  }
  if( s == IDE_RD_OK )  return fsRdy;
  else                  return FALSE;
}

//----------------------------------------------------------------
// Function :   FS_StatSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_StatSetup( T_SIOCMD * cmd, UINT16 * len )
{
  *len = 4;
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_StatData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_StatData( UINT8 * buf )
{
#if FS_SECLEN==256
  *buf++ = 0x30;        // Motor ON (bit 4), DD (bit 5)
#endif
#if FS_SECLEN==128
  *buf++ = 0x10;        // Motor ON (bit 4)
#endif
  *buf++ = 0xFF;        // Error Status (Inverted)
  *buf++ = 0xE0;        // Format TimeOut
  *buf   = 0x00;        //
  return TRUE;
}

//----------------------------------------------------------------
// Function :   FS_GHDPSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GHDPSetup   ( T_SIOCMD * cmd, UINT16 * len )
{
  *len = sizeof(hdPart) + sizeof(hdIDTab);
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_GHDPData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GHDPData    ( UINT8 * buf )
{
  UINT8  i = sizeof( hdIDTab );
  UINT8 *p = (UINT8*)&hdIDTab;

  while( i-- )
  {
    *buf++ = *p++;
  }
  //
  i = sizeof( hdPart );
  p = (UINT8*)&hdPart;
  while( i-- )
  {
    *buf++ = *p++;
  }
  return TRUE;
}

//----------------------------------------------------------------
// Function :   FS_PHDPSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_PHDPSetup   ( T_SIOCMD * cmd, UINT16 * len )
{
  *len = sizeof(hdPart) + sizeof(hdIDTab);
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_PHDPData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_PHDPData    ( UINT8 * buf )
{
  UINT8  i = sizeof( hdIDTab );
  UINT8 *p = (UINT8*)&hdIDTab;

  while( i-- )
  {
    if( (*buf >= FS_MAXHDP)
        &&
        (*buf != FS_NODISK)
      )
    {
      FS_GetHDP();              // Refresh old HD info
      return FALSE;
    }
    *p++ = *buf++;
  }
  //
  i = sizeof( hdPart );
  p = (UINT8*)&hdPart;
  while( i-- )
  {
    *p++ = *buf++;
  }
  return FS_PutHDP();
}

//----------------------------------------------------------------
// Function :   FS_GHDISetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GHDISetup   ( T_SIOCMD * cmd, UINT16 * len )
{
  *len = sizeof(hdInfo) + sizeof( hdS2IInfo );
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_GHDIData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GHDIData    ( UINT8 * buf )
{
  UINT8  i = sizeof( hdInfo );
  UINT8 *p = (UINT8*)&hdInfo;

  while( i-- )
  {
    *buf++ = *p++;
  }
  //
  i = sizeof( hdS2IInfo );
  p = (UINT8*)&hdS2IInfo;
  while( i-- )
  {
    *buf++ = *p++;
  }
  return TRUE;
}

//----------------------------------------------------------------
// Function :   FS_GCfgSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GCfgSetup( T_SIOCMD * cmd, UINT16 * len )
{
  fsCDrv = hdIDTab[ cmd->did - FS_DEVL ];
  *len = 12;
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_GCfgData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GCfgData( UINT8 * buf )
{
  *buf++ = 0x01;        // Number of Tracks
  *buf++ = 0x10;        // Interface version
  *buf++ = high(hdPart[fsCDrv].nSec);  // Total Number of sectors (high byte)
  *buf++ = low(hdPart[fsCDrv].nSec);   // Total Number of sectors (low byte)
  *buf++ = 0x00;        // Side Code
#if FS_SECLEN==256
  *buf++ = 0x0C;        // IDE HD (bit 3), DD (bit 2)
  *buf++ = 0x01;        // Number of Bytes per sector (high byte)
  *buf++ = 0x00;        // Number of Bytes per sector (low byte)
#endif
#if FS_SECLEN==128
  *buf++ = 0x08;        // IDE HD (bit 3)
  *buf++ = 0x00;        // Number of Bytes per sector (high byte)
  *buf++ = 0x80;        // Number of Bytes per sector (low byte)
#endif
  *buf++ = 0x40;        // Translation Control, Drive Present (bit 6)
  *buf++ = 'I';         //
  *buf++ = 'D';         //
  *buf   = 'E';         //
  return TRUE;
}

//----------------------------------------------------------------
// Function :   FS_PCfgSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_PCfgSetup( T_SIOCMD * cmd, UINT16 * len )
{
  fsCDrv = hdIDTab[ cmd->did - FS_DEVL ];
  *len = 12;
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_PCfgData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_PCfgData( UINT8 * buf )
{
  UINT16   sec;

  sec = (UINT16)(((UINT16)buf[2] << 8) | buf[3]);

  if( hdPart[fsCDrv].nSec != sec )      return FALSE;
  else                                  return TRUE;
}

//----------------------------------------------------------------
// Function :   FS_FrmSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_FrmSetup( T_SIOCMD * cmd, UINT16 * len )
{
  *len = FS_SECLEN;
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_FrmData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_FrmData( UINT8 * buf )
{
  UINT16    i = FS_SECLEN - 2;

  *buf++ = 0xFF;        // No Errors, Format always OK
  *buf++ = 0xFF;        //
  while( i-- )
  {
    *buf++ = 0x00;
  }
  return TRUE;
}

//----------------------------------------------------------------
// Function :   FS_HSSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_HSSetup( T_SIOCMD * cmd, UINT16 * len )
{
  *len = 1;
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_HSData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_HSData( UINT8 * buf )
{
  *buf = 0x0A;        // 52000 baud
  return TRUE;
}

//      End