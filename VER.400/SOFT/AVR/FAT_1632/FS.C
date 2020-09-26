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
// Static data
//
STATIC       BOOL            fsRdy;     // FS ready flag
STATIC       UINT8           fsCDrv;    // Current Drive nember
STATIC       UINT16          fsCSec;    // Current Sector number

//
// SIO Command Handlers
//
STATIC BOOL  FS_PutSetup    ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_PutData     ( UINT8 * buf );
STATIC BOOL  FS_GetSetup    ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_GetData     ( UINT8 * buf );
STATIC BOOL  FS_StatSetup   ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_StatData    ( UINT8 * buf );
STATIC BOOL  FS_GCfgSetup   ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_GCfgData    ( UINT8 * buf );
STATIC BOOL  FS_PCfgSetup   ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_PCfgData    ( UINT8 * buf );
STATIC BOOL  FS_FrmSetup    ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_FrmData     ( UINT8 * buf );
STATIC BOOL  FS_HSSetup     ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_HSData      ( UINT8 * buf );
//
STATIC BOOL  FS_GDSKSetup   ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_GDSKData    ( UINT8 * buf );
STATIC BOOL  FS_PDSKSetup   ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_PDSKData    ( UINT8 * buf );
STATIC BOOL  FS_FATRSetup   ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_FATRData    ( UINT8 * buf );
STATIC BOOL  FS_NATRSetup   ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_NATRData    ( UINT8 * buf );
STATIC BOOL  FS_SCFGSetup   ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_SCFGData    ( UINT8 * buf );
STATIC BOOL  FS_ODSKSetup   ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_ODSKData    ( UINT8 * buf );
STATIC BOOL  FS_GDRVSetup   ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_GDRVData    ( UINT8 * buf );
STATIC BOOL  FS_GFSISetup   ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_GFSIData    ( UINT8 * buf );
STATIC BOOL  FS_FDIRSetup   ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_FDIRData    ( UINT8 * buf );
STATIC BOOL  FS_NDIRSetup   ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_NDIRData    ( UINT8 * buf );
STATIC BOOL  FS_SCDIRSetup  ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_SCDIRData   ( UINT8 * buf );
STATIC BOOL  FS_GCDIRSetup  ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_GCDIRData   ( UINT8 * buf );
STATIC BOOL  FS_ICDIRSetup  ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_ICDIRData   ( UINT8 * buf );
//
STATIC BOOL  FS_SSECTSetup  ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_SSECTData   ( UINT8 * buf );
STATIC BOOL  FS_PSECTSetup  ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_PSECTData   ( UINT8 * buf );
STATIC BOOL  FS_GSECTSetup  ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_GSECTData   ( UINT8 * buf );
STATIC BOOL  FS_GLFNSetup   ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_GLFNData    ( UINT8 * buf );
STATIC BOOL  FS_CDRVSetup   ( T_SIOCMD * cmd, UINT16 * len );
STATIC BOOL  FS_CDRVData    ( UINT8 * buf );

