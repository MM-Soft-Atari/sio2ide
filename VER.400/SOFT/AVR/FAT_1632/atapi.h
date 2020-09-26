//****************************************************************
// Copyright (C), 2001 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: ATAPI.H
//
// MODULE NAME: ATAPI
//
// PURPOSE:
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001    4-03-2002 MM     Created
//
//****************************************************************

#ifndef __ATAPI_H__
  #define __ATAPI_H__

//
// ATAPI registers
//
#define ATAPI_BC_L      IDE_REG_CYLO
#define ATAPI_BC_H      IDE_REG_CYHI
#define ATAPI_IR        IDE_REG_SC

//
// Most mandtory and optional ATA commands (from ATA-3), */
//
#define CMD_CFA_ERASE_SECTORS            0xC0
#define CMD_CFA_REQUEST_EXT_ERR_CODE     0x03
#define CMD_CFA_TRANSLATE_SECTOR         0x87
#define CMD_CFA_WRITE_MULTIPLE_WO_ERASE  0xCD
#define CMD_CFA_WRITE_SECTORS_WO_ERASE   0x38
#define CMD_CHECK_POWER_MODE1            0xE5
#define CMD_CHECK_POWER_MODE2            0x98
#define CMD_DEVICE_RESET                 0x08
#define CMD_EXECUTE_DEVICE_DIAGNOSTIC    0x90
#define CMD_FLUSH_CACHE                  0xE7
#define CMD_FORMAT_TRACK                 0x50
#define CMD_IDENTIFY_DEVICE              0xEC
#define CMD_IDENTIFY_DEVICE_PACKET       0xA1
#define CMD_IDENTIFY_PACKET_DEVICE       0xA1
#define CMD_IDLE1                        0xE3
#define CMD_IDLE2                        0x97
#define CMD_IDLE_IMMEDIATE1              0xE1
#define CMD_IDLE_IMMEDIATE2              0x95
#define CMD_INITIALIZE_DRIVE_PARAMETERS  0x91
#define CMD_INITIALIZE_DEVICE_PARAMETERS 0x91
#define CMD_NOP                          0x00
#define CMD_PACKET                       0xA0
#define CMD_READ_BUFFER                  0xE4
#define CMD_READ_DMA                     0xC8
#define CMD_READ_DMA_QUEUED              0xC7
#define CMD_READ_MULTIPLE                0xC4
#define CMD_READ_SECTORS                 0x20
#define CMD_READ_VERIFY_SECTORS          0x40
#define CMD_RECALIBRATE                  0x10
#define CMD_SEEK                         0x70
#define CMD_SET_FEATURES                 0xEF
#define CMD_SET_MULTIPLE_MODE            0xC6
#define CMD_SLEEP1                       0xE6
#define CMD_SLEEP2                       0x99
#define CMD_STANDBY1                     0xE2
#define CMD_STANDBY2                     0x96
#define CMD_STANDBY_IMMEDIATE1           0xE0
#define CMD_STANDBY_IMMEDIATE2           0x94
#define CMD_WRITE_BUFFER                 0xE8
#define CMD_WRITE_DMA                    0xCA
#define CMD_WRITE_DMA_QUEUED             0xCC
#define CMD_WRITE_MULTIPLE               0xC5
#define CMD_WRITE_SECTORS                0x30
#define CMD_WRITE_VERIFY                 0x3C

//
// Most mandtory and optional ATAPI commands
//
#define CMD_START                        0x1B
#define CMD_START_LOAD                   0x03
#define CMD_START_EJECT                  0x02
#define CMD_START_TOC                    0x01
#define CMD_START_STOP                   0x00

#define CMD_INQUIRY                      0x12
#define CMD_MODE_SELECT                  0x55
#define CMD_MODE_SENSE                   0x5A
#define CMD_READ_10                      0x28
#define CMD_READ_12                      0xA8
#define CMD_READ_CD_CAPACITY             0x25
#define CMD_READ_CD                      0xBE
#define CMD_READ_CD_MSF                  0xB9
#define CMD_READ_HEADER                  0x44
#define CMD_READ_SUB_CHANNEL             0x42
#define CMD_READ_TOC                     0x43
#define CMD_REST_SENSE                   0x03
#define CMD_SEEK_ATAPI                   0x2B
#define CMD_STOP_PLAY                    0x4E
#define CMD_TEST_UNIT_READY              0x00
#define CMD_ALLOW_REMOVAL                0x1E
#define CMD_LOAD_CD                      0xA6
#define CMD_MECHANISM_STATUS             0xBD
                                         
#endif

//      End

