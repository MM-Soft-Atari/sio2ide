//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: ide.c
//
// MODULE NAME: ide
//
// PURPOSE:     IDE bus driver module.
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
//#define         DEBUG_IDE
#endif

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
#define IDE_REG_SC              (HCS1|HA01)            // R/W  Sectors to R/W 0-256
#define IDE_REG_SN              (HCS1|HA01|HA00)       // R/W  Sector
#define IDE_REG_CYLO            (HCS1|HA02)            // R/W  Cylinder Low
#define IDE_REG_CYHI            (HCS1|HA02|HA00)       // R/W  Cylinder High
#define IDE_REG_DH              (HCS1|HA02|HA01)       // R/W  Drive+Head (CHS mode) 1010xxxx - master 1011xxxx - slave
#define IDE_REG_STAT            (HCS1|HA02|HA01|HA00)  // R/W  Status/Command
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

//****************************************************************
//
// Static Data
//
//****************************************************************

STATIC  DATA UINT16     RWcnt;          // RW data transfer word counter

//****************************************************************
//
// Implementation
//
//****************************************************************

STATIC VOID IDE_Delay( UINT16 del )
{
  while( del-- )
  {
    _NOP();
  }
}

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

//----------------------------------------------------------------
// Function :   IDE_ReadSectorInit
// Notes    :   sn (0 - 0x000FFFFF)
// History  :
//----------------------------------------------------------------

BOOL IDE_ReadSectorInit( T_DRVINF * DrvInfo, UINT32 sn )
{
  UINT8   hd;                           //
  UINT8   sec;                          //
  UINT16  cyl;                          //
  UINT32  sph;                          //
                                        //
  if( DrvInfo->sec < sn ) return FALSE; //
                                        // Calculate parameters
  sph = (UINT32)((UINT32)DrvInfo->spt * (UINT32)DrvInfo->cyl);
  hd  = sn / sph;                       // hd  = sn / sph
  cyl = (sn - (UINT32)((UINT32)hd * sph)) /
         (UINT32)DrvInfo->spt;
  sec = (sn % (UINT32)DrvInfo->spt) + 1;// sec = (sn % spt) + 1
                                        //
#ifdef DEBUG_IDE
  Printf( "\nC=%d H=%d S=%d", cyl, hd, sec );
#endif
                                        //
  RWcnt = 256;                          //
                                        //
  IDE_RegRD( IDE_REG_ERR );             // Clear any errors
  hd = 0xA0 + (hd & 0x0F);              // Select Head in Master drive (CSH mode)
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
  IDE_RegWR( IDE_REG_STAT, 0x21 );      // Write Command (READ)
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

//----------------------------------------------------------------
// Function :   IDE_WriteSectorInit
// Notes    :   sn (0 - 0x000FFFFF)
// History  :
//----------------------------------------------------------------

BOOL IDE_WriteSectorInit( T_DRVINF * DrvInfo, UINT32 sn )
{
  UINT8   hd;                           //
  UINT8   sec;                          //
  UINT16  cyl;                          //
  UINT32  sph;                          //
                                        //
  if( DrvInfo->sec < sn ) return FALSE; //
                                        // Calculate parameters
  sph = (UINT32)((UINT32)DrvInfo->spt * (UINT32)DrvInfo->cyl);
  hd  = sn / sph;                       // hd  = sn / sph
  cyl = (sn - (UINT32)((UINT32)hd * sph)) /
         (UINT32)DrvInfo->spt;
  sec = (sn % (UINT32)DrvInfo->spt) + 1;// sec = (sn % spt) + 1
                                        //
#ifdef DEBUG_IDE
  Printf( "\nC=%d H=%d S=%d", cyl, hd, sec );
#endif
                                        //
  RWcnt = 256;                          //
                                        //
  IDE_RegRD( IDE_REG_ERR );             // Clear any errors
  hd = 0xA0 + (hd & 0x0F);              // Select Head in Master drive (CSH mode)
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
  IDE_RegWR( IDE_REG_STAT, 0x31 );      // Write Command (WRITE)
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

//----------------------------------------------------------------
// Function :   IDE_GetDriveInfo
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL IDE_GetDriveInfo( T_DRVINF * DrvInfo )
{
  UINT16  cnt;                          //
  UINT16  dat;                          //
                                        //
  DrvInfo->cyl = 0;                     // Clear Drive Info
  DrvInfo->hd  = 0;                     //
  DrvInfo->spt = 0;                     //
  DrvInfo->sec = 0;                     //
                                        //
  IDE_RegRD( IDE_REG_ERR );             // Clear any errors
  IDE_RegWR( IDE_REG_DH, 0xA0 );        // Select Drive
                                        //
  if( !IDE_WaitBSY( TRUE ) )            //
    return FALSE;                       //
                                        //
  IDE_RegWR( IDE_REG_STAT, 0xEC );      // Write Command (IDENTIFY DRIVE)
                                        //
  if( !IDE_WaitDRQ( TRUE ) )            //
    return FALSE;                       //
                                        //
  if( !IDE_Err() )                      // If Data OK?
  {                                     // YES
    for( cnt = 0; cnt < 256; cnt++ )    //
    {                                   //
      dat = IDE_RegRD( IDE_REG_DATA );  //
      switch( cnt )                     //
      {                                 //
        case 1:                         //   Number of Cylinders
          DrvInfo->cyl = dat;           //
        break;                          //
        case 3:                         //   Number of Heads
          DrvInfo->hd = dat;            //
        break;                          //
        case 6:                         //   Number of Sectors/Track
          DrvInfo->spt = dat;           //
        break;                          //
        default:                        //
        break;                          //
      };                                //
    }                                   //
    DrvInfo->sec = (UINT32)DrvInfo->cyl *  //
                   (UINT32)DrvInfo->hd *   //
                   (UINT32)DrvInfo->spt;   //
    DrvInfo->sec--;                     //
    return TRUE;                        //   Info OK
  }                                     // NO
  return FALSE;                         //   Info Error

}

//----------------------------------------------------------------
// Function :   IDE_Init
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL IDE_Init( T_DRVINF * DrvInfo  )
{
  RWcnt = 0;                              //
  IDE_DATA_HIZ();                         // Data Bus in Hi-Z (Default mode)
  IDE_CTRL_INIT();                        // Control Lines in output mode (RD/WR = 1)
  IDE_CTRL_NO_RD();                       // RD OFF
  IDE_CTRL_NO_WR();                       // WR OFF
  IDE_RDYL_ON();                          //
                                          //
  IDE_RegWR( IDE_REG_DH, 0xA0 );          // Select HD
  IDE_WaitBSY( FALSE );                   //
  IDE_WaitRDY( FALSE );                   //
                                          //
  IDE_Delay( 50000 );                     // wait 100ms
                                          //
  if( !IDE_GetDriveInfo( DrvInfo ) )      // Get HD Info
    return FALSE;                         //
                                          //
  return TRUE;                            //
}

//      End
