//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: EXTMEMF.C
//
// MODULE NAME: EXTMEMF
//
// PURPOSE:
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001   22-02-2002 MM     Created
//
//****************************************************************

#include <platform.h>

//----------------------------------------------------------------
// Function :   progmem_read
// Notes    :
// History  :
//----------------------------------------------------------------
#ifdef __GNU__
VOID progmem_read( UINT8 * dest, CONST prog_char * src, size_t n )
{
  while( n-- )
  {
    *dest++ = PRG_RDB( src++ );
  }
}
#endif

//----------------------------------------------------------------
// Function :   memcmpf
// Notes    :
// History  :
//----------------------------------------------------------------
#ifdef __GNU__
INT16   memcmpf ( UINT8 *s1, CONST prog_char *s2, UINT16 maxlen )
{
  while( maxlen-- )
  {
    if( *s1 != PRG_RDB( s2 ) )
    {
      return (*s1 > PRG_RDB( s2 )) ? 1 : -1;
    }
    s1++;
    s2++;
  }
  return 0;
}
#endif
#if defined(__IAR__) || defined(__MSDOS__)
INT16   memcmpf ( UINT8 *s1, UINT8 FLASH *s2, UINT16 maxlen )
{
  while( maxlen-- )
  {
    if( *s1 != *s2 )
    {
      return (*s1 > *s2) ? 1 : -1;
    }
    s1++;
    s2++;
  }
  return 0;
}
#endif

//----------------------------------------------------------------
// Function :   memcpyf
// Notes    :
// History  :
//----------------------------------------------------------------
#ifdef __GNU__
VOID   memcpyf ( UINT8 *s1, CONST prog_char *s2, UINT16 maxlen )
{
  while( maxlen-- )
  {
    *s1++ = PRG_RDB( s2 );
    s2++;
  }
}
#endif
#if defined(__IAR__) || defined(__MSDOS__)
VOID   memcpyf ( UINT8 *s1, UINT8 FLASH *s2, UINT16 maxlen )
{
  while( maxlen-- )
  {
    *s1++ = *s2++;
  }
}
#endif

//----------------------------------------------------------------
// Function :   strchrf
// Notes    :
// History  :
//----------------------------------------------------------------
#ifdef __GNU__
CONST prog_char * strchrf ( CONST prog_char *s1, UINT8 ch )
{
  while( PRG_RDB( s1 ) )
  {
    if( PRG_RDB( s1 ) == ch )
    {
      return s1;
    }
    s1++;
  };
  return (CONST prog_char *)NULL;
}
#endif
#if defined(__IAR__) || defined(__MSDOS__)
UINT8 FLASH * strchrf ( UINT8 FLASH *s1, UINT8 ch )
{
  while( *s1 )
  {
    if( *s1 == ch )
    {
      return s1;
    }
    s1++;
  };
  return (UINT8 FLASH *)NULL;
}
#endif

//----------------------------------------------------------------
// Function :   strtokf
// Notes    :
// History  :
//----------------------------------------------------------------
#ifdef __GNU__
UINT8 * strtokf ( UINT8 *s1, CONST prog_char *s2, UINT8 ** tok )
{
  REGISTER      UINT8  *s;

  if( s1 )
  {
    *tok = s1;
  }
  if( !*tok )
  {
    return NULL;
  }
  while( **tok )         /* skip spaces */
  {
    if( strchrf( s2, **tok ) )
    {
      (*tok)++;
    }
    else
    {
      break;
    }
  }
  if( !**tok )           /* token not found */
  {
    return NULL;
  }
  s = *tok;              /* save token start */
  while( **tok )         /* find end of token */
  {
    if( !strchrf( s2, **tok ) )
    {
      (*tok)++;
    }
    else
    {
      **tok = 0x0;       /* terminate token */
      (*tok)++;          /* save next token start */
      break;
    }
  }
  return s;             /* return token start pointer */
}
#endif
#if defined(__IAR__) || defined(__MSDOS__)
UINT8 * strtokf ( UINT8 *s1, UINT8 FLASH *s2, UINT8 ** tok )
{
  REGISTER      UINT8  *s;

  if( s1 )
  {
    *tok = s1;
  }
  if( !*tok )
  {
    return NULL;
  }
  while( **tok )         /* skip spaces */
  {
    if( strchrf( s2, **tok ) )
    {
      (*tok)++;
    }
    else
    {
      break;
    }
  }
  if( !**tok )           /* token not found */
  {
    return NULL;
  }
  s = *tok;              /* save token start */
  while( **tok )         /* find end of token */
  {
    if( !strchrf( s2, **tok ) )
    {
      (*tok)++;
    }
    else
    {
      **tok = 0x0;       /* terminate token */
      (*tok)++;          /* save next token start */
      break;
    }
  }
  return s;             /* return token start pointer */
}
#endif

//      End
