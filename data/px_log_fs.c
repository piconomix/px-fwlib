/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
 
    Title:          px_log_fs.h : Simple record-based file system for Serial Flash
    Author(s):      Pieter Conradie
    Creation Date:  2014-06-09

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>
#include <stddef.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_log_fs.h"
#include "px_log_fs_glue.h"

#include "px_dbg.h"
PX_DBG_DECL_NAME("px_log_fs")

/* _____LOCAL DEFINITIONS____________________________________________________ */
/// Invalid page value
#define PX_LOG_FS_PAGE_INVALID         0xffff

#if (PX_LOG_FS_CFG_PAGE_END >= PX_LOG_FS_PAGE_INVALID)
#error "PX_LOG_FS_PAGE_INVALID must not be a valid page value"
#endif

/// File attribute data size (page size - file block size)
#define PX_LOG_FS_FILE_ATTR_DATA_SIZE (PX_LOG_FS_CFG_PAGE_SIZE - sizeof(px_log_fs_file_info_t))

/// Record page data size (page size - page header size)
#define PX_LOG_FS_REC_PAGE_DATA_SIZE (PX_LOG_FS_CFG_PAGE_SIZE - sizeof(px_log_fs_page_header_t))

/// Records per page
#define PX_LOG_FS_RECORDS_PER_PAGE (PX_LOG_FS_REC_PAGE_DATA_SIZE / sizeof(px_log_fs_rec_block_t))

/// First record offset
#define PX_LOG_FS_REC_OFFSET_FIRST (sizeof(px_log_fs_page_header_t))

/// Last record offset
#define PX_LOG_FS_REC_OFFSET_LAST  (sizeof(px_log_fs_page_header_t) + (PX_LOG_FS_RECORDS_PER_PAGE - 1) * sizeof(px_log_fs_rec_block_t))

/// Page of first erase block
#define PX_LOG_FS_ERASE_BLOCK_FIRST PX_LOG_FS_CFG_PAGE_START

/// Page of last erase block
#define PX_LOG_FS_ERASE_BLOCK_LAST  (PX_LOG_FS_CFG_PAGE_END - PX_LOG_FS_CFG_ERASE_BLOCK_SIZE + 1)

/// Marker size definition
typedef uint8_t px_log_fs_marker_t;

/// @name Marker values
//@{
#define PX_LOG_FS_MARKER_FREE      0xff /* 1111 1111b */
#define PX_LOG_FS_MARKER_FILE      0x2f /* 0010 1111b */
#define PX_LOG_FS_MARKER_RECORD    0x1f /* 0001 1111b */
#define PX_LOG_FS_MARKER_BAD       0x00 /* 0000 0000b */
//@}

/// Specification of data address in Serial Flash
typedef struct
{
    px_log_fs_page_t   page;       ///< Serial Flash page
    px_log_fs_offset_t offset;     ///< Offset inside page
} px_log_fs_adr_t;

/// /// Rolling number size definition
typedef uint16_t px_log_fs_nr_t;

/// CRC size definition
typedef uint8_t px_log_fs_crc_t;

/// Specification of page header that is stored at the start of each page
typedef struct
{
    px_log_fs_marker_t marker;     ///< FREE, FILE, RECORD or BAD
    px_log_fs_nr_t     nr;         ///< Rolling number to find FILE or RECORD start and end
    px_log_fs_crc_t    crc;        ///< CRC checksum
} PX_ATTR_PACKED px_log_fs_page_header_t;

/// Specification of file info (including CRC) that is stored after a FILE page header
typedef struct
{
    px_log_fs_file_t    file;       ///< File info (page number and time stamp)
    px_log_fs_crc_t     crc;        ///< CRC checksum
} PX_ATTR_PACKED px_log_fs_file_info_t;

/// Specification of a record block written to Serial Flash
typedef struct
{
    px_log_fs_marker_t marker;                             ///< Record marker
    uint8_t            data[PX_LOG_FS_CFG_REC_DATA_SIZE];  ///< Record data content
    px_log_fs_crc_t    crc;                                ///< CRC checksum
} PX_ATTR_PACKED px_log_fs_rec_block_t;

/// File data and state
typedef struct
{
    px_log_fs_page_header_t page_header;       ///< Page header
    px_log_fs_file_info_t   file_info;         ///< File info
    px_log_fs_rec_block_t   record_block;      ///< Record block

    px_log_fs_page_t        file_page_first;   ///< Page number of first file (PX_LOG_FS_PAGE_INVALID if empty)
    px_log_fs_page_t        file_page_last;    ///< Page number of last file (PX_LOG_FS_PAGE_INVALID if empty)
    px_log_fs_page_t        file_page_nr_next; ///< Next file page number to use (starts at 0)

    px_log_fs_page_t        rec_page_first;    ///< First page with records (PX_LOG_FS_PAGE_INVALID if empty)
    px_log_fs_page_t        rec_page_last;     ///< Last page with records (PX_LOG_FS_PAGE_INVALID if empty)
    px_log_fs_page_t        rec_page_nr_next;  ///< Next record page number to use (starts at 0)

    px_log_fs_adr_t         rec_adr_rd;        ///< Current read address  (page = PX_LOG_FS_PAGE_INVALID when file is opened)
    px_log_fs_adr_t         rec_adr_wr;        ///< Next write address (open position)
} px_log_fs_t;

/* _____MACROS_______________________________________________________________ */
/// Test if page is at the start of an erase block
#define PX_LOG_FS_IS_ERASE_BLOCK_START_PAGE(page) \
    ((page & (PX_LOG_FS_CFG_ERASE_BLOCK_SIZE - 1)) == 0)

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_log_fs_t log_fs;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */
/**
 *  Get next page number.
 *  
 *  The Serial Flash pages are treated like a circular buffer. If the end page 
 *  number is specified, the first page number is returned.
 *  
 *  @param  page                The current page number
 *  @return px_log_fs_page_t    The next page number
 */
static px_log_fs_page_t px_log_fs_page_next(px_log_fs_page_t page) PX_ATTR_CONST;

/**
 *  Get previous page number.
 *  
 *  The Serial Flash pages are treated like a circular buffer. If the first page
 *  number is specified, the last page number is returned.
 *  
 *  @param  page                The current page number
 *  @return px_log_fs_page_t    The previous page number
 */
static px_log_fs_page_t px_log_fs_page_previous(px_log_fs_page_t page) PX_ATTR_CONST;

/**
 *  Get page of the next erase block.
 *  
 *  The Serial Flash erase blocks are treated like a circular buffer. If a page
 *  in the last erase block is specified, the first erase block page is
 *  returned.
 *  
 *  @param  page                The current page number
 *  @return px_log_fs_page_t    The next page number
 */
static px_log_fs_page_t px_log_fs_erase_block_next(px_log_fs_page_t page) PX_ATTR_CONST;

/**
 *  Get page of the previous erase block.
 *  
 *  The Serial Flash erase blocks are treated like a circular buffer. If a page
 *  in the first erase block is specified, the last erase block page is
 *  returned.
 *  
 *  @param  page                The current page number
 *  @return px_log_fs_page_t    The previous page number
 */
static px_log_fs_page_t px_log_fs_erase_block_previous(px_log_fs_page_t page) PX_ATTR_CONST;

/**
 *  Calculate a CRC (checksum) over a specified block of bytes.
 *  
 *  @param data                 Pointer to a buffer containing the data
 *  @param nr_of_bytes          Number of bytes to calculate 
 *  
 *  @return px_log_fs_crc_t     Calculated CRC
 */
static px_log_fs_crc_t px_log_fs_crc_calc(const void * data, size_t nr_of_bytes);

/**
 *  Calculate CRC (checksum) of the page header that is stored at the start of 
 *  the page.
 *  
 *  @return px_log_fs_crc_t     Calculated CRC
 */
static px_log_fs_crc_t px_log_fs_crc_page_header(void);

/**
 *  Calculate CRC (checksum) of the file info (page number and time stamp).
 *  
 *  @return px_log_fs_crc_t     Calculated CRC
 */
static px_log_fs_crc_t px_log_fs_crc_file_info(void);

/**
 *  Calculate CRC (checksum) of the record block.
 *  
 *  @return px_log_fs_crc_t     Calculated CRC
 */
static px_log_fs_crc_t px_log_fs_crc_record_block(void);

/**
 *  Read a marker at a specified page and offset.
 *  
 *  The marker is checked to see if it is valid. If it is invalid, it is
 *  overwritten as BAD (0x00).
 *  
 *  @param page                 Page to read
 *  @param offset               Offset to read
 *  
 *  @return px_log_fs_marker_t  Marker value
 */
static px_log_fs_marker_t px_log_fs_marker_rd(px_log_fs_page_t   page,
                                              px_log_fs_offset_t offset);

/**
 *  Write a marker to the specified page and offset.
 *  
 *  @param marker   Marker value to write
 *  @param page     Page to write
 *  @param offset   Offset to write
 *  
 *  @retval true    Marker was succesfully written and verified
 *  @retval false   Marker failed to write correctly and was overwritten as BAD
 *                  (0x00).
 */
static bool px_log_fs_marker_wr(px_log_fs_marker_t marker,
                                px_log_fs_page_t   page,
                                px_log_fs_offset_t offset);

/**
 *  Read a page header from the start of the specified page.
 *  
 *  A page header is read from the specified page. If the CRC check failed, the
 *  marker will be overwritten to BAD (0x00) to invalidate it.
 *  
 *  @param page                 Specified page to read from
 *  
 *  @return px_log_fs_marker_t  Marker of page header. BAD if CRC check failed
 */
