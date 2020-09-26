//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: types.h
//
// MODULE NAME: common
//
// PURPOSE:     Predefined common data types.
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001    6-12-2001 MM     Created
//
//****************************************************************

#ifndef __TYPES_H__
  #define __TYPES_H__

typedef char            INT8;
typedef short           INT16;
typedef long            INT32;
typedef unsigned char   UINT8;
typedef unsigned int    UINT16;
typedef unsigned long   UINT32;
typedef unsigned char   BOOL;

#ifdef BIG_ENDIAN
typedef union {
    struct
    {
        INT8    Type;
        INT8    Index;
    } Descriptor;

    struct
    {
        INT8 tx0;  // MSB for 8051 Keil C
        INT8 tx1;
        INT8 endp;
        INT8 c0;   // LSB for 8051 Keil C
    } chars;

    struct
    {
        INT8 c3;  // MSB for 8051 Keil C
        INT8 c2;
        INT8 c1;
        INT8 c0;   // LSB for 8051 Keil C
    } chars0;

    struct
    {
        INT16 i1;    // MSW for 8051 keil C
        INT16 i0;    // LSW for 8051 Keil C
    } ints;

    INT32 u0;

} FLEXI_INT32;

typedef union  {
    struct
    {
        INT8 c1;   // MSB for 8051 Keil C
        INT8 c0;   // LSB for 8051 Keil C
    } chars;

    INT16 i0;
} FLEXI_INT16, * PFLEXI_INT16;

#else // Little Endian

typedef union {
    struct
    {
        INT8    Type;
        INT8    Index;
    } Descriptor;

    struct
    {
        INT8 endp;
        INT8 c0;
        INT8 tx0;
        INT8 tx1;
    } chars;

    struct
    {
        INT8 c0;        // LSB
        INT8 c1;
        INT8 c2;
        INT8 c3;        // MSB
    } chars0;

    struct
    {
        INT16 i0;    // LSW
        INT16 i1;    // MSW
    } ints;

    INT32 u0;

} FLEXI_INT32;

typedef union  {
    struct
    {
        INT8 c0;   // LSB for 8051 Keil C
        INT8 c1;   // MSB for 8051 Keil C
    } chars;

    INT16 i0;
} FLEXI_INT16, * PFLEXI_INT16;
#endif

#endif  // __TYPES_H__
