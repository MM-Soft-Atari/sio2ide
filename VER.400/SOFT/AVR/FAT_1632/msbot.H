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
//              001   29-07-2002 MM     Created
//
//****************************************************************

#ifndef __MSBOT_H__
  #define __MSBOT_H__

#define CSW_GOOD        0x00
#define CSW_FAIL        0x01
#define CSW_PHASE_ERROR 0x02

#ifdef   BIG_ENDIAN
  #define CBW_SIGNATURE   0x55534243
  #define CSW_SIGNATURE   0x55534253
#else
  #define CBW_SIGNATURE   0x43425355
  #define CSW_SIGNATURE   0x53425355
#endif

#define CBW_FLAG_IN     0x80

#define MAX_CDBLEN      0x10

typedef struct _COMMAND_BLOCK_WRAPPER
{
    UINT32   dCBW_Signature;
    UINT32   dCBW_Tag;
    UINT32   dCBW_DataXferLen;
    UINT8    bCBW_Flag;
    UINT8    bCBW_LUN;
    UINT8    bCBW_CDBLen;
    CDB_RBC cdbRBC;
} CBW, *PCBW;

typedef struct _COMMAND_STATUS_WRAPPER
{
    UINT32   dCSW_Signature;
    UINT32   dCSW_Tag;
    UINT32   dCSW_DataResidue;
    UINT8    bCSW_Status;
} CSW, *PCSW;

typedef union _TPBULK_STRUC
{
    CBW     TPBulk_CommandBlock;
    CSW     TPBulk_CommandStatus;
}TPBLK_STRUC, * PTPBLK_STRUC;

//*************************************************************************
//                       USB Class Request Functions
//                                 and
//                           Public Functions
//*************************************************************************

//
//      Host Device Disagreement Matrix
//
enum _HOST_DEV_DISAGREE
{
  CASEOK = 0,
  CASE1,
  CASE2,
  CASE3,
  CASE4,
  CASE5,
  CASE6,
  CASE7,
  CASE8,
  CASE9,
  CASE10,
  CASE11,
  CASE12,
  CASE13,
  CASECBW,
  CASECMDFAIL
};

//
// Bulk-Only TP Finite State Machine [One-Hot]
//
#define BOT_START       0x00
#define BOT_IDLE        0x01
#define BOT_CBW         0x02
#define BOT_DTIN        0x03
#define BOT_DTINP       0x04
#define BOT_DTOUT       0x05
#define BOT_DTOUTP      0x06
#define BOT_CSW         0x07
#define BOT_CSWP        0x08
#define BOT_STALL       0x09
//      BOT FSM
//      IDLE Stage ->  CBW -> CBW Proc -> DATA OUT Stage -> CSW Proc -> CSW Stage -> IDLE
//      IDLE Stage ->  CBW -> CBW Proc -> DATA IN Stage -> CSW Proc -> CSW Stage -> IDLE
//      STALL Stage ->  IDLE

extern TPBLK_STRUC      TPBulk_Block;   // CBW/CSW buffer
extern XferStat_t       BOTXfer;        // Bulk Xfer control data
extern UINT8            BOTStat;        // Bulk Xfer State Machine state

#define TPBulk_CBW      TPBulk_Block.TPBulk_CommandBlock
#define TPBulk_CSW      TPBulk_Block.TPBulk_CommandStatus
#define CBW_wXferLen    TPBulk_CBW.dCBW_DataXferLen
#define RBC_CDB         TPBulk_CBW.cdbRBC
#define RBC_LUN         TPBulk_CBW.bCBW_LUN

//*************************************************************************
// C[ommand]D[ata]S[tatus] architecture for mass storage device over Bulk
// only Transport
//*************************************************************************

//VOID MSBOT_ResetATA     ( USB_request_t * req );
//VOID MSBOT_GetMaxLUN    ( USB_request_t * req );

VOID MSBOT_ErrorHandler ( UINT8 HostDevCase, INT16 wByteCounterDevWillXfer );
VOID MSBOT_CSWHandler   ( VOID );

VOID MSBOT_Handler      ( VOID );
VOID MSBOT_TimerUpd     ( VOID );

#endif

//      End
