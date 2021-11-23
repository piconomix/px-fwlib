#ifndef __PX_LOG_FS_H__
#define __PX_LOG_FS_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_log_fs.h : Record-based file system for Serial Flash
    Author(s):      Pieter Conradie
    Creation Date:  2019-09-09

============================================================================= */

/**
 *  @ingroup DATA
 *  @defgroup PX_LOG_FS px_log_fs.h : Record-based file system for Serial Flash
 *
 *  Record-based file system for Serial Flash.
 *
 *  File(s):
 *  - data/inc/px_log_fs.h
 *  - data/inc/px_log_fs_cfg_template.h
 *  - data/src/px_log_fs.c
 *  - data/inc/px_log_fs_glue.h
 *  - data/src/px_log_fs_glue_at25s.c
 *  - data/src/px_log_fs_glue_at45d.c
 *
 *  1. Introduction
 *  ===============
 *
 *  <b>px_log_fs</b> is a basic but **resilient** record-based file system to
 *  store sequential log data on Serial Flash, for example
 *  [Adesto AT25S Serial Flash](https://www.adestotech.com/products/standard-serial-flash).
 *  It is designed with the following fundamental characteristics in mind:
 *  - The Serial Flash is evenly divided up into pages
 *  - A block of pages is erased at a time (less is better)
 *  - An erase will reset all bits in a block back to ones (1)
 *  - A write selectively set bits in a page to zero (0)
 *  - A bit that is zero can only be restored to one with a block erase
 *
 *  File system features:
 *  - Basic wear levelling by spreading block erases and page writes
 *    sequentially over the whole Serial Flash
 *  - Serial Flash bit fault tolerance without complete file corruption
 *    (only one record or extreme worst case one page will be lost)
 *  - Power disruption or unplanned reset tolerance (only one record may be lost)
 *  - Oldest records can be marked as archived.
 *
 *  A file is defined as a collection of sequential records that contain
 *  fixed-size log data. The records can be navigated forwards or backwards to
 *  read data, while still being able to append new data. New records will
 *  overwrite oldest records when the file is full
 *  (#PX_LOG_FS_CFG_STOP_WR_WHEN_FULL = 0) or record writing will stop when the
 *  file is full (#PX_LOG_FS_CFG_STOP_WR_WHEN_FULL = 1).
 *
 *  The size of each record is configured with #PX_LOG_FS_CFG_REC_DATA_SIZE.
 *  See 'px_log_fs_cfg_template.h'
 *
 *  The erase block size (number of pages) is configurable with
 *  #PX_LOG_FS_CFG_ERASE_BLOCK_SIZE. For example a single page of the Adesto
 *  [AT45DB041E](https://www.adestotech.com/products/data-flash/) can be erased
 *  at a time (very convenient), but the
 *  [AT25SF041](https://www.adestotech.com/products/standard-serial-flash/) has
 *  a minimum erase block size of 4 KB or 8 pages (a much cheaper option).
 *
 *  @warn_s
 *  The file system must start at the beginning of an erase block and end at the
 *  end of an erase block. It must be at least two erase blocks in size, but
 *  more is better.
 *  @warn_e
 *
 *  <i>First a code example to demonstrate the API, followed by implementation
 *  details...</i>
 *
 *  2. Code example
 *  ===============
 *
 *  @include data/test/px_log_fs_test.c
 *
 *  3. Implementation details
 *  =========================
 *
 *  The Serial Flash pages are treated as one big
 *  [circular buffer](http://en.wikipedia.org/wiki/Circular_buffer).
 *  Records are stored sequentially and record writing will stop (or wrap to
 *  overwrite oldest records) when the start of the oldest record is reached.
 *  This simplified scheme implicitly allows for
 *  [wear leveling](https://en.wikipedia.org/wiki/Wear_leveling), because each
 *  Serial Flash page is erased once per pass through the circular buffer.
 *
 *  The file system leverages a characteristic of Serial Flash. When a block of
 *  pages are erased, all the data bits are reset to 1. When writing a byte,
 *  bits can be cleared (0), but not reset to 1 again. The following 8-bit
 *  marker values are used:
 *
 *  ![8-bit markers](log_fs/markers.png)
 *
 *  @tip_s
 *  By inspecting the bits, it can be observed that a marker can be changed from
 *  FREE to PAGE to PAGE+A to BAD, but not the other way around without erasing
 *  the whole block.
 *  @tip_e
 *
 *  Each written page starts with a header (px_log_fs_header_t) which
 *  consists of an 8-bit marker, a 16-bit incrementing number and an 8-bit CRC.
 *  Only the first 4 bytes of each page is read to quickly index the whole file
 *  system during px_log_fs_init(). The incrementing number is used to figure
 *  out which is the oldest page and which is the newest during
 *  px_log_fs_init().
 *
 *  ![Page header structure](log_fs/page_header.png)
 *
 *  Pages containing records are marked with PAGE (0x5F). Each record
 *  (px_log_fs_record_t) contains an 8-bit RECORD marker (0xAF), the record data
 *  (fixed size set with #PX_LOG_FS_CFG_REC_DATA_SIZE) and an 8-bit CRC.
 *
 *  ![Record structure](log_fs/record.png)
 *
 *  @warn_s
 *  <b>A record cannot span across pages</b>, so there may be unused space at
 *  the end of each page. For example, if a page is 256 bytes and the page
 *  header takes up 4 bytes, then there are 252 bytes left over for records. If
 *  8 data bytes are stored in each record, then the total record size is 10
 *  bytes (including overhead). This means that 25 records can be stored in each
 *  page and 2 bytes will be unused (252 - 25*10 = 2).
 *  @warn_e
 *
 *  4. How to find the FIRST and LAST page
 *  ======================================
 *
 *  To figure out which page contains the oldest records (FIRST page) and
 *  which page contains the newest records (LAST page), a rolling (or wrapping)
 *  number is assigned to each page. This means that each consecutive page
 *  is labelled with an incrementing number. If the number exceeds the maximum,
 *  it starts again at zero (rolls over).
 *
 *  @tip_s
 *  To simplify these examples, the rolling number starts at 00, increments up
 *  to 99 and then rolls over to 00.
 *  @tip_e
 *
 *  Each page's rolling number is compared with the next page's rolling number.
 *  The largest difference in the rolling numbers indicates that the LAST page
 *  has been found. The next valid page is per implication the FIRST.
 *
 *  To be exact, modulo unsigned integer arithmetic is used:
 *
 *      Diff = (rolling_number_next - rolling_number) mod 100
 *
 *  Here are a few select test cases to demonstrate that it works under various
 *  conditions:
 *
 *  ![1. Page 5 is LAST and page 0 is FIRST (largest diff is 95)](log_fs/first_last_01.png)
 *
 *  ![2. Page 9 is LAST and page 4 is FIRST (largest diff = 95)](log_fs/first_last_02.png)
 *
 *  ![3. Page 10 is LAST and page 12 is FIRST (largest diff = 86)](log_fs/first_last_03.png)
 *
 *  ![4. Page 14 is LAST and page 0 is FIRST (largest diff = 86)](log_fs/first_last_04.png)
 *
 *  ![5. Page 2 is LAST and page 3 is FIRST (largest diff = 85)](log_fs/first_last_05.png)
 *
 *  ![6. Page 3 is LAST and page 4 is FIRST (largest diff = 85)](log_fs/first_last_06.png)
 *
 *  ![7. Page 6 is LAST and page 7 is FIRST (largest diff = 85)](log_fs/first_last_07.png)
 *
 *  5. Archiving records
 *  ====================
 *
 *  Oldest records can be marked as archived (RECORD+A marker = 0xAA) for
 *  example when the record has been succesfully uploaded to a server. This
 *  makes it possible to find the next (unarchived) record that must be uploaded
 *  to the server after a reset or power interruption.
 *
 *  If all the records on a page is marked as archived then the page is marked
 *  as archived too (PAGE+A marker = 0x55). This makes it faster to find the
 *  first unarchived record, because each page marker is inspected until the
 *  first unarchived page (PAGE marker = 0x5F) is found. Then each record on
 *  that page is inspected until the first unarchived record is found
 *  (RECORD marker =0xAF).
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"

// Include project specific configuration. See "px_log_fs_cfg_template.h"
#include "px_log_fs_cfg.h"

/*
 *  New log_fs with capability to mark records as archived is not backwards
 *  compatible with an existing file system as CRC calculation now excludes
 *  marker byte.
 *
 *  Compiler error is given to create awareness that logged information may be
 *  lost.
 */
