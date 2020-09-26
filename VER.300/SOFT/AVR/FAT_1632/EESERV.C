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

//----------------------------------------------------------------
// Function :   EE_GetSavedDir
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL EE_GetSavedDir( UINT32 * sec )
{
  UINT8     id  = 0;
  UINT8   * ptr = (UINT8*)sec;

  EEGET( id, SAVE_POS );
  if( id == SAVE_ID_OK )
  {
    EEGET( *ptr++, SAVE_POS + 1 );
    EEGET( *ptr++, SAVE_POS + 2 );
    EEGET( *ptr++, SAVE_POS + 3 );
    EEGET( *ptr++, SAVE_POS + 4 );
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
  UINT8   * ptr = (UINT8*)sec;

  EEPUT( SAVE_POS, SAVE_ID_EMP );
  //
  EEPUT( SAVE_POS + 1, *ptr++ );
  EEPUT( SAVE_POS + 2, *ptr++ );
  EEPUT( SAVE_POS + 3, *ptr++ );
  EEPUT( SAVE_POS + 4, *ptr++ );
  //
  EEPUT( SAVE_POS, SAVE_ID_OK );
  //
  return TRUE;
}

// End
