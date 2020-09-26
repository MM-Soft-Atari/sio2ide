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
//
// USB Mass Storage
//       Class Spec  1.0 Oct. 1998
//       Bulk Only Transport 1.0 Jun.21 1999
// Notes:
//   3. Share Mem between CBW & CSW to minimize Operations as well as RAM
//   2. CSW structure size is 13[0xd] bytes
//   1. bInterfaceProtocol for Bulk-Only Transport
//           0x50 = 'P'

#include <platform.h>
#include "hal4usb.h"
#include "usb_reg.h"
#include "usbxfer.h"
//
#include "rbccmd.h"
#include "msbot.h"
#include "rbc.h"

#ifdef DEBUG
  #undef DEBUG
#endif

//*************************************************************************
//  Public Data
//*************************************************************************

TPBLK_STRUC             TPBulk_Block;   // CBW/CSW buffer
XferStat_t              BOTXfer;        // Bulk Xfer control data
UINT8                   BOTStat;        // Bulk Xfer State Machine state

//*************************************************************************
//  Private Data
//*************************************************************************

STATIC BOOL             BOTBF_StallAtBulkIn;
STATIC BOOL             BOTBF_StallAtBulkOut;
STATIC INT16            BOT_TOut;

//----------------------------------------------------------------
// Function :   MSBOT_IsCBWValid
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC BOOL MSBOT_IsCBWValid( VOID )
{
  if ( TPBulk_CBW.dCBW_Signature == CBW_SIGNATURE &&
       TPBulk_CBW.bCBW_LUN <= 1 &&
       TPBulk_CBW.bCBW_CDBLen <= MAX_CDBLEN
     )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

//----------------------------------------------------------------
// Function :   MSBOT_ErrorHandler
// Notes    :
// History  :
//----------------------------------------------------------------

VOID MSBOT_ErrorHandler( UINT8 HostDevCase, INT16 wByteCounterDevWillXfer )
{
  TPBulk_CSW.dCSW_DataResidue = TPBulk_CBW.dCBW_DataXferLen - wByteCounterDevWillXfer;

#ifdef DEBUG
  Printf( "\nCase%X", HostDevCase );
#endif

  switch ( HostDevCase )
  {
    case CASEOK:
    case CASE1:     /* Hn=Dn*/
    case CASE6:     /* Hi=Di*/
      TPBulk_CSW.bCSW_Status = CSW_GOOD;
      break;
    case CASE12:    /* Ho=Do*/
      TPBulk_CSW.bCSW_Status = CSW_GOOD;
      break;
    case CASE2:     /* Hn<Di*/
    case CASE3:     /* Hn<Do*/
      //BOTBF_StallAtBulkIn = TRUE; // may or may-not
      TPBulk_CSW.bCSW_Status = CSW_PHASE_ERROR;
      break;
    case CASE4:     /* Hi>Dn*/
    case CASE5:     /* Hi>Di*/
//      BOTBF_StallAtBulkIn = TRUE;
      TPBulk_CSW.bCSW_Status = CSW_FAIL;        //CSW_GOOD or CSW_FAIL
      break;
    case CASE7:     /* Hi<Di*/
    case CASE8:     /* Hi<>Do */
      //BOTBF_StallAtBulkIn = TRUE; // may or may-not
      TPBulk_CSW.bCSW_Status = CSW_PHASE_ERROR;
      break;
    case CASE9:     /* Ho>Dn*/
    case CASE11:    /* Ho>Do*/
      //BOTBF_StallAtBulkOut = TRUE; // may or may-not
      TPBulk_CSW.bCSW_Status = CSW_FAIL;//CSW_GOOD or CSW_FAIL
      break;
    case CASE10:    /* Ho<>Di */
    case CASE13:    /* Ho<Do*/
      //TBF_StallAtBulkIn = TRUE;// may or may-not
      //TBF_StallAtBulkOut = TRUE;// may or may-not
      TPBulk_CSW.bCSW_Status = CSW_PHASE_ERROR;
      break;
    case CASECBW:   /* invalid CBW */
      BOTBF_StallAtBulkIn = TRUE;
      BOTBF_StallAtBulkOut = TRUE;

      TPBulk_CSW.bCSW_Status = CSW_PHASE_ERROR;
      break;
    case CASECMDFAIL:
      BOTBF_StallAtBulkIn = TRUE;
      TPBulk_CSW.bCSW_Status = CSW_FAIL;
      break;
    default:
      break;
  };
  TPBulk_CSW.dCSW_Signature = CSW_SIGNATURE;
//  TPBulk_CSW.dCSW_Tag = TPBulk_CBW.dCBW_Tag;
}

//----------------------------------------------------------------
// Function :   MSBOT_TimerUpd
// Notes    :
// History  :
//----------------------------------------------------------------

VOID MSBOT_TimerUpd( VOID )
{
  if( BOT_TOut <= 0 )
  {
    BOT_TOut = 0;
  }
  else
  {
    BOT_TOut--;
  }
}

//----------------------------------------------------------------
// Function :   MSBOT_CSWHandler
// Notes    :
// History  :
//----------------------------------------------------------------

VOID MSBOT_CSWHandler( VOID )
{
#ifdef DEBUG
  Printf( "\nCSW" );
#endif

  if ( BOTBF_StallAtBulkIn )
  {
    USB_STALL_EPN( BOTXfer.ep_num );        // Bulk-In

    TPBulk_CSW.dCSW_DataResidue += BOTXfer.restCnt;
    TPBulk_CSW.dCSW_DataResidue = rev_longword(TPBulk_CSW.dCSW_DataResidue);

    USB_Xfer_Setup( &BOTXfer, (UINT8*)&TPBulk_Block, NULL, sizeof(CSW), EVT_USB_BULK_TX, NULL );
    BOTStat = BOT_CSW;
  }
  else if ( BOTBF_StallAtBulkOut )
  {
    USB_STALL_EPN( BOTXfer.ep_num );        // Bulk-Out

    BOTStat = BOT_STALL;
  }
  else
  {
    TPBulk_CSW.dCSW_DataResidue += BOTXfer.restCnt;
    TPBulk_CSW.dCSW_DataResidue = rev_longword(TPBulk_CSW.dCSW_DataResidue);

    USB_Xfer_Setup( &BOTXfer, (UINT8*)&TPBulk_Block, NULL, sizeof(CSW), EVT_USB_BULK_TX, NULL );
    BOTStat = BOT_CSW;
  }
}

//----------------------------------------------------------------
// Function :   MSBOT_Handler
// Notes    :
// History  :
//----------------------------------------------------------------

VOID MSBOT_Handler( VOID )
{
  switch( BOTStat )
  {
    case BOT_START:
      BOT_TOut = 0;
      BOTBF_StallAtBulkIn  = FALSE;
      BOTBF_StallAtBulkOut = FALSE;
      USB_Xfer_Setup( &BOTXfer, (UINT8*)&TPBulk_Block, NULL, sizeof(CBW), EVT_USB_BULK_RX, NULL );
      BOTStat = BOT_IDLE;
    break;
    case BOT_IDLE:
      if( USB_Xfer_RX( &BOTXfer ) )
      {
#ifdef DEBUG
        Printf( "\nCBW" );
#endif

        if ( MSBOT_IsCBWValid() )
        {
#ifdef DEBUG
          Printf( " OK" );
          {
            UINT8   i = 0x1F;
            UINT8 * msg = (UINT8*)&TPBulk_Block;
            while( i-- )
            {
              Printf( " %X", *msg++ );
            }
          }
#endif

          RBC_Handler();
        }
        else
        {
#ifdef DEBUG
          Printf( " err" );
#endif

          // for Invalid CBW
          // Stall Both Bulk Endpoints
          // Let host goto reset recovery sequence
          MSBOT_ErrorHandler( CASECBW, 0 );
//          RBC_BuildSenseData( SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ADSENSE_ILLEGAL_COMMAND, 0 );
          MSBOT_CSWHandler();
        }
      }
    break;
    // Data from Host
    case BOT_DTOUT:
      BOT_TOut = 10 * T0_1S;
      BOTStat = BOT_DTOUTP;
    case BOT_DTOUTP:
      if( BOT_TOut == 0 )
      {
        BOTStat = BOT_START;
        MSBOT_ErrorHandler( CASE11, BOTXfer.restCnt );
        MSBOT_CSWHandler();
        break;
      }
      if( USB_Xfer_RX( &BOTXfer ) )
      {
#ifdef DEBUG
        Printf( "\noutOK" );
#endif

        MSBOT_CSWHandler();
      }
    break;
    // Data to Host
    case BOT_DTIN:
      BOT_TOut = 10 * T0_1S;
      BOTStat = BOT_DTINP;
    case BOT_DTINP:
      if( BOT_TOut == 0 )
      {
        BOTStat = BOT_START;
        MSBOT_ErrorHandler( CASE7, BOTXfer.restCnt );
        MSBOT_CSWHandler();
        break;
      }
      if( USB_Xfer_TX( &BOTXfer ) )
      {
#ifdef DEBUG
        Printf( "\ninOK" );
#endif

        MSBOT_CSWHandler();
      }
    break;
    case BOT_CSW:
      BOT_TOut = 10 * T0_1S;
      BOTStat = BOT_CSWP;
    case BOT_CSWP:
      if( BOT_TOut == 0 )
      {
        BOTStat = BOT_START;
        MSBOT_ErrorHandler( CASE7, BOTXfer.restCnt );
        MSBOT_CSWHandler();
        break;
      }
      if( USB_Xfer_TX( &BOTXfer ) )
      {
#ifdef DEBUG
        Printf( "\nsOK" );
#endif

        BOTStat = BOT_START;
      }
    break;
    case BOT_STALL:
      BOTStat = BOT_START;
    break;
    default:
      BOTStat = BOT_START;
    break;
  };
}

//*************************************************************************
// Bulk-Only TP - Class Request   Handler
//*************************************************************************

//----------------------------------------------------------------
// Function :   MSBOT_ResetATA
// Notes    :
// History  :
//----------------------------------------------------------------

VOID MSBOT_ResetATA( USB_request_t * req )
{
#ifdef DEBUG
  Printf( "\nrATA" );
#endif

  req = req;
  // reset ATA controller

  BOTStat = BOT_START;
  zero_length_data_response( ENDPOINT_0 );
}

//----------------------------------------------------------------
// Function :   MSBOT_GetMaxLUN
// Notes    :
// History  :
//----------------------------------------------------------------

VOID MSBOT_GetMaxLUN( USB_request_t * req )
{
  UINT8    c = 0;

#ifdef DEBUG
  Printf( "\ngLUN" );
#endif

  req = req;
  //
  send_control_data( (UINT8 *)&c, NULL, sizeof(c) );
//  USB_STALL_EP0();
}

//      End