#if (PX_LOG_FS_CFG_VER < 2)
#error "New log_fs not backwards compatible with existing file system"
#endif

// Check that all project specific options have been specified in "px_log_fs_cfg.h"
#if (   !defined(PX_LOG_FS_CFG_VER              ) \
     || !defined(PX_LOG_FS_CFG_PAGE_SIZE        ) \
     || !defined(PX_LOG_FS_CFG_ERASE_BLOCK_SIZE ) \
     || !defined(PX_LOG_FS_CFG_REC_DATA_SIZE    ) \
     || !defined(PX_LOG_FS_CFG_STOP_WR_WHEN_FULL)  )
#error "One or more options not defined in 'px_log_fs_cfg.h'"
#endif

#if (PX_LOG_FS_CFG_ERASE_BLOCK_SIZE == 0)
#error "PX_LOG_FS_CFG_ERASE_BLOCK_SIZE cannot be zero"
#endif
#if(!PX_VAL_IS_PWR_OF_TWO(PX_LOG_FS_CFG_ERASE_BLOCK_SIZE))
#error "PX_LOG_FS_CFG_ERASE_BLOCK_SIZE must be a multiple of two, e.g. 1, 2, 4, 8, 16, ..."
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Error codes
typedef enum
{
    PX_LOG_FS_ERR_NONE = 0,         ///< No error
    PX_LOG_FS_ERR_EMPTY,            ///< File is empty
    PX_LOG_FS_ERR_FULL,             ///< File is full
    PX_LOG_FS_ERR_WRITE_FAIL,       ///< Failed to write
    PX_LOG_FS_ERR_NO_RECORD,        ///< No record found
    PX_LOG_FS_ERR_FATAL,            ///< Fatal file system error
} px_log_fs_err_t;

