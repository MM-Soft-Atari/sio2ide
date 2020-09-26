
#include <platform.h>
#include "extstr.h"

STATIC   UINT8   buf[15];

#if USE_ATOI == 1
UINT16 AtoI( UINT8 *s )
{
  REGISTER UINT16 val = 0;
  REGISTER UINT8  ch;

  while( *s )
  {
    ch = ( (*s++) - 0x30);
    if( ch > 9 )
    {
      return 0;
    }
    val = (val * 10) + ch;
  }
  return val;
}
#endif

#if USE_BTOH == 1
UINT8 * BtoH( UINT8 *str, UINT8 byte )
{
  STATIC UINT8   HexDig[] = "0123456789ABCDEF";

  str[0] = HexDig[byte >> 4];
  str[1] = HexDig[byte & 0x0F];
  str[2] = 0x00;

  return str;
}
#endif

#if USE_AHTOB == 1
UINT8 AHtoB( UINT8 *s )
{
  REGISTER UINT8  val = 0;
  REGISTER UINT8  ch;
  REGISTER UINT8  len = 2;

  while( *s && (len--) )
  {
    ch = *s++;
    if ( (ch >= '0') && (ch <= '9') )
    {
      ch -= 0x30;
    }
    else if ( (ch >= 'A') && (ch <= 'F') )
    {
      ch -= 0x37;
    }
    else if ( (ch >= 'a') && (ch <= 'f') )
    {
      ch = ch - 0x57;
    }
    else
    {
      return 0;
    }
    val = (val << 4) + ch;
  }
  return val;
}
#endif

//
//  Integer(UINT16) To String
//
//  str  - pointer to output string
//  dt   - data to convertion
#if USE_ITOA == 1
UINT8 * ItoA ( UINT8 *str, UINT16 dt )
{
  REGISTER UINT8  *bptr = &buf[10];

  *(--bptr) = 0;
  do
  {
    *(--bptr) = (dt % 10) + 0x30;
    dt       /= 10;
  }
  while( dt );

  return StrCpy( str, bptr );
}
#endif

//----------------------------------------------------------------
// Function :   LtoA
// Notes    :
// History  :
//----------------------------------------------------------------
#if USE_LTOA == 1
UINT8 * LtoA ( UINT8 *str, UINT32 dt )
{
  REGISTER UINT8  *bptr = &buf[15];

  *(--bptr) = 0;
  do
  {
    *(--bptr) = (dt % 10) + 0x30;
    dt       /= 10;
  }
  while( dt );

  return StrCpy( str, bptr );
}
#endif

#if USE_STRCPY == 1
UINT8 * StrCpy( UINT8 *d, UINT8 *s )
{
  return StrNCpy( d, s, 0xFFFF );
}
#endif

#if USE_STRNCPY == 1
UINT8 * StrNCpy( UINT8 *d, UINT8 *s, UINT16 len )
{
  REGISTER   UINT8  *d1;

  d1 = d;
  while( *s && len-- )
  {
    *d1++ = *s++;
  }
  *d1 = 0;
  return d;
}
#endif

#if USE_STRLEN == 1
UINT16 StrLen( UINT8 *s )
{
  REGISTER UINT16 len = 0;

  while( *s++ )
  {
    len++;
  }
  return len;
}
#endif

#if USE_STRCHR == 1
UINT8 * StrChr(UINT8 *s1, UINT8 ch)
{
  while( *s1 )
  {
    if( *s1 == ch )
    {
      return s1;
    }
    s1++;
  };
  return NULL;
}
#endif

#if USE_STRGET == 1
BOOL StrGet(UINT8 *d, UINT8 *s, UINT8 ch)
{
  REGISTER      UINT8  *ptr;
  STATIC        UINT8  *tok;

  if( s )
  {
    tok = s;
  }
  if( !tok )
  {
    return FALSE;
  }
  ptr = StrChr( tok, ch );
  StrNCpy( d, tok, (ptr - tok) );
  tok = (ptr) ? (ptr + 1) : NULL;
  return TRUE;
}
#endif

#if USE_STRTOK == 1
UINT8 * StrTok(UINT8 *s1, UINT8 *s2)
{
  REGISTER      UINT8  *s;
  STATIC        UINT8  *tok;

  if( s1 )
  {
    tok = s1;
  }
  if( !tok )
  {
    return NULL;
  }
  while( *tok )         /* skip spaces */
  {
    if( StrChr( s2, *tok ) )
    {
      tok++;
    }
    else
    {
      break;
    }
  }
  if( !*tok )           /* token not found */
  {
    return NULL;
  }
  s = tok;              /* save token start */
  while( *tok )         /* find end of token */
  {
    if( !StrChr( s2, *tok ) )
    {
      tok++;
    }
    else
    {
      *tok = 0x0;       /* terminate token */
      tok++;            /* save next token start */
      break;
    }
  }
  return s;             /* return token start pointer */
}
#endif

