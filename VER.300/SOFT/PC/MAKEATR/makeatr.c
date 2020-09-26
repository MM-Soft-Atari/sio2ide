//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: MAKEATR.C
//
// MODULE NAME: MAKEATR
//
// PURPOSE:
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001   25-02-2002 MM     Created
//
//****************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <io.h>

#include <platform.h>
#include "hdrs.h"
#include "cfg.h"

//----------------------------------------------------------------
// Function :   main
// Notes    :
// History  :
//----------------------------------------------------------------

VOID main( int argc, char *argv[] )
{
  INT16         handle;
  UINT32        s, sect = 0;
  T_atrhdr      hdr;
  UINT8         buf[512];

  printf( "\n" );
  printf( "\nATR disk creator (ver. "__SIO2IDE_VER_TXT__")   MMSoft (c) 2002" );

  if( argc == 4 )
  {
    memset( &hdr, 0x0, sizeof(T_atrhdr) );
    hdr.wMagic = NICKATARI;
    hdr.wSecSize = atoi( argv[2] );

    if( (hdr.wSecSize != 128) && (hdr.wSecSize != 256) )
    {
      printf("\nWrong sector size specified (correct: 128/256).");
      exit(1);
    }
    sect = atol( argv[3] );
    if( (sect < 720) || (sect > 65535) )
    {
      printf("\nWrong number of sectors specified (correct: 720-65535).");
      exit(1);
    }

    if( hdr.wSecSize == 128 )
    {
      s = (sect * 128) / 0x10;
    }
    else
    {
      s = ((sect * 256) - (3 * 128)) / 0x10;
    }

    hdr.wPars  = s & 0xFFFF;
    hdr.btParsHigh = s >> 16;

    if ((handle = open( argv[1], O_WRONLY | O_CREAT | O_TRUNC,
                        S_IREAD | S_IWRITE)) == -1)
    {
      printf("\nError opening file.");
      exit(1);
    }

    if( write( handle, &hdr, sizeof(T_atrhdr) ) != sizeof(T_atrhdr) )
    {
      printf("\nError writing to the file.");
      exit(1);
    }

    printf("\nCreating ATR file.\nPlease wait.");
    //
    memset( buf, 0x0, 512 );
    if( write( handle, buf, 128 ) != 128 )
    {
      printf("\nError writing to the file.");
      exit(1);
    }
    if( write( handle, buf, 128 ) != 128 )
    {
      printf("\nError writing to the file.");
      exit(1);
    }
    if( write( handle, buf, 128 ) != 128 )
    {
      printf("\nError writing to the file.");
      exit(1);
    }
    sect -= 3;
    while( sect-- )
    {
      if( write( handle, buf, hdr.wSecSize ) != hdr.wSecSize )
      {
        printf("\nError writing to the file.");
        exit(1);
      }
    }

    close(handle);

    printf( "\nATR file (%s) created OK\n", argv[1] );
  }
  else
  {
    printf( "\nInput parameters missing." );
    printf( "\nUsage:  MAKEATR.EXE <file> <sec size> <sects>" );
    printf( "\n<file>     - ATR file name" );
    printf( "\n<sec size> - sector size (128/256)" );
    printf( "\n<sects>    - number of sectors (720-65535)" );
  }
}

//      End
