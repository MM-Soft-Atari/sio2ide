
#ifndef __EXTIO
  #define __EXTIO

#define FILL_OPT       1        // Fill&Limit Option for Printf/Sprintf 0/1
#define IO_HUART                // Output to Software or Hardware UART

#define USE_FLASH

#define USE_SPRINTF    0
#define USE_PRINTF     1

#ifndef USE_FLASH
UINT16 Printf          ( UINT8 * format, ...);
UINT16 Sprintf         ( UINT8 * buf, UINT8 * format, ...);
#else
#ifdef __GNU__
  UINT16 Printf        ( CONST prog_char * format, ...);
#else
  UINT16 Printf        ( UINT8 FLASH * format, ...);
#endif
#endif

#endif
//     End