static px_log_fs_marker_t px_log_fs_page_header_rd(px_log_fs_page_t page);

/**
 *  Write a page header to the start of the specified page.
 *  
 *  A page header is written to the specified page and verified. If the 
 *  verification failes, the marker is overwritten to BAD (0x00) to invalidate
 *  it.
 *  
 *  @param page             Page to write to
 *  
 *  @retval true            Page header written correctly
 *  @retval false           Page header write failed and marker set to BAD 
 */
static bool px_log_fs_page_header_wr(px_log_fs_page_t page);

/**
 *  Read a file entry (page header & file info) from the specified page.
 *  
 *  @param page             Specified page to read from
 *  
 *  @retval true            Page contains a valid file block
 *  @retval false           Page does not contain a valid file block
 */
static bool px_log_fs_file_entry_rd(px_log_fs_page_t page);

/**
 *  Write a file entry (page header & file info) to the specified page.
 *  
 *  A file entry is written to the specified page and verified. If the 
 *  verification fails, the marker is overwritten to BAD (0x00) to invalidate
 *  it.
 *  
 *  @param page             Page to write to
 *  
 *  @retval true            Page header written correctly
 *  @retval false           Page header write failed and marker set to BAD 
 */
static bool px_log_fs_file_entry_wr(px_log_fs_page_t page);

/**
 *  Read a record entry from the specified page and offset.
 *  
 *  If the CRC is invalid, the marker is overwritten to BAD (0x00) to invalidate
 *  it.
 *  
 *  @param page             Specified page to read from
 *  @param offset           Specified offset to read from
 *  
 *  @retval true            Valid record block read
 *  @retval false           Record block not valid
 */
static bool px_log_fs_record_entry_rd(px_log_fs_page_t   page,
                                      px_log_fs_offset_t offset);
/**
 *  Write a record entry to the specified page and offset.
 *  
 *  The entry is verified and the marker is overwritten to BAD if verification 
 *  failed.
 *  
 *  @param page             Specified page to write to
 *  @param offset           Specfied offset to write to
 *  
 *  @retval true            Record block succesfully written
 *  @retval false           Failed to write record block and marker has been 
 *                          overwritten to BAD
 */
static bool px_log_fs_record_entry_wr(px_log_fs_page_t   page,
                                      px_log_fs_offset_t offset);

/**
 *  Given the start page of a file, this function calculates the first record 
 *  page that may be written to (the start boundary).
 *  
 *  For the #PX_LOG_FS_CFG_TYPE_LINEAR option this is the next page after the
 *  file page.
 *  
 *  For the #PX_LOG_FS_CFG_TYPE_CIRCULAR option this is the next erase block
 *  after the file page.
 *  
 *  @return px_log_fs_page_t    The first record page that may be written to
 */
static px_log_fs_page_t px_log_fs_record_pages_bound_start(void);

/**
 *  Given the start page of a file, this function calculates the last record 
 *  page that may be written to (the end boundary).
 *  
 *  If the number of record pages is not limited (PX_LOG_FS_CFG_MAX_PAGES == 0) 
 *  then the last record page that may be written to is just before the file 
 *  page due to the circular nature of the file system.
 *  
 *  @return px_log_fs_page_t    The last record page that may be written to
 */
static px_log_fs_page_t px_log_fs_record_pages_bound_end(void);

/**
 *  Find page number of the first page that contains the specified marker.
 *  
 *  @param marker               Specified marker to search for (e.g.
 *                              PX_LOG_FS_MARKER_FILE or PX_LOG_FS_MARKER_RECORD).
 *  @param page_start           Start page to search for specified marker
 *  @param page_end             End page to search for specified marker
 *  
 *  @return px_log_fs_page_t    Page number containing specified marker or
 *                              PX_LOG_FS_PAGE_INVALID if page could not be found.
 */
static px_log_fs_page_t px_log_fs_page_header_find_first(px_log_fs_marker_t marker,
                                                         px_log_fs_page_t   page_start, 
                                                         px_log_fs_page_t   page_end);

/**
 *  Find page number of the next page that contains the specified marker.
 *  
 *  @param marker               Specified marker to search for (e.g.
 *                              PX_LOG_FS_MARKER_FILE or PX_LOG_FS_MARKER_RECORD).
 *  @param page_start           Start page to search for specified marker
 *  @param page_end             End page to search for specified marker
 *  @param page_current         Current page that contains a valid marker
 *  
 *  @return px_log_fs_page_t    Page number containing specified marker or
 *                              PX_LOG_FS_PAGE_INVALID if page could not be found.
 */
static px_log_fs_page_t px_log_fs_page_header_find_next(px_log_fs_marker_t marker,
                                                        px_log_fs_page_t   page_start, 
                                                        px_log_fs_page_t   page_end,
                                                        px_log_fs_page_t   page_current);

/* _____LOCAL FUNCTIONS______________________________________________________ */
static px_log_fs_page_t px_log_fs_page_next(px_log_fs_page_t page)
{
    // Sanity check
    PX_DBG_ASSERT(page >= PX_LOG_FS_CFG_PAGE_START);
    PX_DBG_ASSERT(page <= PX_LOG_FS_CFG_PAGE_END);

    // End?
    if(page == PX_LOG_FS_CFG_PAGE_END)
    {
        // Wrap to start
        return PX_LOG_FS_CFG_PAGE_START;
    }
    else
    {
        // Return next page number
        return ++page;
    }
}

static px_log_fs_page_t px_log_fs_page_previous(px_log_fs_page_t page)
{
    // Sanity check
    PX_DBG_ASSERT(page >= PX_LOG_FS_CFG_PAGE_START);
    PX_DBG_ASSERT(page <= PX_LOG_FS_CFG_PAGE_END);

    // Start?
    if(page == PX_LOG_FS_CFG_PAGE_START)
    {
        // Wrap to end
        return PX_LOG_FS_CFG_PAGE_END;
    }
    else
    {
        // Return previous page number
        return --page;
    }
}

static px_log_fs_page_t px_log_fs_erase_block_next(px_log_fs_page_t page)
{
    // Sanity check
    PX_DBG_ASSERT(page >= PX_LOG_FS_CFG_PAGE_START);
    PX_DBG_ASSERT(page <= PX_LOG_FS_CFG_PAGE_END);

    // Clip to start of erase block
    page &= ~(PX_LOG_FS_CFG_ERASE_BLOCK_SIZE - 1);

    // Last erase block?
    if(page == PX_LOG_FS_ERASE_BLOCK_LAST)
    {
        // Wrap to start
        page = PX_LOG_FS_ERASE_BLOCK_FIRST;
    }
    else
    {
        // Next erase block
        page += PX_LOG_FS_CFG_ERASE_BLOCK_SIZE;
    }

    return page;
}

static px_log_fs_page_t px_log_fs_erase_block_previous(px_log_fs_page_t page)
{
    // Sanity check
    PX_DBG_ASSERT(page >= PX_LOG_FS_CFG_PAGE_START);
    PX_DBG_ASSERT(page <= PX_LOG_FS_CFG_PAGE_END);

    // Clip to start of erase block
    page &= ~(PX_LOG_FS_CFG_ERASE_BLOCK_SIZE - 1);

    // First erase block?
    if(page == PX_LOG_FS_ERASE_BLOCK_FIRST)
    {
        // Wrap to last erase block
        page = PX_LOG_FS_ERASE_BLOCK_LAST;
    }
    else
    {
        // Previous erase block
        page -= PX_LOG_FS_CFG_ERASE_BLOCK_SIZE;
    }

    return page;
}

