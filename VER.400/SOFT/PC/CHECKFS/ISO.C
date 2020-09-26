//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: ISO.C
//
// MODULE NAME: ISO
//
// PURPOSE:
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001   28-02-2002 MM     Created
//
//****************************************************************

#include "apps.h"

/* 7.1.1: unsigned INT8 */
UINT8 isonum_711(UINT8 *p)
{
  return *p;
}

/* 7.1.2: signed(?) INT8 */
INT16 isonum_712(INT8 *p)
{
  return *p;
}

/* 7.2.1: unsigned little-endian 16-bit value.  NOT USED IN KERNEL. */
INT16 isonum_721(UINT8 *p)
{
#if defined(UNALIGNED_ACCESS) && (BYTE_ORDER == LITTLE_ENDIAN)
        return *(UINT16 *)p;
#else
        return *p|((INT8)p[1] << 8);
#endif
}

/* 7.2.2: unsigned big-endian 16-bit value.  NOT USED IN KERNEL. */
INT16 isonum_722(UINT8 *p)
{
#if defined(UNALIGNED_ACCESS) && (BYTE_ORDER == BIG_ENDIAN)
        return *(UINT16 *)p;
#else
        return ((INT8)*p << 8)|p[1];
#endif
}

/* 7.2.3: unsigned both-endian (little, then big) 16-bit value */
INT16 isonum_723(UINT8 *p)
{
#if defined(UNALIGNED_ACCESS) && \
    ((BYTE_ORDER == LITTLE_ENDIAN) || (BYTE_ORDER == BIG_ENDIAN))
#if BYTE_ORDER == LITTLE_ENDIAN
        return *(UINT16 *)p;
#else
        return *(UINT16 *)(p + 2);
#endif
#else /* !UNALIGNED_ACCESS or weird byte order */
        return *p|(p[1] << 8);
#endif
}

/* 7.3.1: unsigned little-endian 32-bit value.  NOT USED IN KERNEL. */
INT16 isonum_731(UINT8 *p)
{
#if defined(UNALIGNED_ACCESS) && (BYTE_ORDER == LITTLE_ENDIAN)
        return *(UINT32 *)p;
#else
        return *p|(p[1] << 8)|(p[2] << 16)|(p[3] << 24);
#endif
}

/* 7.3.2: unsigned big-endian 32-bit value.  NOT USED IN KERNEL. */
UINT32 isonum_732(UINT8 *p)
{
#if defined(UNALIGNED_ACCESS) && (BYTE_ORDER == BIG_ENDIAN)
        return *(UINT32 *)p;
#else
        return ((UINT32)*p << 24)|((UINT32)p[1] << 16)|((UINT32)p[2] << 8)|p[3];
#endif
}

/* 7.3.3: unsigned both-endian (little, then big) 32-bit value */
UINT32 isonum_733(UINT8 *p)
{
#if defined(UNALIGNED_ACCESS) && \
    ((BYTE_ORDER == LITTLE_ENDIAN) || (BYTE_ORDER == BIG_ENDIAN))
#if BYTE_ORDER == LITTLE_ENDIAN
        return *(UINT32 *)p;
#else
        return *(UINT32 *)(p + 4);
#endif
#else /* !UNALIGNED_ACCESS or weird byte order */
        return *p|(p[1] << 8)|(p[2] << 16)|(p[3] << 24);
#endif
}

//      End
