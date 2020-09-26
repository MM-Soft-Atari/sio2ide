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
#define SAVE_ID_EMP         0xFF
#define SAVE_ID_OK          0xAA

//----------------------------------------------------------------
// Function :   EE_GetSavedDat
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL EE_GetSavedDat( UINT8 * ptr, UINT8 len, UINT16 start )
{
  UINT8     id  = 0;
  UINT8     pos = start + 1;

  EEGET( id, start );
  if( id == SAVE_ID_OK )
  {
    while( len-- )
    {
      EEGET( *ptr++, pos++ );
    }
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------
// Function :   EE_PutSavedDat
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL EE_PutSavedDat( UINT8 * ptr, UINT8 len, UINT16 start )
{
  UINT8     pos = start + 1;

  EEPUT( start, SAVE_ID_EMP );
  //
  while( len-- )
  {
    EEPUT( pos++, *ptr++ );
  }
  //
  EEPUT( start, SAVE_ID_OK );
  //
  return TRUE;
}

// End
