//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: ide.c
//
// MODULE NAME: ide
//
// PURPOSE:     IDE bus driver module. ATA & ATAPI interface
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

#ifdef DEBUG
//  #define         DEBUG_IDE
#endif

#define SWPB( a )       ((a << 8) | (a >> 8))
//
// IDE Data Bus definitions (16bit)
//
#define IDE_DATA_HIZ()          __outp(DDRA,0xFF);__outp(PORTA,0xFF);\
                                __outp(DDRB,0xFF);__outp(PORTB,0xFF)
#define IDE_DATA_INP()          __outp(DDRA,0x00);__outp(PORTA,0xFF);\
                                __outp(DDRB,0x00);__outp(PORTB,0xFF)
#define IDE_DATA_OUT()          __outp(DDRA,0xFF);__outp(DDRB,0xFF)
#define IDE_DATA_GETL()         (UINT8)__inp(PINA)
#define IDE_DATA_GETH()         (UINT8)__inp(PINB)
#define IDE_DATA_PUTL( d )      __outp(PORTA,d)
#define IDE_DATA_PUTH( d )      __outp(PORTB,d)
//
// IDE Control Bus definitions
//
#define HA00                    (1<<2)
#define HA01                    (1<<3)
#define HA02                    (1<<4)
#define HCS0                    (1<<1)
#define HCS1                    (1<<0)
#define HIOW                    (1<<7)
#define HIOR                    (1<<6)
#define RDYL                    (1<<5)
#define IDE_ADR_MASK            (HCS1|HCS0|HA02|HA01|HA00)
#define IDE_CTRL_INIT()         __outp(DDRC,0xFF);__outp(PORTC,IDE_ADR_MASK)
#define IDE_CTRL_RD()           __port_and(PORTC,~HIOR)
#define IDE_CTRL_WR()           __port_and(PORTC,~HIOW)
#define IDE_CTRL_NO_RD()        __port_or(PORTC,HIOR)
#define IDE_CTRL_NO_WR()        __port_or(PORTC,HIOW)
#define IDE_CTRL_REG( a )       __outp(PORTC,((__inp(PORTC) & ~IDE_ADR_MASK) | a))
#define IDE_RDYL_ON()           __port_and(PORTC,~RDYL)
#define IDE_RDYL_OFF()          __port_or(PORTC,RDYL)
//
// IDE Registers
//
#define IDE_REG_NONE            (HCS1|HCS0|HA02|HA01|HA00)
#define IDE_REG_CTRL            (HCS0|HA02|HA01)       // R/W  Device Ctrl
#define IDE_REG_ADDR            (HCS0|HA02|HA01|HA00)  // R    Drive Address
#define IDE_REG_DATA            (HCS1)                 // R/W  Data
#define IDE_REG_ERR             (HCS1|HA00)            // R    Error Status
#define IDE_REG_FR              (HCS1|HA00)            // W    Feature Register
#define IDE_REG_SC              (HCS1|HA01)            // R/W  Sectors to R/W 0-256
#define IDE_REG_SN              (HCS1|HA01|HA00)       // R/W  Sector
#define IDE_REG_CYLO            (HCS1|HA02)            // R/W  Cylinder Low
#define IDE_REG_CYHI            (HCS1|HA02|HA00)       // R/W  Cylinder High
#define IDE_REG_DH              (HCS1|HA02|HA01)       // R/W  Drive+Head (CHS mode) 1010xxxx - master 1011xxxx - slave
#define IDE_REG_STAT            (HCS1|HA02|HA01|HA00)  // R/W  Status/Command
#define IDE_REG_CMD             (HCS1|HA02|HA01|HA00)  // R/W  Status/Command
//
// Error Bits (IDE_ERR)
//
#define IDEE_BBK        (UINT8)(1 << 7)         // Bad Block Detected
#define IDEE_UNC        (UINT8)(1 << 6)         // Uncorrectable Data Error
#define IDEE_MC         (UINT8)(1 << 5)         // Media Change
#define IDEE_IDNF       (UINT8)(1 << 4)         // ID Not Found
#define IDEE_ABRT       (UINT8)(1 << 3)         // Aborted Commnd
#define IDEE_MCR        (UINT8)(1 << 2)         // Media Change Requested
#define IDEE_TK0NF      (UINT8)(1 << 1)         // Track 0 Not Found
#define IDEE_AMNF       (UINT8)(1 << 0)         // Address Mark Not Found
//
// Status Bits (IDE_STAT)
//
#define IDES_BSY        (UINT8)(1 << 7)         // Drive Busy
#define IDES_DRDY       (UINT8)(1 << 6)         // Drive Ready
#define IDES_DWF        (UINT8)(1 << 5)         // Drive Write Fault
#define IDES_DSC        (UINT8)(1 << 4)         // Drive Seek Complete
#define IDES_DRQ        (UINT8)(1 << 3)         // Data Request
#define IDES_CORR       (UINT8)(1 << 2)         // Corrected Data
#define IDES_IDX        (UINT8)(1 << 1)         // Index
#define IDES_ERR        (UINT8)(1 << 0)         // Error
//
// Control Bits (IDE_CTRL)
//
#define IDEC_HD15       (UINT8)(1 << 3)         // bit should always be set to one
#define IDEC_SRST       (UINT8)(1 << 2)         // soft reset
#define IDEC_NIEN       (UINT8)(1 << 1)         // disable interrupts

