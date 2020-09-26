//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: HAL4IDE.H
//
// MODULE NAME: HAL4IDE
//
// PURPOSE:
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001   19-06-2002 MM     Created
//
//****************************************************************

#ifndef __HAL4IDE_H__
  #define __HAL4IDE_H__

//#define IDE_BULK      // Use IDE Bulk transferr

//
// IDE Data Bus definitions (16bit)
//
#define IDE_DATA_INP()          __outp(DDRA,0);__outp(DDRC,0);\
                                __outp(PORTA,0);__outp(PORTC,0)
//                                __outp(PORTA,0xFF);__outp(PORTC,0xFF)
#define IDE_DATA_HIZ()          IDE_DATA_INP()
//#define IDE_DATA_HIZ()          __outp(DDRA,0xFF);__outp(PORTA,0xFF);\
//                                __outp(DDRC,0xFF);__outp(PORTC,0xFF)
#define IDE_DATA_OUT()          __outp(DDRA,0xFF);__outp(DDRC,0xFF)
#define IDE_DATA_GETL()         (UINT8)__inp(PINA)
#define IDE_DATA_GETH()         (UINT8)__inp(PINC)
#define IDE_DATA_PUTL( d )      __outp(PORTA,d)
#define IDE_DATA_PUTH( d )      __outp(PORTC,d)
//
// IDE Control Bus definitions
//
// Port (PB) R
#define HIOR                    (1<<1)
// Port (PD) W
#define HIOW                    (1<<4)
// Port (PB) A
#define HA00                    (1<<3)
#define HA01                    (1<<2)
#define HA02                    (1<<4)
// Port (PD) CS
#define HCS0                    (1<<5)
#define HCS1                    (1<<6)
//
#define IDE_ADR_MASK_A          (HA02|HA01|HA00)
#define IDE_ADR_MASK_C          (HCS1|HCS0)
#define IDE_CTRL_INIT()         __port_or( DDRB, IDE_ADR_MASK_A );\
                                __port_or( PORTB, IDE_ADR_MASK_A );\
                                __port_or( DDRD, IDE_ADR_MASK_C );\
                                __port_or( PORTD, IDE_ADR_MASK_C );\
                                __port_or( DDRB, HIOR );\
                                __port_or( DDRD, HIOW )
#define IDE_CTRL_RD()           __port_and(PORTB,~HIOR)
#define IDE_CTRL_WR()           __port_and(PORTD,~HIOW)
#define IDE_CTRL_NO_RD()        __port_or(PORTB,HIOR)
#define IDE_CTRL_NO_WR()        __port_or(PORTD,HIOW)
#define IDE_CTRL_NONE()         __port_or(PORTD,IDE_ADR_MASK_C)
#define IDE_CTRL_REG( r )       __outp(PORTD,((__inp(PORTD) & ~IDE_ADR_MASK_C) | (r>>8)));\
                                __outp(PORTB,((__inp(PORTB) & ~IDE_ADR_MASK_A) | (r&0xFF)));
//
// IDE Registers
//
#define IDE_REG_NONE            (UINT16)((HCS1|HCS0)<<8)|(HA02|HA01|HA00)
#define IDE_REG_CTRL            (UINT16)(HCS0<<8)|(HA02|HA01)       // R/W  Device Ctrl
#define IDE_REG_ADDR            (UINT16)(HCS0<<8)|(HA02|HA01|HA00)  // R    Drive Address
#define IDE_REG_DATA            (UINT16)(HCS1<<8)|(0)               // R/W  Data
#define IDE_REG_ERR             (UINT16)(HCS1<<8)|(HA00)            // R    Error Status
#define IDE_REG_FR              (UINT16)(HCS1<<8)|(HA00)            // W    Feature Register
#define IDE_REG_SC              (UINT16)(HCS1<<8)|(HA01)            // R/W  Sectors to R/W 0-256
#define IDE_REG_SN              (UINT16)(HCS1<<8)|(HA01|HA00)       // R/W  Sector
#define IDE_REG_CYLO            (UINT16)(HCS1<<8)|(HA02)            // R/W  Cylinder Low
#define IDE_REG_CYHI            (UINT16)(HCS1<<8)|(HA02|HA00)       // R/W  Cylinder High
#define IDE_REG_DH              (UINT16)(HCS1<<8)|(HA02|HA01)       // R/W  Drive+Head (CHS mode) 1010xxxx - master 1011xxxx - slave
#define IDE_REG_STAT            (UINT16)(HCS1<<8)|(HA02|HA01|HA00)  // R/W  Status/Command
#define IDE_REG_CMD             (UINT16)(HCS1<<8)|(HA02|HA01|HA00)  // R/W  Status/Command

#ifdef __AVR__
  EXTERN UINT8   Swap_Byte( UINT8 );
  #define IDE_Swpb( a )           Swap_Byte( a )
#else
  #define IDE_Swpb( a )           Swpb( a )
#endif

//
// Interface
//
VOID   IDE_RegWR        ( UINT16 reg, UINT16 dat );
UINT16 IDE_RegRD        ( UINT16 reg );

VOID   IDE_WR_DataBlk   ( UINT16 * dat );
VOID   IDE_RD_DataBlk   ( UINT16 * dat );

#endif

//      End