/// Specification of data address in Serial Flash
typedef struct
{
    uint16_t page;                  ///< Page
    uint16_t offset;                ///< Offset inside page
} px_log_fs_adr_t;

typedef struct
{
    uint16_t        fs_page_start;  ///< First page in file system (must be on the start of an erase block)
    uint16_t        fs_page_end;    ///< Last page in file system (must be on the end of an erase block)
    uint16_t        page_first;     ///< First page with records (PX_LOG_FS_PAGE_INVALID if empty)
    uint16_t        page_last;      ///< Last page with records (PX_LOG_FS_PAGE_INVALID if empty)
    uint16_t        page_nr_next;   ///< Next page number to use (starts at 0)
    px_log_fs_adr_t adr_wr;         ///< Next write address (open position)
    px_log_fs_adr_t adr_rd;         ///< Current read address
    bool            archive_flag;   ///< Flag is set if current read record is archived
} px_log_fs_handle_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise log file system.
 *
 *  @param handle           Pointer to file system handle structure
 *  @param fs_page_start    First page in file system (must be on the start of an erase block)
 *  @param fs_page_end      Last page in file system (must be on the end of an erase block)
 *
 *  @retval PX_LOG_FS_ERR_NONE  File system was succesfully indexed
 */
px_log_fs_err_t px_log_fs_init(px_log_fs_handle_t * handle,
                               uint16_t             fs_page_start,
                               uint16_t             fs_page_end);

/**
 *  Reset log file system.
 *
 *  @param handle           Pointer to file system handle structure
 *  @param fs_page_start    First page in file system (must be on the start of an erase block)
 *  @param fs_page_end      Last page in file system (must be on the end of an erase block)
 *
 *  @retval PX_LOG_FS_ERR_NONE  File system was succesfully resetted
 */
px_log_fs_err_t px_log_fs_reset(px_log_fs_handle_t * handle,
                                uint16_t             fs_page_start,
                                uint16_t             fs_page_end);
/**
 *  Read first (oldest) record.
 *
 *  @param handle                   Pointer to file system handle structure
 *  @param data                     Pointer to buffer where record data must be
 *                                  copied to
 *  @param nr_of_bytes              Number of bytes to copy from record
 *
 *  @retval PX_LOG_FS_ERR_NONE      Valid record data found and copied into
 *                                  structure
 *  @retval PX_LOG_FS_ERR_NO_RECORD No record found
 */
px_log_fs_err_t px_log_fs_rd_first(px_log_fs_handle_t * handle,
                                   void *               data,
                                   size_t               nr_of_bytes);

/**
 *  Read first (oldest) unarchived record.
 *
 *  @param handle                   Pointer to file system handle structure
 *  @param data                     Pointer to buffer where record data must be
 *                                  copied to
 *  @param nr_of_bytes              Number of bytes to copy from record
 *
 *  @retval PX_LOG_FS_ERR_NONE      Valid record data found and copied into
 *                                  structure
 *  @retval PX_LOG_FS_ERR_NO_RECORD No record found
 */