#include "atapi.h"

//****************************************************************
//
// Static Data
//
//****************************************************************
#ifdef IDE_INITRW
STATIC  DATA UINT16     RWcnt;          // RW data transfer word counter
#endif

//****************************************************************
//
// Implementation
//
//****************************************************************

//
// IDE Low Level Driver
//

//----------------------------------------------------------------
// Function :   IDE_Delay
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID IDE_Delay( UINT32 del )
{
  del <<= DEL_FACTOR;
  while( del-- )
  {
    _NOP();
  }
}

//----------------------------------------------------------------
// Function :   IDE_ErrorBlink
// Notes    :
// History  :
//----------------------------------------------------------------

VOID IDE_ErrorBlink( VOID )
{
  IDE_Delay( 100000 );
  IDE_RDYL_ON();
  IDE_Delay( 200000 );
  IDE_RDYL_OFF();
}

//----------------------------------------------------------------
// Function :   IDE_Swpb
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC UINT8 IDE_Swpb( UINT8 b )
{
  UINT8  m1 = 0x01;
  UINT8  m2 = 0x80;
  UINT8   o = 0;

  while( m1 )
  {
    if( b & m1 )        o |= m2;
    m1 <<= 1;
    m2 >>= 1;
  }
  return o;
}

//----------------------------------------------------------------
// Function :   IDE_RegWR
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID IDE_RegWR( UINT8 reg, UINT16 dat )
{
  UINT8  l, h;                  //
                                //
  l = low( dat );               //
  h = high( dat );              //
  l = IDE_Swpb( l );            //
  IDE_DATA_OUT();               // IDE Data Bus in Out mode

  disable_interrupt();

  IDE_CTRL_REG( reg );          // Set Register Address
  IDE_CTRL_WR();                // WR ON (low)
  IDE_DATA_PUTL( l );           // Set Data Low
  IDE_DATA_PUTH( h );           // Set Data High
  IDE_CTRL_NO_WR();             // WR OFF (high) rising edge on WR
  _NOP();                       //
  IDE_CTRL_REG( IDE_REG_NONE ); // Release Device

  enable_interrupt();

  IDE_DATA_HIZ();               // IDE Data Bus in Hi-Z mode
}

//----------------------------------------------------------------
// Function :   IDE_RegRD
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC UINT16 IDE_RegRD( UINT8 reg )
{
  UINT8  l, h;

  IDE_DATA_INP();               // IDE Data Bus in Inp mode

  disable_interrupt();

  IDE_CTRL_REG( reg );          // Set Register Address
  IDE_CTRL_RD();                // RD ON (low)
  _NOP();                       //
  IDE_CTRL_NO_RD();             // RD OFF (high) rising edge on RD
  l = IDE_DATA_GETL();          // Get Data Low
  h = IDE_DATA_GETH();          // Get Data High
  IDE_CTRL_REG( IDE_REG_NONE ); // Release Device

  enable_interrupt();

  IDE_DATA_HIZ();               // IDE Data Bus in Hi-Z mode
  l = IDE_Swpb( l );            //
                                //
  return (UINT16)(((UINT16)h << 8) | l);
}

