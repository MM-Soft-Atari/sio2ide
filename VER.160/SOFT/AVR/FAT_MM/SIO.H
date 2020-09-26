//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: sio.h
//
// MODULE NAME: sio
//
// PURPOSE:     Atari SIO protocol driver (header file).
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


#ifndef __SIODRV_H__
  #define __SIODRV_H__

//
//      Internal states
//
#define SIOS_IDLE       0x01
#define SIOS_CMDRDY     0x02
#define SIOS_RXSETUP    0x03
#define SIOS_RXDATA     0x04
#define SIOS_RXRDY      0x05
#define SIOS_TXSETUP    0x06
#define SIOS_TXDATA     0x07
#define SIOS_CMDCERR    0x08
#define SIOS_DATACERR   0x09

//
//      SIO command structure
//
typedef struct
{
  UINT8         did;    // Device ID
  UINT8         cmd;    // Command
  UINT8         aux1;   // Aux 1
  UINT8         aux2;   // Aux 2
  UINT8         chk;    // Checksum
} T_SIOCMD;

//
//      Handlers
//
typedef BOOL ( *T_CSETUP )     ( T_SIOCMD *, UINT16 * );
typedef BOOL ( *T_RXTXDATA )   ( UINT8 * );

//
//      SIO command handler structure
//
typedef struct
{
  UINT8         cmd;            // Command
  T_CSETUP      cmdRun;         // Command Set-up handler
  UINT8         nextStat;       // Next state after Set-up
  T_RXTXDATA    rxtxData;       // RX/TX data handler
} T_SIOHNDL;

typedef T_SIOHNDL    TF_SIOHNDL PROGMEM;

#define CMD_SIZE        sizeof( T_SIOCMD )

EXTERN FLASH TF_SIOHNDL         sioHndl[];

//
//      SIO driver Interface
//
VOID   SIO_Init ( VOID );
VOID   SIO_Run  ( VOID );

#endif  // __SIO_H__
