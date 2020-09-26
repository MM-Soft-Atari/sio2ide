//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: HAL4USB.H
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

#ifndef __hal4usb_h__
  #define __hal4usb_h__

//#define USB_NOSPI                // USB via SPI or NOSPI

EXTERN UINT8    reg_adr;

#ifdef USB_SPI

//
// HAL4USB setting
//
//  #define SPI1_BRATE      1000000L       // SPI1 Baud Rate (Master mode)

  #define read_usb( adr )               \
(                                       \
  adr_set_usb ( adr ),                  \
  dat_get_usb ()                        \
)

  #define  write_usb( adr, dta )        \
(                                       \
  adr_set_usb ( adr ),                  \
  dat_put_usb ( dta )                   \
)

#ifdef __IAR_SYSTEMS_ICC
  #define SPI_IO( dt )                            \
  (                                               \
    __outp( SPDR, dt ),                           \
    _OPC( 0x9B77 ),                               \
    _OPC( 0xCFFE )                                \
  )
#else
  #define SPI_IO( dt )                            \
  {                                               \
    __outp( SPDR, dt );                           \
    loop_until_bit_is_set( SPSR, SPIF );          \
  }
#endif

//----------------------------------------------------------------
// Macro    :   _burstr_get_usb
// Notes    :
// History  :
//----------------------------------------------------------------

#define _burstr_get_usb()                         \
(                                                 \
  SPI_IO( reg_adr ),                              \
  SPI_IO( reg_adr | 0x40 ),                       \
  __inp(SPDR)                                     \
)

//----------------------------------------------------------------
// Macro    :   _burstw_put_usb
// Notes    :
// History  :
//----------------------------------------------------------------

#define _burstw_put_usb( dta )                    \
(                                                 \
  SPI_IO( dta )                                   \
)

#endif

#ifdef USB_NOSPI

// CS/A0/RD/WR Port (PB)
#define CSPIN   (1<<0)          // PB0
#define A0PIN   (1<<5)          // PB5
#define RDPIN   (1<<6)          // PB6
#define WRPIN   (1<<7)          // PB7
#define USBA0_0()       __port_and( PORTB, ~A0PIN )
#define USBA0_1()       __port_or( PORTB, A0PIN )
#define USBRD_0()       __port_and( PORTB, ~RDPIN )
#define USBRD_1()       __port_or( PORTB, RDPIN )
#define USBWR_0()       __port_and( PORTB, ~WRPIN )
#define USBWR_1()       __port_or( PORTB, WRPIN )
#define USBCS_0()       __port_and( PORTB, ~CSPIN )
#define USBCS_1()       __port_or( PORTB, CSPIN )
//
#define LINES_0( l )    __port_and( PORTB, ~(l) )
#define LINES_1( l )    __port_or( PORTB, (l) )
//
// Data Bus (PA)
//
#define USBD_OUT()      (__outp(DDRA,0xFF))
#define USB_O( d )      (__outp(PORTA,d))
#define USBD_INP()      (__outp(DDRA,0),__outp(PORTA,0))
#define USB_I()         (__inp(PINA))
#define USBD_HIZ()      USBD_INP()  //(__outp(DDRA,0xFF),__outp(PORTA,0xFF))

#define WR_A0_0( d )            \
(                               \
  disable_interrupt(),          \
  USBD_OUT(),                   \
  USBCS_0(),                    \
  USBWR_0(),                    \
  _NOP(),                       \
  USB_O( d ),                   \
  USBWR_1(),                    \
  USBCS_1(),                    \
  USBD_HIZ(),                   \
  enable_interrupt()            \
)

#define WR_A0_1( d )            \
(                               \
  disable_interrupt(),          \
  USBD_OUT(),                   \
  USBA0_1(),                    \
  USBCS_0(),                    \
  USBWR_0(),                    \
  _NOP(),                       \
  USB_O( d ),                   \
  USBWR_1(),                    \
  USBCS_1(),                    \
  USBA0_0(),                    \
  USBD_HIZ(),                   \
  enable_interrupt()            \
)

#define RD_A0_0( d )            \
(                               \
  disable_interrupt(),          \
  USBD_INP(),                   \
  USBCS_0(),                    \
  USBRD_0(),                    \
  _NOP(),                       \
  d = USB_I(),                  \
  USBRD_1(),                    \
  USBCS_1(),                    \
  USBD_HIZ(),                   \
  enable_interrupt()            \
)

#define read_usb( adr )                            \
(                                                  \
  adr_set_usb ( adr ),                             \
  dat_get_usb ()                                   \
)

#define  write_usb( adr, dta )                     \
(                                                  \
  adr_set_usb ( adr ),                             \
  dat_put_usb ( dta )                              \
)

#define _burstr_get_usb( dta )                    \
(                                                 \
  RD_A0_0( dta )                                  \
)

#define _burstw_put_usb( dta )                    \
(                                                 \
  WR_A0_0( dta )                                  \
)

#endif

//****************************************************************
//
// Interface
//
//****************************************************************

VOID  enable_USBint     ( VOID );
VOID  disable_USBint    ( VOID );

VOID  hal4usb_init      ( VOID );

VOID  adr_set_usb       ( UINT8 adr );
VOID  dat_put_usb       ( UINT8 dta );
UINT8 dat_get_usb       ( VOID );

VOID  burstw_start_usb  ( UINT8 adr );
VOID  burstw_put_usb    ( UINT8 dta ); // Use macro _burstw_put_usb
VOID  burstw_stop_usb   ( VOID );

VOID  burstr_start_usb  ( UINT8 adr );
UINT8 burstr_get_usb    ( VOID );      // Use macro _burstr_get_usb
VOID  burstr_stop_usb   ( VOID );

#endif