static px_log_fs_crc_t px_log_fs_crc_calc(const void * data, size_t nr_of_bytes)
{
    uint8_t         i;
    uint8_t *       data_u8 = (uint8_t *)data;
    px_log_fs_crc_t crc = 0xff;

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

static px_log_fs_crc_t px_log_fs_crc_page_header(void)
{
    // Calculate CRC over whole block, except for CRC part
    return px_log_fs_crc_calc(&log_fs.page_header, offsetof(px_log_fs_page_header_t, crc));
}

static px_log_fs_crc_t px_log_fs_crc_file_info(void)
{
    // Calculate CRC over whole block, except for CRC part
    return px_log_fs_crc_calc(&log_fs.file_info, offsetof(px_log_fs_file_info_t, crc));
}

static px_log_fs_crc_t px_log_fs_crc_record_block(void)
{
    // Calculate CRC over whole block, except for CRC part
    return px_log_fs_crc_calc(&log_fs.record_block, offsetof(px_log_fs_rec_block_t, crc));
}

static px_log_fs_marker_t px_log_fs_marker_rd(px_log_fs_page_t   page,
                                              px_log_fs_offset_t offset)
{
    px_log_fs_marker_t marker;

    // Read marker
    px_log_fs_glue_rd(&marker,
                      page,
                      offset,
                      sizeof(px_log_fs_marker_t));

    // Invalid marker?
    if(  (marker != PX_LOG_FS_MARKER_FREE  )
       &&(marker != PX_LOG_FS_MARKER_FILE  )
       &&(marker != PX_LOG_FS_MARKER_RECORD)
       &&(marker != PX_LOG_FS_MARKER_BAD   )  )
    {
        PX_DBG_ERR("Invalid marker 0x%02X (page %u, offset %u)", marker, page, offset);

        // Set marker to BAD
        marker = PX_LOG_FS_MARKER_BAD;
        px_log_fs_glue_wr(&marker,
                          page,
                          offset,
                          sizeof(px_log_fs_marker_t));
    }

    return marker;
}

static bool px_log_fs_marker_wr(px_log_fs_marker_t marker,
                                px_log_fs_page_t   page,
                                px_log_fs_offset_t offset)
{
    px_log_fs_marker_t marker_rd;

    // Write marker
    px_log_fs_glue_wr(&marker,
                      page,
                      offset,
                      sizeof(px_log_fs_marker_t));

    // Marker correctly written?
    marker_rd = px_log_fs_marker_rd(page, offset);
    if(marker_rd != marker)
    {
        PX_DBG_ERR("Could not write marker(page %u, offset %u): wr 0x%02X, rd 0x%02X",
                page, offset, marker, marker_rd);
        // Set marker to BAD
        marker = PX_LOG_FS_MARKER_BAD;
        px_log_fs_glue_wr(&marker,
                          page,
                          offset,
                          sizeof(px_log_fs_marker_t));
        // Failure
        return false;
    }
    else
    {
        // Success
        return true;
    }
}

static px_log_fs_marker_t px_log_fs_page_header_rd(px_log_fs_page_t page)
{
    px_log_fs_marker_t marker;
    px_log_fs_crc_t    crc;

    // Read marker (and set to BAD if invalid value)
    marker = px_log_fs_marker_rd(page, 0);
    if(  (marker == PX_LOG_FS_MARKER_FREE) || (marker == PX_LOG_FS_MARKER_BAD)  )
    {
        return marker;
    }

    // Read page header
    px_log_fs_glue_rd(&log_fs.page_header,
                      page,
                      0,
                      sizeof(px_log_fs_page_header_t));

    // Check CRC
    crc = px_log_fs_crc_page_header();
    if(crc != log_fs.page_header.crc)
    {
        // Mark page header as BAD
        PX_DBG_ERR("Page header CRC check failed (page %u): wr 0x%02X, rd 0x%02X",
                   page, log_fs.page_header.crc, crc);
        px_log_fs_marker_wr(PX_LOG_FS_MARKER_BAD, page, 0);
        return PX_LOG_FS_MARKER_BAD;
    }

    // Return marker
    return log_fs.page_header.marker;
}

static bool px_log_fs_page_header_wr(px_log_fs_page_t page)
{
    px_log_fs_page_header_t page_header_rd;

    // Calculate CRC
    log_fs.page_header.crc = px_log_fs_crc_page_header();

    PX_DBG_INFO("Writing page header (page %u, marker 0x%02X, nr %u, crc 0x%02X)",
                page, log_fs.page_header.marker, log_fs.page_header.nr, 
                log_fs.page_header.crc);

    // Write page header
    px_log_fs_glue_wr(&log_fs.page_header,
                      page,
                      0,
                      sizeof(px_log_fs_page_header_t));

    // Read back page header
    px_log_fs_glue_rd(&page_header_rd,
                      page,
                      0,
                      sizeof(px_log_fs_page_header_t));

    // Match?
    if(memcmp(&log_fs.page_header, &page_header_rd, sizeof(px_log_fs_page_header_t)) == 0)
    {
        // Success
        return true;
    }

    // Mark page header as BAD
    PX_DBG_ERR("Page header write failed (page %u)", page);
    px_log_fs_marker_wr(PX_LOG_FS_MARKER_BAD, page, 0);

    // Failure
    return false;
}

static bool px_log_fs_file_entry_rd(px_log_fs_page_t page)
{
    // Read page header (and set to BAD if invalid)
    if(px_log_fs_page_header_rd(page) != PX_LOG_FS_MARKER_FILE)
    {
        return false;
    }

    // Read file info (located after page header)
    px_log_fs_glue_rd(&log_fs.file_info,
                      page,
                      sizeof(px_log_fs_page_header_t),
                      sizeof(px_log_fs_file_info_t));

    // CRC correct?
    if(log_fs.file_info.crc != px_log_fs_crc_file_info())
    {
        // Mark as BAD
        PX_DBG_ERR("File block CRC check failed (page %u)", page);
        px_log_fs_marker_wr(PX_LOG_FS_MARKER_BAD, page, 0);
        return false;
    }

    // Start page match?
    if(log_fs.file_info.file.start_page != page)
    {
        // Mark as BAD
        PX_DBG_ERR("File block start page does not match (page %u)", page);
        px_log_fs_marker_wr(PX_LOG_FS_MARKER_BAD, page, 0);
        return false;
    }

    // File block valid
    return true;
}

static bool px_log_fs_file_entry_wr(px_log_fs_page_t page)
{
    px_log_fs_file_info_t file_block_rd;

    // Set page marker
    log_fs.page_header.marker = PX_LOG_FS_MARKER_FILE;
    // Write page header
    if(!px_log_fs_page_header_wr(page))
    {
        return false;
    }

    // Set start page
    log_fs.file_info.file.start_page = page;
    // Set file block CRC
    log_fs.file_info.crc = px_log_fs_crc_file_info();
    // Write file block
    px_log_fs_glue_wr(&log_fs.file_info,
                      page,
                      sizeof(px_log_fs_page_header_t),
                      sizeof(px_log_fs_file_info_t));
    // Read back file block
    px_log_fs_glue_rd(&file_block_rd,
                      page,
                      sizeof(px_log_fs_page_header_t),
                      sizeof(px_log_fs_file_info_t));
    // Match?
    if(memcmp(&log_fs.file_info, &file_block_rd, sizeof(px_log_fs_file_info_t)) == 0)
    {
        // Success
        return true;
    }

    // Mark page as BAD
    PX_DBG_ERR("File info write failed (page %u)", page);
    px_log_fs_marker_wr(PX_LOG_FS_MARKER_BAD, page, 0);

    // Failure
    return false;
}

static bool px_log_fs_record_entry_rd(px_log_fs_page_t   page,
                                      px_log_fs_offset_t offset)
{
    // Read marker (and set to BAD if invalid value)
    if(px_log_fs_marker_rd(page, offset) != PX_LOG_FS_MARKER_RECORD)
    {
        return false;
    }
    // Read record entry
    px_log_fs_glue_rd(&log_fs.record_block,
                      page,
                      offset,
                      sizeof(px_log_fs_rec_block_t));
    // CRC correct?
    if(log_fs.record_block.crc != px_log_fs_crc_record_block())
    {
        // Mark record as BAD
        PX_DBG_ERR("Record block CRC check failed (page %u, offset %u)", page, offset);
        px_log_fs_marker_wr(PX_LOG_FS_MARKER_BAD, page, offset);
        return false;
    }

    // Record valid
    return true;
}

static bool px_log_fs_record_entry_wr(px_log_fs_page_t   page,
                                      px_log_fs_offset_t offset)
{
    px_log_fs_rec_block_t record_block_rd;

    // Sanity check
    if(offset + sizeof(px_log_fs_rec_block_t) >= PX_LOG_FS_CFG_PAGE_SIZE)
    {
        PX_DBG_ERR("Record block will overflow the page");
        return false;
    }

    // Set record block marker
    log_fs.record_block.marker = PX_LOG_FS_MARKER_RECORD;
    // Set record block CRC
    log_fs.record_block.crc = px_log_fs_crc_record_block();
    // Write record entry
    px_log_fs_glue_wr(&log_fs.record_block,
                      page,
                      offset,
                      sizeof(px_log_fs_rec_block_t));
    // Read back record entry
    px_log_fs_glue_rd(&record_block_rd,
                      page,
                      offset,
                      sizeof(px_log_fs_rec_block_t));
    // Match?
    if(memcmp(&log_fs.record_block, &record_block_rd, sizeof(px_log_fs_rec_block_t)) != 0)
    {
        // Mark record as BAD
        PX_DBG_ERR("Record block write failed (page %u, offset %u)", page, offset);
        px_log_fs_marker_wr(PX_LOG_FS_MARKER_BAD, page, offset);
        // Failure
        return false;
    }

    // Success
    return true;
}

static px_log_fs_page_t px_log_fs_record_pages_bound_start(void)
{
    px_log_fs_page_t file_page = log_fs.file_info.file.start_page;

#if (PX_LOG_FS_CFG_TYPE == PX_LOG_FS_CFG_TYPE_LINEAR)
    // First page after file page
    return px_log_fs_page_next(file_page);
#endif
#if (PX_LOG_FS_CFG_TYPE == PX_LOG_FS_CFG_TYPE_CIRCULAR)
    // First erase block after file page
    return px_log_fs_erase_block_next(file_page);
#endif
}

static px_log_fs_page_t px_log_fs_record_pages_bound_end(void)
{
    px_log_fs_page_t file_page = log_fs.file_info.file.start_page;

#if (PX_LOG_FS_CFG_MAX_PAGES != 0)
    file_page += PX_LOG_FS_CFG_MAX_PAGES;
    // Wrap?
    if(file_page > PX_LOG_FS_CFG_PAGE_END)
    {
        file_page -= (PX_LOG_FS_CFG_PAGE_END - PX_LOG_FS_CFG_PAGE_START);
    }
    return file_page;
#else
    return px_log_fs_page_previous(file_page);
#endif    
}

static px_log_fs_page_t px_log_fs_page_header_find_first(px_log_fs_marker_t marker,
                                                         px_log_fs_page_t   page_start, 
                                                         px_log_fs_page_t   page_end)
{
    // Start at first page
    px_log_fs_page_t page = page_start;

    // Sanity checks
    PX_DBG_ASSERT((page_start >= PX_LOG_FS_CFG_PAGE_START) && (page_start <= PX_LOG_FS_CFG_PAGE_END));
    PX_DBG_ASSERT((page_end   >= PX_LOG_FS_CFG_PAGE_START) && (page_end   <= PX_LOG_FS_CFG_PAGE_END));

    while(true)
    {
        // Read page header. Does it match specified marker?
        if(px_log_fs_page_header_rd(page) == marker)
        {
            // Marker found
            return page;
        }
        // Last page read?
        if(page == page_end)
        {
            // Marker not found
            return PX_LOG_FS_PAGE_INVALID;
        }
        // Next page
        page = px_log_fs_page_next(page);
    }
}

static px_log_fs_page_t px_log_fs_page_header_find_next(px_log_fs_marker_t marker,
                                                        px_log_fs_page_t   page_start, 
                                                        px_log_fs_page_t   page_end,
                                                        px_log_fs_page_t   page_current)
{
    // Start at current page
    px_log_fs_page_t page = page_current;

    // Sanity checks
    PX_DBG_ASSERT((page_start   >= PX_LOG_FS_CFG_PAGE_START) && (page_start   <= PX_LOG_FS_CFG_PAGE_END));
    PX_DBG_ASSERT((page_end     >= PX_LOG_FS_CFG_PAGE_START) && (page_end     <= PX_LOG_FS_CFG_PAGE_END));
    PX_DBG_ASSERT((page_current >= PX_LOG_FS_CFG_PAGE_START) && (page_current <= PX_LOG_FS_CFG_PAGE_END));

    while(true)
    {
        // End page?
        if(page == page_end)
        {
            // Wrap to start page
            page = page_start;
        }
        else
        {
            // Next page
            page = px_log_fs_page_next(page);
        }

        // Wrapped all the way around and now back at current page?
        if(page == page_current)
        {
            // Marker not found
            return PX_LOG_FS_PAGE_INVALID;
        }
        
        // Read page header. Does it match specified marker?
        if(px_log_fs_page_header_rd(page) == marker)
        {
            // Marker found
            return page;
        }
    }
}                        

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
px_log_fs_err_t px_log_fs_init(void)
{
    px_log_fs_page_t file_page_first;
    px_log_fs_page_t file_page;
    px_log_fs_page_t file_page_next;

    px_log_fs_nr_t   file_nr;
    px_log_fs_nr_t   file_nr_next;
    px_log_fs_nr_t   file_nr_diff;
    px_log_fs_nr_t   file_nr_diff_largest;

    // Report structure sizes so that correct packing of structures can be verified
    PX_DBG_INFO("Page header size: %u",  sizeof(px_log_fs_page_header_t));
    PX_DBG_INFO("File block size: %u",   sizeof(px_log_fs_file_info_t));
    PX_DBG_INFO("Record block size: %u", sizeof(px_log_fs_rec_block_t));
    PX_DBG_INFO("Record data size: %u",  PX_LOG_FS_CFG_REC_DATA_SIZE);
    PX_DBG_INFO("Records per page: %u",  PX_LOG_FS_REC_PAGE_DATA_SIZE / sizeof(px_log_fs_rec_block_t));

    // Sanity checks
    PX_DBG_ASSERT(sizeof(px_log_fs_rec_block_t) <= PX_LOG_FS_REC_PAGE_DATA_SIZE);    
    if(PX_LOG_FS_REC_PAGE_DATA_SIZE % sizeof(px_log_fs_rec_block_t) != 0)
    {
        PX_DBG_WARN("%u bytes will be wasted per page", 
                 PX_LOG_FS_REC_PAGE_DATA_SIZE % sizeof(px_log_fs_rec_block_t));
    }

    // Reset status
    memset(&log_fs, 0, sizeof(px_log_fs_t));
    
    /*
     * Find first page containing a file entry.
     * 
     * Side effect: page headers with invalid markers or bad CRC are marked as
     * PX_LOG_FS_MARKER_BAD.
     */
    file_page_first = px_log_fs_page_header_find_first(PX_LOG_FS_MARKER_FILE,
                                                       PX_LOG_FS_CFG_PAGE_START,
                                                       PX_LOG_FS_CFG_PAGE_END);


    // No files found?
    if(file_page_first == PX_LOG_FS_PAGE_INVALID)
    {
        PX_DBG_INFO("No files found");
        log_fs.file_page_first = PX_LOG_FS_PAGE_INVALID;
        log_fs.file_page_last  = PX_LOG_FS_PAGE_INVALID;
        return PX_LOG_FS_ERR_NONE;
    }

    // Reset variable to keep track of largest file number difference
    file_nr_diff_largest = 0;

    // Continue from first page found...
    file_page = file_page_first;
    do
    {
        // Is file page at the start of an erase block?
        if( (file_page & (PX_LOG_FS_CFG_ERASE_BLOCK_SIZE - 1)) != 0)
        {
            PX_DBG_ERR("File page %u not at the start of an erase block", file_page);
        }
        // Note file number of current file entry
        file_nr = log_fs.page_header.nr;
        /*
         * Find next page containing a file entry.
         *
         * Side effect: page headers with invalid markers or bad CRC are marked
         * as PX_LOG_FS_MARKER_BAD.
         */
        file_page_next = px_log_fs_page_header_find_next(PX_LOG_FS_MARKER_FILE,
                                                         PX_LOG_FS_CFG_PAGE_START,
                                                         PX_LOG_FS_CFG_PAGE_END,
                                                         file_page);
        // No other pages with file entries?
        if(file_page_next == PX_LOG_FS_PAGE_INVALID)
        {
            // Only one file found
            PX_DBG_INFO("One file at page %u (last nr %u)", file_page, file_nr);
            log_fs.file_page_first   = file_page;
            log_fs.file_page_last    = file_page;
            log_fs.file_page_nr_next = ++file_nr;
            return PX_LOG_FS_ERR_NONE;
        }
        // Note file number of next file entry
        file_nr_next = log_fs.page_header.nr;
        // Calculate file number difference
        file_nr_diff = file_nr_next - file_nr;
        // Largest file number difference so far?
        if(file_nr_diff_largest < file_nr_diff)
        {
            PX_DBG_INFO("Diff=%u, file_page=%u(nr=%u) ,file_page_next=%u (nr=%u)",
                     file_nr_diff,
                     file_page,      file_nr,
                     file_page_next, file_nr_next);

            // Note largest difference so far
            file_nr_diff_largest = file_nr_diff;
            // Save candidates for first and last file
            log_fs.file_page_first   = file_page_next;
            log_fs.file_page_last    = file_page;
            log_fs.file_page_nr_next = ++file_nr;
        }

        // Next comparison
        file_page = file_page_next;
    }
    while(file_page != file_page_first); // Full circle?

    // Report first and last file
    PX_DBG_INFO("First file at page %u", log_fs.file_page_first);
    PX_DBG_INFO("Last file at page %u",  log_fs.file_page_last);
    PX_DBG_INFO("Next file nr %u",       log_fs.file_page_nr_next);

    return PX_LOG_FS_ERR_NONE;
}

px_log_fs_err_t px_log_fs_create(const px_log_fs_time_stamp_t * time_stamp)
{
    px_log_fs_marker_t marker;
    px_log_fs_page_t   page;
    px_log_fs_page_t   page_last_record;
    px_log_fs_page_t   page_new_file;

    // Any existing file in the file system?
    if(log_fs.file_page_last != PX_LOG_FS_PAGE_INVALID)
    {
        // Find last record page after file page
        page_last_record = log_fs.file_page_last;
        page             = log_fs.file_page_last;
        do
        {
            // Next page
            page = px_log_fs_page_next(page);
            // Read page header
            marker = px_log_fs_page_header_rd(page);
            // Valid record page?
            if(marker == PX_LOG_FS_MARKER_RECORD)
            {
                // Remember last record page
                page_last_record = page;
            }
        }
        while(marker != PX_LOG_FS_MARKER_FILE);

        // Next erase block after last record page is potentially available
        page_new_file = px_log_fs_erase_block_next(page_last_record);

        // Is this page a file marker?
        if(px_log_fs_marker_rd(page_new_file, 0) == PX_LOG_FS_MARKER_FILE)
        {
            // Yes, file system is full
            PX_DBG_WARN("File system is full");
            return PX_LOG_FS_ERR_FULL;
        }

#if (PX_LOG_FS_CFG_TYPE == PX_LOG_FS_CFG_TYPE_LINEAR)
        // At least one more page available for records?
        if(px_log_fs_marker_rd(px_log_fs_page_next(page_new_file), 0) == PX_LOG_FS_MARKER_FILE)
        {
            // No, file system is full
            PX_DBG_WARN("File system is full");
            return PX_LOG_FS_ERR_FULL;
        }
#endif
#if (PX_LOG_FS_CFG_TYPE == PX_LOG_FS_CFG_TYPE_CIRCULAR)
        // At least one more erase block available for records?
        if(px_log_fs_marker_rd(px_log_fs_erase_block_next(page_new_file), 0) == PX_LOG_FS_MARKER_FILE)
        {
            // No, file system is full
            PX_DBG_WARN("File system is full");
            return PX_LOG_FS_ERR_FULL;
        }
#endif
    }
    else
    {
        // Find first free erase block
        page = PX_LOG_FS_ERASE_BLOCK_FIRST;
        do
        {
            // FREE marker?
            if(px_log_fs_marker_rd(page, 0) == PX_LOG_FS_MARKER_FREE)
            {
                // This page will be used to create a new file
                page_new_file = page;
                break;
            }
            // Next erase block
            page = px_log_fs_erase_block_next(page);
        }
        while(page <= PX_LOG_FS_ERASE_BLOCK_LAST);

        // No FREE markers found?
        if(page > PX_LOG_FS_ERASE_BLOCK_LAST)
        {
            // Start at first erase block
            page_new_file = PX_LOG_FS_ERASE_BLOCK_FIRST;            
        }
    }

    // Erase block (if not FREE)
    if(px_log_fs_marker_rd(page_new_file, 0) != PX_LOG_FS_MARKER_FREE)
    {
        PX_DBG_INFO("Erase block %u", page_new_file);
        px_log_fs_glue_erase_block(page_new_file);
    }

    // Populate file info
    log_fs.page_header.nr = log_fs.file_page_nr_next;
    memcpy(&log_fs.file_info.file.time_stamp, 
           time_stamp,
           sizeof(px_log_fs_time_stamp_t));

    // Write file page
    if(!px_log_fs_file_entry_wr(page_new_file))
    {
        PX_DBG_WARN("Write failed");
        return PX_LOG_FS_ERR_WRITE_FAIL;
    }
    PX_DBG_INFO("New file created at page %u (nr %u)", page_new_file, log_fs.page_header.nr);

    // Remember last file
    log_fs.file_page_last = page_new_file;
    // Is this also the first file?
    if(log_fs.file_page_first == PX_LOG_FS_PAGE_INVALID)
    {
        // Remember first file
        log_fs.file_page_first = page_new_file;
    }
    // Remember next file number
    log_fs.file_page_nr_next++;

    // Erase first record block (if not FREE)
#if (PX_LOG_FS_CFG_TYPE == PX_LOG_FS_CFG_TYPE_LINEAR)
    page = px_log_fs_page_next(page_new_file);
#endif
#if (PX_LOG_FS_CFG_TYPE == PX_LOG_FS_CFG_TYPE_CIRCULAR)
    page = px_log_fs_erase_block_next(page_new_file);
#endif
    if(px_log_fs_marker_rd(page, 0) != PX_LOG_FS_MARKER_FREE)
    {
        PX_DBG_INFO("Erase first record block %u", page);
        if(PX_LOG_FS_IS_ERASE_BLOCK_START_PAGE(page))
        {
            px_log_fs_glue_erase_block(page);
        }
        else
        {
            PX_DBG_ERR("Page %u is not at the start of an erase block!", page);
            return PX_LOG_FS_ERR_FILE_INVALID;
        }
    }

    // Reset read and write addresses
    log_fs.rec_page_first    = PX_LOG_FS_PAGE_INVALID;
    log_fs.rec_page_last     = PX_LOG_FS_PAGE_INVALID;
    log_fs.rec_page_nr_next  = 0;
    log_fs.rec_adr_rd.page   = PX_LOG_FS_PAGE_INVALID;
    log_fs.rec_adr_wr.page   = page;
    log_fs.rec_adr_wr.offset = PX_LOG_FS_REC_OFFSET_FIRST;

    return PX_LOG_FS_ERR_NONE;
}

size_t px_log_fs_file_attr_data_wr(px_log_fs_offset_t offset, 
                                   const void *       data, 
                                   size_t             nr_of_bytes)
{
    // Does offset exceed page size?
    if(offset >= PX_LOG_FS_FILE_ATTR_DATA_SIZE)
    {
        // No bytes could be written
        return 0;
    }
    // Does number of bytes exceed page size?
    if(nr_of_bytes > (PX_LOG_FS_FILE_ATTR_DATA_SIZE - offset))
    {
        // Clip number of bytes
        nr_of_bytes = PX_LOG_FS_FILE_ATTR_DATA_SIZE - offset;
    }
    // Write file attribute data
    px_log_fs_glue_wr(data, 
                      log_fs.file_info.file.start_page, 
                      sizeof(px_log_fs_page_header_t) + sizeof(px_log_fs_file_info_t) + offset,
                      nr_of_bytes);

    return nr_of_bytes;
}

size_t px_log_fs_file_attr_data_rd(px_log_fs_offset_t offset, 
                                   void *          data, 
                                   size_t          nr_of_bytes)
{
    // Does offset exceed page size?
    if(offset >= PX_LOG_FS_FILE_ATTR_DATA_SIZE)
    {
        // No bytes could be read
        return 0;
    }
    // Does number of bytes exceed page size?
    if(nr_of_bytes > (PX_LOG_FS_FILE_ATTR_DATA_SIZE - offset))
    {
        // Clip number of bytes
        nr_of_bytes = PX_LOG_FS_FILE_ATTR_DATA_SIZE - offset;
    }
    // Write file attribute data
    px_log_fs_glue_rd(data, 
                      log_fs.file_info.file.start_page, 
                      sizeof(px_log_fs_page_header_t) + sizeof(px_log_fs_file_info_t) + offset,
                      nr_of_bytes);

    return nr_of_bytes;
}

size_t px_log_fs_file_attr_data_size(void)
{
    return PX_LOG_FS_FILE_ATTR_DATA_SIZE;
}

px_log_fs_err_t px_log_fs_file_find_first(px_log_fs_file_t * file)
{
    // Any files?
    if(log_fs.file_page_first == PX_LOG_FS_PAGE_INVALID)
    {
        // No files
        return PX_LOG_FS_ERR_NO_FILE;
    }

    // Read file info
    if(!px_log_fs_file_entry_rd(log_fs.file_page_first))
    {
        // Invalid file info
        PX_DBG_ERR("File info was valid during px_log_fs_init()");
        return PX_LOG_FS_ERR_FILE_INVALID;
    }

    // Return file info
    memcpy(file, &log_fs.file_info.file, sizeof(px_log_fs_file_t));
    PX_DBG_INFO("First file nr %u found at page %u", log_fs.page_header.nr, log_fs.file_info.file.start_page);
    return PX_LOG_FS_ERR_NONE;
}

px_log_fs_err_t px_log_fs_file_find_last(px_log_fs_file_t * file)
{
    // Any files?
    if(log_fs.file_page_last == PX_LOG_FS_PAGE_INVALID)
    {
        // No files
        return PX_LOG_FS_ERR_NO_FILE;
    }

    // Read file info
    if(!px_log_fs_file_entry_rd(log_fs.file_page_last))
    {
        PX_DBG_ERR("File info was valid during px_log_fs_init()");
        return PX_LOG_FS_ERR_FILE_INVALID;
    }

    // Return file info
    memcpy(file, &log_fs.file_info.file, sizeof(px_log_fs_file_t));
    PX_DBG_INFO("Last file nr %u found at page %u", log_fs.page_header.nr, log_fs.file_info.file.start_page);
    return PX_LOG_FS_ERR_NONE;
}

px_log_fs_err_t px_log_fs_file_find_next(px_log_fs_file_t * file)
{
    px_log_fs_page_t page;

    // Any files?
    if(log_fs.file_page_last == PX_LOG_FS_PAGE_INVALID)
    {
        // No files
        return PX_LOG_FS_ERR_NO_FILE;
    }

    // Save page of current file
    page = log_fs.file_info.file.start_page;

    // Was current file the last?
    if(page == log_fs.file_page_last)
    {
        // No next file
        return PX_LOG_FS_ERR_NO_FILE;
    }

    // Start with current file page
    do
    {
        // Next erase block
        page = px_log_fs_erase_block_next(page);        

        // Read (possible) file entry at start of page
        if(px_log_fs_file_entry_rd(page))
        {
            // Return file info
            memcpy(file, &log_fs.file_info.file, sizeof(px_log_fs_file_t));
            PX_DBG_INFO("Next file nr %u found at page %u", log_fs.page_header.nr, log_fs.file_info.file.start_page);
            return PX_LOG_FS_ERR_NONE;
        }
    }
    while(page != log_fs.file_page_last);

    // No file entry found
    return PX_LOG_FS_ERR_NO_FILE;
}

px_log_fs_err_t px_log_fs_file_find_previous(px_log_fs_file_t * file)
{
    px_log_fs_page_t page;

    // Any files?
    if(log_fs.file_page_first == PX_LOG_FS_PAGE_INVALID)
    {
        // No files
        return PX_LOG_FS_ERR_NO_FILE;
    }

    // Save page of current file block
    page = log_fs.file_info.file.start_page;

    // Was current file the first?
    if(page == log_fs.file_page_first)
    {
        // No next file
        return PX_LOG_FS_ERR_NO_FILE;
    }

    // Start with current file page
    do
    {
        // Previous erase block
        page = px_log_fs_erase_block_previous(page);

        // Read (possible) file entry at start of page
        if(px_log_fs_file_entry_rd(page))
        {
            // Return file info
            memcpy(file, &log_fs.file_info.file, sizeof(px_log_fs_file_t));
            PX_DBG_INFO("Previous file nr %u found at page %u", log_fs.page_header.nr, log_fs.file_info.file.start_page);
            return PX_LOG_FS_ERR_NONE;
        }
    }
    while(page != log_fs.file_page_first);

    // No file entry found
    return PX_LOG_FS_ERR_NO_FILE;
}

px_log_fs_err_t px_log_fs_open(const px_log_fs_file_t * file)
{
    px_log_fs_page_t   rec_search_page_start;
    px_log_fs_page_t   rec_search_page_end;

    px_log_fs_page_t   rec_page;
    px_log_fs_page_t   rec_page_first;
    px_log_fs_page_t   rec_page_next;

    px_log_fs_nr_t     rec_nr;
    px_log_fs_nr_t     rec_nr_next;
    px_log_fs_nr_t     rec_nr_diff;
    px_log_fs_nr_t     rec_nr_diff_largest;

    px_log_fs_marker_t marker;
    px_log_fs_offset_t offset;
    
    // Sanity check
    if(file == NULL)
    {
        PX_DBG_ERR("Invalid file");
        return PX_LOG_FS_ERR_FILE_INVALID;
    }

    // Read file entry (again)
    PX_DBG_INFO("File start page=%u", file->start_page);
    if(!px_log_fs_file_entry_rd(file->start_page))
    {
        PX_DBG_ERR("Invalid file");
        return PX_LOG_FS_ERR_FILE_INVALID;
    }    

    // Start search for record pages after file page
    rec_search_page_start = px_log_fs_record_pages_bound_start();

    // Two consecutive file pages?
    if(px_log_fs_page_header_rd(rec_search_page_start) == PX_LOG_FS_MARKER_FILE)
    {
        // File is empty
        PX_DBG_INFO("File is empty");
        log_fs.rec_page_first    = PX_LOG_FS_PAGE_INVALID;
        log_fs.rec_page_last     = PX_LOG_FS_PAGE_INVALID;
        log_fs.rec_page_nr_next  = 0;
        log_fs.rec_adr_rd.page   = PX_LOG_FS_PAGE_INVALID;
        log_fs.rec_adr_wr.page   = rec_search_page_start;
        log_fs.rec_adr_wr.offset = PX_LOG_FS_REC_OFFSET_FIRST;
        return PX_LOG_FS_ERR_NONE;
    }

    // End search for record pages before file page (or boundary if PX_LOG_FS_CFG_MAX_PAGES != 0)
    rec_search_page_end = px_log_fs_record_pages_bound_end();

    // Is there more than one file in the file system?
    if(log_fs.file_page_first != log_fs.file_page_last)
    {
        // Find next file page
        rec_search_page_end = px_log_fs_page_header_find_first(PX_LOG_FS_MARKER_FILE,
                                                               rec_search_page_start,
                                                               log_fs.file_info.file.start_page);
        // Two consecutive file pages?
        if(rec_search_page_end == rec_search_page_start)
        {
            // File is empty
            PX_DBG_INFO("File is empty");
            log_fs.rec_page_first    = PX_LOG_FS_PAGE_INVALID;
            log_fs.rec_page_last     = PX_LOG_FS_PAGE_INVALID;
            log_fs.rec_page_nr_next  = 0;
            log_fs.rec_adr_rd.page   = PX_LOG_FS_PAGE_INVALID;
            log_fs.rec_adr_wr.page   = rec_search_page_start;
            log_fs.rec_adr_wr.offset = PX_LOG_FS_REC_OFFSET_FIRST;
            return PX_LOG_FS_ERR_NONE;
        }
        // Next file page found?
        if(rec_search_page_end != PX_LOG_FS_PAGE_INVALID)
        {
            // End search just before next file page
            rec_search_page_end = px_log_fs_page_previous(rec_search_page_end);
            
        }
        else
        {
            // End search for record pages before file page (or boundary if PX_LOG_FS_CFG_MAX_PAGES != 0)
            rec_search_page_end = px_log_fs_record_pages_bound_end();
        }        
    }
    PX_DBG_INFO("rec_search_page_start=%u", rec_search_page_start);
    PX_DBG_INFO("rec_search_page_end=%u",   rec_search_page_end);

    // Find first valid record page
    rec_page_first = px_log_fs_page_header_find_first(PX_LOG_FS_MARKER_RECORD,
                                                      rec_search_page_start,
                                                      rec_search_page_end);


    // No record pages found?
    if(rec_page_first == PX_LOG_FS_PAGE_INVALID)
    {
        PX_DBG_INFO("No record pages found");
        log_fs.rec_page_first    = PX_LOG_FS_PAGE_INVALID;
        log_fs.rec_page_last     = PX_LOG_FS_PAGE_INVALID;
        log_fs.rec_page_nr_next  = 0;
        log_fs.rec_adr_rd.page   = PX_LOG_FS_PAGE_INVALID;
        log_fs.rec_adr_wr.page   = rec_search_page_start;
        log_fs.rec_adr_wr.offset = PX_LOG_FS_REC_OFFSET_FIRST;

        // First record page not FREE?
        while(px_log_fs_marker_rd(log_fs.rec_adr_wr.page, 0) != PX_LOG_FS_MARKER_FREE)
        {
            // Start of an erase block?
            if(PX_LOG_FS_IS_ERASE_BLOCK_START_PAGE(log_fs.rec_adr_wr.page))
            {
                // Erase record block
                PX_DBG_INFO("Erase first record block %u", rec_search_page_start);
                px_log_fs_glue_erase_block(rec_search_page_start);
                break;
            }
            // Next page
            log_fs.rec_adr_wr.page = px_log_fs_page_next(log_fs.rec_adr_wr.page);
        }

        return PX_LOG_FS_ERR_NONE;
    }    

    // Reset variable to keep track of largest record number difference
    rec_nr_diff_largest = 0;

    // Continue from first page found...
    rec_page = rec_page_first;
    do
    {
        // Note record number of current record page entry
        rec_nr = log_fs.page_header.nr;

        // Find next page containing a record page entry
        rec_page_next = px_log_fs_page_header_find_next(PX_LOG_FS_MARKER_RECORD,
                                                        rec_search_page_start,
                                                        rec_search_page_end,
                                                        rec_page);
        // No other pages with record page entries?
        if(rec_page_next == PX_LOG_FS_PAGE_INVALID)
        {
            // Only one record page found
            PX_DBG_INFO("One record page %u (last nr %u)", rec_page, rec_nr);
            log_fs.rec_page_first    = rec_page;
            log_fs.rec_page_last     = rec_page;
            log_fs.rec_page_nr_next  = ++rec_nr;
            break;
        }

        // Note record number of next record page entry
        rec_nr_next = log_fs.page_header.nr;

        // Calculate record number difference
        rec_nr_diff = rec_nr_next - rec_nr;

        // Largest record number difference so far?
        if(rec_nr_diff_largest < rec_nr_diff)
        {
            PX_DBG_INFO("Diff=%u, rec_page=%u(nr=%u) ,rec_page_next=%u (nr=%u)",
                     rec_nr_diff,
                     rec_page,      rec_nr,
                     rec_page_next, rec_nr_next);

            // Note largest difference so far
            rec_nr_diff_largest = rec_nr_diff;
            // Save candidates for first and last record page
            log_fs.rec_page_first    = rec_page_next;
            log_fs.rec_page_last     = rec_page;
            log_fs.rec_page_nr_next  = ++rec_nr;
        }

        // Next comparison
        rec_page = rec_page_next;
    }
    while(rec_page != rec_page_first); // Full circle?

    PX_DBG_INFO("rec_page_first = %u",    log_fs.rec_page_first);
    PX_DBG_INFO("rec_page_last = %u",     log_fs.rec_page_last);
    PX_DBG_INFO("rec_page_nr_next = %u",  log_fs.rec_page_nr_next);

    // Find next free record position
    rec_page = log_fs.rec_page_last;
    offset   = PX_LOG_FS_REC_OFFSET_FIRST;
    while(offset <= PX_LOG_FS_REC_OFFSET_LAST)
    {
        marker = px_log_fs_marker_rd(rec_page, offset);
        if(marker == PX_LOG_FS_MARKER_FREE)
        {
            break;
        }
        offset += sizeof(px_log_fs_rec_block_t);
    }

    // Free space found?
    if(marker == PX_LOG_FS_MARKER_FREE)
    {
        // Save location of first free record entry
        log_fs.rec_adr_wr.page   = rec_page;
        log_fs.rec_adr_wr.offset = offset;
    }
    else
    {
        // Last record page is full
        PX_DBG_INFO("Last rec page (%u) is full", rec_page);
        // Next page
        rec_page = px_log_fs_page_next(rec_page);

#if (PX_LOG_FS_CFG_TYPE == PX_LOG_FS_CFG_TYPE_LINEAR)
        // Space available?
        if(rec_page != px_log_fs_page_next(px_log_fs_record_pages_bound_end()))
        {
            marker = px_log_fs_marker_rd(rec_page, 0);
            if(marker == PX_LOG_FS_MARKER_FILE)
            {
                PX_DBG_INFO("File is full. More data cannot be appended");
            }
            else if(marker != PX_LOG_FS_MARKER_FREE)
            {
                // Erase next page
                PX_DBG_INFO("Erase page %u", rec_page);
                px_log_fs_glue_erase_block(rec_page);
            }
        }
#endif

#if (PX_LOG_FS_CFG_TYPE == PX_LOG_FS_CFG_TYPE_CIRCULAR)
        // Maximum space reached?
        if(  (rec_page == px_log_fs_page_next(px_log_fs_record_pages_bound_end()))
           ||(px_log_fs_marker_rd(rec_page, 0) == PX_LOG_FS_MARKER_FILE          )  )
        {
            // Wrap
            rec_page = px_log_fs_record_pages_bound_start();
        }
        // Erase next page
        PX_DBG_INFO("Erase page %u", rec_page);
        px_log_fs_glue_erase_block(rec_page);
        // First page erased?
        if(rec_page == log_fs.rec_page_first)
        {
            // Find next page containing a record page entry
            log_fs.rec_page_first = px_log_fs_page_header_find_next(PX_LOG_FS_MARKER_RECORD,
                                                                    rec_search_page_start,
                                                                    rec_search_page_end,
                                                                    rec_page);
        }
#endif

        // Next record write address
        log_fs.rec_adr_wr.page   = rec_page;
        log_fs.rec_adr_wr.offset = PX_LOG_FS_REC_OFFSET_FIRST;
    }

    PX_DBG_INFO("rec_adr_wr.page=%u",   log_fs.rec_adr_wr.page);
    PX_DBG_INFO("rec_adr_wr.offset=%u", log_fs.rec_adr_wr.offset);

    return PX_LOG_FS_ERR_NONE;
}

px_log_fs_err_t px_log_fs_file_delete(const px_log_fs_file_t * file)
{
    px_log_fs_marker_t marker;
    px_log_fs_page_t   page = file->start_page;

    // Not the oldest file?
    if(page != log_fs.file_page_first)
    {
        PX_DBG_ERR("Not the oldest file");
        return PX_LOG_FS_ERR_FILE_INVALID;
    }

    // (Sanity check) valid file page?
    if(!px_log_fs_file_entry_rd(page))
    {
        PX_DBG_ERR("Invalid file");
        return PX_LOG_FS_ERR_FILE_INVALID;
    }

    // Only one file in file system?
    if(log_fs.file_page_first == log_fs.file_page_last)
    {
        PX_DBG_INFO("Deleting last file");
        // No files in file system anymore (after this one is deleted)
        log_fs.file_page_first = PX_LOG_FS_PAGE_INVALID;
        log_fs.file_page_last  = PX_LOG_FS_PAGE_INVALID;
    }
    else
    {
        // Start before next file page
        page = px_log_fs_page_header_find_next(PX_LOG_FS_MARKER_FILE,
                                               log_fs.file_page_first,
                                               log_fs.file_page_last,
                                               log_fs.file_page_first);

        // Next file will be the first (after this one is deleted)
        log_fs.file_page_first = page;
        PX_DBG_INFO("First file will be at page %u", log_fs.file_page_first);
    }

    // Reverse delete all records
    do
    {
        // Previous page
        page = px_log_fs_page_previous(page);

        // Read page header
        marker = px_log_fs_page_header_rd(page);

        if(marker == PX_LOG_FS_MARKER_RECORD)
        {
            // Mark RECORD page as BAD
            PX_DBG_INFO("Marking RECORD page %u as BAD", page);
            px_log_fs_marker_wr(PX_LOG_FS_MARKER_BAD, page, 0);
        }
    }
    while(marker != PX_LOG_FS_MARKER_FILE);
    
    // Mark FILE page as BAD
    PX_DBG_INFO("Marking FILE page %u as BAD", page);
    px_log_fs_marker_wr(PX_LOG_FS_MARKER_BAD, page, 0);

    return PX_LOG_FS_ERR_NONE;
}

px_log_fs_err_t px_log_fs_record_rd_first(void * data, size_t nr_of_bytes)
{
    // No record pages?
    if(log_fs.rec_page_first == PX_LOG_FS_PAGE_INVALID)
    {
        return PX_LOG_FS_ERR_NO_RECORD;
    }

    // Start before first record
    log_fs.rec_adr_rd.page   = px_log_fs_page_previous(log_fs.rec_page_first);
    log_fs.rec_adr_rd.offset = PX_LOG_FS_REC_OFFSET_LAST;    

    // Return first valid record
    return px_log_fs_record_rd_next(data, nr_of_bytes);
}

px_log_fs_err_t px_log_fs_record_rd_last(void * data, size_t nr_of_bytes)
{
    // No record pages?
    if(log_fs.rec_page_last == PX_LOG_FS_PAGE_INVALID)
    {
        return PX_LOG_FS_ERR_NO_RECORD;
    }

    // Start at free position
    log_fs.rec_adr_rd.page   = log_fs.rec_adr_wr.page;
    log_fs.rec_adr_rd.offset = log_fs.rec_adr_wr.offset;
    
    // Return first valid record
    return px_log_fs_record_rd_previous(data, nr_of_bytes);
}

px_log_fs_err_t px_log_fs_record_rd_next(void * data, size_t nr_of_bytes)
{
    px_log_fs_marker_t marker;
    px_log_fs_page_t   page   = log_fs.rec_adr_rd.page;
    px_log_fs_page_t   offset = log_fs.rec_adr_rd.offset;

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
        // End of page?
        if(offset >= PX_LOG_FS_REC_OFFSET_LAST)
        {
            // Start record reading after page header
            offset = PX_LOG_FS_REC_OFFSET_FIRST;
            
            // Next record page
            do
            {

#if (PX_LOG_FS_CFG_TYPE == PX_LOG_FS_CFG_TYPE_LINEAR)
                // Next page
                page = px_log_fs_page_next(page);
                // Wrapped completely?
                if(page == log_fs.rec_adr_rd.page)
                {
                    // Should have stopped at file marker, but it was not found
                    PX_DBG_ERR("Valid file marker not found");
                    log_fs.rec_adr_rd.page = PX_LOG_FS_PAGE_INVALID;
                    // Stop
                    return PX_LOG_FS_ERR_NO_RECORD;
                }
#endif

#if (PX_LOG_FS_CFG_TYPE == PX_LOG_FS_CFG_TYPE_CIRCULAR)
                // End page reached?
                if(page == px_log_fs_record_pages_bound_end())
                { 
                    // Wrap to start page
                    page = px_log_fs_record_pages_bound_start();
                }
                else
                {
                    // Next page
                    page = px_log_fs_page_next(page);
                }
                // Has last record been read?
                if(  (page   == log_fs.rec_adr_wr.page  )
                   &&(offset == log_fs.rec_adr_wr.offset)  )
                {
                    // Stop
                    return PX_LOG_FS_ERR_NO_RECORD;
                }
#endif

                // Read page header
                marker = px_log_fs_page_header_rd(page);

#if (PX_LOG_FS_CFG_TYPE == PX_LOG_FS_CFG_TYPE_LINEAR)
                // Next file reached?
                if(marker == PX_LOG_FS_MARKER_FILE)
                {
                    // Stop
                    return PX_LOG_FS_ERR_NO_RECORD;
                }
#endif

            }
            while(marker != PX_LOG_FS_MARKER_RECORD);
        }
        else
        {
            // Next record address
            offset += sizeof(px_log_fs_rec_block_t);        
        }

        // Has last record been read?
        if(  (page   == log_fs.rec_adr_wr.page  )
           &&(offset == log_fs.rec_adr_wr.offset)  )
        {
            // Stop
            return PX_LOG_FS_ERR_NO_RECORD;
        }

        // Read record
        if(px_log_fs_record_entry_rd(page, offset))
        {
            // Next valid record found
            log_fs.rec_adr_rd.page   = page;
            log_fs.rec_adr_rd.offset = offset;
            // Copy content of record to user supplied buffer
            memcpy(data, &log_fs.record_block.data, nr_of_bytes);
            // Valid record
            return PX_LOG_FS_ERR_NONE;
        }
    }
}

