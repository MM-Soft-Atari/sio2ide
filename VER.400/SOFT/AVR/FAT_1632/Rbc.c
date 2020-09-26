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

//*************************************************************************
//  Private Data
//*************************************************************************

#define SECT_SIZE       512
#define BUF_SIZE        SECT_SIZE

#pragma memory=dataseg(CRAM01)
UINT8            XFerBuf[ BUF_SIZE ];
#pragma memory=default

//*************************************************************************
//  Const Data
//*************************************************************************

CONST REQUEST_SENSE_DATA   Req_SenseData=
{
  // 70 00 05 00 00 00 00 0a 00 00 00 00 24 00
  //0x70, 0x00, 0x06, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x29,
  //0x00, 0x00, 0x00, 0x00, 0x00

  0x70, // INT8 ResponseCode : 7;
  0,     //   INT8 Valid : 1;

  0,//   INT8 SegmentNum;

  0x5,//   INT8 SenseKey : 4; 5= illegal request
  0,  // INT8 Reserved0 : 1;
  0,//   INT8 WrongLenIndicator : 1;
  0,//   INT8 EndofMedium : 1;
  0,//   INT8 FileMark : 1;

  0,//   INT8 Info_0;
  0,//   INT8 Info_1;
  0,//   INT8 Info_2;
  0,//   INT8 Info_3;

  0xA,//    INT8 AdditionalSenseLen;

  0,//    INT8 CommandSpecInfo_0;
  0,//    INT8 CommandSpecInfo_1;
  0,//    INT8 CommandSpecInfo_2;
  0,//    INT8 CommandSpecInfo_3;

  0x24,//    INT8 ASC;
  0,//    INT8 ASCQ;
//  0,//    INT8 FieldReplacableUnitCode;
//  0,//    INT8 SenseKeySpec_0 : 7;
//  0,//    INT8 SenseKeySpecValid : 1;
//  0,//    INT8 SenseKeySpec_1;
//  0 //    INT8 SenseKeySpec_2;

};

STATIC RBC_PROPERTY                    RBC_PropertyData;
STATIC T_drvinf                      * hddInfo;
STATIC UINT32                          aSect;
STATIC UINT16                          aLen;

#define PARAMETER_LIST_LENGTH   (sizeof(MODE_PARAMETER_HEAD))

CONST MODE_PARAMETER_HEAD ParaHeadMask =
{
  PARAMETER_LIST_LENGTH-1,    /* mode data length*/
  0,                          /* medium type*/
  0,                          /* device spec Param*/
  0                           /* block Descriptor length*/
};

CONST MODE_RBC_DEVICE_PARAMETERS_PAGE ParaPageMask =
{
  MODE_PAGE_RBC_DEVICE_PARAMETERS,
  0,      /*Reserved*/
  0,      /*PageSavable*/

  0x00,   /*PageLength*/

  0,      /*WriteCacheDisable*/
  00,     /*Reserved*/

  {
    /*Logical block Size = 512 Bytes*/
    0x00,
    0x00
  },

  {
    /*Number of logical blocks*/
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
  },

  0x00,   /*Power/Peformance*/

  0,      /*LockDisable*/
  0,      /*FormatDisable*/
  0,      /*WriteDisable*/
  0,      /*ReadDisable*/
  0x0,    /*Reserved*/

  0x00    /*Reserved*/
};

CONST MODE_PARAMETER_HEAD DefaultParaHead =
{
  PARAMETER_LIST_LENGTH-1,    /* mode data length*/
  0,                          /* medium type*/
  0,                          /* device spec Param*/
  0                           /* block Descriptor length*/
};

CONST MODE_RBC_DEVICE_PARAMETERS_PAGE DefaultParaPage =
{
  MODE_PAGE_RBC_DEVICE_PARAMETERS,
  0,      /*Reserved*/
  1,      /*PageSavable*/

  0x0B,   /*PageLength*/

  0,      /*WriteCacheDisable*/
  00,     /*Reserved*/

  {
    /*Logical block Size = 512 Bytes*/
    0x02,
    0x00
  },

  {
    /*Number of logical blocks*/
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
  },

  0xFF,   /*Power/Peformance*/

  0,      /*LockDisable*/
  0,      /*FormatDisable*/
  0,      /*WriteDisable*/
  0,      /*ReadDisable*/
  0x0,    /*Reserved*/

  0x00    /*Reserved*/
};

CONST VPD_SERIAL_PAGE SerialPage =
{
  DIRECT_ACCESS_DEVICE,
  0x00,

  VPDPAGE_SERIAL_NUMBER,

  0x00,

  24,     //size of SerialNumber

  {
    // SerialNumber
    '0',0,
    '0',0,
    '0',0,
    '0',0,

    '0',0,
    '0',0,
    '0',0,
    '0',0,

    '0',0,
    '0',0,
    '0',0,
    '0',0
  }
};

