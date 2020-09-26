//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: EESERV.H
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
//              001    6-03-2002 MM     Created
//
//****************************************************************

#ifndef __EESERV_H__
  #define __EESERV_H__

#define SAVE_POS1           100
#define SAVE_POS2           150

#ifdef __IAR_SYSTEMS_ICC

#define EEPUT(ADR,VAL)          \
{                               \
  while (EECR & 0x02);          \
  _CLI();                       \
  EEAR = (ADR);                 \
  EEDR = (VAL);                 \
  EECR |= 0x04;                 \
  EECR |= 0x02;                 \
  _SEI();                       \
}
#define EEGET(VAR, ADR)         \
{                               \
  while (EECR & 0x02);          \
  EEAR = (ADR);                 \
  EECR |= 0x01;                 \
  (VAR) = EEDR;                 \
}

#endif

#ifdef __GNUC__

#define EEPUT(ADR, VAL)         eeprom_wb(ADR, VAL)
#define EEGET(VAR, ADR)         (VAR) = eeprom_rb(ADR)

#endif

BOOL EE_GetSavedDat     ( UINT8 * ptr, UINT8 len, UINT16 start );
BOOL EE_PutSavedDat     ( UINT8 * ptr, UINT8 len, UINT16 start );

#endif

//      End