#if USE_STRSTR == 1
UINT8 * StrStr(UINT8 *s1, UINT8 *s2)
{
  STATIC UINT8 *p;

  p = (char *)s1;
  while( *p )
  {
    if( StrLen(p) < StrLen(s2) )
    {
      return NULL;
    }
    if( StrNCmp( p, s2, StrLen( s2 ) ) == 0 )
    {
      return p;
    }
    p++;
  }
  return NULL;
}
#endif

#if USE_STRUPR == 1
UINT8 *  StrUpr(UINT8 *s)
{
  STATIC UINT8 *p;

  p = s;
  while( *p )
  {
    if( (*p > 0x60) && (*p < 0x7B) )
    {
      *p = *p - 0x20;
    }
    p++;
  }
  return s;
}
#endif

#if USE_STRLWR == 1
UINT8 *  StrLwr(UINT8 *s)
{
  STATIC UINT8 *p;

  p = s;
  while( *p )
  {
    if( (*p > 0x40) && (*p < 0x5B) )
    {
      *p = *p + 0x20;
    }
    p++;
  }
  return s;
}
#endif

#if USE_STRCMP == 1
INT16   StrCmp  ( UINT8 *s1, UINT8 *s2)
{
  while( *s1 && *s2 )
  {
    if( *s1 != *s2 )
    {
      return (*s1 > *s2) ? 1 : -1;
    }
    s1++;
    s2++;
  }
  return (*s1 == *s2) ? 0 : (( *s1 ) ? 1 : -1);
}
#endif

#if USE_STRNCMP == 1
INT16   StrNCmp ( UINT8 *s1, UINT8 *s2, UINT16 maxlen)
{
  while( *s1 && *s2 && maxlen-- )
  {
    if( *s1 != *s2 )
    {
      return (*s1 > *s2) ? 1 : -1;
    }
    s1++;
    s2++;
  }
  if ( maxlen )
  {
    return (*s1 == *s2) ? 0 : (( *s1 ) ? 1 : -1);
  }
  return 0;
}
#endif

#if USE_STRPBRK == 1
UINT8 * StrpBrk ( UINT8 *s1, UINT8 *s2)
{
  REGISTER   UINT8  *d1;

  d1 = s1;
  while( *d1 )
  {
    if( StrChr( s2, *d1 ) )
    {
      return d1;
    }
    d1++;
  }
  return NULL;
}
#endif

#if USE_STRCAT == 1
UINT8 * StrCat  ( UINT8 *d, UINT8 *s)
{
  StrCpy( (UINT8*)(d + StrLen( d )), s );
  return d;
}
#endif

#if USE_ISSPACE == 1
// (0x09 to 0x0D, 0x20)
UINT8   IsSpace ( UINT8 c)
{
  return (((c >= 0x09) && (c <= 0x0D)) || (c == 0x20)) ? TRUE : FALSE;
}
#endif

#if USE_ISDIGIT == 1
// (0x30 to 0x39)
UINT8   IsDigit ( UINT8 c)
{
  return ((c >= 0x30) && (c <= 0x39)) ? TRUE : FALSE;
}
#endif

#if USE_MEMSET == 1
VOID * MemSet ( VOID *s, UINT8 c, UINT16 n )
{
  REGISTER   UINT8  *d1;

  d1 = s;
  while( n-- )
  {
    *d1++ = c;
  }
  return s;
}
#endif

#if USE_MEMMOVE == 1
VOID * MemMove ( VOID *dest, VOID *src, UINT16 n )
{
  REGISTER   UINT8  *d;
  REGISTER   UINT8  *s;

  d = dest;
  s = src;
  if( d > s )
  {
    d += (n - 1);
    s += (n - 1);
    while( n-- )
    {
      *d-- = *s--;
    }
  }
  else if( d < s )
  {
    while( n-- )
    {
      *d++ = *s++;
    }
  }
  return dest;
}
#endif

#if USE_MEMCPY == 1
VOID MemCpy(UINT8* dest, UINT8* src, UINT8 cnt)
{
    while ( cnt-- )
    {
        *(dest++) = *(src++);
    }
    return;
}
#endif

// End
