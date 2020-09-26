//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: EESERV.C
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

#include <platform.h>

#define MAX_EE_SIZE         512         // EEPROM size (AT90S8515)
#define SAVE_POS            100
#define SAVE_ID_EMP         0xFF
#define SAVE_ID_OK          0xAA

STATIC UINT32   ssave = 0;
STATIC UINT8    sid = SAVE_ID_EMP;

//----------------------------------------------------------------
// Function :   EE_GetSavedDir
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL EE_GetSavedDir( UINT32 * sec )
{
  if( sid == SAVE_ID_OK )
  {
    *sec = ssave;
    //
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   EE_PutSavedDir
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL EE_PutSavedDir( UINT32 * sec )
{
  sid = SAVE_ID_EMP;
  //
  ssave = *sec;
  //
  sid = SAVE_ID_OK;
  //
  return TRUE;
}

// End
