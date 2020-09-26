//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE:
//
// MODULE NAME:
//
// PURPOSE:
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001   30-07-2002 MM     Created
//
//****************************************************************

#include <platform.h>
#include "usb.h"
#include "usbxfer.h"

#ifdef DEBUG
  #undef DEBUG
#endif

//----------------------------------------------------------------
// Function :   USB_Xfer_Setup
// Notes    :
// History  :
//----------------------------------------------------------------

VOID USB_Xfer_Setup( XferStat_t * xfer, UINT8 * bufRam, UINT8 CONST * bufRom, UINT16 count, UINT8 event, Xfer_RXhndl_t hndl )
{
  UINT16  i;

#ifdef DEBUG
  Printf( "\nXfer(%u)", count );
#endif

  TST1_ON();
  for( i = 0; i < 750; i++ )
  {
    _NOP();
  }
  TST1_OFF();

  ASSERT( (UINT16)bufRam != (UINT16)bufRom );

  //
//  clear_event( xfer->event );
  //
  xfer->dat     = bufRam;
  xfer->bufRam  = bufRam;
  xfer->bufRom  = bufRom;
  xfer->Cnt     = count;
  xfer->restCnt = count;
  xfer->event   = event;
  xfer->datHndl = hndl;
  //
  if( event == EVT_USB_BULK_TX )
  {
#ifdef DEBUG
    Printf( "T1" );
#endif

    if( xfer->datHndl )
    {
      xfer->restCnt = 0;
    }
    xfer->ep_num = ENDPOINT_1;
    send_event( event );        // Start TX
  }
  if( event == EVT_USB_BULK_RX )
  {
#ifdef DEBUG
    Printf( "R2" );
#endif

    xfer->ep_num = ENDPOINT_2;
  }
  if( event == EVT_USB_ISO_TX )
  {
#ifdef DEBUG
    Printf( "T3" );
#endif

    if( xfer->datHndl )
    {
      xfer->restCnt = 0;
    }
    xfer->ep_num = ENDPOINT_3;
    send_event( event );        // Start TX
  }
  if( event == EVT_USB_ISO_RX )
  {
#ifdef DEBUG
    Printf( "R4" );
#endif

    xfer->ep_num = ENDPOINT_4;
  }
}

//----------------------------------------------------------------
// Function :   USB_Xfer_RX
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL USB_Xfer_RX( XferStat_t * xfer )
{
  UINT8      readCnt;

  if( event_table & xfer->event )
  {
    clear_event( xfer->event );
    readCnt = USB_Receive_Data( xfer->restCnt, xfer->bufRam, xfer->ep_num );
    //
    xfer->restCnt -= readCnt;
    xfer->bufRam  += readCnt;
    //
    if( xfer->restCnt <= 0 )
    {
      if( xfer->datHndl )
      {
        // Write data from bufRam to IO
        if( xfer->datHndl( xfer ) == 0 )
        {
#ifdef DEBUG
//          Printf( "\nRe" );
#endif
          TST1_OFF();

          return TRUE;
        }
        else
        {
          TST1_ON();

          xfer->restCnt = xfer->Cnt;    // RX next data block
          xfer->bufRam  = xfer->dat;
        }
      }
      else
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   USB_Xfer_RX_Data
// Notes    :
// History  :
//----------------------------------------------------------------

//UINT16 USB_Xfer_RX_Data( XferStat_t * xfer )
//{
//  if( xfer->datHndl )
//  {
//    return xfer->datHndl( xfer );
//  }
//  return 0;
//}

//----------------------------------------------------------------
// Function :   USB_Xfer_TX
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL USB_Xfer_TX( XferStat_t * xfer )
{
  UINT16      writeCnt;

  if( event_table & xfer->event )
  {
    clear_event( xfer->event );
    //
    if( xfer->datHndl )
    {
      if( xfer->restCnt == 0 )
      {
        TST1_ON();

        // Read data from IO to bufRam
        if( xfer->datHndl( xfer ) == 0 )
        {
#ifdef DEBUG
//          Printf( "\nSe" );
#endif
          TST1_OFF();

          //------------------------------------
          // Send zero length packet to
          // mark end of the data, if needed
          // -----------------------------------
    //      usbn9604_tx_enable(ENDPOINT_1);
          return TRUE;
        }
        else
        {
#ifdef DEBUG
//          Printf( "\nSs" );
#endif
          xfer->restCnt = xfer->Cnt;
          xfer->bufRam  = xfer->dat;
        }
      }
    }
    if( xfer->bufRam )
    {
      writeCnt = USB_Transmit_Data( xfer->restCnt, xfer->bufRam, NULL, xfer->ep_num );
      //
      xfer->restCnt -= writeCnt;
      xfer->bufRam  += writeCnt;

      ASSERT( xfer->restCnt >= 0 );

#ifdef DEBUG
//      Printf( "\nWr%X,Rst%X", writeCnt, xfer->restCnt );
#endif

      if( (xfer->restCnt == 0) && !xfer->datHndl )
      {
        //------------------------------------
        // Send zero length packet to
        // mark end of the data, if needed
        // -----------------------------------
  //      usbn9604_tx_enable(ENDPOINT_1);
        return TRUE;
      }
    }
    if( xfer->bufRom )
    {
      writeCnt = USB_Transmit_Data( xfer->restCnt, NULL, xfer->bufRom, xfer->ep_num );
      //
      xfer->restCnt -= writeCnt;
      xfer->bufRom  += writeCnt;

#ifdef DEBUG
//      Printf( "\nWr%X,Rst%X", writeCnt, xfer->restCnt );
#endif

      ASSERT( xfer->restCnt >= 0 );

      if( xfer->restCnt == 0 )
      {
        //------------------------------------
        // Send zero length packet to
        // mark end of the data, if needed
        // -----------------------------------
  //      usbn9604_tx_enable(ENDPOINT_1);
        return TRUE;
      }
    }
  }
  return FALSE;
}

//      End
