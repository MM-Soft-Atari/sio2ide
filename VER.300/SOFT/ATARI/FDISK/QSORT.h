//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: QSORT.H
//
// MODULE NAME: QSORT
//
// PURPOSE:     Quick sort header.
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

#ifndef __QSORT_H__
  #define __QSORT_H__

typedef INT16 (*T_compare) (int, int);
typedef void  (*T_swap)    (int, int);

void QuickSort ( int Lo, int Hi, T_compare Compare, T_swap Swap );

#endif

//      End