//
// SIO Handlers Table
//
FLASH TF_SIOHNDL          sioHndl[] =
{
 {SIOC_STAT,   (T_CSETUP)FS_StatSetup,SIOS_TXSETUP,(T_RXTXDATA)FS_StatData },
 {SIOC_PUT,    (T_CSETUP)FS_PutSetup, SIOS_RXSETUP,(T_RXTXDATA)FS_PutData },
 {SIOC_PUTV,   (T_CSETUP)FS_PutSetup, SIOS_RXSETUP,(T_RXTXDATA)FS_PutData },
 {SIOC_GET,    (T_CSETUP)FS_GetSetup, SIOS_TXSETUP,(T_RXTXDATA)FS_GetData },
 {SIOC_GCFG,   (T_CSETUP)FS_GCfgSetup,SIOS_TXSETUP,(T_RXTXDATA)FS_GCfgData },
 {SIOC_PCFG,   (T_CSETUP)FS_PCfgSetup,SIOS_RXSETUP,(T_RXTXDATA)FS_PCfgData },
 {SIOC_FORMAT, (T_CSETUP)FS_FrmSetup, SIOS_TXSETUP,(T_RXTXDATA)FS_FrmData },
 {SIOC_HS,     (T_CSETUP)FS_HSSetup,  SIOS_TXSETUP,(T_RXTXDATA)FS_HSData },
 //
 {SIOC_GETDSK, (T_CSETUP)FS_GDSKSetup,SIOS_TXSETUP,(T_RXTXDATA)FS_GDSKData },
 {SIOC_PUTDSK, (T_CSETUP)FS_PDSKSetup,SIOS_RXSETUP,(T_RXTXDATA)FS_PDSKData },
 {SIOC_FATR,   (T_CSETUP)FS_FATRSetup,SIOS_TXSETUP,(T_RXTXDATA)FS_FATRData },
 {SIOC_NATR,   (T_CSETUP)FS_NATRSetup,SIOS_TXSETUP,(T_RXTXDATA)FS_NATRData },
 {SIOC_SCFG,   (T_CSETUP)FS_SCFGSetup,SIOS_TXSETUP,(T_RXTXDATA)FS_SCFGData },
 {SIOC_OFFDSK, (T_CSETUP)FS_ODSKSetup,SIOS_TXSETUP,(T_RXTXDATA)FS_ODSKData },
 {SIOC_GETDRV, (T_CSETUP)FS_GDRVSetup,SIOS_TXSETUP,(T_RXTXDATA)FS_GDRVData },
 {SIOC_GETFSI, (T_CSETUP)FS_GFSISetup,SIOS_TXSETUP,(T_RXTXDATA)FS_GFSIData },
 {SIOC_FDIR,   (T_CSETUP)FS_FDIRSetup,SIOS_TXSETUP,(T_RXTXDATA)FS_FDIRData },
 {SIOC_NDIR,   (T_CSETUP)FS_NDIRSetup,SIOS_TXSETUP,(T_RXTXDATA)FS_NDIRData },
 {SIOC_SCDIR,  (T_CSETUP)FS_SCDIRSetup,SIOS_RXSETUP,(T_RXTXDATA)FS_SCDIRData },
 {SIOC_GCDIR,  (T_CSETUP)FS_GCDIRSetup,SIOS_TXSETUP,(T_RXTXDATA)FS_GCDIRData },
 {SIOC_ICDIR,  (T_CSETUP)FS_ICDIRSetup,SIOS_TXSETUP,(T_RXTXDATA)FS_ICDIRData },
 //
 {SIOC_SSECT,  (T_CSETUP)FS_SSECTSetup,SIOS_RXSETUP,(T_RXTXDATA)FS_SSECTData },
 {SIOC_GSECT,  (T_CSETUP)FS_GSECTSetup,SIOS_TXSETUP,(T_RXTXDATA)FS_GSECTData },
 {SIOC_PSECT,  (T_CSETUP)FS_PSECTSetup,SIOS_RXSETUP,(T_RXTXDATA)FS_PSECTData },
 {SIOC_GLFN,   (T_CSETUP)FS_GLFNSetup,SIOS_TXSETUP,(T_RXTXDATA)FS_GLFNData },
 {SIOC_CDRV,   (T_CSETUP)FS_CDRVSetup,SIOS_RXSETUP,(T_RXTXDATA)FS_CDRVData },
 //
 {SIOC_NONE,   (T_CSETUP)NULL,        SIOS_IDLE,   (T_RXTXDATA)NULL }
};

//
// FS CFG jumpers
//
//   Port (PB)
#define FCFG2                   (1<<6)          // ON/OFF device D1
#define FCFG3                   (1<<5)          // Master/Slave
#define FS_CFG_INIT()           __port_and(DDRB,~(FCFG2|FCFG3));\
                                __port_or(PORTB,(FCFG2|FCFG3))
#define FS_CFG2()               (BOOL)((__inp(PINB) & FCFG2) ? FALSE : TRUE)
#define FS_CFG3()               (BOOL)((__inp(PINB) & FCFG3) ? FALSE : TRUE)

//****************************************************************
//
// FS Implementation
//
//****************************************************************

//----------------------------------------------------------------
// Function :   FS_Init
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL FS_Init( VOID )
{
  FS_CFG_INIT();
  //
//  if( FS_CFG2() )
  {
    FS_RES_LO();        // Reset active if HD 1 present
  }
//  else
  {
//    FS_RES_HI();        // Else Reset inactive
  }
  //
  fsRdy = FALSE;

  if( !FATFS_Init() )
  {
    FS_RES_HI();        // Else Reset inactive
    return FALSE;
  }

//  if( FS_CFG2() )
  {
    FS_RES_HI();        // Reset inactive HD 1 Ready
  }

  fsRdy = TRUE;
  return TRUE;
}

