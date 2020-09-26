
#ifndef __STDUART
  #define __STDUART

#define BRATE           38400           // UART Baud Rate Max is 115200

#define RSBUF_SIZE      8               // UART buffer size Max size is 128
#define BRATE_TOL       1               // UART Baud Rate tolerance in %

EXTERN   VOID     init_comm   ( VOID );
EXTERN   UINT8    inp_status  ( VOID );
EXTERN   VOID     inp_flush   ( VOID );
EXTERN   VOID     uninit_comm ( VOID );
EXTERN   UINT8    inp_char    ( VOID );
EXTERN   VOID     outp_char   ( UINT8 c );
EXTERN   VOID     inp_disable ( VOID );
EXTERN   VOID     inp_enable  ( VOID );
EXTERN   VOID     inp_lock    ( VOID );
EXTERN   VOID     inp_unlock  ( VOID );

#endif