CONST VPD_DEVICE_ID_PAGE DeviceIDPage =
{
  DIRECT_ACCESS_DEVICE,
  0x00,
  VPDPAGE_DEVICE_IDENTITY,
  0x00,
  sizeof(ASCII_ID_DESCRIPTOR),
//  {
    0x02,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    sizeof(ASCII_ID_STRING),
    {
      // ASCII_ID_STRING
      'S','I','O','2',
      'I','D','E',' ',
      'S','u','p','e',
      'r',' ','D','i',
      's','k',' ',' ',
      ' ',' ',' ',' ',
      ' ',' ',' ',' ',
      ' ',' ',' ',' '
    },
//  }
};

CONST STD_INQUIRYDATA inquiryData =
{
  DIRECT_ACCESS_DEVICE,
  0,//INT8 Reserved0 : 3;

  0,//INT8 Reserved1 : 7;
  1,//INT8 RemovableMedia : 1;

  2,//INT8 Reserved2;

  2,//INT8 Reserved3 : 5;
  0,//INT8 NormACA : 1;
  0,//INT8 Obsolete0 : 1;
  0,//INT8 AERC : 1;

  //INT8 Reserved4[3];
  {
    0x1F,0,0
  },

  0,//INT8 SoftReset : 1;
  0,//INT8 CommandQueue : 1;
  0,//INT8 Reserved5 : 1;
  0,//INT8 LinkedCommands : 1;
  0,//INT8 Synchronous : 1;
  0,//INT8 Wide16Bit : 1;
  0,//INT8 Wide32Bit : 1;
  0,//INT8 RelativeAddressing : 1;

  //INT8 VendorId[8];
  {
    'M','M','S','o',
    'f','t',' ',' '
  },

  //INT8 ProductId[16];
  {
    'S','I','O','2',
    'I','D','E',' ',
    'D','r','i','v',
    'e',' ',' ',' '
  },

  //INT8 ProductRevisionLevel[4];
  {
    '1','.','1','1'
  }

#ifndef USE_SMALLINQ

//  Above is 36 bytes
//  can be tranmitted by Bulk

  //INT8 VendorSpecific[20]; out[64 bytes] within one packet only.
  ,{
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
  },

  0,//INT8 InfoUnitSupport : 1;
  0,//INT8 QuickArbitSupport : 1;
  0,//INT8 Clocking : 2;
  0,//INT8 Reserved1 : 4;
  0,//INT8  Reserved2 ;

  //USHORT VersionDescriptor[8] ;
  {
    0, 0, 0, 0,
    0, 0, 0, 0
  },

  //INT8 Reserved3[22];
  {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0
  }
#endif

};

//*************************************************************************
//  Subroutines
//*************************************************************************

//----------------------------------------------------------------
// Function :   RBC_Init
// Notes    :
// History  :
//----------------------------------------------------------------

VOID RBC_Init( T_drvinf * DrvInfo  )
{
  hddInfo = DrvInfo;
  memset( XFerBuf, 0, BUF_SIZE );
}

//----------------------------------------------------------------
// Function :   RBC_BuildSenseData
// Notes    :
// History  :
//----------------------------------------------------------------
/*
VOID RBC_BuildSenseData( UINT8 SenseKey, UINT8 ASC, UINT8 ASCQ )
{
  RBC_SenseData.ResponseCode = SCSI_RESPONSECODE_CURRENT_ERROR;
  RBC_SenseData.Valid = 0;
  //RBC_SenseData.SegmentNum = 0;
  RBC_SenseData.SenseKey =  SenseKey;
  //RBC_SenseData.Reserved0 = 0;
  //RBC_SenseData.WrongLenIndicator = 0;
  //RBC_SenseData.EndofMedium = 0;
  //RBC_SenseData.FileMark = 0;
  //RBC_SenseData.Info_0 = 0;
  //RBC_SenseData.Info_1 = 0;
  //RBC_SenseData.Info_2 = 0;
  //RBC_SenseData.Info_3 = 0;
  RBC_SenseData.AdditionalSenseLen = 0xa;
  //RBC_SenseData.CommandSpecInfo_0 = 0;
  //RBC_SenseData.CommandSpecInfo_1 = 0;
  //RBC_SenseData.CommandSpecInfo_2 = 0;
  //RBC_SenseData.CommandSpecInfo_3 = 0;
  RBC_SenseData.ASC = ASC;
  RBC_SenseData.ASCQ = ASCQ;
  //RBC_SenseData.FieldReplacableUnitCode = 0;
  //RBC_SenseData.SenseKeySpec_0 = 0;
  //RBC_SenseData.SenseKeySpecValid = 0;
  //RBC_SenseData.SenseKeySpec_1 = 0;
  //RBC_SenseData.SenseKeySpec_2 = 0;
}
*/

