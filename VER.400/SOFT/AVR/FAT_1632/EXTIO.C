//
//      Simple printf and sprintf routines
//
//      features:
//      - only unsigned data supported
//      formats:
//      - %d    - unsigned char
//      - %i    - unsigned int
//      - %u    - unsigned int
//      - %x %X - unsigned char (00 - FF)
//      - %c    - unsigned char
//      - %s    - pointer to string (always convert to INT16)
//      - %%    - percent character
//      modifier:
//      - 0     - fill with zeros
//      - SP    - fill with spaces
//
//      !!! DOS version for Small memory model ONLY !!!
//

#include <platform.h>
#include <stdarg.h>
#include "extstr.h"
#include "extio.h"
#include "sio.h"
#include "uartdrv.h"

#ifdef __MSDOS__
  #include <conio.h>
#endif

STATIC      va_list    ioArgs;
STATIC      UINT8    * ioBPtr;               // pointer to sprintf output buffer
STATIC      UINT8      ioSbuf[10];           // formated write temp buffer

STATIC VOID putchr( UINT8 c )
{
  if( ioBPtr )
  {
    *ioBPtr++ = c;
  }
  else
  {
#ifdef __MSDOS__
     putch( c );
#else
  #ifdef DEBUG
    outp_char( c );           // UART
  #endif
  #ifdef NDEBUG
    SIO_Put( c );
  #endif
#endif
  }
}

#if FILL_OPT==1
STATIC UINT8   writeStr( UINT8 * str, UINT8 limit, UINT8 fill )
#else
STATIC UINT8   writeStr( UINT8 * str, UINT8 limit )
#endif
{
  REGISTER UINT8  cnt = 0;

#if FILL_OPT==1
  REGISTER UINT16 len;

  if( fill )
  {
    len = StrLen( (UINT8*)str );
    if ( len < limit )
    {
      len = limit - len;
      while( *str && len-- )
      {
        putchr( fill );
        limit--;
        cnt++;
      }
    }
  }
#endif
  while( *str && limit-- )
  {
    putchr( *str++ );
    cnt++;
  }
  return cnt;
}

STATIC UINT16 formatted_Write ( UINT8 * format )
{
  REGISTER UINT16    cnt;
  REGISTER UINT8     chr;
  REGISTER UINT8     limit;
#if FILL_OPT==1
  REGISTER UINT8     fill;
#endif

  cnt      = 0;
  while( (chr = *format++) != 0 )
  {
    if( chr == '%' )
    {
      chr   = *format++;
#if FILL_OPT==1
      fill  = 0;
      if( chr == '0' || chr == ' ' )    // fill character
      {
        fill = chr;
        chr  = *format++;
      }
#endif
      limit = 0xFF;
      if( IsDigit( chr ) )
      {
        limit = 0;
        while( IsDigit( chr ) )        // length limit (AtoI conversion)
        {
          limit = (limit * 10) + (chr & 0x0F);
          chr   = *format++;
        }
      }
      switch( chr )
      {
        case 'd':
        case 'u':
        case 'i':
#if FILL_OPT==1
          cnt += writeStr( ItoA( ioSbuf, va_arg( ioArgs, UINT16 ) ), limit, fill );
#else
          cnt += writeStr( ItoA( ioSbuf, va_arg( ioArgs, UINT16 ) ), limit );
#endif
        break;
        case 'l':
#if FILL_OPT==1
          cnt += writeStr( LtoA( ioSbuf, va_arg( ioArgs, UINT32 ) ), limit, fill );
#else
          cnt += writeStr( LtoA( ioSbuf, va_arg( ioArgs, UINT32 ) ), limit );
#endif
        break;
        case 'x':
        case 'X':
#if FILL_OPT==1
          cnt += writeStr( BtoH( ioSbuf, va_arg( ioArgs, UINT16 ) ), 2, 0 );
#else
          cnt += writeStr( BtoH( ioSbuf, va_arg( ioArgs, UINT16 ) ), 2 );
#endif
        break;
        case 's':
#if FILL_OPT==1
          cnt += writeStr( (UINT8 *)va_arg( ioArgs, UINT16 ), limit, 0 );
#else
          cnt += writeStr( (UINT8 *)va_arg( ioArgs, UINT16 ), limit );
#endif
        break;
        case 'c':
          chr = va_arg( ioArgs, UINT16 );
        case '%':
          putchr( chr );
          cnt++;
        break;
      }
    }
    else
    {
#ifdef __MSDOS__
      if( (chr == '\n') && !ioBPtr )    /* conversion for printf only */
      {
        putchr( '\r' );
        cnt++;
      }
#endif
      putchr( chr );
      cnt++;
    }
  }
  return cnt;
}

#if USE_PRINTF == 1
#ifndef USE_FLASH
UINT16 Printf( UINT8 * format, ...)
#else
#ifdef __GNU__
  UINT16 Printf( CONST prog_char * format, ...)
#else
  UINT16 Printf( UINT8 FLASH * format, ...)
#endif
#endif
{
  REGISTER UINT16         nr_of_chars;
#ifdef USE_FLASH
  STATIC   UINT8          frm[80];

  memcpyf( frm, format, 80 );
  //
  ioBPtr   = NULL;
  va_start (ioArgs, format);      /* Variable argument begin */
  nr_of_chars = formatted_Write (frm);
  va_end (ioArgs);                /* Variable argument end */
#else
  //
  ioBPtr   = NULL;
  va_start (ioArgs, format);      /* Variable argument begin */
  nr_of_chars = formatted_Write (format);
  va_end (ioArgs);                /* Variable argument end */
#endif
  return (nr_of_chars);       /* According to ANSI */
}
#endif

#if USE_SPRINTF == 1
UINT16 Sprintf( UINT8 * buf, UINT8 * format, ...)
{
  REGISTER UINT16         nr_of_chars;

  ioBPtr   = buf;
  va_start (ioArgs, format);      /* Variable argument begin */
  nr_of_chars = formatted_Write (format);
  va_end (ioArgs);                /* Variable argument end */
  *ioBPtr = 0;                    /* Close string (for sprintf) */

  return (nr_of_chars);       /* According to ANSI */
}
#endif

// END
