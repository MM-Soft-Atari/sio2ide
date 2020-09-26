//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: HAL4IDE.C
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

#include <platform.h>
#include "hal4ide.h"

//
// IDE Low Level Driver
//

#ifndef __AVR__

//----------------------------------------------------------------
// Function :   IDE_Swpb
// Notes    :
// History  :
//----------------------------------------------------------------

UINT8 Swpb( UINT8 b )
{
  REGISTER UINT8  m1 = 0x01;
  REGISTER UINT8  m2 = 0x80;
  REGISTER UINT8   o = 0;

  while( m1 )
  {
    if( b & m1 )        o |= m2;
    m1 <<= 1;
    m2 >>= 1;
  }
  return o;
}

#endif

//----------------------------------------------------------------
// Function :   IDE_RegWR
// Notes    :
// History  :
//----------------------------------------------------------------

VOID IDE_RegWR( UINT16 reg, UINT16 dat )
{
  REGISTER UINT8  l;            //
                                //
//  h = high( dat );              //
  l = IDE_Swpb( low( dat ) );   //
  IDE_DATA_OUT();               // IDE Data Bus in Out mode

  disable_interrupt();

  IDE_CTRL_REG( reg );          // Set Register Address
  IDE_CTRL_WR();                // WR ON (low)
  IDE_DATA_PUTL( l );           // Set Data Low
  IDE_DATA_PUTH( high( dat ) ); // Set Data High
  IDE_CTRL_NO_WR();             // WR OFF (high) rising edge on WR
  _NOP();                       //
  IDE_CTRL_NONE();              // Release Device

  enable_interrupt();

  IDE_DATA_HIZ();               // IDE Data Bus in Hi-Z mode
}

//----------------------------------------------------------------
// Function :   IDE_WR_DataBlk
// Notes    :
// History  :
//----------------------------------------------------------------

#ifdef IDE_BULK
VOID IDE_WR_DataBlk( UINT16 * dat )
{
  REGISTER UINT8  h,l;            //
  REGISTER UINT16 cnt = 256;      //
                                  //
  IDE_DATA_OUT();                 // IDE Data Bus in Out mode
  while( cnt-- )
  {
    h = high( *dat );             //
    l = IDE_Swpb( low( *dat ) );  //
    dat++;                        //

    disable_interrupt();

//    IDE_CTRL_REG( IDE_REG_DATA ); // Set Register Address
    _OPC( 0x9895 );
    _OPC( 0x9A96 );
    _OPC( 0x98C2 );
    _OPC( 0x98C3 );
    _OPC( 0x98C4 );
    IDE_CTRL_WR();                // WR ON (low)
    IDE_DATA_PUTL( l );           // Set Data Low
    IDE_DATA_PUTH( h );           // Set Data High
    IDE_CTRL_NO_WR();             // WR OFF (high) rising edge on WR
    _NOP();                       //
    IDE_CTRL_NONE();              // Release Device

    enable_interrupt();
  }
  IDE_DATA_HIZ();               // IDE Data Bus in Hi-Z mode
}
#endif

//----------------------------------------------------------------
// Function :   IDE_RegRD
// Notes    :
// History  :
//----------------------------------------------------------------

UINT16 IDE_RegRD( UINT16 reg )
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
  IDE_CTRL_NONE();              // Release Device

  enable_interrupt();

  IDE_DATA_HIZ();               // IDE Data Bus in Hi-Z mode
  l = IDE_Swpb( l );            //
                                //
  return (UINT16)(((UINT16)h << 8) | l);
}

//----------------------------------------------------------------
// Function :   IDE_RD_DataBlk
// Notes    :
// History  :
//----------------------------------------------------------------

#ifdef IDE_BULK
VOID IDE_RD_DataBlk( UINT16 * dat )
{
  REGISTER UINT8  l, h;
  REGISTER UINT16 cnt = 256;

  IDE_DATA_INP();               // IDE Data Bus in Inp mode

  while( cnt-- )
  {
    disable_interrupt();

    IDE_CTRL_REG( IDE_REG_DATA ); // Set Register Address
//    _OPC( 0x9895 );
//    _OPC( 0x9A96 );
//    _OPC( 0x98C2 );
//    _OPC( 0x98C3 );
//    _OPC( 0x98C4 );
    IDE_CTRL_RD();                // RD ON (low)
    _NOP();                       //
    IDE_CTRL_NO_RD();             // RD OFF (high) rising edge on RD
    l = IDE_DATA_GETL();          // Get Data Low
    h = IDE_DATA_GETH();          // Get Data High
    IDE_CTRL_NONE();              // Release Device

    enable_interrupt();

    l = IDE_Swpb( l );            //
                                  //
    *dat++ = (UINT16)(((UINT16)h << 8) | l);
  }
  IDE_DATA_HIZ();               // IDE Data Bus in Hi-Z mode
}
#endif

//     End