px_log_fs_err_t px_log_fs_record_rd_previous(void * data, size_t nr_of_bytes)
{
    px_log_fs_marker_t marker;
    px_log_fs_page_t   page   = log_fs.rec_adr_rd.page;
    px_log_fs_page_t   offset = log_fs.rec_adr_rd.offset;

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
        // End of page?
        if(offset <= PX_LOG_FS_REC_OFFSET_FIRST)
        {
            // Start record reading at last offset
            offset = PX_LOG_FS_REC_OFFSET_LAST;
            
            // Previous record page
            do
            {
#if (PX_LOG_FS_CFG_TYPE == PX_LOG_FS_CFG_TYPE_LINEAR)
                // Previous page
                page = px_log_fs_page_previous(page);
                // Has first record been read?
                if(page == log_fs.file_info.file.start_page)
                {
                    // Stop
                    return PX_LOG_FS_ERR_NO_RECORD;
                }
#endif
#if (PX_LOG_FS_CFG_TYPE == PX_LOG_FS_CFG_TYPE_CIRCULAR)
                // Start page reached?
                if(page == px_log_fs_record_pages_bound_start())
                { 
                    // Wrap to end page
                    page = px_log_fs_record_pages_bound_end();
                }
                else
                {
                    // Previous page
                    page = px_log_fs_page_previous(page);
                }
                // Has first record been read?
                if(page == log_fs.rec_adr_wr.page)
                {
                    // Stop
                    return PX_LOG_FS_ERR_NO_RECORD;
                }
#endif
                // Read page header
                marker = px_log_fs_page_header_rd(page);
            }
            while(marker != PX_LOG_FS_MARKER_RECORD);            
        }
        else
        {
            // Previous record address
            offset -= sizeof(px_log_fs_rec_block_t);        
        }

        // Read record
        if(px_log_fs_record_entry_rd(page, offset))
        {
            // Next valid record found
            log_fs.rec_adr_rd.page   = page;
            log_fs.rec_adr_rd.offset = offset;
            // Copy content of record to user supplied buffer
            memcpy(data, &log_fs.record_block.data, nr_of_bytes);
            // Valid record
            return PX_LOG_FS_ERR_NONE;
        }
    }
}