//----------------------------------------------------------------
// Function :   RBC_Handler
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL RBC_Handler( VOID )
{
#define cdbGeneric RBC_CDB.RbcCdb_Generic
  BOOL retStatus = FALSE;

#ifdef DEBUG
  Printf( "\nRBC:%X", cdbGeneric.OperationCode );
#endif

  switch ( cdbGeneric.OperationCode )
  {
    /* required command */
    case RBC_CMD_READ10:
      retStatus = RBC_Read();
      break;
    case RBC_CMD_READCAPACITY:
      retStatus = RBC_ReadCapacity();
      break;
    case RBC_CMD_STARTSTOPUNIT:
      retStatus = RBC_OnOffUnit();
      break;
    case RBC_CMD_SYNCCACHE:
      retStatus = RBC_SyncCache();
      break;
    case RBC_CMD_VERIFY10:
      retStatus = RBC_Verify();
      break;
    case RBC_CMD_WRITE10:
      retStatus = RBC_Write();
      break;
    case SPC_CMD_INQUIRY:
      retStatus = SPC_Inquiry();
      break;
    case SPC_CMD_MODESELECT6:                   // 0x15
      retStatus = SPC_ModeSelect();
      break;
    case SPC_CMD_MODESENSE6:
      retStatus = SPC_ModeSense();              // 0x1A
      break;
    case SPC_CMD_PRVENTALLOWMEDIUMREMOVAL:
      retStatus = SPC_LockMedia();
      break;
    case SPC_CMD_TESTUNITREADY:                 // 0x00
      retStatus = SPC_TestUnit();
      break;
    case SPC_CMD_REQUESTSENSE:                  // 0x03
      retStatus = SPC_RequestSense();
      break;

#ifdef RBC_USEOPTIONAL
      /* optional commands */
    case RBC_CMD_FORMAT:
      retStatus = RBC_Format();
      break;
    case SPC_CMD_RESERVE6:
      retStatus = SPC_Reserve6();
      break;
    case SPC_CMD_RELEASE6:
      retStatus = SPC_Release6();
      break;
    case SPC_CMD_PERSISTANTRESERVIN:
      retStatus = SPC_PersisReserveIn();
      break;
    case SPC_CMD_PERSISTANTRESERVOUT:
      retStatus = SPC_PersisReserveOut();
      break;
    case SPC_CMD_WRITEBUFFER:
      retStatus = SPC_WriteBuff();
      break;
    case SPC_CMD_READLONG:
      retStatus = SPC_READLONG();//0x23
      break;
#endif

    default:
      // Invalid CBW
      MSBOT_ErrorHandler( CASECBW, 0 );
//      RBC_BuildSenseData( SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ADSENSE_ILLEGAL_COMMAND, 0 );
      MSBOT_CSWHandler();
    return retStatus;
  };
//  if( !retStatus )
//  {
//    MSBOT_ErrorHandler( CASECBW, 0 );
//    MSBOT_CSWHandler();
//  }
  return retStatus;
#undef cdbGeneric
}

////////////////////////////////////////////////////////////////////////////////////
// Reduced Block Command Support
////////////////////////////////////////////////////////////////////////////////////

EXTERN CONST INT16 fifo_sizes[];

//----------------------------------------------------------------
// Function :   RBC_Read_Data
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC UINT16 RBC_Read_Data( XferStat_t * xfer )
{
#ifdef DEBUG
//  Printf( "\nIDEr%u,%l", aLen, aSect );
#endif
  // Read Data from ATA
  if( aLen-- )
  {
    if( IDE_SectorGet( (UINT16*)xfer->dat, hddInfo, aSect++ ) )
    {
      return xfer->Cnt;
    }
  }
  return 0;
}

