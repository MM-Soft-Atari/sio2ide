//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: gnu.h
//
// MODULE NAME: common (GNU)
//
// PURPOSE:     Definitions for GNU compiler.
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

#ifndef __EXTGNU_H__
  #define __EXTGNU_H__

#define __outp(reg,val)         __outb(val,reg)
#define __inp(reg)              __inb(reg)

#define _NOP()                                  \
        asm volatile (                          \
                "nop"                           \
        )

#define enable_interrupt()                      \
        asm volatile (                          \
                "sei"                           \
        )

#define disable_interrupt()                     \
        asm volatile (                          \
                "cli"                           \
        )

#endif  // __EXTGNU_H__
