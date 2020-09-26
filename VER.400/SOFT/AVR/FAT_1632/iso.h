//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: ISO.H
//
// MODULE NAME: ISO
//
// PURPOSE:
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001   28-02-2002 MM     Created
//
//****************************************************************

#ifndef __ISO9660_H__
  #define __ISO9660_H__

#define BIG_ENDIAN      1
#define LITTLE_ENDIAN   2

#define UNALIGNED_ACCESS
#define BYTE_ORDER      LITTLE_ENDIAN

/*
 * Definitions describing ISO9660 file system structure, as well as
 * the functions necessary to access fields of ISO9660 file system
 * structures.
 */

#define ISODCL(from, to) (to - from + 1)

typedef struct
{
        INT8 type[ISODCL(1,1)]; /* 711 */
        INT8 id[ISODCL(2,6)];
        INT8 version[ISODCL(7,7)];
        INT8 data[ISODCL(8,2048)];
} T_ISO_VOL_DESC;

/* volume descriptor types */
#define ISO_VD_PRIMARY          1
#define ISO_VD_SUPPLEMENTARY    2
#define ISO_VD_END              255

#define ISO_STANDARD_ID         "CD001"
#define ISO_ECMA_ID             "CDW01"

typedef struct
{
        INT8 type                       [ISODCL (  1,   1)]; /* 711 */
        INT8 id                         [ISODCL (  2,   6)];
        INT8 version                    [ISODCL (  7,   7)]; /* 711 */
        INT8 unused1                    [ISODCL (  8,   8)];
        INT8 system_id                  [ISODCL (  9,  40)]; /* achars */
        INT8 volume_id                  [ISODCL ( 41,  72)]; /* dchars */
        INT8 unused2                    [ISODCL ( 73,  80)];
        INT8 volume_space_size          [ISODCL ( 81,  88)]; /* 733 */
        INT8 unused3                    [ISODCL ( 89, 120)];
        INT8 volume_set_size            [ISODCL (121, 124)]; /* 723 */
        INT8 volume_sequence_number     [ISODCL (125, 128)]; /* 723 */
        INT8 logical_block_size         [ISODCL (129, 132)]; /* 723 */
        INT8 path_table_size            [ISODCL (133, 140)]; /* 733 */
        INT8 type_l_path_table          [ISODCL (141, 144)]; /* 731 */
        INT8 opt_type_l_path_table      [ISODCL (145, 148)]; /* 731 */
        INT8 type_m_path_table          [ISODCL (149, 152)]; /* 732 */
        INT8 opt_type_m_path_table      [ISODCL (153, 156)]; /* 732 */
        INT8 root_directory_record      [ISODCL (157, 190)]; /* 9.1 */
        INT8 volume_set_id              [ISODCL (191, 318)]; /* dchars */
        INT8 publisher_id               [ISODCL (319, 446)]; /* achars */
        INT8 preparer_id                [ISODCL (447, 574)]; /* achars */
        INT8 application_id             [ISODCL (575, 702)]; /* achars */
        INT8 copyright_file_id          [ISODCL (703, 739)]; /* 7.5 dchars */
        INT8 abstract_file_id           [ISODCL (740, 776)]; /* 7.5 dchars */
        INT8 bibliographic_file_id      [ISODCL (777, 813)]; /* 7.5 dchars */
        INT8 creation_date              [ISODCL (814, 830)]; /* 8.4.26.1 */
        INT8 modification_date          [ISODCL (831, 847)]; /* 8.4.26.1 */
        INT8 expiration_date            [ISODCL (848, 864)]; /* 8.4.26.1 */
        INT8 effective_date             [ISODCL (865, 881)]; /* 8.4.26.1 */
        INT8 file_structure_version     [ISODCL (882, 882)]; /* 711 */
        INT8 unused4                    [ISODCL (883, 883)];
        INT8 application_data           [ISODCL (884, 1395)];
        INT8 unused5                    [ISODCL (1396, 2048)];
} T_ISO_PRIM_DESC;

#define ISO_DEFAULT_BLOCK_SIZE          2048

