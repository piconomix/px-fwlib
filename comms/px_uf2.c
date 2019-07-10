/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_uf2.h : Microsoft UF2 bootloader over USB MSC (Mass Storage Class)
    Author(s):      Pieter Conradie
    Creation Date:  2019-05-25

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_uf2.h"
#include "px_uf2_defs.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("px_uf2");

/// Content of "info_uf2.txt"
static const char px_uf2_file_content_info[] =
    "UF2 Bootloader " PX_UF2_CFG_INFO_VERSION  "\r\n"
    "Model: "         PX_UF2_CFG_INFO_MODEL    "\r\n"
    "Board-ID: "      PX_UF2_CFG_INFO_BOARD_ID "\r\n";

// Content of "index.htm"
const char px_uf2_file_content_index_htm[] =
    "<!doctype html>\n"
    "<html>"
    "<body>"
    "<script>\n"
    "location.replace(\"" PX_UF2_CFG_INDEX_URL "\");\n"
    "</script>"
    "</body>"
    "</html>\n";

/// List of files in root directory
static const px_uf2_file_t px_uf2_file[] =
{
    {.name = "INFO_UF2TXT", .content = px_uf2_file_content_info},
    {.name = "INDEX   HTM", .content = px_uf2_file_content_index_htm},
    {.name = "CURRENT UF2", .content = NULL},
};

/// Number of media sectors (needs to be more than ~4200 to force FAT16)
#define PX_UF2_MEDIA_SECTORS            16000
/// Sector size is fixed at 512 bytes
#define PX_UF2_SECTOR_SIZE              512
/// Payload size is fixed at 256 bytes
#define PX_UF2_PAYLOAD_SIZE             256
/// Number of files in root directory
#define PX_UF2_FILES                    PX_SIZEOF_ARRAY(px_uf2_file)
/// Number of root directory entries (includes volume label as first entry)
#define PX_UF2_DIR_ENTRIES              (PX_UF2_FILES + 1)

/// UF2 file size in bytes (256 bytes of FLASH per UF2 block / sector)
#define PX_UF2_FILE_SIZE                (PX_UF2_CFG_FLASH_SIZE * 2)
/// UF2 file size in sectors
#define PX_UF2_FILE_SIZE_SECTORS        (PX_UF2_FILE_SIZE / PX_UF2_SECTOR_SIZE)
/// First sector of "current.uf2" file content; previous text file contents must fit in one sector each
#define PX_UF2_FIRST_SECTOR             (PX_UF2_FILES + 1)
/// Last sector of "current.uf2" file content
#define PX_UF2_LAST_SECTOR              (PX_UF2_FIRST_SECTOR + PX_UF2_FILE_SIZE_SECTORS - 1)

/// Number of FAT reserved sectors (contains boot block)
#define PX_UF2_RESERVED_SECTORS         1
/// Number of sectors per FAT
#define PX_UF2_SECTORS_PER_FAT          PX_UDIV_ROUNDUP(PX_UF2_MEDIA_SECTORS * 2, PX_UF2_SECTOR_SIZE)
/// Number of root directory sectors
#define PX_UF2_ROOT_DIR_SECTORS         4
/// Number of directory entries per sector
#define PX_UF2_DIR_ENTRIES_PER_SECTOR   (PX_UF2_SECTOR_SIZE / sizeof(px_uf2_fat_dir_entry_t))

/// FAT0 section starts after reserved sectors
#define PX_UF2_START_FAT0               PX_UF2_RESERVED_SECTORS
/// FAT1 section starts after FAT0
#define PX_UF2_START_FAT1               (PX_UF2_START_FAT0 + PX_UF2_SECTORS_PER_FAT)
/// Root directory region starts after FAT1
#define PX_UF2_START_ROOT_DIR           (PX_UF2_START_FAT1 + PX_UF2_SECTORS_PER_FAT)
/// File & directory data region start after root directory
#define PX_UF2_START_CLUSTERS           (PX_UF2_START_ROOT_DIR + PX_UF2_ROOT_DIR_SECTORS)


/// Content of FAT16 boot block in reserved section (first sector on media)
static const px_uf2_fat_boot_block_t px_uf2_fat_boot_block =
{
    .jmp_boot               = {0xeb, 0x3c, 0x90},
    .oem_name               = "UF2 UF2 ",
    .bytes_per_sector       = PX_UF2_SECTOR_SIZE,
    .sectors_per_cluster    = 1,
    .res_sector_count       = PX_UF2_RESERVED_SECTORS,
    .num_fats               = 2,
    .root_dir_entry_count   = PX_UF2_ROOT_DIR_SECTORS * PX_UF2_DIR_ENTRIES_PER_SECTOR,
    .total_sectors_16       = PX_UF2_MEDIA_SECTORS - 2,
    .media                  = 0xf0,
    .fat_sectors_16         = PX_UF2_SECTORS_PER_FAT,
    .sectors_per_track      = 1,
    .num_of_heads           = 1,
    .hidden_sectors         = 0,
    .total_sectors_32       = 0,
    .drive_num              = 0x80,
    .reserved               = 0,
    .boot_sig               = 0x29,
    .volume_id              = 0x00420042,
    .volume_label           = PX_UF2_CFG_VOLUME_LABEL,
    .file_system_type       = "FAT16   ",
};