//----------------------------------------------------------------
// Function :   RBC_Read
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL RBC_Read( VOID )
{
#define cdbRead RBC_CDB.RbcCdb_Read
  aLen    = (UINT16)(cdbRead.XferLength_1<<8) +
            (cdbRead.XferLength_0);

//  aSect   = (UINT32)((cdbRead.LBA.LBA_W8.LBA_3&0xF)<<24) +
//            (UINT32)(cdbRead.LBA.LBA_W8.LBA_2<<16) +
//            (UINT32)(cdbRead.LBA.LBA_W8.LBA_1<<8) +
//            (UINT32)(cdbRead.LBA.LBA_W8.LBA_0);

  aSect = rev_longword( cdbRead.LBA.LBA_W32 ) & 0x0FFFFFFF;

  // Read Data from ATA
#ifdef DEBUG
  Printf( "\nATAr%u,%l,%u", aLen, aSect, CBW_wXferLen );
#endif

  if( aLen == 0 )
  {
    return FALSE;
  }
  if( !hddInfo || ((CBW_wXferLen / aLen) != SECT_SIZE) )
  {
    return FALSE;
  }

  // Config TPBulkXfer Paras (data to Host)
  USB_Xfer_Setup( &BOTXfer, XFerBuf, NULL, SECT_SIZE, EVT_USB_BULK_TX, RBC_Read_Data );
  BOTStat = BOT_DTIN;

  MSBOT_ErrorHandler( CASE6, CBW_wXferLen );
//  RBC_BuildSenseData( SCSI_SENSE_NO_SENSE, 0, 0 );

  return TRUE;

#undef cdbRead
}

//----------------------------------------------------------------
// Function :   RBC_Write_Data
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC UINT16 RBC_Write_Data( XferStat_t * xfer )
{
#ifdef DEBUG
//  Printf( "\nIDEw%u,%l", aLen, aSect );
#endif
  // Write Data to ATA
  if( aLen-- )
  {
    if( IDE_SectorPut( (UINT16*)xfer->dat, hddInfo, aSect++ ) )
    {
      return (aLen == 0) ? 0 : xfer->Cnt;
    }
  }
  return 0;
}

//----------------------------------------------------------------
// Function :   RBC_Write
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL RBC_Write( VOID )
{
#define cdbWrite    RBC_CDB.RbcCdb_Write
  aLen    = (UINT16)(cdbWrite.XferLength_1<<8) +
            (cdbWrite.XferLength_0);

//  aSect   = (UINT32)((cdbWrite.LBA.LBA_W8.LBA_3&0xF)<<24) +
//            (UINT32)(cdbWrite.LBA.LBA_W8.LBA_2<<16) +
//            (UINT32)(cdbWrite.LBA.LBA_W8.LBA_1<<8) +
//            (UINT32)(cdbWrite.LBA.LBA_W8.LBA_0);

  aSect = rev_longword( cdbWrite.LBA.LBA_W32 ) & 0x0FFFFFFF;

  // Write Data to ATA
#ifdef DEBUG
  Printf( "\nATAw%u,%l,%u", aLen, aSect, CBW_wXferLen );
#endif

  if( aLen == 0 )
  {
    return FALSE;
  }
  if( !hddInfo || ((CBW_wXferLen / aLen) != SECT_SIZE) )
  {
    return FALSE;
  }

  // Config TPBulkXfer Paras (data from Host)
  USB_Xfer_Setup( &BOTXfer, XFerBuf, NULL, SECT_SIZE, EVT_USB_BULK_RX, RBC_Write_Data );
  BOTStat = BOT_DTOUT;

  MSBOT_ErrorHandler( CASE12, CBW_wXferLen );
//  RBC_BuildSenseData(SCSI_SENSE_NO_SENSE,0,0);

  return TRUE;

#undef cdbWrite
}

//----------------------------------------------------------------
// Function :   RBC_ReadCapacity
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL RBC_ReadCapacity( VOID )
{
#define cdbReadCap RBC_CDB.RbcCdb_ReadCapacity

  // Calculate last sector.
  if( hddInfo )
  {
    cdbReadCap.tmpVar.l[1] = IDE_GetMaxSec( hddInfo );
  }
  else
  {
    cdbReadCap.tmpVar.l[1] = 1;
  }

#ifdef DEBUG
  Printf( "\nCAP,%l", cdbReadCap.tmpVar.l[1] );
#endif

  /* store it in big endian */
  cdbReadCap.tmpVar.CapData.LBA_3 = ( INT8 ) cdbReadCap.tmpVar.l0[1].chars0.c3;
  cdbReadCap.tmpVar.CapData.LBA_2 = ( INT8 ) cdbReadCap.tmpVar.l0[1].chars0.c2;
  cdbReadCap.tmpVar.CapData.LBA_1 = ( INT8 ) cdbReadCap.tmpVar.l0[1].chars0.c1;
  cdbReadCap.tmpVar.CapData.LBA_0 = ( INT8 ) cdbReadCap.tmpVar.l0[1].chars0.c0;

  // Bytes Per Block is 512 Bytes
  // 00020000 is 0x200 in big endian
  cdbReadCap.tmpVar.CapData.BlockLen_3 = 0;
  cdbReadCap.tmpVar.CapData.BlockLen_2 = 0;
  cdbReadCap.tmpVar.CapData.BlockLen_1 = 0x02;
  cdbReadCap.tmpVar.CapData.BlockLen_0 = 0;

  // Config TPBulkXfer Paras (data to Host)
  USB_Xfer_Setup( &BOTXfer, (UINT8*)&cdbReadCap.tmpVar, NULL, sizeof(READ_CAPACITY_DATA), EVT_USB_BULK_TX, NULL );
  BOTStat = BOT_DTIN;

  MSBOT_ErrorHandler( CASE6, sizeof(READ_CAPACITY_DATA) );
//  RBC_BuildSenseData( SCSI_SENSE_NO_SENSE, 0, 0 );

  return TRUE;
#undef cdbReadCap
}

