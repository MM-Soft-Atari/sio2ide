//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: SIOSRV.C
//
// MODULE NAME: SIOSRV
//
// PURPOSE:     SIO service.
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001   24-01-2002 MM     Created
//
//****************************************************************

#include "plat.h"

#pragma codeseg ("CODE2")

extern UINT8 siov( void );

//----------------------------------------------------------------
// Function :   SIO_GetDisk
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SIO_GetDisk( UINT8 dev, UINT8 disk, T_file * file )
{
  UINT8   s;
  T_file  tfile;

#ifdef DEBUG
  return 1;
#endif

  DCB->device   = FS_SIO2IDE;            // SIO2IDE
  DCB->unit     = dev;                   //
  DCB->command  = SIOC_GETDSK;           //
  DCB->aux1     = disk;                   //
  DCB->status   = 0x40;                  // READ
  DCB->buffer   = (UINT8*)&tfile;        //
  DCB->timeout  = 20;                    // TimeOut
  DCB->xfersize = sizeof(T_file);        // Data Size

  if( (s = siov()) != 0 )
  {
    return 0;
  }
  *file = tfile;        // Copy Data
  return 1;
}

//----------------------------------------------------------------
// Function :   SIO_PutDisk
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SIO_PutDisk( UINT8 dev, UINT8 disk, T_file * file )
{
  UINT8  s;

#ifdef DEBUG
  return 1;
#endif

  DCB->device   = FS_SIO2IDE;            // SIO2IDE
  DCB->unit     = dev;                   //
  DCB->command  = SIOC_PUTDSK;           //
  DCB->aux1     = disk;                   //
  DCB->status   = 0x80;                  // WRITE
  DCB->buffer   = (UINT8*)file;          //
  DCB->timeout  = 30;                    // TimeOut
  DCB->xfersize = sizeof(T_file);        // Data Size

  if( (s = siov()) != 0 )
  {
    return 0;
  }
  return 1;
}

//----------------------------------------------------------------
// Function :   SIO_GetFirstATR
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SIO_GetFirstATR( UINT8 dev, T_file * file )
{
  UINT8   s;
  T_file  tfile;

#ifdef DEBUG
  return 1;
#endif

  DCB->device   = FS_SIO2IDE;            // SIO2IDE
  DCB->unit     = dev;                   //
  DCB->command  = SIOC_FATR;             //
  DCB->status   = 0x40;                  // READ
  DCB->buffer   = (UINT8*)&tfile;        //
  DCB->timeout  = 10;                    // TimeOut
  DCB->xfersize = sizeof(T_file);        // Data Size

  if( (s = siov()) != 0 )
  {
    return 0;
  }
  *file = tfile;        // Copy Data
  return 1;
}

//----------------------------------------------------------------
// Function :   SIO_GetNextATR
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SIO_GetNextATR( UINT8 dev, T_file * file )
{
  UINT8   s;
  T_file  tfile;

#ifdef DEBUG
  return 1;
#endif

  DCB->device   = FS_SIO2IDE;            // SIO2IDE
  DCB->unit     = dev;                   //
  DCB->command  = SIOC_NATR;             //
  DCB->status   = 0x40;                  // READ
  DCB->buffer   = (UINT8*)&tfile;        //
  DCB->timeout  = 10;                    // TimeOut
  DCB->xfersize = sizeof(T_file);        // Data Size

  if( (s = siov()) != 0 )
  {
    return 0;
  }
  *file = tfile;        // Copy Data
  return 1;
}

//----------------------------------------------------------------
// Function :   SIO_GetDriveInfo
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SIO_GetDriveInfo( UINT8 dev, T_drvinf * drv )
{
  UINT8     s;
  T_drvinf  tdrv;

#ifdef DEBUG
  return 1;
#endif

  DCB->device   = FS_SIO2IDE;            // SIO2IDE
  DCB->unit     = dev;                   //
  DCB->command  = SIOC_GETDRV;           //
  DCB->status   = 0x40;                  // READ
  DCB->buffer   = (UINT8*)&tdrv;         //
  DCB->timeout  = 5;                     // TimeOut
  DCB->xfersize = sizeof(T_drvinf);      // Data Size

  if( (s = siov()) != 0 )
  {
    return 0;
  }
  *drv = tdrv;        // Copy Data
  return 1;
}

