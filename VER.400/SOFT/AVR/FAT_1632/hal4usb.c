//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: HAL4USB.C
//
// MODULE NAME: HAL4USB
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

#include <platform.h>
#include "hal4usb.h"

UINT8    reg_adr;

#ifdef USB_SPI

// SPI Port (PB)
#define SSPIN   (1<<4)          // PB4
#define SOPIN   (1<<5)          // PB5
#define SIPIN   (1<<6)          // PB6
#define SKPIN   (1<<7)          // PB7
//
//    USB CS
//
//  Port (PB)
#define CSPIN   (1<<0)          // PB0
#define USBCS_INIT()    __port_or( DDRB, CSPIN )
#define USBCS_0()       __port_and( PORTB, ~CSPIN )
#define USBCS_1()       __port_or( PORTB, CSPIN )

//----------------------------------------------------------------
// Function :   INT0_Hndl
// Notes    :
// History  :
//----------------------------------------------------------------

#ifdef __IAR__
//interrupt [INT0_vect] VOID INT0_Hndl ( VOID )
#endif
#ifdef __GNU__
//SIGNAL( INT0_vect )
#endif
//{
//  usb_node_handler();
//}

//----------------------------------------------------------------
// Function :   enable_USBint
// Notes    :
// History  :
//----------------------------------------------------------------

VOID  enable_USBint( VOID )
{
//  __port_or( GIMSK, (1<<INT0) );
}

//----------------------------------------------------------------
// Function :   disable_USBint
// Notes    :
// History  :
//----------------------------------------------------------------

VOID  disable_USBint( VOID )
{
//  __port_and( GIMSK, ~(1<<INT0) );
}

//----------------------------------------------------------------
// Function :   hal4usb_init
// Notes    :
// History  :
//----------------------------------------------------------------

VOID   hal4usb_init( VOID )                       //
{                                                 //
  USBCS_INIT();                                   // CS init
  USBCS_1();                                      // CS = 1
                                                  //
  __port_or( DDRB, (SSPIN | SOPIN | SKPIN) );     // SS & SO & SK as output
                                                  //
  __outp( SPCR, (0<<CPHA)+(1<<SPE)+(1<<MSTR)+1 ); // fclk/16 Master Mode SPI
  __port_or( SPSR, (1<<SPI2X) );                  // fclk/8
  __inp( SPDR );                                  // Clear INT
                                                  //
//  __port_or( MCUCR, (1<<ISC01) );                 //
  enable_USBint();                                // USB INT = INT0 on Low Level
}

//----------------------------------------------------------------
// Function :   adr_set_usb
// Notes    :
// History  :
//----------------------------------------------------------------

VOID  adr_set_usb ( UINT8 adr )
{
  reg_adr = (adr & 0x3F);
}

//----------------------------------------------------------------
// Function :   dat_put_usb
// Notes    :
// History  :
//----------------------------------------------------------------

VOID  dat_put_usb ( UINT8 dta )
{
  USBCS_0();                                    // CS = 0
  SPI_IO( reg_adr | 0x80 );                     // Address (Normal Write)
  SPI_IO( dta );                                // Write Data
  USBCS_1();                                    // CS = 1
}

//----------------------------------------------------------------
// Function :   burstw_start_usb
// Notes    :
// History  :
//----------------------------------------------------------------

VOID  burstw_start_usb ( UINT8 adr )
{
  USBCS_0();                                    // CS = 0
  SPI_IO( 0xC0 | (adr & 0x3F) );                // Address (Burst Write)
}

//----------------------------------------------------------------
// Function :   burstw_put_usb
// Notes    :
// History  :
//----------------------------------------------------------------

VOID  burstw_put_usb ( UINT8 dta )
{
  SPI_IO( dta );                                // Write Data
}

//----------------------------------------------------------------
// Function :   burstw_stop_usb
// Notes    :
// History  :
//----------------------------------------------------------------

VOID  burstw_stop_usb ( VOID )
{
  USBCS_1();                                    // CS = 1
}

//----------------------------------------------------------------
// Function :   dat_get_usb
// Notes    :
// History  :
//----------------------------------------------------------------

UINT8 dat_get_usb ( VOID )
{
  USBCS_0();                                    // CS = 0
  SPI_IO( reg_adr );                            // Address
  SPI_IO( reg_adr | 0x40 );                     // Address (No Action Read previous data)
  USBCS_1();                                    // CS = 1
  return __inp(SPDR);                           // return data
}

//----------------------------------------------------------------
// Function :   burstr_start_usb
// Notes    :
// History  :
//----------------------------------------------------------------

