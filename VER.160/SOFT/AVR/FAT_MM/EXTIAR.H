//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: extiar.h
//
// MODULE NAME: common (IAR)
//
// PURPOSE:     Definitions for IAR compiler.
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


#ifndef __EXTIAR_H__
  #define __EXTIAR_H__

#define __outp(reg,val)                      (reg = (val))
#define __inp(reg)                           (reg)
#define __port_and(reg,val)                  (reg &= val)
#define __port_or(reg,val)                   (reg |= val)
#define loop_until_bit_is_set(reg,bit)       while( !(reg & (1<<bit)) )
#define loop_until_bit_is_clear(reg,bit)     while( (reg & (1<<bit)) )

#endif  // __EXTIAR_H__