//----------------------------------------------------------------
// Function :   FS_CheckDev
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL FS_CheckDev( UINT8 * did, UINT8 cmd )
{
  //
  // Device ID (SIO2IDE)
  //
  if( ((*did == FS_S2IM) && FS_CFG3())           // Master
      ||
      ((*did == FS_S2IS) && !FS_CFG3())          // Slave
    )
  {
    //
    // Only SIO2IDE Extra commands
    //
    if( (cmd == SIOC_GETDSK)  ||
        (cmd == SIOC_PUTDSK)  ||
        (cmd == SIOC_SCFG)    ||
        (cmd == SIOC_FATR)    ||
        (cmd == SIOC_NATR)    ||
        (cmd == SIOC_OFFDSK)  ||
        (cmd == SIOC_GETDRV)  ||
        (cmd == SIOC_GETFSI)  ||
        (cmd == SIOC_FDIR)    ||
        (cmd == SIOC_NDIR)    ||
        (cmd == SIOC_SCDIR)   ||
        (cmd == SIOC_GCDIR)   ||
        (cmd == SIOC_ICDIR)   ||
        (cmd == SIOC_SSECT)   ||
        (cmd == SIOC_GSECT)   ||
        (cmd == SIOC_PSECT)   ||
        (cmd == SIOC_GLFN)    ||
        (cmd == SIOC_CDRV)
      )
    {
      return TRUE;
    }
    return FALSE;
  }
  //
  // Device ID (Disk)
  //
  if( !((*did >= FS_DEVL)
      &&
      (*did <= FS_DEVH))
    )
  {
    return FALSE;
  }
  //
  // ON/OFF Optional Device (D1<->D9 assign Common drive)
  //
  if( !FS_CFG2() && (*did == FS_DEVO) )
  {
    *did = FS_DEVH + 1;         // D9 as D1 (Common drive)
//    return FALSE;
  }
  //
  // Swap devices
  //
//  if( FS_CFG1() )
//  {
//    if( *did == FS_DSW1 )
//    {
//      *did = FS_DSW2;
//    }
//    else if( *did == FS_DSW2 )
//    {
//      *did = FS_DSW1;
//    }
//  }
  //
  return FATFS_DiskSize( *did - FS_DEVL + 1 ) ? TRUE : FALSE;
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
  if( !fsRdy )  return FALSE;

  fsCDrv = cmd->did - FS_DEVL + 1;
  fsCSec = (UINT16)(((UINT16)cmd->aux2 << 8) | cmd->aux1);

  if( (fsCSec > FATFS_DiskSize( fsCDrv ) ) ||
      (fsCSec == 0)
    )
    return FALSE;

  if( (fsCSec >= 1) && (fsCSec <= 3) )  *len = 128;
  else                                  *len = FATFS_DiskSecSize( fsCDrv );
  //
  return TRUE;
}

//----------------------------------------------------------------
// Function :   FS_PutData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_PutData( UINT8 * buf )
{
  return FATFS_PutFileSec( fsCDrv, fsCSec, buf );
}

//----------------------------------------------------------------
// Function :   FS_GetSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GetSetup( T_SIOCMD * cmd, UINT16 * len )
{
  if( !fsRdy )  return FALSE;

  fsCDrv = cmd->did - FS_DEVL + 1;
  fsCSec = (UINT16)(((UINT16)cmd->aux2 << 8) | cmd->aux1);

  if( (fsCSec > FATFS_DiskSize( fsCDrv ) ) ||
      (fsCSec == 0)
    )
    return FALSE;

  if( (fsCSec >= 1) && (fsCSec <= 3) )  *len = 128;
  else                                  *len = FATFS_DiskSecSize( fsCDrv );
  //
  return TRUE;
}

//----------------------------------------------------------------
// Function :   FS_GetData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GetData( UINT8 * buf )
{
  return FATFS_GetFileSec( fsCDrv, fsCSec, buf );
}

