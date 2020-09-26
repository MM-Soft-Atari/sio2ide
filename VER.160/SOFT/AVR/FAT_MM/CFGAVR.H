//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: cfgavr.h
//
// MODULE NAME: common (Hardware)
//
// PURPOSE:     Hardware dependent configuration.
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


#ifndef __CFG_AVR
  #define __CFG_AVR

//
//      AVR processor configuration
//
#define FOSC            7372800L        // Main clock
#define FCPU            (FOSC/1)        // CPU clock

#endif
