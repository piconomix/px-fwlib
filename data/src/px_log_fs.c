/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          px_log_fs.h : Record-based file system for Serial Flash
    Author(s):      Pieter Conradie
    Creation Date:  2019-09-09

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>
#include <stddef.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_log_fs.h"
#include "px_log_fs_glue.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("px_log_fs");

/// Invalid page value
#define PX_LOG_FS_PAGE_INVALID      0xffff

/// @name Marker values
//@{
#define PX_LOG_FS_MARKER_FREE       0xff        /* 1111_1111b */
#define PX_LOG_FS_MARKER_PAGE       0x5f        /* 0101_1111b */
#define PX_LOG_FS_MARKER_PAGE_A     0x55        /* 0101_0101b */
#define PX_LOG_FS_MARKER_RECORD     0xaf        /* 1010_1111b */
#define PX_LOG_FS_MARKER_RECORD_A   0xaa        /* 1010_1010b */
#define PX_LOG_FS_MARKER_BAD        0x00        /* 0000_0000b */
//@}

/// Definition of header structure that is stored at the start of each page
typedef struct
{
    uint8_t  marker;                            ///< FREE, PAGE, PAGE_A or BAD
    uint16_t nr;                                ///< Rolling number to find first and last marked page
    uint8_t  crc;                               ///< Checksum calculated over nr (excludes marker)
} PX_ATTR_PACKED px_log_fs_header_t;

/// Definition of a record structure
typedef struct
{
    uint8_t marker;                             ///< RECORD or RECORD_A
    uint8_t data[PX_LOG_FS_CFG_REC_DATA_SIZE];  ///< Record data content
    uint8_t crc;                                ///< Checksum calculated over data (excludes marker)
} PX_ATTR_PACKED px_log_fs_record_t;

