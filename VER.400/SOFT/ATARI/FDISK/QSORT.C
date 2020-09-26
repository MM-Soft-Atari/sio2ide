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

static T_compare  Cmp;
static T_swap     Swp;

/*

//----------------------------------------------------------------
// Function :   QuickSort
// Notes    :
// History  :
//----------------------------------------------------------------

static void IntQuickSort ( INT16 Lo, INT16 Hi )
{
  INT16 I, J;

  while (Hi > Lo)
  {
    I = Lo;
    J = Hi;
    while (I <= J)
    {
      while (I <= J && Cmp ( Lo, I ) >= 0)
      {
        I++;
      }
      while (I <= J && Cmp ( Lo, J ) < 0)
      {
        J--;
      }
      if (I <= J)
      {
        Swp ( I, J );
        I++;
        J--;
      }
    }
    if (J != Lo)
    {
      Swp ( J, Lo );
    }
    if (((unsigned) J) * 2 > (Hi + Lo))
    {
      IntQuickSort ( J + 1, Hi );
      Hi = J - 1;
    }
    else
    {
      IntQuickSort ( Lo, J - 1 );
      Lo = J + 1;
    }
  }
}

*/

//----------------------------------------------------------------
// Function :   IntQuickSort
// Notes    :
// History  :
//----------------------------------------------------------------

static void IntQuickSort ( INT16 l, INT16 r )
{
  INT16  i,j;

  i = l - 1;
  j = r;
  //
  if( r <= l )  return;
  //
  for(;;)
  {
    while( Cmp( ++i, r ) < 0 );
    while( Cmp( --j, r ) >=  0 )   if( j == l )  break;
    //
    if( i >= j )  break;
    //
    Swp( i, j );
  }
  Swp( i, r );
  //
  IntQuickSort ( l, i-1 );
  IntQuickSort ( i+1, r );
}

//----------------------------------------------------------------
// Function :   QuickSort
// Notes    :
// History  :
//----------------------------------------------------------------

void QuickSort ( INT16 Lo, INT16 Hi, T_compare Compare, T_swap Swap )
{
  Cmp = Compare;
  Swp = Swap;
  //
  IntQuickSort ( Lo, Hi );
}

//      End

