/*
    Uniwersalne deklaracje dla programow w C

  plik: GLOBAL.H (deklaracje)
  autor:        Marek Mikolajewski
  data: 11.10.1996
  wersja:       1.0
*/

#ifndef __UTILS
  #define __UTILS

#ifdef BIG_ENDIAN
  #define rev_word( w )         (w)
  #define rev_longword( lw )    (lw)
#else
  UINT16 rev_word       ( UINT16 w );
  UINT32 rev_longword   ( UINT32 l );
#endif

VOID             Delay           (UINT16 Cnt);

#endif  /* __GLOBAL */