//----------------------------------------------------------------
// Function :   IDE_WaitBSY
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL IDE_WaitBSY( BOOL tout )
{
  UINT8   stat;
  UINT16  try = IDE_TOUT;               //
                                        //
  do                                    //
  {                                     //
    if( tout )                          //
    if( --try == 0 )                    //
    {                                   //
      IDE_RDYL_ON();                    //
      return FALSE;                     // TimeOut
    }                                   //
                                        //
    stat = IDE_RegRD( IDE_REG_STAT );   // Read Drive Status
  }while( stat & IDES_BSY );            // Wait if Drive is Busy
  return TRUE;                          //
}

//----------------------------------------------------------------
// Function :   IDE_WaitRDY
// Notes    :
// History  :
//----------------------------------------------------------------
/*
STATIC BOOL IDE_WaitRDY( BOOL tout )
{
  UINT8   stat;
  UINT16  try = IDE_TOUT;               //
                                        //
  do                                    //
  {                                     //
    if( tout )                          //
    if( --try == 0 )                    //
    {                                   //
      IDE_RDYL_ON();                    //
      return FALSE;                     // TimeOut
    }                                   //
                                        //
    stat = IDE_RegRD( IDE_REG_STAT );   // Read Drive Status
  }while( !(stat & IDES_DRDY) );        // Wait if Drive NOT Ready
  return TRUE;                          //
}
*/

//----------------------------------------------------------------
// Function :   IDE_WaitDRQ
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL IDE_WaitDRQ( BOOL tout )
{
  UINT8   stat;
  UINT16  try = IDE_TOUT;               //
                                        //
  do                                    //
  {                                     //
    if( tout )                          //
    if( --try == 0 )                    //
    {                                   //
      IDE_RDYL_ON();                    //
      return FALSE;                     // TimeOut
    }                                   //
                                        //
    stat = IDE_RegRD( IDE_REG_STAT );   // Read Drive Status
  }while( !(stat & IDES_DRQ) );         // Wait if Data NOT Ready
  return TRUE;                          //
}

//----------------------------------------------------------------
// Function :   IDE_Err
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL IDE_Err( VOID )
{
  if( (IDE_RegRD( IDE_REG_STAT )        // If ERROR?
       & IDES_ERR)                      //
    )                                   //
  {                                     // YES
    IDE_RDYL_ON();                      //
    return TRUE;                        //
  }                                     //
  else                                  // NO
  {                                     //
    IDE_RDYL_OFF();                     //
    return FALSE;                       //
  }                                     //
}

//
// IDE ATA interface (Identify Drive, Read, Write)
//

//----------------------------------------------------------------
// Function :   IDE_ReadSectorInit
// Notes    :   sn (0 - 0x000FFFFF)
// History  :
//----------------------------------------------------------------

BOOL IDE_ReadSectorInit( T_drvinf * DrvInfo, UINT32 sn )
{
  UINT8   hd;                           //
  UINT8   sec;                          //
  UINT16  cyl;                          //
                                        //
                                        //
  if( DrvInfo->sec < sn ) return FALSE; //
                                        // Calculate parameters
  if( !(DrvInfo->flg & DRV_LBA) )
  {
    sec  = 1 + (sn  %  (UINT32)DrvInfo->spt);
    hd   = (sn / (UINT32)DrvInfo->spt)  %  (UINT32)DrvInfo->hd;
    cyl  = sn / ((UINT32)DrvInfo->spt * (UINT32)DrvInfo->hd);
    hd   = 0xA0 + (hd & 0x0F);          // Select Head in Master drive (CSH mode)
  }
  else
  {
    sec = sn & 0xFF;
    cyl = (sn >> 8) & 0xFFFF;
    hd  = (sn >> 24) & 0x0F;
    hd  += 0xE0;                        // Select Head in Master drive (LBA mode)
  }

#ifdef DEBUG_IDE
  Printf( "\nS=%i", (UINT16)sn );
  Printf( " C=%i", (UINT16)cyl );
  Printf( " H=%i", (UINT16)hd );
  Printf( " S=%i", (UINT16)sec );
#endif
                                        //
#ifdef IDE_INITRW
  RWcnt = 256;                          //
#endif
                                        //
  IDE_RegRD( IDE_REG_ERR );             // Clear any errors
                                        //
  if( !IDE_WaitBSY( TRUE ) )            //
    return FALSE;                       //
                                        //
  IDE_RegWR( IDE_REG_DH, hd );          //
  IDE_RegWR( IDE_REG_SC, 1 );           // Sector Count to read (1 sector)
  IDE_RegWR( IDE_REG_SN, sec );         // Sector Number to read
  IDE_RegWR( IDE_REG_CYLO, low(cyl) );  // Cylinder Number LOW
  IDE_RegWR( IDE_REG_CYHI, high(cyl) ); // Cylinder Number HIGH
                                        //
  IDE_RegWR( IDE_REG_CMD, 0x21 );       // Write Command (READ)
                                        //
  if( !IDE_WaitDRQ( TRUE ) )            //
    return FALSE;                       //
                                        //
  if( !IDE_Err() )                      // If Data OK?
  {                                     // YES
    return TRUE;                        //   Init OK
  }                                     // NO
  return FALSE;                         //   Reading Init error
}

