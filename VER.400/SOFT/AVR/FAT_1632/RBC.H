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
//  Implementation Notes:
//      1. LUN
//          In SCSI-2, 3 bits LUN locates the highest 3 bits of the byte next to
//          OperationCode of the command Request.
//          In SCSI-3, 4 Bits LUN is controlled by Transport Protocol,
//              RBC and SPC-2 are parts of SCSI-3
//

#ifndef __RBC_H__
  #define __RBC_H__

#include "idestruc.h"
#include "ide.h"

#define RBC_USEOPTIONAL

typedef union _RBC_PROP
{
  struct
  {
    UINT8    MediumRemovFlag : 2;
    UINT8    MediumState : 2;
    UINT8    PowerState : 4;
  } bits;
  UINT8 value;
} RBC_PROPERTY, * PRBC_PROPERTY;

////////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////////

VOID    RBC_Init                ( T_drvinf * DrvInfo  );
BOOL    RBC_Handler             ( VOID );

BOOL    RBC_Read                ( VOID );
BOOL    RBC_ReadCapacity        ( VOID );
BOOL    RBC_OnOffUnit           ( VOID );
BOOL    RBC_Verify              ( VOID );
BOOL    RBC_Write               ( VOID );
BOOL    RBC_SyncCache           ( VOID );

BOOL    SPC_Inquiry             ( VOID );
BOOL    SPC_ModeSelect          ( VOID );
BOOL    SPC_ModeSense           ( VOID );
BOOL    SPC_LockMedia           ( VOID );
BOOL    SPC_TestUnit            ( VOID );
BOOL    SPC_RequestSense        ( VOID );

// Optional

BOOL    RBC_Format              ( VOID );
BOOL    SPC_Reserve6            ( VOID );
BOOL    SPC_Release6            ( VOID );
BOOL    SPC_PersisReserveIn     ( VOID );
BOOL    SPC_PersisReserveOut    ( VOID );
BOOL    SPC_WriteBuff           ( VOID );
BOOL    SPC_READLONG            ( VOID );

VOID    RBC_BuildSenseData      ( UINT8 SenseKey, UINT8 ASC, UINT8 ASCQ );

#endif

//      End
