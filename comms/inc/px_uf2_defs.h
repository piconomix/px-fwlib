#ifndef __PX_UF2_DEFS_H__
#define __PX_UF2_DEFS_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_uf2_defs.h : PX_UF2 related definitions
    Author(s):      Pieter Conradie
    Creation Date:  2019-05-25

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

#ifdef __cplusplus
extern "C"
{
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
#define PX_UF2_MAGIC_START0            0x0a324655  // "PX_UF2\n"
#define PX_UF2_MAGIC_START1            0x9e5d5157  // Randomly selected
#define PX_UF2_MAGIC_END               0x0ab16f30  // Ditto

// If set, the block is "comment" and should not be flashed to the device
#define PX_UF2_FLAG_NOFLASH            0x00000001
#define PX_UF2_FLAG_FAMILYID_PRESENT   0x00002000

typedef struct
{
    // 32 byte header
    uint32_t magic_start0;
    uint32_t magic_start1;
    uint32_t flags;
    uint32_t target_addr;
    uint32_t payload_size;
    uint32_t block_nr;
    uint32_t nr_of_blocks;
    uint32_t family_id;

    // raw data;
    uint8_t data[476];

    // store magic also at the end to limit damage from partial block reads
    uint32_t magic_end;
} PX_ATTR_PACKED px_uf2_block_t;

typedef struct
{
    uint8_t  jmp_boot[3];                   // Jump instruction to boot code
    uint8_t  oem_name[8];                   // OEM Name Identifier
    uint16_t bytes_per_sector;              // Count of bytes per secto
    uint8_t  sectors_per_cluster;           // Number of sectors per allocation unit
    uint16_t res_sector_count;              // Number of sectors in the reserved region of the volume
    uint8_t  num_fats;                      // The count of file allocation tables (FATs) on the volume
    uint16_t root_dir_entry_count;          // The count of directory entries in the root directory
    uint16_t total_sectors_16;              // Old 16-bit total count of sectors on the volume
    uint8_t  media;                         // Specify media type (fixed, removable, ...)
    uint16_t fat_sectors_16;                // 16-bit count of sectors occupied by one FAT
    uint16_t sectors_per_track;             // Sector per track
    uint16_t num_of_heads;                  // Number of heads
    uint32_t hidden_sectors;                // Count of hidden sectors preceding the partition that contains this FAT volume
    uint32_t total_sectors_32;              // new 32-bit count of sectors on the volume
    uint8_t  drive_num;                     // Drive number
    uint8_t  reserved;
    uint8_t  boot_sig;                      // Extended boot signature
    uint32_t volume_id;                     // Volume serial number
    uint8_t  volume_label[11];              // Volume label
    uint8_t  file_system_type[8];           // "FAT12   ", "FAT16   " or "FAT     "
} PX_ATTR_PACKED px_uf2_fat_boot_block_t;

typedef struct
{
    char     name[8];
    char     ext[3];
    uint8_t  attrs;
    uint8_t  reserved;
    uint8_t  create_time_fine;
    uint16_t create_time;
    uint16_t create_date;
    uint16_t last_access_date;
    uint16_t high_start_cluster;
    uint16_t update_time;
    uint16_t update_date;
    uint16_t start_cluster;
    uint32_t size;
} PX_ATTR_PACKED px_uf2_fat_dir_entry_t;

#define PX_UF2_FAT_DIR_ATTR_READ_ONLY   0x01
#define PX_UF2_FAT_DIR_ATTR_HIDDEN      0x02
#define PX_UF2_FAT_DIR_ATTR_SYSTEM      0x04
#define PX_UF2_FAT_DIR_ATTR_VOLUME_ID   0x08
#define PX_UF2_FAT_DIR_ATTR_DIRECTORY   0x10
#define PX_UF2_FAT_DIR_ATTR_ARCHIVE     0x20

typedef struct
{
    const char   name[11];
    const char * content;
} px_uf2_file_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

#endif