//----------------------------------------------------------------
// Function :   IDE_ReadSectorData
// Notes    :
// History  :
//----------------------------------------------------------------
#ifdef IDE_INITRW
UINT8 IDE_ReadSectorData( UINT16 * dat )
{
  if( RWcnt )                           //
  {                                     //
    RWcnt--;                            //
    *dat = IDE_RegRD( IDE_REG_DATA );   //
    return IDE_RD_BSY;                  //
  }                                     //
  *dat = 0;                             //
  return IDE_RD_OK;                     //
}
#endif

//----------------------------------------------------------------
// Function :   IDE_WriteSectorInit
// Notes    :   sn (0 - 0x000FFFFF)
// History  :
//----------------------------------------------------------------

BOOL IDE_WriteSectorInit( T_drvinf * DrvInfo, UINT32 sn )
{
  UINT8   hd;                           //
  UINT8   sec;                          //
  UINT16  cyl;                          //
                                        //
  if( DrvInfo->sec < sn ) return FALSE; //
                                        // Calculate parameters
  if( DrvInfo->flg & DRV_CD )
  {
    return FALSE;
  }
  if( !(DrvInfo->flg & DRV_LBA) )
  {
    sec  = 1 + (sn  %  (UINT32)DrvInfo->spt);
    hd   = (sn / (UINT32)DrvInfo->spt)  %  (UINT32)DrvInfo->hd;
    cyl  = sn / ((UINT32)DrvInfo->spt * (UINT32)DrvInfo->hd);
    hd = 0xA0 + (hd & 0x0F);              // Select Head in Master drive (CSH mode)
  }
  else
  {
    sec = sn & 0xFF;
    cyl = (sn >> 8) & 0xFFFF;
    hd  = (sn >> 24) & 0x0F;
    hd  += 0xE0;                        // Select Head in Master drive (LBA mode)
  }

#ifdef DEBUG_IDE
  Printf( "\nS=%i", (UINT16)sn );
  Printf( " C=%i", (UINT16)cyl );
  Printf( " H=%i", (UINT16)hd );
  Printf( " S=%i", (UINT16)sec );
#endif
                                        //
#ifdef IDE_INITRW
  RWcnt = 256;                          //
#endif
                                        //
  IDE_RegRD( IDE_REG_ERR );             // Clear any errors
                                        //
  if( !IDE_WaitBSY( TRUE ) )            //
    return FALSE;                       //
                                        //
  IDE_RegWR( IDE_REG_DH, hd );          //
  IDE_RegWR( IDE_REG_SC, 1 );           // Sector Count to read (1 sector)
  IDE_RegWR( IDE_REG_SN, sec );         // Sector Number to read
  IDE_RegWR( IDE_REG_CYLO, low(cyl) );  // Cylinder Number LOW
  IDE_RegWR( IDE_REG_CYHI, high(cyl) ); // Cylinder Number HIGH
                                        //
  IDE_RegWR( IDE_REG_CMD, 0x31 );       // Write Command (WRITE)
                                        //
  if( !IDE_WaitDRQ( TRUE ) )            //
    return FALSE;                       // Init OK
                                        //
  return TRUE;                          // Init error
}

//----------------------------------------------------------------
// Function :   IDE_WriteSectorData
// Notes    :
// History  :
//----------------------------------------------------------------
#ifdef IDE_INITRW
UINT8 IDE_WriteSectorData( UINT16 dat )
{
  if( RWcnt )                                   //
  {                                             //
    RWcnt--;                                    //
    IDE_RegWR( IDE_REG_DATA, dat );             //
    return IDE_WR_BSY;                          //
  }                                             //
                                                //
  if( !IDE_WaitBSY( TRUE ) )                    //
    return IDE_WR_ERR;                          //
                                                //
  if( IDE_Err() )                               // If Data NOT OK?
  {                                             // YES
    return IDE_WR_ERR;                          //   Write Error
  }                                             // NO
  return IDE_WR_OK;                             //   Write OK
}
#endif