//----------------------------------------------------------------
// Function :   SIO_GetFSInfo
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SIO_GetFSInfo( UINT8 dev, T_fsinf * fs )
{
  UINT8     s;
  T_fsinf   tfs;

#ifdef DEBUG
  return 1;
#endif

  DCB->device   = FS_SIO2IDE;            // SIO2IDE
  DCB->unit     = dev;                   //
  DCB->command  = SIOC_GETFSI;           //
  DCB->status   = 0x40;                  // READ
  DCB->buffer   = (UINT8*)&tfs;          //
  DCB->timeout  = 5;                     // TimeOut
  DCB->xfersize = sizeof(T_fsinf);       // Data Size

  if( (s = siov()) != 0 )
  {
    return 0;
  }
  *fs = tfs;        // Copy Data
  return 1;
}

//----------------------------------------------------------------
// Function :   SIO_DiskOff
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SIO_DiskOff( UINT8 dev, UINT8 disk )
{
  UINT8  s;

#ifdef DEBUG
  return 1;
#endif

  DCB->device   = FS_SIO2IDE;            // SIO2IDE
  DCB->unit     = dev;                   //
  DCB->command  = SIOC_OFFDSK;           //
  DCB->aux1     = disk;                  //
  DCB->status   = 0x40;                  // READ
  DCB->buffer   = &s;                    //
  DCB->timeout  = 5;                     // TimeOut
  DCB->xfersize = 1;                     // Data Size

  if( siov() != 0 )
  {
    return 0;
  }
  if( s != 0xFF )
  {
    return 0;
  }
  return 1;
}

//----------------------------------------------------------------
// Function :   SIO_SaveConfig
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SIO_SaveConfig( UINT8 dev )
{
  UINT8  s;

#ifdef DEBUG
  return 1;
#endif

  DCB->device   = FS_SIO2IDE;            // SIO2IDE
  DCB->unit     = dev;                   //
  DCB->command  = SIOC_SCFG;             //
  DCB->status   = 0x40;                  // READ
  DCB->buffer   = &s;                    //
  DCB->timeout  = 30;                    // TimeOut
  DCB->xfersize = 1;                     // Data Size

  if( siov() != 0 )
  {
    return 0;
  }
  if( s != 0xFF )
  {
    return 0;
  }
  return 1;
}

//----------------------------------------------------------------
// Function :   SIO_GetFirstDir
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SIO_GetFirstDir( UINT8 dev, T_file * dir )
{
  UINT8   s;
  T_file  tdir;

#ifdef DEBUG
  return 1;
#endif

  DCB->device   = FS_SIO2IDE;            // SIO2IDE
  DCB->unit     = dev;                   //
  DCB->command  = SIOC_FDIR;             //
  DCB->status   = 0x40;                  // READ
  DCB->buffer   = (UINT8*)&tdir;         //
  DCB->timeout  = 10;                    // TimeOut
  DCB->xfersize = sizeof(T_file);        // Data Size

  if( (s = siov()) != 0 )
  {
    return 0;
  }
  *dir = tdir;        // Copy Data
  return 1;
}

//----------------------------------------------------------------
// Function :   SIO_GetNextDir
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SIO_GetNextDir( UINT8 dev, T_file * dir )
{
  UINT8   s;
  T_file  tdir;

#ifdef DEBUG
  return 1;
#endif

  DCB->device   = FS_SIO2IDE;            // SIO2IDE
  DCB->unit     = dev;                   //
  DCB->command  = SIOC_NDIR;             //
  DCB->status   = 0x40;                  // READ
  DCB->buffer   = (UINT8*)&tdir;         //
  DCB->timeout  = 10;                    // TimeOut
  DCB->xfersize = sizeof(T_file);        // Data Size

  if( (s = siov()) != 0 )
  {
    return 0;
  }
  *dir = tdir;        // Copy Data
  return 1;
}

//----------------------------------------------------------------
// Function :   SIO_SetCurDir
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SIO_SetCurDir( UINT8 dev, T_file * dir )
{
  UINT8   s;

#ifdef DEBUG
  return 1;
#endif

  DCB->device   = FS_SIO2IDE;            // SIO2IDE
  DCB->unit     = dev;                   //
  DCB->command  = SIOC_SCDIR;            //
  DCB->status   = 0x80;                  // WRITE
  DCB->buffer   = (UINT8*)dir;           //
  DCB->timeout  = 10;                    // TimeOut
  DCB->xfersize = sizeof(T_file);        // Data Size

  if( (s = siov()) != 0 )
  {
    return 0;
  }
  return 1;
}

