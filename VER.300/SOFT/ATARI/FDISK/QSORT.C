//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: QSORT.C
//
// MODULE NAME: QSORT
//
// PURPOSE:     Quick sort.
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

#include "plat.h"

#pragma codeseg ("CODE2")

//----------------------------------------------------------------
// Function :   QuickSort
// Notes    :
// History  :
//----------------------------------------------------------------

void QuickSort ( int Lo, int Hi, T_compare Compare, T_swap Swap )
{
  INT16 I, J;

  while (Hi > Lo)
  {
    I = Lo;
    J = Hi;
    while (I <= J)
    {
      while (I <= J && Compare ( Lo, I ) >= 0)
      {
        I++;
      }
      while (I <= J && Compare ( Lo, J ) < 0)
      {
        J--;
      }
      if (I <= J)
      {
        Swap ( I, J );
        I++;
        J--;
      }
    }
    if (J != Lo)
    {
      Swap ( J, Lo );
    }
    if (((unsigned) J) * 2 > (Hi + Lo))
    {
      QuickSort ( J + 1, Hi, Compare, Swap );
      Hi = J - 1;
    }
    else
    {
      QuickSort ( Lo, J - 1, Compare, Swap );
      Lo = J + 1;
    }
  }
}

//      End

