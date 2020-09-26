
#include <apps.h>
#include "extio.h"
#include "uartdrv.h"

#define T0_FRQ          100             // 100Hz

//
//      Timer/Counter0 clock divider
//
#define CLK0_DIV        5        // 1-CK/1, 2-CK/8, 3-CK/64, 4-CK/256, 5-CK/1024
//
#if (CLK0_DIV==1)
  #define CLK0_FACT     1
#elif (CLK0_DIV==2)
  #define CLK0_FACT     8
#elif (CLK0_DIV==3)
  #define CLK0_FACT     64
#elif (CLK0_DIV==4)
  #define CLK0_FACT     256
#elif (CLK0_DIV==5)
  #define CLK0_FACT     1024
#endif

#define T0_FACT         (0xFF - (FOSC / CLK0_FACT / T0_FRQ))

//
//      SIO LED
//
#define SRDYL                   (1<<7)
#define SIO_RDYL_INIT()         __outp( DDRD, (__inp(DDRD) | SRDYL) )
#define SIO_RDYL_ON()           __outp( PORTD, (__inp(PORTD) & ~SRDYL) )
#define SIO_RDYL_OFF()          __outp( PORTD, (__inp(PORTD) | SRDYL) )
#define SIO_RDYL_TGL()          __outp( PORTD, (__inp(PORTD) ^ SRDYL) )

STATIC  T_file         disk;
STATIC  UINT8          name[12];
STATIC  UINT8          i;

#define FL_DATA_OFFS    (sizeof(T_atrhdr))

VOID main( VOID )
{
  STATIC UINT32 deFileSize;
  STATIC UINT16 flSize;
  STATIC UINT16 ssize;

  __outp( TCNT0, T0_FACT );
  __outp( TCCR0, CLK0_DIV );
  __port_or( TIMSK, (1<<TOIE0) );                 // Timer0 Ovf Int Enabled

  SIO_RDYL_INIT();
  SIO_RDYL_OFF();

//  __outp(DDRA,0xFF);__outp(DDRB,0xFF);
//  __outp(PORTA,0xAA);

  init_comm();

  enable_interrupt();

  Printf( "\nStart" );

//  ssize = 256;
//  deFileSize = 183936+FL_DATA_OFFS;
//  flSize = ((deFileSize - FL_DATA_OFFS - (UINT32)(3 * 128)) / (UINT32)ssize) + 3;
//  Printf( "\nFs=%i", flSize );
//  flSize = ((deFileSize - FL_DATA_OFFS - (UINT32)(3 * 128)) % (UINT32)ssize);
//  Printf( "\nFs=%i", flSize );
//  deFileSize = 16776576+FL_DATA_OFFS;
//  flSize = ((deFileSize - FL_DATA_OFFS - (UINT32)(3 * 128)) / (UINT32)ssize) + 3;
//  Printf( "\nFs=%i", flSize );
//  flSize = ((deFileSize - FL_DATA_OFFS - (UINT32)(3 * 128)) % (UINT32)ssize);
//  Printf( "\nFs=%i", flSize );

//  goto stop;

  if( FS_Init() )
  {
    Printf( "\nOK" );
  }
  else
  {
    Printf( "\nERR" );
  }
  //
  if( FATFS_GetCurDir( &disk ) )
  {
    if( disk.flDet.flStat & FLS_DIROK )
    {
      memset( name, 0, 12 );
      memcpy( name, disk.flName, 8 );
      Printf( "\nCurDir > '%s'", name );
      if( disk.flDet.flStat & FLS_S2IDIR )
      {
        Printf( " with SIO2IDE cfg" );
      }
    }
  }
  if( FATFS_GetFirstDir( &disk ) )
  {
    do
    {
      if( disk.flDet.flStat & FLS_DIROK )
      {
        memset( name, 0, 12 );
        memcpy( name, disk.flName, 8 );
        Printf( "\nDir > '%s'", name );
        if( disk.flDet.flStat & FLS_S2IDIR )
        {
          Printf( " with SIO2IDE cfg" );
        }
      }
      else
      {
        break;
      }
    }while( FATFS_GetNextDir( &disk ) );
  }

//  goto stop;

  Printf( "\nCfg saved" );
  if( FATFS_SaveConfig() )
  {
    Printf( " OK" );
  }

  for( i = 1; i <= MAX_FL_ID; i++ )
  {
    if( FATFS_DiskGet( i, &disk ) )
    {
      memset( name, 0, 12 );
      memcpy( name, disk.flName, 8 );
      Printf( "\nD%d: > '%s'", i, name );
    }
  }

  Printf( "\nDisk set" );
  FATFS_DiskGet( 1, &disk );
  if( FATFS_DiskSet( 9, &disk ) )
  {
    Printf( " OK" );
  }

  for( i = 1; i <= MAX_FL_ID; i++ )
  {
    if( FATFS_DiskGet( i, &disk ) )
    {
      memset( name, 0, 12 );
      memcpy( name, disk.flName, 8 );
      Printf( "\nD%d: > '%s'", i, name );
    }
  }

  Printf( "\nDisk off" );
  if( FATFS_DiskOff( COM_FL_ID ) )
  {
    Printf( " OK" );
  }

  Printf( "\nCfg saved" );
  if( FATFS_SaveConfig() )
  {
    Printf( " OK" );
  }

  Printf( "\nInit dir" );
  if( FATFS_InitCurDir( &disk ) )
  {
    Printf( " OK" );
  }

  Printf( "\nCfg saved" );
  if( FATFS_SaveConfig() )
  {
    Printf( " OK" );
  }

  for( i = 1; i <= MAX_FL_ID; i++ )
  {
    if( FATFS_DiskGet( i, &disk ) )
    {
      memset( name, 0, 12 );
      memcpy( name, disk.flName, 8 );
      Printf( "\nD%d: > '%s'", i, name );
    }
  }

stop:
  Printf( "\nEND" );
  for(;;)
  {
    if( inp_status() )
    {
      outp_char( inp_char() );
    }
  }
}

STATIC UINT16 Timer = 50;

interrupt [TIMER0_OVF_vect] VOID TC0_Ovf ( VOID )
{
  if( Timer )
  {
    Timer--;
  }
  else
  {
    Timer = 50;
    SIO_RDYL_TGL();
  }

  __outp(PORTA,__inp(PORTA) ^ 0xFF);

  __outp( TCNT0, T0_FACT );
}
