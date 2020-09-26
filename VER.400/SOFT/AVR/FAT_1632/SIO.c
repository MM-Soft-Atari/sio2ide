//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: sio.c
//
// MODULE NAME: sio
//
// PURPOSE:     SIO driver module.
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

//#ifdef DEBUG
//  #define DEBUG_SIO
//#endif

#define SIO_BRATE_L     19200L          // UART Baud Rate Max is 115200
#define SIO_BRATE_H     51200L          // UART Baud Rate Max is 115200

#define SIOB_SIZE       256*2           // UART buffer size
#define SIO_TO1S        (T0_1S*1.0)     // Tout 1sek
#define SIO_TOUTL       (T0_1S*1.0)     // Long Tout 1000ms
#define SIO_TOUTS       (T0_1S*0.02)    // Short Tout 20ms

#define SIO_TSTOP       0xFE            // STOP TimeOut Timer

#define SIORATE_FACT_L  ((FOSC / (16L * SIO_BRATE_L)) - 1L)
#define SIORATE_FACT_H  ((FOSC / (16L * SIO_BRATE_H)) - 1L)

//
// HW Ctrl bits
//
//   Port (PD)
#define RX_PIN          0       // RX Bit in Port D
#define TX_PIN          1       // TX Bit in Port D
#define CMD_PIN         2       // Command Bit in Port D
//
//      SIO Answer codes
//
#define SIOA_ACK        0x41
#define SIOA_NAK        0x4E
#define SIOA_CMPL       0x43
#define SIOA_ERR        0x45

//
// Static data
//

#pragma memory=dataseg(CRAM00)
STATIC      VOLATILE UINT8     sioBuf[SIOB_SIZE];   // RX/TX Buffer
#pragma memory=default

STATIC      VOLATILE UINT16    sioBPtr;             // Buffer Data Pointer
STATIC      VOLATILE UINT16    sioBDLen;            // Buffer Data Length
STATIC      VOLATILE UINT8     sioChk;              // Checksum
//
STATIC      VOLATILE UINT8     sioStat;             // Current state
STATIC      VOLATILE UINT8     sioCmd;              // Current command
STATIC      VOLATILE UINT8     sioTOut;             // TimeOut Timer

//****************************************************************
//
// Low Level SIO Interface
//
//****************************************************************

#define SIO_ChkClr()    (sioChk = 0)
#define SIO_Chk( a )    (sioChk += (((UINT16)sioChk + (UINT16)a) > 0x00FF) ? a + 1 : a)

//----------------------------------------------------------------
// Function :   SIO_SpeedChange
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID SIO_SpeedChange( VOID )
{
  if( __inp( UBRR0 ) == SIORATE_FACT_L )
  {
    __outp( UBRR0, SIORATE_FACT_H );
  }
  else
  {
    __outp( UBRR0, SIORATE_FACT_L );
  }
}

//----------------------------------------------------------------
// Function :   SIO_BufSet
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID SIO_BufSet( UINT16 len )
{
  __port_and( UCSR0B, ~(1<<RXCIE) );                           //
  sioBDLen = len;
  sioBPtr = 0;
  SIO_ChkClr();
  __port_or( UCSR0B, (1<<RXCIE) );                            //
}

//----------------------------------------------------------------
// Function :   SIO_TOutSet
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID SIO_TOutSet( UINT8 tout )
{
  TMRS_Stop();
  sioTOut = tout;                         //
  TMRS_Start();
}

//----------------------------------------------------------------
// Function :   SIO_TOutExp
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL SIO_TOutExp( VOID )
{
  BOOL  res;

  TMRS_Stop();
  if( sioTOut )  res = FALSE;
  else           res = TRUE;
  TMRS_Start();

  return res;
}

//----------------------------------------------------------------
// Function :   SIO_Clear
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID SIO_Clear( VOID )
{
//  while( !(USR & (1<<UDRE)) );                  // wait for TX empty
  SIO_TOutSet( SIO_TSTOP );                     //
  SIO_BufSet( CMD_SIZE - 1 );                   //
  SIO_RDYL_OFF();                               //
  sioStat = SIOS_IDLE;                          //
}

//----------------------------------------------------------------
// Function :   SIO_Init
// Notes    :
// History  :
//----------------------------------------------------------------

VOID   SIO_Init( VOID )
{
  __port_or( PORTD, (1<<RX_PIN) );                // Internal Pull-Up RXD
  __outp( UBRR0, SIORATE_FACT_L );                // Baud Rate factor
  __outp( UCSR0B, ((1<<RXCIE) + (1<<RXEN) + (1<<TXEN)) );
  //
  __port_and( DDRD, ~(1<<CMD_PIN) );              // CMD pin as input
  __port_or( PORTD, (1<<CMD_PIN) );               // Internal Pull-Up CMD
  //
  SIO_Clear();
}