VOID  burstr_start_usb ( UINT8 adr )
{
  reg_adr = (adr & 0x3F);                       //
  USBCS_0();                                    // CS = 0
}

//----------------------------------------------------------------
// Function :   burstr_get_usb
// Notes    :
// History  :
//----------------------------------------------------------------

UINT8  burstr_get_usb ( VOID )
{
  SPI_IO( reg_adr );                            // Address
  SPI_IO( reg_adr | 0x40 );                     // Address (No Action Read previous data)
  return __inp(SPDR);                           // return data
}

//----------------------------------------------------------------
// Function :   burstr_stop_usb
// Notes    :
// History  :
//----------------------------------------------------------------

VOID  burstr_stop_usb ( VOID )
{
  USBCS_1();                                    // CS = 1
}

#endif

#ifdef USB_NOSPI

//----------------------------------------------------------------
// Function :   INT0_Hndl
// Notes    :
// History  :
//----------------------------------------------------------------

#ifdef __IAR__
//interrupt [INT0_vect] VOID INT0_Hndl ( VOID )
#endif
#ifdef __GNU__
//SIGNAL( INT0_vect )
#endif
//{
//  usb_node_handler();
//}

//----------------------------------------------------------------
// Function :   enable_USBint
// Notes    :
// History  :
//----------------------------------------------------------------

VOID  enable_USBint( VOID )
{
//  __port_or( GIMSK, (1<<INT0) );
}

//----------------------------------------------------------------
// Function :   disable_USBint
// Notes    :
// History  :
//----------------------------------------------------------------

VOID  disable_USBint( VOID )
{
//  __port_and( GIMSK, ~(1<<INT0) );
}

//----------------------------------------------------------------
// Function :   hal4usb_init
// Notes    :
// History  :
//----------------------------------------------------------------

VOID   hal4usb_init( VOID )                       //
{                                                 //
  __port_or( DDRB, (CSPIN | A0PIN | RDPIN | WRPIN) ); // CS & A0 & RD & WR as output
  USBCS_1();                                      // CS = 1
  USBA0_0();                                      // A0 = 0
  USBRD_1();                                      // RD = 1
  USBWR_1();                                      // WR = 1
                                                  //
  enable_USBint();                                // USB INT = INT0 on Low Level
}

//----------------------------------------------------------------
// Function :   adr_set_usb
// Notes    :
// History  :
//----------------------------------------------------------------

VOID  adr_set_usb ( UINT8 adr )
{
  reg_adr = (adr & 0x3F);
}

//----------------------------------------------------------------
// Function :   dat_put_usb
// Notes    :
// History  :
//----------------------------------------------------------------

VOID  dat_put_usb ( UINT8 dta )
{
  WR_A0_1( reg_adr );
  WR_A0_0( dta );
}

//----------------------------------------------------------------
// Function :   burstw_start_usb
// Notes    :
// History  :
//----------------------------------------------------------------

VOID  burstw_start_usb ( UINT8 adr )
{
//  reg_adr = (adr & 0x3F);
  WR_A0_1( adr & 0x3F );
}

//----------------------------------------------------------------
// Function :   burstw_put_usb
// Notes    :
// History  :
//----------------------------------------------------------------

VOID  burstw_put_usb ( UINT8 dta )
{
//  WR_A0_1( reg_adr );
  WR_A0_0( dta );
}

//----------------------------------------------------------------
// Function :   burstw_stop_usb
// Notes    :
// History  :
//----------------------------------------------------------------

VOID  burstw_stop_usb ( VOID )
{
}

//----------------------------------------------------------------
// Function :   dat_get_usb
// Notes    :
// History  :
//----------------------------------------------------------------

UINT8 dat_get_usb ( VOID )
{
  REGISTER UINT8  d;

  WR_A0_1( reg_adr );
  RD_A0_0( d );
  return d;
}

//----------------------------------------------------------------
// Function :   burstr_start_usb
// Notes    :
// History  :
//----------------------------------------------------------------

VOID  burstr_start_usb ( UINT8 adr )
{
//  reg_adr = (adr & 0x3F);
  WR_A0_1( adr & 0x3F );
}

//----------------------------------------------------------------
// Function :   burstr_get_usb
// Notes    :
// History  :
//----------------------------------------------------------------

UINT8  burstr_get_usb ( VOID )
{
  REGISTER UINT8  d;

//  WR_A0_1( reg_adr );
  RD_A0_0( d );
  return d;
}

//----------------------------------------------------------------
// Function :   burstr_stop_usb
// Notes    :
// History  :
//----------------------------------------------------------------

VOID  burstr_stop_usb ( VOID )
{
}

#endif

//     End