/// Page data size (page size - page header)
#define PX_LOG_FS_PAGE_DATA_SIZE    (PX_LOG_FS_CFG_PAGE_SIZE - sizeof(px_log_fs_header_t))
/// Records per page
#define PX_LOG_FS_RECORDS_PER_PAGE  (PX_LOG_FS_PAGE_DATA_SIZE / sizeof(px_log_fs_record_t))
/// First record offset
#define PX_LOG_FS_REC_OFFSET_FIRST  (sizeof(px_log_fs_header_t))
/// Last record offset
#define PX_LOG_FS_REC_OFFSET_LAST   (sizeof(px_log_fs_header_t) + (PX_LOG_FS_RECORDS_PER_PAGE - 1) * sizeof(px_log_fs_record_t))

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static inline bool px_log_fs_marker_is_free(uint8_t marker)
{
    if(marker == PX_LOG_FS_MARKER_FREE)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static inline bool px_log_fs_marker_is_page(uint8_t marker)
{
    if(  (marker == PX_LOG_FS_MARKER_PAGE  )
       ||(marker == PX_LOG_FS_MARKER_PAGE_A)  )
    {
        return true;
    }
    else
    {
        return false;
    }
}

static inline bool px_log_fs_marker_is_record(uint8_t marker)
{
    if(  (marker == PX_LOG_FS_MARKER_RECORD  )
       ||(marker == PX_LOG_FS_MARKER_RECORD_A)  )
    {
        return true;
    }
    else
    {
        return false;
    }
}

static inline bool px_log_fs_marker_is_bad(uint8_t marker)
{
    if(marker == PX_LOG_FS_MARKER_BAD)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static uint16_t px_log_fs_page_next(const px_log_fs_handle_t * handle,
                                    uint16_t                   page)
{
    // End?
    if(page == handle->fs_page_end)
    {
        // Wrap to start
        return handle->fs_page_start;
    }
    else
    {
        // Return next page number
        return ++page;
    }
}

static uint16_t px_log_fs_page_previous(const px_log_fs_handle_t * handle,
                                        uint16_t                   page)
{
    // Start?
    if(page == handle->fs_page_start)
    {
        // Wrap to end
        return handle->fs_page_end;
    }
    else
    {
        // Return previous page number
        return --page;
    }
}

static void px_log_fs_record_adr_next(const px_log_fs_handle_t * handle,
                                      px_log_fs_adr_t *          adr)
{
    // Last record on page?
    if(adr->offset == PX_LOG_FS_REC_OFFSET_LAST)
    {
        // First record on next page
        adr->page = px_log_fs_page_next(handle, adr->page);
        adr->offset = PX_LOG_FS_REC_OFFSET_FIRST;
    }
    else
    {
        // Next record
        adr->offset += sizeof(px_log_fs_record_t);
    }
}

static void px_log_fs_record_adr_previous(const px_log_fs_handle_t * handle,
                                          px_log_fs_adr_t *          adr)
{
    // First record on page?
    if(adr->offset == PX_LOG_FS_REC_OFFSET_FIRST)
    {
        // Last record on previous page
        adr->page = px_log_fs_page_previous(handle, adr->page);
        adr->offset = PX_LOG_FS_REC_OFFSET_LAST;
    }
    else
    {
        // Previous record
        adr->offset -= sizeof(px_log_fs_record_t);
    }
}

static uint8_t px_log_fs_crc(const void * data, size_t nr_of_bytes)
{
    uint8_t   i;
    uint8_t * data_u8 = (uint8_t *)data;
    uint8_t   crc = 0xff;

    // http://www.ece.cmu.edu/~koopman/roses/dsn04/koopman04_crc_poly_embedded.pdf
    // width=8 poly=0x4d init=0xff refin=true refout=true xorout=0xff check=0xd8
    // name="CRC-8/KOOP"
    
    // Repeat until all the data bytes have been processed...
    while(nr_of_bytes != 0)
    {
        nr_of_bytes--;

        // XOR CRC with 8-bit data
        crc = crc ^ (*data_u8++);

        // Repeat 8 times (for each bit)
        for(i=8; i!=0; i--)
        {
            // Is lowest bit set?
            if((crc & 1) != 0)
            {
                // Shift right and XOR with reverse of polynomial x^8 + x^6 + x^3 + x^2 + x^0
                crc = (crc >> 1) ^ 0xb2;
            }
            else
            {
                // Shift right
                crc = (crc >> 1);
            }
        }
    }
    return crc;
}

static uint8_t px_log_fs_marker_rd(const px_log_fs_adr_t * adr)
{
    uint8_t marker_rd;

    px_log_fs_glue_rd(&marker_rd, adr->page, adr->offset, sizeof(marker_rd));

    return marker_rd;
}

static bool px_log_fs_marker_wr(uint8_t                 marker,
                                const px_log_fs_adr_t * adr)
{
    uint8_t marker_wr = marker;
    uint8_t marker_rd;

    px_log_fs_glue_wr(&marker_wr, adr->page, adr->offset, sizeof(marker_wr));
    px_log_fs_glue_rd(&marker_rd, adr->page, adr->offset, sizeof(marker_rd));
    if(marker_rd == marker_wr)
    {
        // Success
        return true;
    }
    else
    {
        // Failure
        return false;
    }
}

static uint8_t px_log_fs_header_rd(px_log_fs_header_t * header, uint16_t page)
{
    uint8_t crc;

    // Read page header
    px_log_fs_glue_rd(header,
                      page,
                      0,
                      sizeof(px_log_fs_header_t));
    // Page FREE or BAD?
    if(    px_log_fs_marker_is_free(header->marker)
        || px_log_fs_marker_is_bad(header->marker )  )
    {
        return header->marker;
    }
    // Invalid marker?
    if(!px_log_fs_marker_is_page(header->marker))
    {
        // Mark as BAD
        PX_LOG_E("Invalid marker 0x%02X @ page %u", header->marker, page);
        header->marker = PX_LOG_FS_MARKER_BAD;
        px_log_fs_glue_wr(&header->marker, page, 0, 1);
        return header->marker;
    }
    // Check CRC
    crc = px_log_fs_crc(&header->nr, sizeof(header->nr));
    if(crc != header->crc)
    {
        // Mark as BAD
        PX_LOG_E("Header CRC check failed @ page %u (0x%02X != 0x%02X)",
                   page, header->crc, crc);
        header->marker = PX_LOG_FS_MARKER_BAD;
        px_log_fs_glue_wr(&header->marker, page, 0, 1);
        return header->marker;
    }
    return header->marker;
}

static uint16_t px_log_fs_header_rd_first(const px_log_fs_handle_t * handle,
                                          px_log_fs_header_t *       header)
{
    uint8_t  marker;
    uint16_t page;

    // Start at first page
    page = handle->fs_page_start;
    while(true)
    {
        // Read page header. Is it marked?
        marker = px_log_fs_header_rd(header, page);
        if(px_log_fs_marker_is_page(marker))
        {
            // Marked page found
            return page;
        }
        // Last page read?
        if(page == handle->fs_page_end)
        {
            // Marker not found
            return PX_LOG_FS_PAGE_INVALID;
        }
        // Next page
        page = px_log_fs_page_next(handle, page);
    }
}

static uint16_t px_log_fs_header_rd_next(const px_log_fs_handle_t * handle,
                                         px_log_fs_header_t *       header,
                                         uint16_t                   page_current)
{
    uint8_t  marker;
    uint16_t page;

    // Start at current page
    page = page_current;
    while(true)
    {
        // Next page
        page = px_log_fs_page_next(handle, page);
        // Wrapped all the way around and now back at current page?
        if(page == page_current)
        {
            // Marker not found
            return PX_LOG_FS_PAGE_INVALID;
        }
        // Read page header. Is it a marked page?
        marker = px_log_fs_header_rd(header, page);
        if(px_log_fs_marker_is_page(marker))
        {
            // Marked page found
            return page;
        }
    }
}

static bool px_log_fs_header_wr(px_log_fs_header_t * header, uint16_t page)
{
    px_log_fs_header_t header_rd;

    // Set CRC
    header->crc = px_log_fs_crc(&header->nr, sizeof(header->nr));
    // Write header
    px_log_fs_glue_wr(header, page, 0, sizeof(*header));
    // Read back header
    px_log_fs_glue_rd(&header_rd, page, 0, sizeof(*header));
    // Match?
    if(memcmp(header, &header_rd, sizeof(*header)) != 0)
    {
        // Mark header as BAD
        PX_LOG_E("Header write failed @ page %u", page);
        header->marker = PX_LOG_FS_MARKER_BAD;
        px_log_fs_glue_wr(&header->marker, page, 0, 1);
        // Failure
        return false;
    }
    // Success
    return true;
}

static uint8_t px_log_fs_record_rd(px_log_fs_record_t *    record,
                                   const px_log_fs_adr_t * adr)
{
    uint8_t crc;

    // Read record
    px_log_fs_glue_rd(record, adr->page, adr->offset, sizeof(*record));
    // FREE or BAD?
    if(    px_log_fs_marker_is_free(record->marker)
        || px_log_fs_marker_is_bad (record->marker)  )
    {
        return record->marker;
    }
    // Marker correct?
    if(!px_log_fs_marker_is_record(record->marker))
    {
        // No. Mark as BAD
        PX_LOG_W("Invalid record marker %02X @ page %u offset %u",
                    record->marker, adr->page, adr->offset);
        record->marker = PX_LOG_FS_MARKER_BAD;
        px_log_fs_glue_wr(&record->marker, adr->page, adr->offset, 1);
        return record->marker;
    }
    // CRC correct?
    crc = px_log_fs_crc(&record->data, sizeof(record->data));
    if(crc != record->crc)
    {
        // Mark record as BAD
        PX_LOG_E("Record CRC check failed @ page %u offset %u (0x%02X != 0x%02X)",
                   adr->page, adr->offset, record->crc != crc);
        record->marker = PX_LOG_FS_MARKER_BAD;
        px_log_fs_glue_wr(&record->marker, adr->page, adr->offset, 1);
        return record->marker;
    }
    // Record is good
    return record->marker;
}

static bool px_log_fs_record_wr(px_log_fs_record_t *    record,
                                const px_log_fs_adr_t * adr)
{
    px_log_fs_record_t record_rd;

    // Sanity check
    if(adr->offset + sizeof(px_log_fs_record_t) > PX_LOG_FS_CFG_PAGE_SIZE)
    {
        PX_LOG_E("Record will overflow the page");
        return false;
    }

    // Set marker
    record->marker = PX_LOG_FS_MARKER_RECORD;
    // Set CRC
    record->crc = px_log_fs_crc(&record->data, sizeof(record->data));
    // Write record
    px_log_fs_glue_wr(record, adr->page, adr->offset, sizeof(*record));
    // Read back record
    px_log_fs_glue_rd(&record_rd, adr->page, adr->offset, sizeof(*record));
    // Match?
    if(memcmp(record, &record_rd, sizeof(*record)) != 0)
    {
        // Mark record as BAD
        PX_LOG_E("Record write failed @ page %u offset %u",
                   adr->page, adr->offset);
        record->marker = PX_LOG_FS_MARKER_BAD;
        px_log_fs_glue_wr(&record->marker, adr->page, adr->offset, 1);
        // Failure
        return false;
    }
    // Success
    return true;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
px_log_fs_err_t px_log_fs_init(px_log_fs_handle_t * handle,
                               uint16_t             fs_page_start,
                               uint16_t             fs_page_end)
{
    px_log_fs_header_t header;
    px_log_fs_record_t record;
    uint16_t           page_first;
    uint16_t           page;
    uint16_t           page_next;
    uint16_t           page_nr;
    uint16_t           page_nr_next;
    uint16_t           page_nr_diff;
    uint16_t           page_nr_diff_largest = 0;

    // Sanity checks
    PX_LOG_ASSERT(fs_page_start != PX_LOG_FS_PAGE_INVALID);                                     // Must not be reserved invalid value
    PX_LOG_ASSERT(fs_page_end   != PX_LOG_FS_PAGE_INVALID);                                     // Must not be reserved invalid value
    PX_LOG_ASSERT((fs_page_start % PX_LOG_FS_CFG_ERASE_BLOCK_SIZE)  == 0);                      // Must be at the start of an erase block
    PX_LOG_ASSERT(((fs_page_end + 1) % PX_LOG_FS_CFG_ERASE_BLOCK_SIZE)  == 0);                  // Must be at the end of an erase block
    PX_LOG_ASSERT(((fs_page_end + 1 - fs_page_start) / PX_LOG_FS_CFG_ERASE_BLOCK_SIZE) >= 2);   // Must be at least 2 erase blocks in size (more is better)
    PX_LOG_ASSERT(sizeof(px_log_fs_record_t) <= PX_LOG_FS_PAGE_DATA_SIZE);                      // A record must fit in a page
    if(PX_LOG_FS_PAGE_DATA_SIZE % sizeof(px_log_fs_record_t) != 0)
    {
        PX_LOG_W("%u bytes will be wasted per page", PX_LOG_FS_PAGE_DATA_SIZE % sizeof(px_log_fs_record_t));
    }

    // Save file system start and end page
    handle->fs_page_start = fs_page_start;
    handle->fs_page_end   = fs_page_end;
    // Read first marked page
    page_first = px_log_fs_header_rd_first(handle, &header);
    if(page_first == PX_LOG_FS_PAGE_INVALID)
    {
        // None found. File system is empty
        PX_LOG_I("No marked pages found");
        handle->page_first    = PX_LOG_FS_PAGE_INVALID;
        handle->page_last     = PX_LOG_FS_PAGE_INVALID;
        handle->page_nr_next  = 0;
        handle->adr_wr.page   = handle->fs_page_start;
        handle->adr_wr.offset = PX_LOG_FS_REC_OFFSET_FIRST;
        return PX_LOG_FS_ERR_NONE;
    }
    // Continue from first marked page...
    page = page_first;
    do
    {
        // Save page number
        page_nr = header.nr;
        // Read next marked page
        page_next = px_log_fs_header_rd_next(handle, &header, page);
        if(page_next == PX_LOG_FS_PAGE_INVALID)
        {
            // Only one marked page found
            handle->page_first   = page;
            handle->page_last    = page;
            handle->page_nr_next = ++page_nr;
            break;
        }
        // Calculate difference
        page_nr_next = header.nr;
        page_nr_diff = page_nr_next - page_nr;
        // Largest difference so far?
        if(page_nr_diff_largest < page_nr_diff)
        {
            PX_LOG_I("Diff=%u, page %u (nr %u) , page_next %u (nr %u)",
                     page_nr_diff, page, page_nr, page_next, page_nr_next);
            // Save largest difference so far
            page_nr_diff_largest = page_nr_diff;
            // Save candidates for first and last marked page
            handle->page_first   = page_next;
            handle->page_last    = page;
            handle->page_nr_next = ++page_nr;
        }
        // Next page
        page = page_next;
    }
    while(page != page_first); // Finished?

    // Find first FREE record
    handle->adr_wr.page   = handle->page_last;
    handle->adr_wr.offset = PX_LOG_FS_REC_OFFSET_FIRST;
    while(!px_log_fs_marker_is_free(px_log_fs_record_rd(&record, &handle->adr_wr)))
    {
        px_log_fs_record_adr_next(handle, &handle->adr_wr);
        if(handle->adr_wr.offset == PX_LOG_FS_REC_OFFSET_FIRST)
        {
            break;
        }
    }

    // Report first and last page
    PX_LOG_I("First marked page %u", handle->page_first);
    PX_LOG_I("Last marked page %u",  handle->page_last);
    PX_LOG_I("Next page nr %u",      handle->page_nr_next);

    return PX_LOG_FS_ERR_NONE;
}

px_log_fs_err_t px_log_fs_reset(px_log_fs_handle_t * handle,
                                uint16_t             fs_page_start,
                                uint16_t             fs_page_end)
{
    px_log_fs_header_t header;
    uint16_t           page;
    uint8_t            marker;

    // Sanity checks
    PX_LOG_ASSERT(fs_page_start != PX_LOG_FS_PAGE_INVALID);                                     // Must not be reserved invalid value
    PX_LOG_ASSERT(fs_page_end   != PX_LOG_FS_PAGE_INVALID);                                     // Must not be reserved invalid value
    PX_LOG_ASSERT((fs_page_start % PX_LOG_FS_CFG_ERASE_BLOCK_SIZE)  == 0);                      // Must be at the start of an erase block
    PX_LOG_ASSERT(((fs_page_end + 1) % PX_LOG_FS_CFG_ERASE_BLOCK_SIZE)  == 0);                  // Must be at the end of an erase block
    PX_LOG_ASSERT(((fs_page_end + 1 - fs_page_start) / PX_LOG_FS_CFG_ERASE_BLOCK_SIZE) >= 2);   // Must be at least 2 erase blocks in size (more is better)
    PX_LOG_ASSERT(sizeof(px_log_fs_record_t) <= PX_LOG_FS_PAGE_DATA_SIZE);                      // A record must fit in a page
    if(PX_LOG_FS_PAGE_DATA_SIZE % sizeof(px_log_fs_record_t) != 0)
    {
        PX_LOG_W("%u bytes will be wasted per page", PX_LOG_FS_PAGE_DATA_SIZE % sizeof(px_log_fs_record_t));
    }

    // Save file system start and end page
    handle->fs_page_start = fs_page_start;
    handle->fs_page_end   = fs_page_end;

    // Mark all used pages as BAD
    for(page = fs_page_start; page <= fs_page_end; page++)
    {
        marker = px_log_fs_header_rd(&header, page);
        if(px_log_fs_marker_is_page(marker))
        {
            header.marker = PX_LOG_FS_MARKER_BAD;
            px_log_fs_glue_wr(&header.marker, page, 0, 1);
        }
    }
    // Find first FREE page on the start of an erase block
    for(page = fs_page_start; page <= fs_page_end; page += PX_LOG_FS_CFG_ERASE_BLOCK_SIZE)
    {
        if(px_log_fs_marker_is_free(px_log_fs_header_rd(&header, page)))
        {
            break;
        }
    }
    // No FREE page found?
    if(page > fs_page_end)
    {
        // Start at beginning
        page = fs_page_start;
    }
    // Reset fields
    handle->page_first    = PX_LOG_FS_PAGE_INVALID;
    handle->page_last     = PX_LOG_FS_PAGE_INVALID;
    handle->page_nr_next  = 0;
    handle->adr_wr.page   = page;
    handle->adr_wr.offset = PX_LOG_FS_REC_OFFSET_FIRST;
    return PX_LOG_FS_ERR_NONE;
}

px_log_fs_err_t px_log_fs_rd_first(px_log_fs_handle_t * handle,
                                   void *               data,
                                   size_t               nr_of_bytes)
{
    // No marked pages?
    if(handle->page_first == PX_LOG_FS_PAGE_INVALID)
    {
        return PX_LOG_FS_ERR_EMPTY;
    }
    // Start before first record
    handle->adr_rd.page   = handle->page_first;
    handle->adr_rd.offset = PX_LOG_FS_REC_OFFSET_FIRST;
    px_log_fs_record_adr_previous(handle, &handle->adr_rd);
    // Return first valid record
    return px_log_fs_rd_next(handle, data, nr_of_bytes);
}

px_log_fs_err_t px_log_fs_rd_first_unarchived(px_log_fs_handle_t * handle,
                                              void *               data,
                                              size_t               nr_of_bytes)
{
    px_log_fs_header_t header;
    uint8_t            marker;

    // No marked pages?
    if(handle->page_first == PX_LOG_FS_PAGE_INVALID)
    {
        return PX_LOG_FS_ERR_EMPTY;
    }
    // Find first unarchived page with records
    handle->adr_rd.page   = handle->page_first;
    handle->adr_rd.offset = PX_LOG_FS_REC_OFFSET_FIRST;
    while(handle->adr_rd.page != handle->adr_wr.page)
    {
        // Read page marker
        marker = px_log_fs_header_rd(&header, handle->adr_rd.page);
        // First unarchived page?
        if(marker == PX_LOG_FS_MARKER_PAGE)
        {
            // First unarchived page found. Stop
            break;
        }
        else if(marker == PX_LOG_FS_MARKER_FREE)
        {
            PX_LOG_E("Unexpected free page found");
            return PX_LOG_FS_ERR_FATAL;
        }
        // Next page
        handle->adr_rd.page = px_log_fs_page_next(handle, handle->adr_rd.page);
    }
    // Start before first record
    px_log_fs_record_adr_previous(handle, &handle->adr_rd);
    // Find first unarchived record
    do
    {
        // Read next record
        if(px_log_fs_rd_next(handle, data, nr_of_bytes) != PX_LOG_FS_ERR_NONE)
        {
            // No unarchived records found
            return PX_LOG_FS_ERR_NO_RECORD;
        }
    }
    while(handle->archive_flag);
}

px_log_fs_err_t px_log_fs_rd_next(px_log_fs_handle_t * handle,
                                  void *               data,
                                  size_t               nr_of_bytes)
{
    px_log_fs_header_t header;
    px_log_fs_record_t record;
    uint8_t            marker;

    // More bytes requested than can be stored in record?
    if(nr_of_bytes > PX_LOG_FS_CFG_REC_DATA_SIZE)
    {
        PX_LOG_E("More bytes requested than can be stored in a record");
        // Clip number of bytes that will be copied
        nr_of_bytes = PX_LOG_FS_CFG_REC_DATA_SIZE;
    }

    // Find next valid record
    while(true)
    {
        // Next record address
        px_log_fs_record_adr_next(handle, &handle->adr_rd);
        // Has last record been read?
        if(  (handle->adr_rd.page   == handle->adr_wr.page  )
           &&(handle->adr_rd.offset == handle->adr_wr.offset)  )
        {
            // Stop
            px_log_fs_record_adr_previous(handle, &handle->adr_rd);
            return PX_LOG_FS_ERR_NO_RECORD;
        }
        // Next page?
        if(handle->adr_rd.offset == PX_LOG_FS_REC_OFFSET_FIRST)
        {
            // Find next marked page
            while(!px_log_fs_marker_is_page(px_log_fs_header_rd(&header, handle->adr_rd.page)))
            {
                if(handle->adr_rd.page == handle->page_last)
                {
                    PX_LOG_E("File system must be corrupt");
                    return PX_LOG_FS_ERR_FATAL;
                }
                handle->adr_rd.page = px_log_fs_page_next(handle, handle->adr_rd.page);
            }
        }
        // Read record
        marker = px_log_fs_record_rd(&record, &handle->adr_rd);
        if(px_log_fs_marker_is_record(marker))
        {
            // Archived?
            if(marker == PX_LOG_FS_MARKER_RECORD_A)
            {
                handle->archive_flag = true;
            }
            else
            {
                handle->archive_flag = false;
            }
            // Copy content of record to user supplied buffer
            memcpy(data, &record.data, nr_of_bytes);
            // Valid record
            return PX_LOG_FS_ERR_NONE;
        }
    }
}

px_log_fs_err_t px_log_fs_rd_last(px_log_fs_handle_t * handle,
                                  void *               data,
                                  size_t               nr_of_bytes)
{
    // No marked pages?
    if(handle->page_first == PX_LOG_FS_PAGE_INVALID)
    {
        return PX_LOG_FS_ERR_EMPTY;
    }
    // Start after last record
    handle->adr_rd.page   = handle->adr_wr.page;
    handle->adr_rd.offset = handle->adr_wr.offset;
    // Return last valid record
    return px_log_fs_rd_previous(handle, data, nr_of_bytes);
}

px_log_fs_err_t px_log_fs_rd_previous(px_log_fs_handle_t * handle,
                                      void *               data,
                                      size_t               nr_of_bytes)
{
    px_log_fs_header_t header;
    px_log_fs_record_t record;
    uint8_t            marker;

    // More bytes requested than can be stored in record?
    if(nr_of_bytes > PX_LOG_FS_CFG_REC_DATA_SIZE)
    {
        PX_LOG_E("More bytes requested than can be stored in a record");
        // Clip number of bytes that will be copied
        nr_of_bytes = PX_LOG_FS_CFG_REC_DATA_SIZE;
    }

    // Find previous valid record
    while(true)
    {
        // Has first record been read?
        if(  (handle->adr_rd.page   == handle->page_first        )
           &&(handle->adr_rd.offset == PX_LOG_FS_REC_OFFSET_FIRST)  )
        {
            // Stop
            return PX_LOG_FS_ERR_NO_RECORD;
        }
        // Previous record address
        px_log_fs_record_adr_previous(handle, &handle->adr_rd);
        // Previous page?
        if(handle->adr_rd.offset == PX_LOG_FS_REC_OFFSET_LAST)
        {
            // Find previous marked page
            while(!px_log_fs_marker_is_page(px_log_fs_header_rd(&header, handle->adr_rd.page)))
            {
                if(handle->adr_rd.page == handle->page_first)
                {
                    PX_LOG_E("File system must be corrupt");
                    return PX_LOG_FS_ERR_FATAL;
                }
                handle->adr_rd.page = px_log_fs_page_previous(handle, handle->adr_rd.page);
            }
        }
        // Read record
        marker = px_log_fs_record_rd(&record, &handle->adr_rd);
        if(px_log_fs_marker_is_record(marker))
        {
            // Archived?
            if(marker == PX_LOG_FS_MARKER_RECORD_A)
            {
                handle->archive_flag = true;
            }
            else
            {
                handle->archive_flag = false;
            }
            // Copy content of record to user supplied buffer
            memcpy(data, &record.data, nr_of_bytes);
            // Valid record
            return PX_LOG_FS_ERR_NONE;
        }
    }
}

px_log_fs_err_t px_log_fs_rd_rec_set_archive(px_log_fs_handle_t * handle)
{
    uint8_t marker;
    px_log_fs_adr_t adr;

    // Sanity check
    if(handle->archive_flag)
    {
        PX_LOG_W("Record is already archived");
        return PX_LOG_FS_ERR_NONE;
    }
    // Read record marker first (sanity check)
    marker = px_log_fs_marker_rd(&handle->adr_rd);
    if(marker == PX_LOG_FS_MARKER_RECORD_A)
    {
        PX_LOG_E("Record is already archived but archive flag was not set");
        return PX_LOG_FS_ERR_NONE;
    }
    else if(marker != PX_LOG_FS_MARKER_RECORD)
    {
        PX_LOG_E("Invalid marker");
        return PX_LOG_FS_ERR_WRITE_FAIL;
    }
    // Write record marker
    if(!px_log_fs_marker_wr(PX_LOG_FS_MARKER_RECORD_A, &handle->adr_rd))
    {
        PX_LOG_E("Failed to write archive marker");
        return PX_LOG_FS_ERR_WRITE_FAIL;
    }
    // Is this the last record in the page?
    adr.page   = handle->adr_rd.page;
    adr.offset = handle->adr_rd.offset;
    while(true)
    {
        // Next record address
        px_log_fs_record_adr_next(handle, &adr);
        // Next page?
        if(adr.offset == PX_LOG_FS_REC_OFFSET_FIRST)
        {
            // This means that last unarchived record on page has been marked as archived.
            // Mark whole page as archived.
            adr.page   = handle->adr_rd.page;
            adr.offset = 0;
            if(!px_log_fs_marker_wr(PX_LOG_FS_MARKER_PAGE_A, &adr))
            {
                PX_LOG_E("Failed to mark page as archived. Lost whole page of records!");
                return PX_LOG_FS_ERR_WRITE_FAIL;
            }
            // Success
            return PX_LOG_FS_ERR_NONE;
        }
        // Has last record been checked?
        if(  (adr.page   == handle->adr_wr.page  )
           &&(adr.offset == handle->adr_wr.offset)  )
        {
            // Do nothing
            return PX_LOG_FS_ERR_NONE;
        }
        // Read record marker
        marker = px_log_fs_marker_rd(&adr);
        if(marker == PX_LOG_FS_MARKER_RECORD)
        {
            // Do nothing. There are still unarchived records in page.
            return PX_LOG_FS_ERR_NONE;
        }
        else if(marker == PX_LOG_FS_MARKER_RECORD_A)
        {
            // Error! Records must be archived from oldest to newest
            PX_LOG_E("Out of sequence archived record detected");
            return PX_LOG_FS_ERR_NONE;
        }
        else if(marker == PX_LOG_FS_MARKER_FREE)
        {
            // Do nothing. There is still free space in page.
            return PX_LOG_FS_ERR_NONE;
        }
    }
}

px_log_fs_err_t px_log_fs_wr(px_log_fs_handle_t * handle,
                             const void *         data,
                             size_t               nr_of_bytes)
{
    px_log_fs_header_t header;
    px_log_fs_record_t record;
    uint16_t           page;
    uint8_t            marker;

    // Record too small?
    if(nr_of_bytes > PX_LOG_FS_CFG_REC_DATA_SIZE)
    {
        PX_LOG_E("Record size too small and %u bytes will be discarded",
                (nr_of_bytes - PX_LOG_FS_CFG_REC_DATA_SIZE));
        // Clip number of bytes that will be copied
        nr_of_bytes = PX_LOG_FS_CFG_REC_DATA_SIZE;
    }
    // Set unused data to 0xFF
    memset(&record.data, 0xff, sizeof(record.data));
    // Copy data
    memcpy(&record.data, data, nr_of_bytes);

    // New page?
    if(handle->adr_wr.offset == PX_LOG_FS_REC_OFFSET_FIRST)
    {
        // Find next FREE page
        while(!px_log_fs_marker_is_free(px_log_fs_header_rd(&header, handle->adr_wr.page)))
        {
            // Next page
            handle->adr_wr.page = px_log_fs_page_next(handle, handle->adr_wr.page);
#if PX_LOG_FS_CFG_STOP_WR_WHEN_FULL
            if(handle->adr_wr.page == handle->page_first)
            {
                return PX_LOG_FS_ERR_FULL;
            }
#endif
            // Start of new erase block?
            if((handle->adr_wr.page % PX_LOG_FS_CFG_ERASE_BLOCK_SIZE) == 0)
            {
                // Erase block
                px_log_fs_glue_erase_block(handle->adr_wr.page);
                // Has oldest RECORD pages been erased?
                if(  (handle->page_first >= handle->adr_wr.page)
                   &&(handle->page_first <  handle->adr_wr.page + PX_LOG_FS_CFG_ERASE_BLOCK_SIZE)  )
                {
                    // Find new first page
                    page = handle->adr_wr.page + PX_LOG_FS_CFG_ERASE_BLOCK_SIZE;
                    while(true)
                    {
                        // Is page marked?
                        marker = px_log_fs_header_rd(&header, handle->adr_wr.page);
                        if(px_log_fs_marker_is_page(marker))
                        {
                            // Save new first page
                            handle->page_first = page;
                            PX_LOG_I("New first page is %u", handle->page_first);
                            break;
                        }
                        if(page == handle->page_last)
                        {
                            PX_LOG_E("File system must be corrupt");
                            return PX_LOG_FS_ERR_FATAL;
                        }
                        // Next page
                        page = px_log_fs_page_next(handle, page);
                    }
                }
                break;
            }
        }
        // Mark page
        header.marker = PX_LOG_FS_MARKER_PAGE;
        header.nr     = handle->page_nr_next++;
        if(!px_log_fs_header_wr(&header, handle->adr_wr.page))
        {
            // Header write failed. Try next page
            handle->adr_wr.page = px_log_fs_page_next(handle, handle->adr_wr.page);
            return PX_LOG_FS_ERR_WRITE_FAIL;
        }
    }

    // Write new record
    if(!px_log_fs_record_wr(&record, &handle->adr_wr))
    {
        // Record write failed. Try next record address
        px_log_fs_record_adr_next(handle, &handle->adr_wr);
        return PX_LOG_FS_ERR_WRITE_FAIL;
    }
    // First record?
    if(handle->page_first == PX_LOG_FS_PAGE_INVALID)
    {
        // Save first page
        handle->page_first = handle->adr_wr.page;
    }
    // Update last page
    handle->page_last = handle->adr_wr.page;
    // Advance write address to next position
    px_log_fs_record_adr_next(handle, &handle->adr_wr);

    // Success
    return PX_LOG_FS_ERR_NONE;
}

void px_log_fs_dbg_report_info(px_log_fs_handle_t * handle)
{
    px_log_fs_header_t header;
    uint16_t           page;
    uint8_t            i;

    printf("Page start:       0x%04X\n", handle->fs_page_start);
    printf("Page end:         0x%04X\n", handle->fs_page_end);
    printf("Page size:        %u\n",     PX_LOG_FS_CFG_PAGE_SIZE);
    printf("Record size:      %u\n",     sizeof(px_log_fs_record_t));
    printf("Record data size: %u\n",     PX_LOG_FS_CFG_REC_DATA_SIZE);
    printf("Records per page: %u\n",     PX_LOG_FS_RECORDS_PER_PAGE);
    printf("First page:       0x%04X\n", handle->page_first);
    printf("Last page:        0x%04X\n", handle->page_last);

    printf("\n        ");
    for(i = 0; i < 16; i++)
    {
        printf("%01X      ", i);
    }
    printf("\n");

    i=0;
    for(page = handle->fs_page_start; page <= handle->fs_page_end; page++)
    {
        if(i == 0)
        {
            printf("\n0x%04X: ", page);
        }
        px_log_fs_header_rd(&header, page);
        switch(header.marker)
        {
        case PX_LOG_FS_MARKER_FREE:
            printf("x      ");
            break;
        case PX_LOG_FS_MARKER_PAGE:
            printf("P %04x ", header.nr);
            break;
        case PX_LOG_FS_MARKER_PAGE_A:
            printf("P*%04x ", header.nr);
            break;
        case PX_LOG_FS_MARKER_BAD:
            // Fall through...
        default:
            printf("B      ");
            break;
        }
        if(++i == 16)
        {
            i = 0;
        }
    }
    printf("\n");
}
