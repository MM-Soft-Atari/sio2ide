
#include <apps.h>
#include "extio.h"
#include "uartdrv.h"

STATIC  T_drvinf       drv;
STATIC  UINT8          buf[512];
STATIC  UINT16         i;

#define DSK     4

VOID main( VOID )
{
  T_ISO_PRIM_DESC   * pdesc;
  T_ISO_DIR_REC     * rdir;

  init_comm();

  enable_interrupt();

  Printf( "\nInit" );

  switch( IDE_Init( &drv ) )
  {
    case IDE_CD:
      Printf( "\nCD" );

      MemSet( buf, 0, 512 );

      if( IDE_SectorGetAt( (UINT16 *)buf, &drv, (0x10 << 2), 0 ) )
      {
        Printf( "\nS" );
        Printf( "\n" );
        for( i = 0; i < 256; i++ )
        {
          Printf( "%c", buf[i] );
        }
        Printf( "\n" );

        pdesc = (T_ISO_PRIM_DESC*)buf;
        //
        if( isonum_711( pdesc->type ) == ISO_VD_PRIMARY )
        {
          Printf( "\nP" );

          if( memcmpf( pdesc->id, "CD001", 5 ) == 0 )
          {
            Printf( "\nISO" );
            //
            rdir = (T_ISO_DIR_REC*)&pdesc->root_directory_record;
            //
          }
        }
      }
      else
      {
        Printf( "\nERR 1" );
      }
    break;
    case IDE_HD:
      Printf( "\nHDD" );
    break;
    case IDE_NONE:
      Printf( "\nNONE" );
    break;
    default:
      Printf( "\nERR 2" );
    break;
  }
  Printf( "\nDone" );

stop:
  for(;;)
  {
    if( inp_status() )
    {
//      outp_char( inp_char() );
    }
  }
}