typedef struct
{
        INT8 type                       [ISODCL (  1,   1)]; /* 711 */
        INT8 id                         [ISODCL (  2,   6)];
        INT8 version                    [ISODCL (  7,   7)]; /* 711 */
        INT8 flags                      [ISODCL (  8,   8)]; /* 711? */
        INT8 system_id                  [ISODCL (  9,  40)]; /* achars */
        INT8 volume_id                  [ISODCL ( 41,  72)]; /* dchars */
        INT8 unused2                    [ISODCL ( 73,  80)];
        INT8 volume_space_size          [ISODCL ( 81,  88)]; /* 733 */
        INT8 escape                     [ISODCL ( 89, 120)];
        INT8 volume_set_size            [ISODCL (121, 124)]; /* 723 */
        INT8 volume_sequence_number     [ISODCL (125, 128)]; /* 723 */
        INT8 logical_block_size         [ISODCL (129, 132)]; /* 723 */
        INT8 path_table_size            [ISODCL (133, 140)]; /* 733 */
        INT8 type_l_path_table          [ISODCL (141, 144)]; /* 731 */
        INT8 opt_type_l_path_table      [ISODCL (145, 148)]; /* 731 */
        INT8 type_m_path_table          [ISODCL (149, 152)]; /* 732 */
        INT8 opt_type_m_path_table      [ISODCL (153, 156)]; /* 732 */
        INT8 root_directory_record      [ISODCL (157, 190)]; /* 9.1 */
        INT8 volume_set_id              [ISODCL (191, 318)]; /* dchars */
        INT8 publisher_id               [ISODCL (319, 446)]; /* achars */
        INT8 preparer_id                [ISODCL (447, 574)]; /* achars */
        INT8 application_id             [ISODCL (575, 702)]; /* achars */
        INT8 copyright_file_id          [ISODCL (703, 739)]; /* 7.5 dchars */
        INT8 abstract_file_id           [ISODCL (740, 776)]; /* 7.5 dchars */
        INT8 bibliographic_file_id      [ISODCL (777, 813)]; /* 7.5 dchars */
        INT8 creation_date              [ISODCL (814, 830)]; /* 8.4.26.1 */
        INT8 modification_date          [ISODCL (831, 847)]; /* 8.4.26.1 */
        INT8 expiration_date            [ISODCL (848, 864)]; /* 8.4.26.1 */
        INT8 effective_date             [ISODCL (865, 881)]; /* 8.4.26.1 */
        INT8 file_structure_version     [ISODCL (882, 882)]; /* 711 */
        INT8 unused4                    [ISODCL (883, 883)];
        INT8 application_data           [ISODCL (884, 1395)];
        INT8 unused5                    [ISODCL (1396, 2048)];
} T_ISO_SUP_DESC;

typedef struct
{
        INT8 length                     [ISODCL (1, 1)]; /* 711 */
        INT8 ext_attr_length            [ISODCL (2, 2)]; /* 711 */
        UINT8 extent                   [ISODCL (3, 10)]; /* 733 */
        UINT8 size                     [ISODCL (11, 18)]; /* 733 */
        INT8 date                       [ISODCL (19, 25)]; /* 7 by 711 */
        INT8 flags                      [ISODCL (26, 26)];
        INT8 file_unit_size             [ISODCL (27, 27)]; /* 711 */
        INT8 interleave                 [ISODCL (28, 28)]; /* 711 */
        INT8 volume_sequence_number     [ISODCL (29, 32)]; /* 723 */
        INT8 name_len                   [ISODCL (33, 33)]; /* 711 */
        INT8 name                       [1];                    /* XXX */
} T_ISO_DIR_REC;

/* can't take sizeof(iso_directory_record), because of possible alignment
   of the last entry (34 instead of 33) */

#define ISO_DIRECTORY_RECORD_SIZE       33

struct iso_extended_attributes {
        UINT8 owner                    [ISODCL (1, 4)]; /* 723 */
        UINT8 group                    [ISODCL (5, 8)]; /* 723 */
        UINT8 perm                     [ISODCL (9, 10)]; /* 9.5.3 */
        INT8 ctime                      [ISODCL (11, 27)]; /* 8.4.26.1 */
        INT8 mtime                      [ISODCL (28, 44)]; /* 8.4.26.1 */
        INT8 xtime                      [ISODCL (45, 61)]; /* 8.4.26.1 */
        INT8 ftime                      [ISODCL (62, 78)]; /* 8.4.26.1 */
        INT8 recfmt                     [ISODCL (79, 79)]; /* 711 */
        INT8 recattr                    [ISODCL (80, 80)]; /* 711 */
        UINT8 reclen                   [ISODCL (81, 84)]; /* 723 */
        INT8 system_id                  [ISODCL (85, 116)]; /* achars */
        INT8 system_use                 [ISODCL (117, 180)];
        INT8 version                    [ISODCL (181, 181)]; /* 711 */
        INT8 len_esc                    [ISODCL (182, 182)]; /* 711 */
        INT8 reserved                   [ISODCL (183, 246)];
        UINT8 len_au                   [ISODCL (247, 250)]; /* 723 */
};

UINT8 isonum_711 (UINT8 *);
INT16 isonum_712 (INT8 *);
INT16 isonum_721 (UINT8 *);
INT16 isonum_722 (UINT8 *);
INT16 isonum_723 (UINT8 *);
INT16 isonum_731 (UINT8 *);
UINT32 isonum_732 (UINT8 *);
UINT32 isonum_733 (UINT8 *);

/*
 * Associated files have a leading '='.
 */
#define ASSOCCHAR       '='

#endif

//      End