//----------------------------------------------------------------
// Function :   FS_StatSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_StatSetup( T_SIOCMD * cmd, UINT16 * len )
{
  fsCDrv = cmd->did - FS_DEVL + 1;
  *len = 4;
  return FATFS_DiskSize( fsCDrv ) == 0 ? FALSE : fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_StatData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_StatData( UINT8 * buf )
{
  if( FATFS_DiskSecSize( fsCDrv ) == 256 )
  {
    *buf++ = 0x30;        // Motor ON (bit 4), DD (bit 5)
  }
  else
  {
    *buf++ = 0x10;        // Motor ON (bit 4)
  }
  *buf++ = 0xFF;        // Error Status (Inverted)
  *buf++ = 0xE0;        // Format TimeOut
  *buf   = 0x00;        //
  return TRUE;
}

//----------------------------------------------------------------
// Function :   FS_GCfgSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GCfgSetup( T_SIOCMD * cmd, UINT16 * len )
{
  fsCDrv = cmd->did - FS_DEVL + 1;
  *len = 12;
  return FATFS_DiskSize( fsCDrv ) == 0 ? FALSE : fsRdy;
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
  *buf++ = high(FATFS_DiskSize( fsCDrv ));  // Total Number of sectors (high byte)
  *buf++ = low(FATFS_DiskSize( fsCDrv ));   // Total Number of sectors (low byte)
  *buf++ = 0x00;        // Side Code
  //
  if( FATFS_DiskSecSize( fsCDrv ) == 256 )
  {
    *buf++ = 0x0C;        // IDE HD (bit 3), DD (bit 2)
    *buf++ = 0x01;        // Number of Bytes per sector (high byte)
    *buf++ = 0x00;        // Number of Bytes per sector (low byte)
  }
  else
  {
    *buf++ = 0x08;        // IDE HD (bit 3)
    *buf++ = 0x00;        // Number of Bytes per sector (high byte)
    *buf++ = 0x80;        // Number of Bytes per sector (low byte)
  }
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
  fsCDrv = cmd->did - FS_DEVL + 1;
  *len = 12;
  return FATFS_DiskSize( fsCDrv ) == 0 ? FALSE : fsRdy;
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

  if( FATFS_DiskSize( fsCDrv ) != sec )      return FALSE;
  else                                       return TRUE;
}

//----------------------------------------------------------------
// Function :   FS_FrmSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_FrmSetup( T_SIOCMD * cmd, UINT16 * len )
{
  fsCDrv = cmd->did - FS_DEVL + 1;
  *len = FATFS_DiskSecSize( fsCDrv );
  return *len == 0 ? FALSE : fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_FrmData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_FrmData( UINT8 * buf )
{
  UINT16    i = FATFS_DiskSecSize( fsCDrv ) - 2;

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

//
// SIO2IDE specific commands
//

//----------------------------------------------------------------
// Function :   FS_GDSKSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GDSKSetup( T_SIOCMD * cmd, UINT16 * len )
{
  fsCDrv = cmd->aux1;
  *len   = sizeof(T_file);
  return FATFS_DiskCheck( fsCDrv );
}

//----------------------------------------------------------------
// Function :   FS_GDSKData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GDSKData( UINT8 * buf )
{
  return FATFS_DiskGet( fsCDrv, (T_file*)buf );
}

//----------------------------------------------------------------
// Function :   FS_PDSKSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_PDSKSetup( T_SIOCMD * cmd, UINT16 * len )
{
  fsCDrv = cmd->aux1;
  *len   = sizeof(T_file);
  return FATFS_DiskCheck( fsCDrv );
}

//----------------------------------------------------------------
// Function :   FS_PDSKData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_PDSKData( UINT8 * buf )
{
  return FATFS_DiskSet( fsCDrv, (T_file*)buf );
}

//----------------------------------------------------------------
// Function :   FS_FATRSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_FATRSetup( T_SIOCMD * cmd, UINT16 * len )
{
  *len   = sizeof(T_file);
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_FATRData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_FATRData( UINT8 * buf )
{
  return FATFS_GetFirstFile( (T_file*)buf );
}

//----------------------------------------------------------------
// Function :   FS_NATRSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_NATRSetup( T_SIOCMD * cmd, UINT16 * len )
{
  *len   = sizeof(T_file);
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_NATRData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_NATRData( UINT8 * buf )
{
  return FATFS_GetNextFile( (T_file*)buf );
}

//----------------------------------------------------------------
// Function :   FS_ODSKSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_ODSKSetup( T_SIOCMD * cmd, UINT16 * len )
{
  *len = 1;
  return FATFS_DiskOff( cmd->aux1 );
}

//----------------------------------------------------------------
// Function :   FS_ODSKData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_ODSKData( UINT8 * buf )
{
  *buf = 0xFF;
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_SCFGSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_SCFGSetup( T_SIOCMD * cmd, UINT16 * len )
{
  *len = 1;
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_SCFGData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_SCFGData( UINT8 * buf )
{
  *buf = 0xFF;
  return FATFS_SaveConfig();
}

//----------------------------------------------------------------
// Function :   FS_GDRVSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GDRVSetup( T_SIOCMD * cmd, UINT16 * len )
{
  *len   = sizeof(T_drvinf);
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_GDRVData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GDRVData( UINT8 * buf )
{
  FATFS_GetDrvInfo( (T_drvinf*)buf );
  return TRUE;
}

//----------------------------------------------------------------
// Function :   FS_GFSISetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GFSISetup( T_SIOCMD * cmd, UINT16 * len )
{
  *len   = sizeof(T_fsinf);
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_GFSIData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GFSIData( UINT8 * buf )
{
  FATFS_GetFsInfo( (T_fsinf*)buf );
  return TRUE;
}

//----------------------------------------------------------------
// Function :   FS_FDIRSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_FDIRSetup( T_SIOCMD * cmd, UINT16 * len )
{
  *len   = sizeof(T_file);
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_FDIRData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_FDIRData( UINT8 * buf )
{
  return FATFS_GetFirstDir( (T_file*)buf );
}

//----------------------------------------------------------------
// Function :   FS_NDIRSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_NDIRSetup( T_SIOCMD * cmd, UINT16 * len )
{
  *len   = sizeof(T_file);
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_NDIRData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_NDIRData( UINT8 * buf )
{
  return FATFS_GetNextDir( (T_file*)buf );
}

//----------------------------------------------------------------
// Function :   FS_SCDIRSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_SCDIRSetup( T_SIOCMD * cmd, UINT16 * len )
{
  *len   = sizeof(T_file);
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_SCDIRData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_SCDIRData( UINT8 * buf )
{
  return FATFS_SetCurDir( (T_file*)buf );
}

//----------------------------------------------------------------
// Function :   FS_GCDIRSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GCDIRSetup( T_SIOCMD * cmd, UINT16 * len )
{
  *len   = sizeof(T_file);
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_GCDIRData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GCDIRData( UINT8 * buf )
{
  FATFS_GetCurDir( (T_file*)buf );
  return TRUE;
}

//----------------------------------------------------------------
// Function :   FS_ICDIRSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_ICDIRSetup( T_SIOCMD * cmd, UINT16 * len )
{
  *len   = sizeof(T_file);
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_ICDIRData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_ICDIRData( UINT8 * buf )
{
  if( FATFS_InitCurDir( (T_file*)buf  ) )
  {
    FATFS_GetCurDir( (T_file*)buf );
    //
    FATFS_SaveCurDir();
  }
  else
  {
    memset( buf, 0, sizeof(T_file) );
  }
  return TRUE;
}

//****************************************************************
//
// Additional (extended) SIO commands
//
//****************************************************************

EXTERN       T_drvinf   drvInf;         // Drive phisical info
STATIC       UINT32     phSect;         // Current Phisical Sector number

//----------------------------------------------------------------
// Function :   FS_SSECTSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_SSECTSetup( T_SIOCMD * cmd, UINT16 * len )
{
  if( drvInf.flg & DRV_CD )
  {
    return FALSE;
  }
  *len   = sizeof(UINT32);
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_SSECTData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_SSECTData( UINT8 * buf )
{
  phSect = *(UINT32*)buf;

  if( phSect > IDE_GetMaxSec( &drvInf ) )
  {
    return FALSE;
  }
  return TRUE;
}

//----------------------------------------------------------------
// Function :   FS_GSECTSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GSECTSetup( T_SIOCMD * cmd, UINT16 * len )
{
  if( drvInf.flg & DRV_CD )
  {
    return FALSE;
  }
  *len   = SECTOR_SIZE;
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_GSECTData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GSECTData( UINT8 * buf )
{
  return IDE_SectorGet( (UINT16*)buf, &drvInf, phSect );
}

//----------------------------------------------------------------
// Function :   FS_PSECTSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_PSECTSetup( T_SIOCMD * cmd, UINT16 * len )
{
  if( drvInf.flg & DRV_CD )
  {
    return FALSE;
  }
  *len   = SECTOR_SIZE;
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_PSECTData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_PSECTData( UINT8 * buf )
{
  return IDE_SectorPut( (UINT16*)buf, &drvInf, phSect );
}

//----------------------------------------------------------------
// Function :   FS_GLFNSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GLFNSetup( T_SIOCMD * cmd, UINT16 * len )
{
  *len   = LFN_MAX_SIZE;
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_GLFNData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_GLFNData( UINT8 * buf )
{
  return FATFS_GetLFN( phSect, buf );
}

//----------------------------------------------------------------
// Function :   FS_CDRVSetup
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_CDRVSetup( T_SIOCMD * cmd, UINT16 * len )
{
  *len = 1;
  return fsRdy;
}

//----------------------------------------------------------------
// Function :   FS_CDRVData
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  FS_CDRVData( UINT8 * buf )
{
  UINT8  oldd;

  if( (oldd = IDE_GetDrv()) == *buf )
  {
    return TRUE;
  }
  if( IDE_ChgDrv( *buf ) )
  {
    if( FATFS_Init() )
    {
      return TRUE;
    }
    else
    {
      IDE_ChgDrv( oldd );
      if( FATFS_Init() )
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}

//      End
