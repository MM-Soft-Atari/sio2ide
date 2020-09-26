
#ifndef __EXTSTR
  #define __EXTSTR

#define USE_ATOI        1
#define USE_STRLEN      1
#define USE_STRGET      1
#define USE_STRNCPY     1
#define USE_STRCHR      1
#define USE_STRSTR      0
#define USE_STRLWR      0
#define USE_STRUPR      0
#define USE_STRPBRK     0
#define USE_ISSPACE     0
#define USE_ISDIGIT     1
#define USE_BTOH        1
#define USE_STRCPY      1
#define USE_STRCAT      1
#define USE_STRTOK      1
#define USE_AHTOB       1
#define USE_ITOA        1
#define USE_LTOA        1
#define USE_STRNCMP     0
#define USE_STRCMP      1
#define USE_STRTOK2     0
#define USE_ITOAB       0

#define USE_MEMSET      1
#define USE_MEMMOVE     1
#define USE_MEMCPY      1

UINT16  AtoI    ( UINT8 *s );
UINT8 * BtoH    ( UINT8 *str, UINT8 byte );
UINT8   AHtoB   ( UINT8 *s );
UINT8 * ItoA    ( UINT8 *str, UINT16 dt );
UINT8 * LtoA    ( UINT8 *str, UINT32 dt );
UINT8 * StrTok  ( UINT8 *s1, UINT8 *s2 );
BOOL    StrGet  ( UINT8 *d, UINT8 *s, UINT8 ch );
UINT8 * StrCpy  ( UINT8 *d, UINT8 *s );
UINT8 * StrNCpy ( UINT8 *d, UINT8 *s, UINT16 len );
UINT16  StrLen  ( UINT8 *s );
UINT8 * StrChr  ( UINT8 *s1, UINT8 ch );
UINT8 * StrStr  ( UINT8 *s1, UINT8 *s2 );
UINT8 * StrUpr  ( UINT8 *s );
UINT8 * StrLwr  ( UINT8 *s );
INT16   StrCmp  ( UINT8 *s1, UINT8 *s2);
INT16   StrNCmp ( UINT8 *s1, UINT8 *s2, UINT16 maxlen);
UINT8 * StrpBrk ( UINT8 *s1, UINT8 *s2);
UINT8 * StrCat  ( UINT8 *d, UINT8 *s);
UINT8   IsSpace ( UINT8 c);
UINT8   IsDigit ( UINT8 c);
VOID *  MemSet  ( VOID *s, UINT8 c, UINT16 n );
VOID *  MemMove ( VOID *dest, VOID *src, UINT16 n );
VOID    MemCpy  ( UINT8 *dest, UINT8 *src, UINT8 cnt );

#endif