px_log_fs_err_t px_log_fs_record_wr(const void * data, size_t nr_of_bytes)
{
    size_t             i;
    px_log_fs_marker_t marker;
    px_log_fs_page_t   page    = log_fs.rec_adr_wr.page;
    px_log_fs_page_t   offset  = log_fs.rec_adr_wr.offset;
    uint8_t *          data_u8 = (uint8_t *)data;
    bool               success;

    // Record too small to hold all of the supplied data?
    if(nr_of_bytes > PX_LOG_FS_CFG_REC_DATA_SIZE)
    {
        PX_DBG_ERR("Record size too small and %u bytes will be discarded",
                (nr_of_bytes - PX_LOG_FS_CFG_REC_DATA_SIZE));
    }

    // First record to write in page?
    if(offset == PX_LOG_FS_REC_OFFSET_FIRST)
    {
        // Read page's marker
        marker = px_log_fs_marker_rd(page, 0);
 
#if (PX_LOG_FS_CFG_TYPE == PX_LOG_FS_CFG_TYPE_LINEAR)
        // Maximum space reached?
        if(  (marker == PX_LOG_FS_MARKER_FILE                               )
           ||(page   == px_log_fs_page_next(px_log_fs_record_pages_bound_end()))  )
        {
            // File system full
            PX_DBG_WARN("File system is full");
            return PX_LOG_FS_ERR_FULL;
        }
#endif

        // Page marked as BAD?
        if(marker == PX_LOG_FS_MARKER_BAD)
        {
            PX_DBG_INFO("Erase page %u", page);
            px_log_fs_glue_erase_block(page);
        }

        // Write record page header
        log_fs.page_header.marker  = PX_LOG_FS_MARKER_RECORD;
        log_fs.page_header.nr      = log_fs.rec_page_nr_next;
        if(!px_log_fs_page_header_wr(page))
        {
            // Failed to write record page header. Move on to next page
            page = px_log_fs_page_next(page);

            // Read next page's marker
            marker = px_log_fs_marker_rd(page, 0);

            // Maximum space reached?
            if(  (marker == PX_LOG_FS_MARKER_FILE                               )
               ||(page   == px_log_fs_page_next(px_log_fs_record_pages_bound_end()))  )
            {
#if (PX_LOG_FS_CFG_TYPE == PX_LOG_FS_CFG_TYPE_LINEAR)
                // File system full
                PX_DBG_WARN("File system is full");
                return PX_LOG_FS_ERR_FULL;
#endif

#if (PX_LOG_FS_CFG_TYPE == PX_LOG_FS_CFG_TYPE_CIRCULAR)
                // Wrap
                page = px_log_fs_record_pages_bound_start();
#endif
            }

            // Erase next page
            PX_DBG_INFO("Erase page %u", page);
            px_log_fs_glue_erase_block(page);
            // Next record write page (record number is not incremented)
            log_fs.rec_adr_wr.page = page;

#if (PX_LOG_FS_CFG_TYPE == PX_LOG_FS_CFG_TYPE_CIRCULAR)
            // First record page erased?
            if(page == log_fs.rec_page_first)
            {
                // Next record page will be the first
                log_fs.rec_page_first = px_log_fs_page_header_find_next(PX_LOG_FS_MARKER_RECORD,
                                                                        px_log_fs_record_pages_bound_start(),
                                                                        px_log_fs_record_pages_bound_end(),
                                                                        log_fs.rec_page_first);
                PX_DBG_INFO("rec_page_first=%u", log_fs.rec_page_first);
            }
#endif
            // Failed to write record page header correctly
            PX_DBG_WARN("Write failed");
            return PX_LOG_FS_ERR_WRITE_FAIL;
        }
        PX_DBG_INFO("Page %u written with RECORD page header (nr=%u)",
                    log_fs.rec_adr_wr.page,
                    log_fs.page_header.nr);
        // Next record number
        log_fs.rec_page_nr_next++;
        PX_DBG_INFO("rec_page_nr_next=%u", log_fs.rec_page_nr_next);

        // Update last record page
        log_fs.rec_page_last = page;
        PX_DBG_INFO("rec_page_last=%u", log_fs.rec_page_last);
        // Is this also the first record page?
        if(log_fs.rec_page_first == PX_LOG_FS_PAGE_INVALID)
        {
            log_fs.rec_page_first = page;
            PX_DBG_INFO("rec_page_first=%u", log_fs.rec_page_first);
        }
        
    }

    // Copy data
    for(i=0; i<PX_LOG_FS_CFG_REC_DATA_SIZE; i++)
    {
        // Byte to copy?
        if(nr_of_bytes != 0)
        {
            nr_of_bytes--;
            log_fs.record_block.data[i] = *data_u8++;
        }
        else
        {
            // Fill rest of array with 0xff
            log_fs.record_block.data[i] = 0xff;
        }
    }    
    
    // Write record
    PX_DBG_INFO("Write record (page %u, offset %u)", page, offset);
    success = px_log_fs_record_entry_wr(page, offset);

    // Last record in page?
    if(offset == PX_LOG_FS_REC_OFFSET_LAST)
    {
        // Next page
        page = px_log_fs_page_next(page);

#if (PX_LOG_FS_CFG_TYPE == PX_LOG_FS_CFG_TYPE_LINEAR)
        // Space available?
        if(  (page != px_log_fs_page_next(px_log_fs_record_pages_bound_end()))
           &&(px_log_fs_marker_rd(page, 0) != PX_LOG_FS_MARKER_FILE          )  )
        {
            // Erase next page
            PX_DBG_INFO("Erase page %u", page);
            px_log_fs_glue_erase_block(page);
        }
#endif

#if (PX_LOG_FS_CFG_TYPE == PX_LOG_FS_CFG_TYPE_CIRCULAR)
        // Maximum space reached?
        if(  (page == px_log_fs_page_next(px_log_fs_record_pages_bound_end()))
           ||(px_log_fs_marker_rd(page, 0) == PX_LOG_FS_MARKER_FILE          )  )
        {
            // Wrap
            page = px_log_fs_record_pages_bound_start();
        }
        // Erase next page
        PX_DBG_INFO("Erase page %u", page);
        px_log_fs_glue_erase_block(page);

        // First record page erased?
        if(page == log_fs.rec_page_first)
        {
            // Next record page will be the first
            log_fs.rec_page_first = px_log_fs_page_header_find_next(PX_LOG_FS_MARKER_RECORD,
                                                                    px_log_fs_record_pages_bound_start(),
                                                                    px_log_fs_record_pages_bound_end(),
                                                                    log_fs.rec_page_first);
            PX_DBG_INFO("rec_page_first=%u", log_fs.rec_page_first);
        }
#endif

        // Next record write address
        log_fs.rec_adr_wr.page   = page;
        log_fs.rec_adr_wr.offset = PX_LOG_FS_REC_OFFSET_FIRST;        
    }
    else
    {
        // Next offset
        log_fs.rec_adr_wr.offset = offset + sizeof(px_log_fs_rec_block_t);
    }
    
    // Success?
    if(success)
    {
        return PX_LOG_FS_ERR_NONE;
    }
    else
    {
        PX_DBG_WARN("Write failed");
        return PX_LOG_FS_ERR_WRITE_FAIL;
    }
}