//----------------------------------------------------------------
// Function :   RBC_OnOffUnit
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL RBC_OnOffUnit( VOID )
{
#define cdbOnOffUnit RBC_CDB.RbcCdb_OnOffUnit

  switch ( cdbOnOffUnit.Flags.bits1.PowerConditions )
  {
  case PWR_NOCHANGE:
    switch ( cdbOnOffUnit.Flags.bits1.MediumState )
    {
    case MEDIUM_LOAD:
      break;
    case MEDIUM_UNLOAD:
      break;
    case MEDIUM_STOP:
      break;
    case MEDIUM_READY:
      break;
    }
    break;
  case PWR_ACTIVE:
    break;
  case PWR_IDLE:
    break;
  case PWR_STANDBY:
    break;
  case PWR_SLEEP:
    break;
  case PWR_DEVCTRL:
  default:
    break;
  }

  RBC_PropertyData.bits.MediumState = cdbOnOffUnit.Flags.bits1.MediumState;
  RBC_PropertyData.bits.PowerState = cdbOnOffUnit.Flags.bits1.PowerConditions;

  MSBOT_ErrorHandler(CASE1,0);
//  RBC_BuildSenseData(SCSI_SENSE_NO_SENSE,0,0);

  MSBOT_CSWHandler();  // Goto USBFSM4BOT_CSWPROC;

  return TRUE;
#undef cdbOnOffUnit
}

//----------------------------------------------------------------
// Function :   RBC_SyncCache
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL RBC_SyncCache( VOID )
{
#define cdbSyncRBC RBC_CDB.RbcCdb_SyncCache

  MSBOT_ErrorHandler(CASE1,0);
//  RBC_BuildSenseData(SCSI_SENSE_NO_SENSE,0,0);

  MSBOT_CSWHandler();   // Goto USBFSM4BOT_CSWPROC;

  return TRUE;
#undef cdbSyncRBC
}

//----------------------------------------------------------------
// Function :   RBC_Verify
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL  RBC_Verify( VOID )
{
#define cdbVerifyRBC RBC_CDB.RbcCdb_Verify

  if ( CBW_wXferLen == 0 )
  {
    MSBOT_ErrorHandler( CASE1, 0 );
//    RBC_BuildSenseData( SCSI_SENSE_NO_SENSE, 0, 0 );
    MSBOT_CSWHandler();// Goto USBFSM4BOT_CSWPROC;
  }
  else
  {
    // Config TPBulkXfer Paras
    MSBOT_ErrorHandler( CASE12, CBW_wXferLen );
//    RBC_BuildSenseData( SCSI_SENSE_NO_SENSE, 0, 0 );

//    BOTFSMstate = USBFSM4BOT_DATAOUT;
  }
  return TRUE;
#undef cdbVerifyRBC
}