//----------------------------------------------------------------
// Function :   IDE_SectorGet
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL  IDE_SectorGet( UINT16 *buf, T_drvinf * drv, UINT32 sec )
{
  UINT16  cnt = 256;

  if( drv->flg & DRV_CD )
  {
    return IDE_SectorGetAt( buf, drv, sec, 0 );
  }
  //
  if( IDE_ReadSectorInit( drv, sec ) )
  {
    while( cnt-- )
    {
      *buf++ = IDE_RegRD( IDE_REG_DATA );
    }
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   IDE_SectorPut
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL  IDE_SectorPut( UINT16 *buf, T_drvinf * drv, UINT32 sec )
{
  UINT16  cnt = 256;

  if( drv->flg & DRV_CD )
  {
    return FALSE;
  }
  if( IDE_WriteSectorInit( drv, sec ) )
  {
    while( cnt-- )
    {
      IDE_RegWR( IDE_REG_DATA, *buf++ );
    }
    //
    if( !IDE_WaitBSY( TRUE ) )                    //
      return FALSE;                               //
                                                  //
    if( IDE_Err() )                               // If Data NOT OK?
    {                                             // YES
      return FALSE;                               //   Write Error
    }                                             // NO
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   IDE_ATAIdentifyDevice
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  IDE_ATAIdentifyDevice( T_drvinf * drv )
{
  UINT16  cnt;                          //
  UINT16  dat;                          //
                                        //
  IDE_RegRD( IDE_REG_ERR );             // Clear any errors
                                        //
  if( !IDE_WaitBSY( TRUE ) )            //
    return FALSE;                       //
                                        //
  IDE_RegWR( IDE_REG_DH, 0xA0 );        // Select Drive
  IDE_RegWR( IDE_REG_CMD, 0xEC );       // Write Command (IDENTIFY DRIVE)
                                        //
  if( !IDE_WaitDRQ( TRUE ) )            //
    return FALSE;                       //
                                        //
  if( !IDE_Err() )                      // If Data OK?
  {                                     //
    for( cnt = 0; cnt < 256; cnt++ )    //
    {                                   //
      dat = IDE_RegRD( IDE_REG_DATA );  //
      switch( cnt )                     //
      {                                 //
        case 0:                         //   General Config
          if( dat & 0x8000 )            //   NON ATA device?
          {                             //
            return FALSE;               //
          }                             //
        break;                          //
        case 1:                         //   Number of Cylinders
          drv->cyl = dat;               //
        break;                          //
        case 3:                         //   Number of Heads
          drv->hd = dat;                //
        break;                          //
        case 6:                         //   Number of Sectors/Track
          drv->spt = dat;               //
        break;                          //
        case 49:                        //   Drive Capabilities
          if( dat & 0x0200 )            //   (bit 9 = LBA supported)
          {                             //
            drv->flg |= DRV_LBA;        //   LBA mode ON
          }                             //
        break;                          //
        case 60:                        //   Number of sectors (LBA mode only)
          if( drv->flg & DRV_LBA )      //
          {                             //
            drv->sec = dat;             //   Low word
          }                             //
        break;                          //
        case 61:                        //   Number of sectors (LBA mode only)
          if( drv->flg & DRV_LBA )      //
          {                             //
            drv->sec += ((UINT32)dat << 16);     //   High word
          }                             //
        break;                          //
        default:                        //
        break;                          //
      };                                //
    }                                   //
    //
    if( !(drv->flg & DRV_LBA) )         //
    {                                   //
      drv->sec = (UINT32)drv->cyl       //
                     *                  //
                     (UINT32)drv->hd    //
                     *                  //
                     (UINT32)drv->spt;  //
      drv->sec--;                       //
    }
    //
#ifdef DEBUG_IDE
    Printf( "\nHD" );
    Printf( "\nH=%i", drv->hd );
    Printf( "\nC=%i", drv->cyl );
    Printf( "\nS=%i", drv->spt );
#endif
    //
    return TRUE;
  }
  //
  return FALSE;                      //   Info OK
}

//
// IDE ATAPI interface (Identify, Read)
//

//----------------------------------------------------------------
// Function :   IDE_ATAPIPacket
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  IDE_ATAPIPacket( VOID )
{
  IDE_RegRD( IDE_REG_ERR );             // Clear any errors
                                        //
  if( !IDE_WaitBSY( TRUE ) )            //
    return FALSE;                       //
                                        //
  IDE_RegWR( IDE_REG_DH, 0xA0 );        // Device 0
  IDE_RegWR( ATAPI_BC_L, 12 );          //
  IDE_RegWR( ATAPI_BC_H, 0 );           //
  IDE_RegWR( IDE_REG_FR, 0 );           //
  IDE_RegWR( IDE_REG_SC, 0 );           //
  IDE_RegWR( IDE_REG_SN, 0 );           //
  IDE_RegWR( IDE_REG_CYLO, low(2048) ); // byte count ATA 4 limit
  IDE_RegWR( IDE_REG_CYHI, high(2048) );//
  IDE_RegWR( IDE_REG_CMD, CMD_PACKET ); // Write Command
                                        //
  if( !IDE_WaitDRQ( TRUE ) )            //
    return FALSE;                       //
                                        //
  if( !IDE_WaitBSY( TRUE ) )            //
    return FALSE;                       //
                                        //
  return TRUE;                          //
}

//----------------------------------------------------------------
// Function :   IDE_ATAPISectorGet
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  IDE_ATAPISectorGet( UINT8 *buf, UINT32 sec, UINT16 offs, UINT16 len )
{
  UINT16    cnt = 1024;
  UINT16    pkt;

#ifdef DEBUG_IDE
  Printf( "\nR1" );
#endif
  //
  if( !IDE_ATAPIPacket() )
    return FALSE;
  //
  pkt = CMD_READ_12;                      // Packet Command
  IDE_RegWR( IDE_REG_DATA, pkt );         //
  pkt = SWPB( (UINT16)((UINT32)sec >> 16) );
  IDE_RegWR( IDE_REG_DATA, pkt );         //
  pkt = SWPB( (UINT16)sec );              //
  IDE_RegWR( IDE_REG_DATA, pkt );         //
  pkt = 0;                                //
  IDE_RegWR( IDE_REG_DATA, pkt );         //
  pkt = SWPB( 1 );                        //
  IDE_RegWR( IDE_REG_DATA, pkt );         //
  pkt = 0;                                //
  IDE_RegWR( IDE_REG_DATA, pkt );         //
                                          //
#ifdef DEBUG_IDE
  Printf( "\nR2" );
#endif

  if( !IDE_WaitBSY( FALSE ) )             // Wait for CD
    return FALSE;                         //
                                          //
#ifdef DEBUG_IDE
  Printf( "\nR3" );
#endif

  if( !IDE_WaitDRQ( FALSE ) )             //
    return FALSE;                         //
                                          //
#ifdef DEBUG_IDE
  Printf( "\nR4" );
#endif
  if( !IDE_Err() )                        // If Data OK?
  {                                       //

#ifdef DEBUG_IDE
//    Printf( "\nS=%i O=%i L=%i", (UINT16)sec, (UINT16)offs, (UINT16)len );
#endif
    while( cnt-- )                        // Read data
    {
      pkt = IDE_RegRD( IDE_REG_DATA );
      //
      if( offs )  offs--;
      else
      {
        if( len )
        {
          len--;
          *buf++ = low(pkt);
        }
      }
      if( offs )  offs--;
      else
      {
        if( len )
        {
          len--;
          *buf++ = high(pkt);
        }
      }
    }
    if( IDE_WaitDRQ( TRUE ) )           // NOT all received (NON data CD?)
    {
      do
      {
        IDE_RegRD( IDE_REG_DATA );
      }while( IDE_WaitDRQ( TRUE ) );
      //
      return FALSE;
    }
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   IDE_SectorGetAt
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL  IDE_SectorGetAt( UINT16 *buf, T_drvinf * drv, UINT32 sec, UINT16 offs )
{
  if( !(drv->flg & DRV_CD) )
    return FALSE;
  //
  if( sec >= drv->sec )
    return FALSE;
  //
  if( offs > 512 )
    return FALSE;
  //
  if( (offs > 0) && (((UINT32)sec % 4) == 3) )
  {
    (UINT32)sec >>= 2;
    if( !IDE_ATAPISectorGet( (UINT8*)buf, sec, (3 * 512) + offs, (512 - offs) ) )
      return FALSE;
    //
    (UINT32)sec++;
    if( !IDE_ATAPISectorGet( (UINT8*)buf + (512 - offs), sec, 0, offs ) )
      return FALSE;
  }
  else
  {
    if( !IDE_ATAPISectorGet( (UINT8*)buf, ((UINT32)sec >> 2), (((UINT32)sec % 4) * 512) + offs, 512 ) )
      return FALSE;
  }
  return TRUE;
}

//----------------------------------------------------------------
// Function :   IDE_ATAPIStartCD
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  IDE_ATAPIStartCD( T_drvinf * drv, UINT8 cmd )
{
  UINT16  pkt;

  //
  if( !IDE_ATAPIPacket() )
    return FALSE;
  //
  pkt = (0x0100 | CMD_START);             // Packet Command
  IDE_RegWR( IDE_REG_DATA, pkt );         //
  pkt = 0;                                //
  IDE_RegWR( IDE_REG_DATA, pkt );         //
  pkt = cmd;                              //
  IDE_RegWR( IDE_REG_DATA, pkt );         //
  pkt = 0;                                //
  IDE_RegWR( IDE_REG_DATA, pkt );         //
  IDE_RegWR( IDE_REG_DATA, pkt );         //
  IDE_RegWR( IDE_REG_DATA, pkt );         //
                                          //
  if( !IDE_WaitBSY( FALSE ) )             // Wait for CD
    return FALSE;                         //
                                          //
  if( IDE_Err() )                         // If ERROR?
    return FALSE;                         //
                                          //
  return TRUE;                            //
}

//----------------------------------------------------------------
// Function :   IDE_ATAPIEjectCD
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL  IDE_ATAPIEjectCD( T_drvinf * drv  )
{
  return IDE_ATAPIStartCD( drv, CMD_START_EJECT );
}

//----------------------------------------------------------------
// Function :   IDE_ATAPILoadCD
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL  IDE_ATAPILoadCD( T_drvinf * drv  )
{
  return IDE_ATAPIStartCD( drv, CMD_START_LOAD );
}

//----------------------------------------------------------------
// Function :   IDE_ATAPIIdentifyDevice
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL  IDE_ATAPIIdentifyDevice( T_drvinf * drv )
{
  UINT16  cnt;                          //
  UINT16  dat;                          //
                                        //
  IDE_RegRD( IDE_REG_ERR );             // Clear any errors
                                        //
  if( !IDE_WaitBSY( TRUE ) )            //
    return FALSE;                       //
                                        //
  IDE_RegWR( IDE_REG_DH, 0xA0 );        // Select Drive
  IDE_RegWR( IDE_REG_CMD, 0xA1 );       // Write Command (ATAPI IDENTIFY DEVICE)
                                        //
  if( !IDE_WaitDRQ( TRUE ) )            //
    return FALSE;                       //
                                        //
  if( !IDE_Err() )                      // If Data OK?
  {                                     //
    for( cnt = 0; cnt < 256; cnt++ )    //
    {                                   //
      dat = IDE_RegRD( IDE_REG_DATA );  //
      switch( cnt )                     //
      {                                 //
        case 0:                         //   General Config
          if( !(dat & 0x8000) )         //   NON ATAPI device?
          {                             //
            return FALSE;               //
          }                             //
        break;                          //
        case 49:                        //   Drive Capabilities
          if( dat & 0x0200 )            //   (bit 9 = LBA supported)
          {                             //
            drv->flg |= DRV_LBA;        //   LBA mode ON (Mandatory)
          }                             //
          else return FALSE;            //
        break;                          //
        default:                        //
        break;                          //
      };                                //
    }                                   //
    //                                  //
    drv->flg |= DRV_CD;                 //   CD device
    //                                  //
    drv->sec = 380000 << 2;             //   max 750MB
    //
#ifdef DEBUG_IDE
    Printf( "\nCD" );
#endif
    //
    return TRUE;
  }
  //
  return FALSE;                      //   Info OK
}

//----------------------------------------------------------------
// Function :   IDE_ATAPIGetSense
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL IDE_ATAPIGetSense( UINT8 * ASC, UINT8 * ASCQ )
{
  UINT16    cnt;
  UINT16    dat;

  if( !IDE_ATAPIPacket() )
    return FALSE;
  //
  dat = CMD_CFA_REQUEST_EXT_ERR_CODE;     // Get Sense Command (0)
  IDE_RegWR( IDE_REG_DATA, dat );         //
  dat = 0;                                //
  IDE_RegWR( IDE_REG_DATA, dat );         //
  dat = 18;                               //
  IDE_RegWR( IDE_REG_DATA, dat );         // Allocation Length (4)
  dat = 0;                                //
  IDE_RegWR( IDE_REG_DATA, dat );         //
  IDE_RegWR( IDE_REG_DATA, dat );         //
  IDE_RegWR( IDE_REG_DATA, dat );         //
                                          //
  if( !IDE_WaitBSY( TRUE ) )              //
    return FALSE;                         //
                                          //
  if( !IDE_WaitDRQ( TRUE ) )              //
    return FALSE;                         //
                                          //
  if( !IDE_Err() )                        // If Data OK?
  {                                       //
    for( cnt = 0; cnt < 9; cnt++ )        //
    {                                     //
      dat = IDE_RegRD( IDE_REG_DATA );    //
      switch( cnt )                       //
      {                                   //
        case 6:                           //   ASC & ASCQ (12, 13)
          *ASC  = low( dat );             //
          *ASCQ = high( dat );            //
        break;                            //
        default:                          //
        break;                            //
      };                                  //
    }                                     //
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   IDE_ATAPICheck
// Notes    :
// History  :
//----------------------------------------------------------------

#define CD_NONE         0
#define CD_OK           1
#define CD_ERROR        2

STATIC UINT8 IDE_ATAPICheck( T_drvinf * drv )
{
  UINT8 ASC, ASCQ;

  if( !(drv->flg & DRV_CD) )
  {
    return CD_ERROR;
  }
  if( IDE_ATAPIGetSense( &ASC, &ASCQ ) )
  {
    switch( ASC )
    {
      case 0x00:                // NO_ERROR (CD IN)
        return CD_OK;
      case 0x3a:                // NO_MEDIUM (NO CD)
        return CD_NONE;
      case 0x28:                // MEDIUM_CHANGED
      case 0x02:                // NO SEEK complite
      case 0x04:                // DRIVE_NOT_READY
      case 0x29:                // POWER_ON
      case 0xFF:                // bad..........
      default:                  // other errors
        return CD_ERROR;
    };
  }
  return CD_ERROR;
}

//----------------------------------------------------------------
// Function :   IDE_GetDriveInfo
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC UINT8 IDE_GetDriveInfo( T_drvinf * drv )
{
  drv->cyl = 0;                         // Clear Drive Info
  drv->hd  = 0;                         //
  drv->spt = 0;                         //
  drv->sec = 0;                         //
  drv->flg = 0;                         //
                                        //
#ifdef DEBUG_IDE
  Printf( "\nG" );
#endif
  if( IDE_ATAPIIdentifyDevice( drv ) )  // Check ATAPI
  {                                     //
    IDE_ATAPILoadCD( drv );             //
                                        //
    while( IDE_ATAPICheck( drv ) != CD_OK );    //   Wait for CD IN
                                        //
    return IDE_CD;                      //   CD detected
  }                                     //
  else                                  //
  if( IDE_ATAIdentifyDevice( drv ) )    // Check ATA
  {                                     //
    return IDE_HD;                      //   HD detected
  }                                     //
  return IDE_NONE;                      //
}

//----------------------------------------------------------------
// Function :   IDE_Reset
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID IDE_Reset( VOID )
{
//  IDE_WaitRDY( FALSE );                   //
  //
  IDE_RegWR( IDE_REG_DH, 0xA0 );        // Select Drive
  IDE_RegWR( IDE_REG_CTRL, (IDEC_HD15 | IDEC_SRST | IDEC_NIEN) );
  IDE_Delay( 10 );
  IDE_RegWR( IDE_REG_CTRL, (IDEC_HD15 | IDEC_NIEN) );
  IDE_Delay( 500000 );
  //
  IDE_WaitBSY( FALSE );                   //

#ifdef DEBUG_IDE
  Printf( "\nR" );
#endif
}

//----------------------------------------------------------------
// Function :   IDE_Init
// Notes    :
// History  :
//----------------------------------------------------------------

UINT8 IDE_Init( T_drvinf * DrvInfo  )
{
  IDE_DATA_HIZ();                         // Data Bus in Hi-Z (Default mode)
  IDE_CTRL_INIT();                        // Control Lines in output mode (RD/WR = 1)
  IDE_CTRL_NO_RD();                       // RD OFF
  IDE_CTRL_NO_WR();                       // WR OFF
  IDE_RDYL_ON();                          //
                                          //
  IDE_Reset();                            //
                                          //
  return IDE_GetDriveInfo( DrvInfo );     // Get HD/CD Info
}

//      End
