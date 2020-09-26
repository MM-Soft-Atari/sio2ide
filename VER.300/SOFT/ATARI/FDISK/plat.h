//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: PLAT.H
//
// MODULE NAME: PLAT
//
// PURPOSE:     Platform header.
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001   27-03-2002 MM     Created
//
//****************************************************************

#ifndef __PLAT_H__
  #define __PLAT_H__

//#define DEBUG

#define MAX_ATR_ID      250     // Max number of ATR files serviced
#define MAX_DIR_ID      50      // Max number of DIR entries serviced

#include "type.h"

#include <atari.h>
//
#include <cfg.h>
#include <ptable.h>
#include <direntry.h>
#include <iso.h>
#include <idestruc.h>
#include <fatstruc.h>
#include <siocmds.h>

#include "keys.h"
#include "siosrv.h"
#include "screen.h"
#include "menu.h"
#include "listv.h"
#include "qsort.h"

#endif

//      End