////////////////////////////////////////////////////////////////////////////////////
// SCSI Primary Command Support
////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------
// Function :   SPC_Inquiry
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SPC_Inquiry( VOID )
{
#define cdbInquirySPC RBC_CDB.SpcCdb_Inquiry
  BOOL           retStatus = FALSE;
  UINT8  CONST * BOTXfer_pData;
  INT16          BOTXfer_wResidue;

#ifdef DEBUG
//  Printf( " Inq" );
#endif

  if ( cdbInquirySPC.EnableVPD )
  {
    switch ( cdbInquirySPC.PageCode )
    {
    case VPDPAGE_SERIAL_NUMBER:

      BOTXfer_pData = (UINT8 CONST*)&SerialPage;
      BOTXfer_wResidue = sizeof(VPD_SERIAL_PAGE);
      break;
    case VPDPAGE_DEVICE_IDENTITY:
      retStatus = TRUE;
      BOTXfer_pData = (UINT8 CONST*)&DeviceIDPage;
      BOTXfer_wResidue = sizeof(VPD_DEVICE_ID_PAGE);
      break;
    default:
      //retStatus = FALSE;
      MSBOT_ErrorHandler(CASECMDFAIL,BOTXfer_wResidue);
//      RBC_BuildSenseData(SCSI_SENSE_ILLEGAL_REQUEST,SCSI_ADSENSE_ILLEGAL_COMMAND,0x00);

      MSBOT_CSWHandler();// Goto USBFSM4BOT_CSWPROC;
      return retStatus;
    }
    // Config TPBulkXfer Paras (data to Host)
    USB_Xfer_Setup( &BOTXfer, NULL, BOTXfer_pData, BOTXfer_wResidue, EVT_USB_BULK_TX, NULL );
  }
  else if ( cdbInquirySPC.CmdSupportData )
  {
    //retStatus = FALSE;
    MSBOT_ErrorHandler( CASECMDFAIL, BOTXfer_wResidue );
//    RBC_BuildSenseData(SCSI_SENSE_ILLEGAL_REQUEST,SCSI_ADSENSE_ILLEGAL_COMMAND,0x00);

    MSBOT_CSWHandler();// Goto USBFSM4BOT_CSWPROC;
    return retStatus;
  }
  else
  {
#ifdef DEBUG
    Printf( " Inq,%d", CBW_wXferLen );
#endif
    // Config TPBulkXfer Paras (data to Host)
    BOTXfer_wResidue = CBW_wXferLen; //sizeof(STD_INQUIRYDATA);
    USB_Xfer_Setup( &BOTXfer, NULL, (UINT8 CONST *)&inquiryData, BOTXfer_wResidue, EVT_USB_BULK_TX, NULL );
  }

  retStatus = TRUE;

  if ( BOTXfer_wResidue > CBW_wXferLen )
  {
    BOTXfer_wResidue = CBW_wXferLen;
    MSBOT_ErrorHandler( CASE6, BOTXfer_wResidue );
//    RBC_BuildSenseData( SCSI_SENSE_NO_SENSE, 0, 0 );
  }
  else if ( BOTXfer_wResidue == CBW_wXferLen )
  {
    MSBOT_ErrorHandler( CASE6,BOTXfer_wResidue );
//    RBC_BuildSenseData( SCSI_SENSE_NO_SENSE, 0, 0 );
  }
  else
  {
    MSBOT_ErrorHandler( CASE5, BOTXfer_wResidue );
//    RBC_BuildSenseData( SCSI_SENSE_NO_SENSE, 0, 0 );
  }

  BOTStat = BOT_DTIN;

  return retStatus;
#undef cdbInquirySPC
}

//----------------------------------------------------------------
// Function :   SPC_ModeSelect
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SPC_ModeSelect( VOID )
{
#define cdbModeSelectSPC    RBC_CDB.SpcCdb_ModeSelect
  BOOL     retStatus = FALSE;
  INT16    BOTXfer_wResidue;

  //
  //Just Retrieve and discard data from USB FIFO
  BOTXfer_wResidue = cdbModeSelectSPC.ParameterLen;

  // Config TPBulkXfer Paras (data from Host)
  USB_Xfer_Setup( &BOTXfer, XFerBuf, NULL, BOTXfer_wResidue, EVT_USB_BULK_RX, NULL );

  if ( cdbModeSelectSPC.SavePage != 1 )
  {
    if ( CBW_wXferLen < BOTXfer_wResidue )
    {
      BOTXfer_wResidue = CBW_wXferLen;
      MSBOT_ErrorHandler(CASE13,BOTXfer_wResidue);
//      RBC_BuildSenseData(SCSI_SENSE_ILLEGAL_REQUEST,SCSI_ADSENSE_INVALID_PARAMETER,0);

      //retStatus = FALSE;
      MSBOT_CSWHandler();// Goto USBFSM4BOT_CSWPROC;
      return retStatus;
    }
    else if ( CBW_wXferLen == BOTXfer_wResidue )
    {
      MSBOT_ErrorHandler(CASE12,BOTXfer_wResidue);
//      RBC_BuildSenseData(SCSI_SENSE_NO_SENSE,0,0);
      retStatus = TRUE;
    }
    else
    {
      MSBOT_ErrorHandler(CASE11,BOTXfer_wResidue);
//      RBC_BuildSenseData(SCSI_SENSE_NO_SENSE,0,0);
      retStatus = TRUE;
    }

    BOTStat = BOT_DTOUT;
  }
  else
  {
    BOTXfer_wResidue = CBW_wXferLen;
    MSBOT_ErrorHandler(CASECMDFAIL,BOTXfer_wResidue);
//    RBC_BuildSenseData(SCSI_SENSE_ILLEGAL_REQUEST,SCSI_ADSENSE_INVALID_CDB,0);

    //retStatus = FALSE;
    MSBOT_CSWHandler();// Goto USBFSM4BOT_CSWPROC;
  }
  return retStatus;

#undef cdbModeSelectSPC
}

