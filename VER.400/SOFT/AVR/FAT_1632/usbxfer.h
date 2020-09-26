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

#ifndef __USBXFER_H__
  #define __USBXFER_H__

typedef struct XferStat  XferStat_t;

typedef UINT16 (*Xfer_RXhndl_t)( XferStat_t * xfer );

struct XferStat
{
  UINT8             * dat;
  UINT8             * bufRam;
  UINT8 CONST       * bufRom;
  INT16               restCnt;
  INT16               Cnt;
  endpoint_t          ep_num;
  UINT8               event;
  Xfer_RXhndl_t       datHndl;
};

VOID   USB_Xfer_Setup     ( XferStat_t * xfer, UINT8 * bufRam, UINT8 CONST * bufRom, UINT16 count, UINT8 event, Xfer_RXhndl_t hndl );
BOOL   USB_Xfer_TX        ( XferStat_t * xfer );
BOOL   USB_Xfer_RX        ( XferStat_t * xfer );
//UINT16 USB_Xfer_RX_Data   ( XferStat_t * xfer );

#endif // __usbxfer_h__

//      End
