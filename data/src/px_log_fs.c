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
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("px_log_fs");

/// Invalid page value
#define PX_LOG_FS_PAGE_INVALID      0xffff

/// @name Marker values
//@{
#define PX_LOG_FS_MARKER_FREE       0xff        /* 1111_1111b */
#define PX_LOG_FS_MARKER_USED       0x5a        /* 0101_1010b */
#define PX_LOG_FS_MARKER_RECORD     0xa5        /* 1010_0101b */
#define PX_LOG_FS_MARKER_BAD        0x00        /* 0000_0000b */
//@}

/// Definition of header structure that is stored at the start of each page
typedef struct
{
    uint8_t  marker;                            ///< FREE, USED or BAD
    uint16_t nr;                                ///< Rolling number to find first and last used page
    uint8_t  crc;                               ///< Checksum
} PX_ATTR_PACKED px_log_fs_header_t;

/// Definition of a record structure
typedef struct
{
    uint8_t marker;                             ///< Record marker
    uint8_t data[PX_LOG_FS_CFG_REC_DATA_SIZE];  ///< Record data content
    uint8_t crc;                                ///< Checksum
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
                // Shift right and XOR with reverse of polynomial x^8+x^6+x^3+x^2+x^0
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

static uint8_t px_log_fs_header_rd(px_log_fs_header_t * header, uint16_t page)
{
    uint8_t crc;

    // Read page header
    px_log_fs_glue_rd(header,
                      page,
                      0,
                      sizeof(px_log_fs_header_t));
    // Page FREE or BAD?
    if(  (header->marker == PX_LOG_FS_MARKER_FREE)
       ||(header->marker == PX_LOG_FS_MARKER_BAD )  )
    {
        return header->marker;
    }
    // Invalid marker?
    if(header->marker != PX_LOG_FS_MARKER_USED)
    {
        // Mark as BAD
        PX_DBG_ERR("Invalid marker 0x%02X @ page %u", header->marker, page);
        header->marker = PX_LOG_FS_MARKER_BAD;
        px_log_fs_glue_wr(&header->marker, page, 0, 1);
        return header->marker;
    }
    // Check CRC
    crc = px_log_fs_crc(header, offsetof(px_log_fs_header_t, crc));
    if(crc != header->crc)
    {
        // Mark as BAD
        PX_DBG_ERR("Header CRC check failed @ page %u (0x%02X != 0x%02X)",
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
    // Start at first page
    uint16_t page = handle->fs_page_start;
    while(true)
    {
        // Read page header. Is it USED?
        if(px_log_fs_header_rd(header, page) == PX_LOG_FS_MARKER_USED)
        {
            // USED page found
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
    // Start at current page
    uint16_t page = page_current;
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
        // Read page header. Is it a USED page?
        if(px_log_fs_header_rd(header, page) == PX_LOG_FS_MARKER_USED)
        {
            // USED page found
            return page;
        }
    }
}

static bool px_log_fs_header_wr(px_log_fs_header_t * header, uint16_t page)
{
    px_log_fs_header_t header_rd;

    // Set CRC
    header->crc = px_log_fs_crc(header, offsetof(px_log_fs_header_t, crc));
    // Write header
    px_log_fs_glue_wr(header, page, 0, sizeof(*header));
    // Read back header
    px_log_fs_glue_rd(&header_rd, page, 0, sizeof(*header));
    // Match?
    if(memcmp(header, &header_rd, sizeof(*header)) != 0)
    {
        // Mark header as BAD
        PX_DBG_ERR("Header write failed @ page %u", page);
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
    if(  (record->marker == PX_LOG_FS_MARKER_FREE)
       ||(record->marker == PX_LOG_FS_MARKER_BAD )  )
    {
        return record->marker;
    }
    // Marker correct?
    if(record->marker != PX_LOG_FS_MARKER_RECORD)
    {
        // No. Mark as BAD
        PX_DBG_WARN("Invalid record marker %02X @ page %u offset %u",
                    record->marker, adr->page, adr->offset);
        record->marker = PX_LOG_FS_MARKER_BAD;
        px_log_fs_glue_wr(&record->marker, adr->page, adr->offset, 1);
        return record->marker;
    }
    // CRC correct?
    crc = px_log_fs_crc(record, offsetof(px_log_fs_record_t, crc));
    if(crc != record->crc)
    {
        // Mark record as BAD
        PX_DBG_ERR("Record CRC check failed @ page %u offset %u (0x%02X != 0x%02X)", 
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
        PX_DBG_ERR("Record will overflow the page");
        return false;
    }

    // Set marker
    record->marker = PX_LOG_FS_MARKER_RECORD;
    // Set CRC
    record->crc = px_log_fs_crc(record, offsetof(px_log_fs_record_t, crc));
    // Write record
    px_log_fs_glue_wr(record, adr->page, adr->offset, sizeof(*record));
    // Read back record
    px_log_fs_glue_rd(&record_rd, adr->page, adr->offset, sizeof(*record));
    // Match?
    if(memcmp(record, &record_rd, sizeof(*record)) != 0)
    {
        // Mark record as BAD
        PX_DBG_ERR("Record write failed @ page %u offset %u", 
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
    PX_DBG_ASSERT(fs_page_start != PX_LOG_FS_PAGE_INVALID);                                     // Must not be reserved invalid value
    PX_DBG_ASSERT(fs_page_end   != PX_LOG_FS_PAGE_INVALID);                                     // Must not be reserved invalid value
    PX_DBG_ASSERT((fs_page_start % PX_LOG_FS_CFG_ERASE_BLOCK_SIZE)  == 0);                      // Must be at the start of an erase block
    PX_DBG_ASSERT(((fs_page_end + 1) % PX_LOG_FS_CFG_ERASE_BLOCK_SIZE)  == 0);                  // Must be at the end of an erase block
    PX_DBG_ASSERT(((fs_page_end + 1 - fs_page_start) / PX_LOG_FS_CFG_ERASE_BLOCK_SIZE) >= 2);   // Must be at least 2 erase blocks in size (more is better)
    PX_DBG_ASSERT(sizeof(px_log_fs_record_t) <= PX_LOG_FS_PAGE_DATA_SIZE);                      // A record must fit in a page
    if(PX_LOG_FS_PAGE_DATA_SIZE % sizeof(px_log_fs_record_t) != 0)
    {
        PX_DBG_WARN("%u bytes will be wasted per page", PX_LOG_FS_PAGE_DATA_SIZE % sizeof(px_log_fs_record_t));
    }

    // Save file system start and end page
    handle->fs_page_start = fs_page_start;
    handle->fs_page_end   = fs_page_end;
    // Read first USED page
    page_first = px_log_fs_header_rd_first(handle, &header);
    if(page_first == PX_LOG_FS_PAGE_INVALID)
    {
        // None found. File system is empty
        PX_DBG_INFO("No USED pages found");
        handle->page_first    = PX_LOG_FS_PAGE_INVALID;
        handle->page_last     = PX_LOG_FS_PAGE_INVALID;
        handle->page_nr_next  = 0;
        handle->adr_wr.page   = handle->fs_page_start;
        handle->adr_wr.offset = PX_LOG_FS_REC_OFFSET_FIRST;
        return PX_LOG_FS_ERR_NONE;
    }
    // Continue from first USED page...
    page = page_first;
    do
    {
        // Save page number
        page_nr = header.nr;
        // Read next USED page
        page_next = px_log_fs_header_rd_next(handle, &header, page);
        if(page_next == PX_LOG_FS_PAGE_INVALID)
        {
            // Only one USED page found
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
            PX_DBG_INFO("Diff=%u, page %u (nr %u) , page_next %u (nr %u)",
                     page_nr_diff, page, page_nr, page_next, page_nr_next);
            // Save largest difference so far
            page_nr_diff_largest = page_nr_diff;
            // Save candidates for first and last USED page
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
    while(px_log_fs_record_rd(&record, &handle->adr_wr) != PX_LOG_FS_MARKER_FREE)
    {
        px_log_fs_record_adr_next(handle, &handle->adr_wr);
        if(handle->adr_wr.offset == PX_LOG_FS_REC_OFFSET_FIRST)
        {
            break;
        }
    }

    // Report first and last page
    PX_DBG_INFO("First USED page %u", handle->page_first);
    PX_DBG_INFO("Last USED page %u",  handle->page_last);
    PX_DBG_INFO("Next page nr %u",    handle->page_nr_next);

    return PX_LOG_FS_ERR_NONE;
}

px_log_fs_err_t px_log_fs_reset(px_log_fs_handle_t * handle,
                                uint16_t             fs_page_start,
                                uint16_t             fs_page_end)
{
    px_log_fs_header_t header;
    uint16_t           page;

    // Sanity checks
    PX_DBG_ASSERT(fs_page_start != PX_LOG_FS_PAGE_INVALID);                                     // Must not be reserved invalid value
    PX_DBG_ASSERT(fs_page_end   != PX_LOG_FS_PAGE_INVALID);                                     // Must not be reserved invalid value
    PX_DBG_ASSERT((fs_page_start % PX_LOG_FS_CFG_ERASE_BLOCK_SIZE)  == 0);                      // Must be at the start of an erase block
    PX_DBG_ASSERT(((fs_page_end + 1) % PX_LOG_FS_CFG_ERASE_BLOCK_SIZE)  == 0);                  // Must be at the end of an erase block
    PX_DBG_ASSERT(((fs_page_end + 1 - fs_page_start) / PX_LOG_FS_CFG_ERASE_BLOCK_SIZE) >= 2);   // Must be at least 2 erase blocks in size (more is better)
    PX_DBG_ASSERT(sizeof(px_log_fs_record_t) <= PX_LOG_FS_PAGE_DATA_SIZE);                      // A record must fit in a page
    if(PX_LOG_FS_PAGE_DATA_SIZE % sizeof(px_log_fs_record_t) != 0)
    {
        PX_DBG_WARN("%u bytes will be wasted per page", PX_LOG_FS_PAGE_DATA_SIZE % sizeof(px_log_fs_record_t));
    }

    // Save file system start and end page
    handle->fs_page_start = fs_page_start;
    handle->fs_page_end   = fs_page_end;

    // Mark all USED pages as BAD
    for(page = fs_page_start; page <= fs_page_end; page++)
    {
        if(px_log_fs_header_rd(&header, page) == PX_LOG_FS_MARKER_USED)
        {
            header.marker = PX_LOG_FS_MARKER_BAD;
            px_log_fs_glue_wr(&header.marker, page, 0, 1);
        }
    }
    // Find first FREE page on the start of an erase block
    for(page = fs_page_start; page <= fs_page_end; page += PX_LOG_FS_CFG_ERASE_BLOCK_SIZE)
    {
        if(px_log_fs_header_rd(&header, page) == PX_LOG_FS_MARKER_FREE)
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
    // No USED pages?
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

px_log_fs_err_t px_log_fs_rd_next(px_log_fs_handle_t * handle, 
                                  void *               data, 
                                  size_t               nr_of_bytes)
{
    px_log_fs_header_t header;
    px_log_fs_record_t record;

    // More bytes requested than can be stored in record?
    if(nr_of_bytes > PX_LOG_FS_CFG_REC_DATA_SIZE)
    {
        PX_DBG_ERR("More bytes requested than can be stored in a record");
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
            // Find next USED page
            while(px_log_fs_header_rd(&header, handle->adr_rd.page) != PX_LOG_FS_MARKER_USED)
            {
                if(handle->adr_rd.page == handle->page_last)
                {
                    PX_DBG_ERR("File system must be corrupt");
                    return PX_LOG_FS_ERR_FATAL;
                }
                handle->adr_rd.page = px_log_fs_page_next(handle, handle->adr_rd.page);
            }
        }
        // Read record
        if(px_log_fs_record_rd(&record, &handle->adr_rd) == PX_LOG_FS_MARKER_RECORD)
        {
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
    // No USED pages?
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

    // More bytes requested than can be stored in record?
    if(nr_of_bytes > PX_LOG_FS_CFG_REC_DATA_SIZE)
    {
        PX_DBG_ERR("More bytes requested than can be stored in a record");
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
            // Find previous USED page
            while(px_log_fs_header_rd(&header, handle->adr_rd.page) != PX_LOG_FS_MARKER_USED)
            {
                if(handle->adr_rd.page == handle->page_first)
                {
                    PX_DBG_ERR("File system must be corrupt");
                    return PX_LOG_FS_ERR_FATAL;
                }
                handle->adr_rd.page = px_log_fs_page_previous(handle, handle->adr_rd.page);
            }
        }
        // Read record
        if(px_log_fs_record_rd(&record, &handle->adr_rd) == PX_LOG_FS_MARKER_RECORD)
        {
            // Copy content of record to user supplied buffer
            memcpy(data, &record.data, nr_of_bytes);
            // Valid record
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

    // Record too small?
    if(nr_of_bytes > PX_LOG_FS_CFG_REC_DATA_SIZE)
    {
        PX_DBG_ERR("Record size too small and %u bytes will be discarded",
                (nr_of_bytes - PX_LOG_FS_CFG_REC_DATA_SIZE));
        // Clip number of bytes that will be copied
        nr_of_bytes = PX_LOG_FS_CFG_REC_DATA_SIZE;
    }
    // Copy data
    memcpy(&record.data, data, nr_of_bytes);

    // New page?
    if(handle->adr_wr.offset == PX_LOG_FS_REC_OFFSET_FIRST)
    {
        // Find next FREE page
        while(px_log_fs_header_rd(&header, handle->adr_wr.page) != PX_LOG_FS_MARKER_FREE)
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
                        // Is page USED?
                        if(px_log_fs_header_rd(&header, handle->adr_wr.page) == PX_LOG_FS_MARKER_USED)
                        {
                            // Save new first page
                            handle->page_first = page;
                            PX_DBG_INFO("New first page is %u", handle->page_first);
                            break;
                        }
                        if(page == handle->page_last)
                        {
                            PX_DBG_ERR("File system must be corrupt");
                            return PX_LOG_FS_ERR_FATAL;
                        }
                        // Next page
                        page = px_log_fs_page_next(handle, page);
                    }
                }
                break;
            }        
        }
        // Mark page as USED
        header.marker = PX_LOG_FS_MARKER_USED;
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

    PX_DBG_TRACE("Page start: 0x%04X\n",   handle->fs_page_start);
    PX_DBG_TRACE("Page end: 0x%04X\n",     handle->fs_page_end);
    PX_DBG_TRACE("Page size: %u\n",        PX_LOG_FS_CFG_PAGE_SIZE);
    PX_DBG_TRACE("Record size: %u\n",      sizeof(px_log_fs_record_t));
    PX_DBG_TRACE("Record data size: %u\n", PX_LOG_FS_CFG_REC_DATA_SIZE);
    PX_DBG_TRACE("Records per page: %u\n", PX_LOG_FS_RECORDS_PER_PAGE);
    PX_DBG_TRACE("First page: %u\n",       handle->page_first);
    PX_DBG_TRACE("Last page: %u\n",        handle->page_last);

    PX_DBG_TRACE("\n        ");
    for(i = 0; i < 16; i++)
    {
        PX_DBG_TRACE("%01X      ", i);
    }
    PX_DBG_TRACE("\n");

    i=0;
    for(page = handle->fs_page_start; page <= handle->fs_page_end; page++)
    {
        if(i == 0)
        {
            PX_DBG_TRACE("\n0x%04X: ", page);
        }
        px_log_fs_header_rd(&header, page);
        switch(header.marker)
        {
        case PX_LOG_FS_MARKER_FREE:
            PX_DBG_TRACE("x      ");
            break;
        case PX_LOG_FS_MARKER_USED:
            PX_DBG_TRACE("U %04x ", header.nr);
            break;
        case PX_LOG_FS_MARKER_BAD:
            // Fall through...
        default:
            PX_DBG_TRACE("B      ");
            break;
        }
        if(++i == 16)
        {
            i = 0;
        }
    }
    PX_DBG_TRACE("\n");
}