//----------------------------------------------------------------
// Function :   SPC_ModeSense
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SPC_ModeSense( VOID )
{
#define cdbModeSenseSPC RBC_CDB.SpcCdb_ModeSense
  BOOL           retStatus = FALSE;
  UINT8  CONST * BOTXfer_pData;
  INT16          BOTXfer_wResidue;

//  if ( cdbModeSenseSPC.PageCode == 0 /*MODE_PAGE_RBC_DEVICE_PARAMETERS*/ )
  {
    switch ( cdbModeSenseSPC.PageControl )
    {
      case PAGECTRL_CHANGEABLE:
        BOTXfer_pData = (UINT8 CONST *)&ParaHeadMask;
        BOTXfer_wResidue = sizeof(ParaHeadMask);
        break;
      case PAGECTRL_DEFAULT:
      case PAGECTRL_CURRENT:
        BOTXfer_pData =(UINT8 CONST *)&DefaultParaHead;
        BOTXfer_wResidue = sizeof(DefaultParaHead);
        break;
      case PAGECTRL_SAVED:
      default:
        //retStatus = FALSE;
        MSBOT_ErrorHandler(CASECMDFAIL,BOTXfer_wResidue);
//        RBC_BuildSenseData(SCSI_SENSE_ILLEGAL_REQUEST,SCSI_ADSENSE_SAVE_ERROR,0);
        MSBOT_CSWHandler();// Goto USBFSM4BOT_CSWPROC;
      return retStatus;
    };

    if ( CBW_wXferLen < BOTXfer_wResidue )
    {
      BOTXfer_wResidue = CBW_wXferLen;
      MSBOT_ErrorHandler(CASE6,BOTXfer_wResidue);
//      RBC_BuildSenseData(SCSI_SENSE_NO_SENSE,0,0);
    }
    else if ( CBW_wXferLen == BOTXfer_wResidue )
    {
      MSBOT_ErrorHandler(CASE6,BOTXfer_wResidue);
//      RBC_BuildSenseData(SCSI_SENSE_NO_SENSE,0,0);
    }
    else
    {
      MSBOT_ErrorHandler(CASE5,BOTXfer_wResidue);
//      RBC_BuildSenseData(SCSI_SENSE_NO_SENSE,0,0);
    }

    // Config TPBulkXfer Paras (data to Host)
    USB_Xfer_Setup( &BOTXfer, NULL, BOTXfer_pData, BOTXfer_wResidue, EVT_USB_BULK_TX, NULL );
    BOTStat = BOT_DTIN;

    retStatus = TRUE;
  }
//  else
  {
    //retStatus = FALSE;
//    MSBOT_ErrorHandler(CASECMDFAIL,BOTXfer_wResidue);
////    RBC_BuildSenseData(SCSI_SENSE_ILLEGAL_REQUEST,SCSI_ADSENSE_INVALID_CDB,0);
//    MSBOT_CSWHandler();// Goto USBFSM4BOT_CSWPROC;
  }

  return retStatus;
#undef cdbModeSenseSPC
}

//----------------------------------------------------------------
// Function :   SPC_LockMedia
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SPC_LockMedia( VOID )
{
#define cdbLockSPC RBC_CDB.SpcCdb_Remove

  RBC_PropertyData.bits.MediumRemovFlag = cdbLockSPC.Prevent;

//  if ( RBC_PropertyData.bits.MediumRemovFlag == 01 )
//    IDE_CS = 0;
//  else
//    IDE_CS = 1;

  MSBOT_ErrorHandler( CASE1, 0 );
//  RBC_BuildSenseData( SCSI_SENSE_NO_SENSE, 0, 0 );
  MSBOT_CSWHandler();// Goto USBFSM4BOT_CSWPROC;
  return TRUE;

#undef cdbLockSPC
}

//----------------------------------------------------------------
// Function :   SPC_TestUnit
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SPC_TestUnit( VOID )
{
#define cdbTestUnit RBC_CDB.SpcCdb_TestUnit

  if ( TRUE /*ATABF_IsAttached*/ )
  {
    MSBOT_ErrorHandler( CASE1, 0 );
//    RBC_BuildSenseData( SCSI_SENSE_NO_SENSE, 0, 0 );
  }
  else
  {
    MSBOT_ErrorHandler( CASECMDFAIL, 0 );
//    RBC_BuildSenseData( SCSI_SENSE_NOT_READY, SCSI_ADSENSE_NO_MEDIA_IN_DEVICE, 0 );
  }

  MSBOT_CSWHandler();// Goto USBFSM4BOT_CSWPROC;
  return TRUE;

#undef cdbTestUnit
}

