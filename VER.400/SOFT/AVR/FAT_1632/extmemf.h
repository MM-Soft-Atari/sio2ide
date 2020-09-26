//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: EXTMEMF.H
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

#ifndef __EXTMEMF_H__
  #define __EXTMEM_H__

#if defined(__IAR__) || defined(__MSDOS__)
VOID            memcpyf ( UINT8 *s1, UINT8 FLASH *s2, UINT16 maxlen );
INT16           memcmpf ( UINT8 *s1, UINT8 FLASH *s2, UINT16 maxlen );
UINT8 FLASH *   strchrf ( UINT8 FLASH *s1, UINT8 ch );
UINT8 *         strtokf ( UINT8 *s1, UINT8 FLASH *s2, UINT8 ** tok );
#endif

#ifdef __GNU__
VOID              memcpyf ( UINT8 *s1, CONST prog_char *s2, UINT16 maxlen );
INT16             memcmpf ( UINT8 *s1, CONST prog_char *s2, UINT16 maxlen );
CONST prog_char * strchrf ( CONST prog_char *s1, UINT8 ch );
UINT8 *           strtokf ( UINT8 *s1, CONST prog_char *s2, UINT8 ** tok );
VOID         progmem_read ( UINT8 * dest, CONST prog_char * src, size_t n );
#endif

#endif

//      End