px_log_fs_err_t px_log_fs_rd_first_unarchived(px_log_fs_handle_t * handle,
                                              void *               data,
                                              size_t               nr_of_bytes);
/**
 *  Read next (newer) record.
 *
 *  @param handle                   Pointer to file system handle structure
 *  @param data                     Pointer to buffer where record data must be
 *                                  copied to
 *  @param nr_of_bytes              Number of bytes to copy from record
 *
 *  @retval PX_LOG_FS_ERR_NONE      Valid record data found and copied into
 *                                  structure
 *  @retval PX_LOG_FS_ERR_NO_RECORD No record found
 */
px_log_fs_err_t px_log_fs_rd_next(px_log_fs_handle_t * handle,
                                  void *               data,
                                  size_t               nr_of_bytes);

/**
 *  Read last (newest) record.
 *
 *  @param handle                   Pointer to file system handle structure
 *  @param data                     Pointer to buffer where record data must be
 *                                  copied to
 *  @param nr_of_bytes              Number of bytes to copy from record
 *
 *  @retval PX_LOG_FS_ERR_NONE      Valid record data found and copied into
 *                                  structure
 *  @retval PX_LOG_FS_ERR_NO_RECORD No record found
 */
px_log_fs_err_t px_log_fs_rd_last(px_log_fs_handle_t * handle,
                                  void *               data,
                                  size_t               nr_of_bytes);

/**
 *  Read previous (older) record.
 *
 *  @param handle                   Pointer to file system handle structure
 *  @param data                     Pointer to buffer where record data must be
 *                                  copied to
 *  @param nr_of_bytes              Number of bytes to copy from record
 *
 *  @retval PX_LOG_FS_ERR_NONE      Valid record data found and copied into
 *                                  structure
 *  @retval PX_LOG_FS_ERR_NO_RECORD No record found
 */
px_log_fs_err_t px_log_fs_rd_previous(px_log_fs_handle_t * handle,
                                      void *               data,
                                      size_t               nr_of_bytes);

/**
 *  See if currently read record is archived or not.
 *
 *  NB! Record *MUST* have been succesfully read first.
 *
 *  @param handle                   Pointer to file system handle structure
 *
 *  @retval true                    record is marked as archived
 *  @retval false                   record is NOT marked as archived
 *
 */
bool inline px_log_fs_rd_rec_is_archived(px_log_fs_handle_t * handle)
{
    return handle->archive_flag;
}

/**
 *  Set currently read record as archived.
 *
 *  NB! Record *MUST* have been succesfully read first.
 *
 *  As a side effect, if this is the last record in the page, then the page
 *  marker is set as archived for quick indexing.
 *
 *  It is important that the records are marked as archived starting from the
 *  oldest to the newest in sequence otherwise the algorithm that mark a whole
 *  page as archived will not work correctly.
 *
 *  @param handle                       Pointer to file system handle structure
 *
 *  @retval PX_LOG_FS_ERR_NONE          Success. Record is set as archived.
 *  @retval PX_LOG_FS_ERR_WRITE_FAIL    Failed to set record as archived.
 */
px_log_fs_err_t px_log_fs_rd_rec_set_archive(px_log_fs_handle_t * handle);

/**
 *  Write a record to the file.
 *
 *  A new record is appended to the end of the list of records. The maximum
 *  number of bytes that can be stored in the record is specified by
 *  PX_LOG_FS_CFG_DATA_SIZE.
 *
 *  @param handle                   Pointer to file system handle structure
 *  @param data                     Pointer to buffer containing data that must
 *                                  be stored in the record
 *  @param nr_of_bytes              Number of bytes that must be written
 *
 *  @retval PX_LOG_FS_ERR_NONE         Success
 *  @retval PX_LOG_FS_ERR_FULL         File (or file system) is full
 *  @retval PX_LOG_FS_ERR_WRITE_FAIL   Failed to write record (verify failed)
 */
px_log_fs_err_t px_log_fs_wr(px_log_fs_handle_t * handle,
                             const void *         data,
                             size_t               nr_of_bytes);

/**
 *  Report log file system info.
 *
 *  @param handle                   Pointer to file system handle structure
 */
void px_log_fs_dbg_report_info(px_log_fs_handle_t * handle);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