//----------------------------------------------------------------
// Function :   SPC_RequestSense
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SPC_RequestSense( VOID )
{
#define cdbRequestSenseSPC RBC_CDB.SpcCdb_RequestSense

  // Config TPBulkXfer Paras (data to Host)
  USB_Xfer_Setup( &BOTXfer, NULL, (UINT8 CONST *)&Req_SenseData, sizeof(Req_SenseData), EVT_USB_BULK_TX, NULL );
  BOTStat = BOT_DTIN;

  MSBOT_ErrorHandler( CASE6, sizeof(Req_SenseData) );

  return TRUE;
#undef cdbRequestSenseSPC
}

// Optional

#ifdef RBC_USEOPTIONAL

//----------------------------------------------------------------
// Function :   RBC_Format
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL RBC_Format( VOID )
{
  MSBOT_ErrorHandler( CASECMDFAIL, 0 );
//  RBC_BuildSenseData( SCSI_SENSE_MEDIUM_ERROR, SCSI_ADSENSE_FORMAT_ERROR, 0x01 );
  MSBOT_CSWHandler();// Goto USBFSM4BOT_CSWPROC;
  return TRUE;
}

//----------------------------------------------------------------
// Function :   SPC_Reserve6
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SPC_Reserve6( VOID )
{
  MSBOT_ErrorHandler( CASECMDFAIL, 0 );
//  RBC_BuildSenseData( SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ADSENSE_ILLEGAL_COMMAND, 0 );
  MSBOT_CSWHandler();// Goto USBFSM4BOT_CSWPROC;
  return TRUE;
}

//----------------------------------------------------------------
// Function :   SPC_Release6
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SPC_Release6( VOID )
{
  MSBOT_ErrorHandler( CASECMDFAIL, 0 );
//  RBC_BuildSenseData( SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ADSENSE_ILLEGAL_COMMAND, 0 );
  MSBOT_CSWHandler();// Goto USBFSM4BOT_CSWPROC;
  return TRUE;
}

//----------------------------------------------------------------
// Function :   SPC_PersisReserveIn
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SPC_PersisReserveIn( VOID )
{
  MSBOT_ErrorHandler( CASECMDFAIL, 0 );
//  RBC_BuildSenseData( SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ADSENSE_ILLEGAL_COMMAND, 0 );
  MSBOT_CSWHandler();// Goto USBFSM4BOT_CSWPROC;
  return TRUE;
}

//----------------------------------------------------------------
// Function :   SPC_PersisReserveOut
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SPC_PersisReserveOut( VOID )
{
  //Just Retrieve and discard data from USB FIFO

  // Config TPBulkXfer Paras (data from Host)
  USB_Xfer_Setup( &BOTXfer, XFerBuf, NULL, CBW_wXferLen, EVT_USB_BULK_RX, NULL );
  BOTStat = BOT_DTOUT;

  MSBOT_ErrorHandler( CASECMDFAIL, CBW_wXferLen );
//  RBC_BuildSenseData( SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ADSENSE_CMDSEQ_ERROR, 0 );

  return TRUE;
}

//----------------------------------------------------------------
// Function :   SPC_WriteBuff
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SPC_WriteBuff( VOID )
{
#define cdbWriteBuff RBC_CDB.SpcCdb_WriteBuffer

  //Just Retrieve and discard data from USB FIFO

  // Config TPBulkXfer Paras (data from Host)
  USB_Xfer_Setup( &BOTXfer, XFerBuf, NULL, CBW_wXferLen, EVT_USB_BULK_RX, NULL );
  BOTStat = BOT_DTOUT;

  MSBOT_ErrorHandler( CASECMDFAIL, CBW_wXferLen );
//  RBC_BuildSenseData( SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ADSENSE_CMDSEQ_ERROR, 0 );

  return TRUE;
#undef cdbWriteBuff
}

//----------------------------------------------------------------
// Function :   SPC_READLONG
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SPC_READLONG( VOID )
{
  MSBOT_ErrorHandler( CASECMDFAIL, CBW_wXferLen );
//  RBC_BuildSenseData( SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ADSENSE_INVALID_CDB, 0 );
  MSBOT_CSWHandler();// Goto USBFSM4BOT_CSWPROC;
  return TRUE;
}

#endif

//      End