//----------------------------------------------------------------
// Function :   SIO_Delay
// Notes    :   2us*(del+1) @ Xtal = 4.9152MHz
// History  :
//----------------------------------------------------------------

STATIC VOID SIO_Delay( UINT16 del )
{
  del <<= DEL_FACTOR;
  while( del-- )
  {
    _NOP();
  }
}

//----------------------------------------------------------------
// Function :   SIO_Put
// Notes    :
// History  :
//----------------------------------------------------------------

VOID   SIO_Put( UINT8 c )
{
  loop_until_bit_is_set( UCSR0A, UDRE );   // wait for TX empty
  __outp( UDR0, c );
}

//----------------------------------------------------------------
// Function :   SIO_Run
// Notes    :
// History  :
//----------------------------------------------------------------

VOID    SIO_Run( VOID )
{
#ifdef __GNU__
  T_SIOHNDL    hndl;
#endif
  T_SIOCMD   * cmd = (T_SIOCMD*)sioBuf;

  if( SIO_TOutExp() )
  {
//    SIO_Put( 'T' );
    SIO_SpeedChange();        // Change Speed
    SIO_Clear();
  }
  switch( sioStat )
  {
//
// Command RX
//
    case SIOS_CMDRDY:
#ifndef DEBUG_SIO
      if( __inp(PIND) & (1<<CMD_PIN) )       // If Command Line HIGH?
#endif
      {
        // Stop TimeOut
        SIO_TOutSet( SIO_TSTOP );
        // Check Device ID
        if( !FS_CheckDev( &cmd->did, cmd->cmd ) )
        {
          SIO_Clear();                      // Wrong Device ID
          return;                           // Return
        }
#ifdef __IAR__
        // Check Command
        sioCmd = 0;
        while( sioHndl[sioCmd].cmd != SIOC_NONE )
        {
          if( sioHndl[sioCmd].cmd == cmd->cmd )
          {
            if( sioHndl[sioCmd].cmdRun )
            if( sioHndl[sioCmd].cmdRun( cmd, (UINT16*)&sioBDLen ) )
            {
              sioStat = sioHndl[sioCmd].nextStat;
              SIO_BufSet( sioBDLen );
              SIO_RDYL_ON();
              return;
            }
          }
          sioCmd++;
        }
#endif
#ifdef __GNU__
        // Check Command
        sioCmd = 0;
        progmem_read( (VOID *)&hndl, (CONST prog_char *)&sioHndl[sioCmd],
                      sizeof(T_SIOHNDL) );
        while( hndl.cmd != SIOC_NONE )
        {
          if( hndl.cmd == cmd->cmd )
          {
            if( hndl.cmdRun )
            if( hndl.cmdRun( cmd, (UINT16*)&sioBDLen ) )
            {
              sioStat = hndl.nextStat;
              SIO_BufSet( sioBDLen );
              SIO_RDYL_ON();
              return;
            }
          }
          sioCmd++;
          progmem_read( (VOID *)&hndl, (CONST prog_char *)&sioHndl[sioCmd],
                        sizeof(T_SIOHNDL) );
        }
#endif
        SIO_Put( SIOA_NAK );            // Send NAK
        SIO_Clear();                    // Wrong Command
      }
    break;
//
// Data TX
//
    case SIOS_TXSETUP:
      //
      SIO_Delay( 200 );         // Wait min 400us
      //
      SIO_Put( SIOA_ACK );
      //
      SIO_Delay( 250 );         // Wait min 500us (+IDE IO <=30ms)
      //
#ifdef __IAR__
      if( sioHndl[sioCmd].rxtxData )
      {
        if( sioHndl[sioCmd].rxtxData( (UINT8*)sioBuf ) )
#endif
#ifdef __GNU__
      progmem_read( (VOID *)&hndl, (CONST prog_char *)&sioHndl[sioCmd],
                    sizeof(T_SIOHNDL) );
      if( hndl.rxtxData )
      {
        if( hndl.rxtxData( (UINT8*)sioBuf ) )
#endif
        {
          SIO_Put( SIOA_CMPL );
          sioStat = SIOS_TXDATA;
        }
        else
        {
          SIO_Put( SIOA_ERR );
          SIO_Clear();
        }
      }
      else
      {
        SIO_Put( SIOA_ERR );
        SIO_Clear();
      }
    break;
    case SIOS_TXDATA:
      //
      SIO_Delay( 250 );         // Wait min 500us
      //
      while( sioBPtr < sioBDLen )
      {
        SIO_Put( sioBuf[sioBPtr] );
        SIO_Chk( sioBuf[sioBPtr] );
        sioBPtr++;
      }
      SIO_Put( sioChk );
      SIO_Clear();
    break;
//
// Data RX
//
    case SIOS_RXSETUP:
      //
      SIO_Delay( 200 );         // Wait min 400us
      //
      SIO_TOutSet( SIO_TOUTS );
      //
      SIO_Put( SIOA_ACK );
      sioStat = SIOS_RXDATA;
    break;
    case SIOS_RXRDY:
      //
      SIO_Delay( 2000 );        // Wait min 4000us
      //
      SIO_Put( SIOA_ACK );
      //
      SIO_Delay( 250 );         // Wait min 500us (+IDE IO <=30ms)
      //
#ifdef __IAR__
      if( sioHndl[sioCmd].rxtxData )
      {
        if( sioHndl[sioCmd].rxtxData( (UINT8*)sioBuf ) )
#endif
#ifdef __GNU__
      progmem_read( (VOID *)&hndl, (CONST prog_char *)&sioHndl[sioCmd],
                    sizeof(T_SIOHNDL) );
      if( hndl.rxtxData )
      {
        if( hndl.rxtxData( (UINT8*)sioBuf ) )
#endif
        {
          SIO_Put( SIOA_CMPL );
        }
        else
        {
          SIO_Put( SIOA_ERR );
        }
      }
      else
      {
        SIO_Put( SIOA_ERR );
      }
      SIO_Clear();
    break;
//
// Immediate Command
//
    case SIOS_IMMCMD:
      //
      SIO_Delay( 200 );         // Wait min 400us
      //
      SIO_Put( SIOA_ACK );
      //
      SIO_Delay( 250 );         // Wait min 500us (+IDE IO <=30ms)
      //
      SIO_Put( SIOA_CMPL );
      //
      SIO_Clear();
    break;
//
// Checksum Errors
//
    case SIOS_DATACERR:
      //
      SIO_Delay( 500 );         // Wait 1000us
      //
      SIO_Put( SIOA_NAK );
    case SIOS_CMDCERR:
//      SIO_Put( LOW(sioChk) );
      SIO_Clear();
    break;
    default:
    return;
  };
}

//----------------------------------------------------------------
// Function :   SIO_RXint
// Notes    :
// History  :
//----------------------------------------------------------------

#ifdef __IAR__
interrupt [UART0_RX_vect] VOID SIO_RXint ( VOID )
#endif
#ifdef __GNU__
SIGNAL( SIG_UART_RECV )
#endif
{
  UINT8 dt;

  dt = __inp( UDR0 );

#ifndef DEBUG_SIO
  if( !(__inp(PIND) & (1<<CMD_PIN) ) )  // If Command Line LOW?
#endif
  {                                     // YES
    if( sioStat == SIOS_IDLE )
    {
      SIO_TOutSet( SIO_TOUTS );

      if( sioBPtr < sioBDLen )
      {
        sioBuf[sioBPtr++] = dt;
        SIO_Chk( dt );
      }
      else
      {
        if( sioChk == dt )  sioStat = SIOS_CMDRDY;
        else                sioStat = SIOS_CMDCERR;
      }
    }
  }                                     //
#ifndef DEBUG_SIO
  if( __inp(PIND) & (1<<CMD_PIN) )      // If Command Line HIGH?
#endif
  {                                     //   Receive Data Block
    if( sioStat == SIOS_RXDATA )        //   If SIO RXDATA?
    {                                   //   YES
      SIO_TOutSet( SIO_TOUTS );

      if( sioBPtr < sioBDLen )
      {
        sioBuf[sioBPtr++] = dt;
        SIO_Chk( dt );
      }
      else
      {
        SIO_TOutSet( SIO_TSTOP );
        if( sioChk == dt )  sioStat = SIOS_RXRDY;
        else
        {
          SIO_SpeedChange();
          sioStat = SIOS_DATACERR;
        }
      }
    }                                   //
  }                                     //
}

//----------------------------------------------------------------
// Function :   SIO_TimerUpd
// Notes    :
// History  :
//----------------------------------------------------------------

VOID SIO_TimerUpd( VOID )
{
  if( sioTOut != SIO_TSTOP )
  {
#ifndef DEBUG_SIO
    if( sioTOut )  sioTOut--;
#endif
  }
}

//      End