void px_log_fs_info(void)
{
    px_log_fs_page_t page;
    px_log_fs_marker_t marker;
    uint8_t i;

    PX_PRINTF_P("Page start: 0x%04X\n",   PX_LOG_FS_CFG_PAGE_START);
    PX_PRINTF_P("Page end: 0x%04X\n",     PX_LOG_FS_CFG_PAGE_END);
    PX_PRINTF_P("Page size: %u\n",        PX_AT45D_PAGE_SIZE);
    PX_PRINTF_P("Record size: %u\n",      sizeof(px_log_fs_rec_block_t));
    PX_PRINTF_P("Record data size: %u\n", PX_LOG_FS_CFG_REC_DATA_SIZE);
    PX_PRINTF_P("Records per page: %u\n", PX_LOG_FS_REC_PAGE_DATA_SIZE / sizeof(px_log_fs_rec_block_t));
    PX_PRINTF_P("First file: %u\n",       log_fs.file_page_first);
    PX_PRINTF_P("Last file: %u\n",        log_fs.file_page_last);

    PX_PRINTF_P("\n        ");
    for(i = 0; i < 16; i++)
    {
        PX_PRINTF_P("%01X      ", i);
    }
    putchar('\n');

    i=0;
    for(page = PX_LOG_FS_CFG_PAGE_START; page<=PX_LOG_FS_CFG_PAGE_END; page++)
    {
        if(i == 0)
        {
            PX_PRINTF_P("\n0x%04X: ", page);
        }

        marker = px_log_fs_page_header_rd(page);
        switch(marker)
        {
        case PX_LOG_FS_MARKER_FREE:
            PX_PRINTF_P("x      ");
            break;
        case PX_LOG_FS_MARKER_FILE:
            PX_PRINTF_P("F %04x ", log_fs.page_header.nr);
            break;
        case PX_LOG_FS_MARKER_RECORD:
            PX_PRINTF_P("R %04x ", log_fs.page_header.nr);
            break;
        case PX_LOG_FS_MARKER_BAD:
            // Fall through...
        default:
            PX_PRINTF_P("B      ");
            break;
        }

        if(++i == 16)
        {
            i = 0;
        }
    }
    putchar('\n');
}