//----------------------------------------------------------------
// Function :   SIO_GetCurDir
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SIO_GetCurDir( UINT8 dev, T_file * dir )
{
  UINT8   s;
  T_file  tdir;

#ifdef DEBUG
  return 1;
#endif

  DCB->device   = FS_SIO2IDE;            // SIO2IDE
  DCB->unit     = dev;                   //
  DCB->command  = SIOC_GCDIR;            //
  DCB->status   = 0x40;                  // READ
  DCB->buffer   = (UINT8*)&tdir;         //
  DCB->timeout  = 10;                    // TimeOut
  DCB->xfersize = sizeof(T_file);        // Data Size

  if( (s = siov()) != 0 )
  {
    return 0;
  }
  *dir = tdir;        // Copy Data
  return 1;
}

//----------------------------------------------------------------
// Function :   SIO_InitCurDir
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SIO_InitCurDir( UINT8 dev, T_file * dir )
{
  UINT8   s;
  T_file  tdir;

#ifdef DEBUG
  return 1;
#endif

  DCB->device   = FS_SIO2IDE;            // SIO2IDE
  DCB->unit     = dev;                   //
  DCB->command  = SIOC_ICDIR;            //
  DCB->status   = 0x40;                  // READ
  DCB->buffer   = (UINT8*)&tdir;         //
  DCB->timeout  = 90;                    // TimeOut
  DCB->xfersize = sizeof(T_file);        // Data Size

  if( (s = siov()) != 0 )
  {
    return 0;
  }
  *dir = tdir;        // Copy Data
  return 1;
}

//----------------------------------------------------------------
// Function :   SIO_SetSect
// Notes    :
// History  :
//----------------------------------------------------------------

static BOOL SIO_SetSect( UINT8 dev, UINT32 sect )
{
  UINT8  s;

#ifdef DEBUG
  return 1;
#endif

  DCB->device   = FS_SIO2IDE;            // SIO2IDE
  DCB->unit     = dev;                   //
  DCB->command  = SIOC_SSECT;            //
  DCB->status   = 0x80;                  // WRITE
  DCB->buffer   = (UINT8*)&sect;         //
  DCB->timeout  = 20;                    // TimeOut
  DCB->xfersize = sizeof(UINT32);        // Data Size

  if( (s = siov()) != 0 )
  {
    return 0;
  }
  return 1;
}

//----------------------------------------------------------------
// Function :   SIO_PutSect
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SIO_PutSect( UINT8 dev, UINT32 sect, UINT8 * data )
{
  UINT8  s;

#ifdef DEBUG
  return 1;
#endif

  if( !SIO_SetSect( dev, sect ) )
    return 0;

  DCB->device   = FS_SIO2IDE;            // SIO2IDE
  DCB->unit     = dev;                   //
  DCB->command  = SIOC_PSECT;            //
  DCB->status   = 0x80;                  // WRITE
  DCB->buffer   = (UINT8*)data;          //
  DCB->timeout  = 30;                    // TimeOut
  DCB->xfersize = 512;                   // Data Size

  if( (s = siov()) != 0 )
  {
    return 0;
  }
  return 1;
}

//----------------------------------------------------------------
// Function :   SIO_GetSect
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SIO_GetSect( UINT8 dev, UINT32 sect, UINT8 * buf )
{
  UINT8   s;

#ifdef DEBUG
  return 1;
#endif

  if( !SIO_SetSect( dev, sect ) )
    return 0;

  DCB->device   = FS_SIO2IDE;            // SIO2IDE
  DCB->unit     = dev;                   //
  DCB->command  = SIOC_GSECT;            //
  DCB->status   = 0x40;                  // READ
  DCB->buffer   = (UINT8*)buf;           //
  DCB->timeout  = 30;                    // TimeOut
  DCB->xfersize = 512;                   // Data Size

  if( (s = siov()) != 0 )
  {
    return 0;
  }
  return 1;
}

//----------------------------------------------------------------
// Function :   SIO_GetLFN
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL SIO_GetLFN( UINT8 dev, UINT32 fsect, UINT8 * buf )
{
  UINT8   s;

#ifdef DEBUG
  return 1;
#endif

  if( !SIO_SetSect( dev, fsect ) )
    return 0;

  DCB->device   = FS_SIO2IDE;            // SIO2IDE
  DCB->unit     = dev;                   //
  DCB->command  = SIOC_GLFN;             //
  DCB->status   = 0x40;                  // READ
  DCB->buffer   = (UINT8*)buf;           //
  DCB->timeout  = 30;                    // TimeOut
  DCB->xfersize = LFN_MAX_SIZE;          // Data Size

  if( (s = siov()) != 0 )
  {
    return 0;
  }
  return 1;
}

//      End
