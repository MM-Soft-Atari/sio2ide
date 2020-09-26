
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>

#include "plat.h"

#pragma codeseg ("CODE1")

#ifdef __CART__
       void CardWait    ( void );
#endif

//----------------------------------------------------------------
// Function :   WaitKeyFor
// Notes    :
// History  :
//----------------------------------------------------------------
#ifdef __CART__
static UINT8 WaitKeyFor( UINT8 * title, UINT8 * title1, UINT8 * title2, UINT8 * keys, UINT16 time )
{
  UINT8        key;
  UINT16       i;
  T_progress   prog;

  SCR_DrawWindow( title, 6, 7, 33, 16 );
  //
  SCR_ProgressShow( &prog, 9, 9, 20, (time * 10) );
  //
  gotoxy( 6, 13 );
  SCR_LineCenter( title1, 28 );
  gotoxy( 6, 14 );
  SCR_LineCenter( title2, 28 );

  for( i = 0; i < (time * 10); i++ )
  {
    key = 0xFF;
    if( kbhit() )
    {
      key = cgetc();
      if( strchr( keys, key ) )        return key;
    }
    SCR_ProgressUpdate( &prog, i );
    MENU_Delay( 350 );
  }
  return 0;
}
#endif

//----------------------------------------------------------------
// Function :   ShowYesNoButtons
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL ShowYesNoButtons( UINT8 * name, UINT8 pos )
{
  static T_menuitem       buttonItm[] =
  {
    {MITM_EXIT, " 'Y'es", "Yy", NULL},
    {MITM_EXIT, " 'N'o ", "\033Nn", NULL},
    {MITM_EMPTY,  "", "", NULL}
  };
  static T_menu           buttonCnf =
  {
    MNU_HOR, "", 9, 12, 29, 14, buttonItm, "", NULL
  };

  buttonCnf.title = name;
  buttonCnf.hy    = pos;
  buttonCnf.ly    = pos + 2;
  return (MENU_Show( &buttonCnf ) == 0) ? TRUE : FALSE;
}

//----------------------------------------------------------------
// Function :   DrawMainWindows
// Notes    :
// History  :
//----------------------------------------------------------------

void DrawMainWindows( BOOL mod )
{
  //
  SCR_DrawFrame( 0, 21, 39, 23, TRUE );
  gotoxy( 1, 22 );
#ifdef DEBUG
  #ifdef __DOS__
    SCR_LineCenter( "MMSoft (c) 2002  (DOS demo ver)", 38 );
  #endif
  #ifdef __CART__
    SCR_LineCenter( "MMSoft (c) 2002  (Cart demo ver)", 38 );
  #endif
#else
  #ifdef __DOS__
    SCR_LineCenter( "MMSoft (c) 2002  (DOS version)", 38 );
  #endif
  #ifdef __CART__
    SCR_LineCenter( "MMSoft (c) 2002  (Cartridge version)", 38 );
  #endif
#endif
  //
  SCR_DrawFrame( 0, 0, 39, 2, FALSE );
  gotoxy( 1, 1 );
  if( mod )
  {
    SCR_LineCenter( "SIO2IDE "__SIO2IDE_VER_TXT__" configuration center", 38 );
  }
  else
  {
    SCR_LineCenter( "SIO2IDE "__SIO2IDE_VER_TXT__" BootLoader", 38 );
  }
  SCR_DrawFrame( 0, 3, 39, 20, TRUE );
}


UINT16 main ( void )
{
  SCR_Init();

#ifdef __CART__
  DrawMainWindows( FALSE );
  switch( WaitKeyFor( "Wait for disk BOOT",
                      " Åóã   - SIO2IDE setup",
                      "Óðáãå  - skip setup   ",
                      "\033\040", 5 )
        )
  {
    case KEY_ESC:
    {
      DrawMainWindows( FALSE );
      if( !ShowYesNoButtons( "Leave CART ON ?", 10 ) )
      {
        SCR_DrawFrame( 7, 10, 31, 12, TRUE );
        gotoxy( 1, 11 );
        SCR_LineCenter( "Switch OFF the CART", 38 );
        CardWait();
      }
    }
    break;
    case 0x20:
    default:
    break;
  };
#endif

  SCR_DeInit();

  return EXIT_SUCCESS;
}