#define UF2_MAX_BLOCKS (PX_UF2_CFG_FLASH_SIZE / 256 + 100)
typedef struct
{
    uint32_t nr_of_blocks;
    uint32_t num_written;
    uint8_t  written_mask[UF2_MAX_BLOCKS / 8 + 1];
} px_uf2_write_state_t;

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Pointer to function to handle write to flash
static px_uf2_on_wr_flash_block_t px_uf2_on_wr_flash_block;

/// Pointer to function called when last block has been written
static px_uf2_on_wr_flash_done_t px_uf2_on_wr_flash_done;

/// Structure to track state of UF2 blocks written to FLASH
static px_uf2_write_state_t px_uf2_write_state;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void px_uf2_padded_memcpy(char * dst, const char * src, int len)
{
    for(int i = 0; i < len; i++)
    {
        if(*src != 0)
        {
            *dst = *src++;
        }
        else
        {
            *dst = ' ';
        }
        dst++;
    }
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_uf2_init(px_uf2_on_wr_flash_block_t on_wr_flash_block,
                 px_uf2_on_wr_flash_done_t  on_wr_flash_done)
{
    px_uf2_on_wr_flash_block = on_wr_flash_block;
    px_uf2_on_wr_flash_done  = on_wr_flash_done;
}

void px_uf2_on_rd_sector(uint32_t sector_adr, uint8_t * buf)
{
    uint32_t section_index = sector_adr;

    // Clear buffer
    memset(buf, 0, PX_UF2_SECTOR_SIZE);

    // Read of boot sector in reserved section?
    if(sector_adr == 0)
    {
        // Return FAT16 boot sector info
        memcpy(buf, &px_uf2_fat_boot_block, sizeof(px_uf2_fat_boot_block));
        // Set signature word at the end of sector
        buf[510] = 0x55;
        buf[511] = 0xaa;
    }

    // Read of FAT table?
    else if(sector_adr < PX_UF2_START_ROOT_DIR)
    {
        // Adjust index to start of FAT0
        section_index -= PX_UF2_START_FAT0;
        // Index in FAT1?
        if(section_index >= PX_UF2_SECTORS_PER_FAT)
        {
            // FAT1 is the same as FAT0
            section_index -= PX_UF2_SECTORS_PER_FAT;
        }

        // Read of first sector in FAT?
        if(section_index == 0)
        {
            // FAT[0] contains the BPB_Media byte value in its low 8 bits, and all other bits are set to 1.
            // FAT[1] is set by the format utility to the EOC value (0xffff)
            // Mark 16-bit cluster entries of files as allocated and final (0xffff = end-of-file)
            // Rest of clusters are free (0x0000)
            buf[0] = px_uf2_fat_boot_block.media;
            for(int i = 1; i < (PX_UF2_FILES * 2 + 4); i++)
            {
                buf[i] = 0xff;
            }            
        }
        // Calculate each cluster entry in this FAT sector
        for(int i = 0; i < 256; i++)
        {
            uint32_t cluster = section_index * 256 + i;
            // Is this cluster part of UF2 file?
            if(  (cluster >= PX_UF2_FIRST_SECTOR)
               &&(cluster <= PX_UF2_LAST_SECTOR )  )
            {
                // Not last sector?
                if(cluster != PX_UF2_LAST_SECTOR)
                {
                    // Point cluster entry to next cluster (form singly linked list)
                    ((uint16_t *)buf)[i] = cluster + 1;
                }
                else
                {
                    // Mark last cluster as allocated and final (end-of-file)
                    ((uint16_t *)buf)[i] = 0xffff;
                }                
            }
        }        
    }

    // Read block from root directory?
    else if(sector_adr < PX_UF2_START_CLUSTERS)
    {
        // Adjust index to start of root directory
        section_index -= PX_UF2_START_ROOT_DIR;
        // First sector in root directory?
        if(section_index == 0)
        {
            px_uf2_fat_dir_entry_t * dir_entry = (px_uf2_fat_dir_entry_t *)buf;

            // Set first directory entry to volume label
            px_uf2_padded_memcpy(dir_entry->name, (char *)px_uf2_fat_boot_block.volume_label, 11);
            dir_entry->attrs = PX_UF2_FAT_DIR_ATTR_VOLUME_ID | PX_UF2_FAT_DIR_ATTR_ARCHIVE;
            // Create root directory entries from table
            for(int i = 0; i < PX_UF2_FILES; i++)
            {
                const px_uf2_file_t * file = &px_uf2_file[i];
                dir_entry++;

                if(file->content != NULL)
                {
                    dir_entry->size = strlen(file->content);
                }
                else
                {
                    dir_entry->size = PX_UF2_FILE_SIZE;
                }
                dir_entry->start_cluster = i + 2;
                px_uf2_padded_memcpy(dir_entry->name, file->name, 11);
                dir_entry->create_date = 0x4d99;
                dir_entry->update_date = 0x4d99;
            }
        }
    }

    // Read from file & directory data region
    else
    {
        // Adjust index to start of region
        section_index -= PX_UF2_START_CLUSTERS;
        // Each file occupies exactly one sector
        if(section_index < PX_UF2_FILES - 1)
        {
            // Return file content
            memcpy(buf,
                   px_uf2_file[section_index].content, 
                   strlen(px_uf2_file[section_index].content));
        }
        else
        {
            // Return UF2 file content
            section_index -= PX_UF2_FILES - 1;
            uint32_t addr  = section_index * 256;
            if (addr < PX_UF2_CFG_FLASH_SIZE)
            {
                // Fill UF2 fields with section of FLASH
                px_uf2_block_t *bl = (void *)buf;

                bl->magic_start0  = PX_UF2_MAGIC_START0;
                bl->magic_start1  = PX_UF2_MAGIC_START1;
                bl->flags         = 0;
                bl->target_addr   = addr;
                bl->payload_size  = 256;
                bl->block_nr      = section_index;
                bl->nr_of_blocks  = PX_UF2_CFG_FLASH_SIZE / 256;
                bl->flags        |= PX_UF2_FLAG_FAMILYID_PRESENT;
                bl->family_id     = PX_UF2_CFG_FAMILY_ID;
                memcpy(bl->data, (void *)addr, bl->payload_size);
                bl->magic_end     = PX_UF2_MAGIC_END;
            }
        }
    }
}

void px_uf2_on_wr_sector(uint32_t sector_adr, const uint8_t * buf)
{
    px_uf2_block_t *bl = (void *)buf;

    // Valid UF2 magic markers?
    if(  (bl->magic_start0 != PX_UF2_MAGIC_START0)
       ||(bl->magic_start1 != PX_UF2_MAGIC_START1)
       ||(bl->magic_end    != PX_UF2_MAGIC_END   )  )                
    {
        // No. Ignore
        return;
    }

    // Correct Family ID present?
    if(  ((bl->flags & PX_UF2_FLAG_FAMILYID_PRESENT) == 0)
       ||(bl->family_id != PX_UF2_CFG_FAMILY_ID          )  )
    {
        // No. Ignore
        return;
    }

    // Valid UF2 flash address and payload size?
    if(  (bl->flags        &  PX_UF2_FLAG_NOFLASH       )
       ||(bl->payload_size != PX_UF2_PAYLOAD_SIZE       )
       ||(bl->target_addr  &  (PX_UF2_PAYLOAD_SIZE - 1) )
       ||(bl->target_addr  <  PX_UF2_CFG_FLASH_START_ADR)
       ||(bl->target_addr  >= PX_UF2_CFG_FLASH_SIZE     )  )
    {
        // this happens when we're trying to re-flash "current.uf2" file previously
        // copied from a device; we still want to count these blocks to reset properly
        PX_DBG_ERR("Invalid target address 0x%08X", bl->target_addr);
    }
    else
    {
        PX_DBG_INFO("Write 0x%08X %u bytes", bl->target_addr, bl->payload_size);
        (*px_uf2_on_wr_flash_block)(bl->data, bl->target_addr, bl->payload_size);
    }

    if(bl->nr_of_blocks == 0)
    {
        return;
    }

    // Is this the first block?
    if(px_uf2_write_state.nr_of_blocks != bl->nr_of_blocks)
    {
        if(bl->nr_of_blocks >= UF2_MAX_BLOCKS || px_uf2_write_state.nr_of_blocks)
        {
            // Reset number of blocks to max
            px_uf2_write_state.nr_of_blocks = 0xffffffff;
        }
        else
        {
            // Save number of blocks to write
            px_uf2_write_state.nr_of_blocks = bl->nr_of_blocks;
        }
    }

    // Valid block?
    if(bl->block_nr < UF2_MAX_BLOCKS)
    {
        uint32_t pos = bl->block_nr / 8;
        uint8_t mask = 1 << (bl->block_nr % 8);        

        // Has block been written before?
        if(!(px_uf2_write_state.written_mask[pos] & mask))
        {
            // Set bit to indicate that block has been written
            px_uf2_write_state.written_mask[pos] |= mask;
            // Increment count of blocks written
            px_uf2_write_state.num_written++;
            // Have all blocks been written?
            if(px_uf2_write_state.num_written >= px_uf2_write_state.nr_of_blocks)
            {
                // Signal that las block has been written
                PX_DBG_INFO("Write done");
                (*px_uf2_on_wr_flash_done)();
            }
        }        
    }
}

