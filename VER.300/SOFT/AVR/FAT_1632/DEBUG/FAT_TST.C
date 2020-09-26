
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

#ifdef ATmega163
  #define TOIE0           0
#endif
#ifdef ATmega161
  #define TOIE0           1
#endif

//
//      SIO LED
//
#define SRDYL                   (1<<4)
#define SIO_RDYL_INIT()         __outp( DDRD, (__inp(DDRD) | SRDYL) )
#define SIO_RDYL_ON()           __outp( PORTD, (__inp(PORTD) & ~SRDYL) )
#define SIO_RDYL_OFF()          __outp( PORTD, (__inp(PORTD) | SRDYL) )
#define SIO_RDYL_TGL()          __outp( PORTD, (__inp(PORTD) ^ SRDYL) )

STATIC UINT16   maxTime = 50;
STATIC UINT16   Timer = 50;

//----------------------------------------------------------------
// Function :   main
// Notes    :
// History  :
//----------------------------------------------------------------

VOID main( VOID )
{
  __outp( TCNT0, T0_FACT );
  __outp( TCCR0, CLK0_DIV );
  __port_or( TIMSK, (1<<TOIE0) );                 // Timer0 Ovf Int Enabled

  SIO_RDYL_INIT();
  SIO_RDYL_OFF();

  __outp(DDRA,0xFF);__outp(DDRB,0xFF);
  __outp(PORTA,0xAA);

  init_comm();

  enable_interrupt();

  Printf( "\nSIO2IDE (c) "__SIO2IDE_VER_TXT__" test software, MMSoft 2003" );

  FATFS_Test();

  Printf( "\nEnd (please restart SIO2IDE)" );

  maxTime = 25;

stop:
  for(;;)
  {
  }
}

//----------------------------------------------------------------
// Function :   TC0_Ovf
// Notes    :
// History  :
//----------------------------------------------------------------

interrupt [TIMER0_OVF_vect] VOID TC0_Ovf ( VOID )
{
  if( Timer )
  {
    Timer--;
  }
  else
  {
    Timer = maxTime;
    SIO_RDYL_TGL();
  }

  __outp(PORTA,__inp(PORTA) ^ 0xFF);
  __outp( TCNT0, T0_FACT );
}

//      End
