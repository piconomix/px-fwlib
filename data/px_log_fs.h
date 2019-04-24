#ifndef __PX_LOG_FS_H__
#define __PX_LOG_FS_H__
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
 
    Title:          px_log_fs.h : Record-based file system for Serial Flash
    Author(s):      Pieter Conradie
    Creation Date:  2014-06-09

============================================================================= */

/** 
 *  @ingroup DATA
 *  @defgroup PX_LOG_FS px_log_fs.h : Record-based file system for Serial Flash
 *  
 *  Record-based file system for Serial Flash.
 *  
 *  File(s):
 *  - data/px_log_fs.h
 *  - data/px_log_fs_cfg_template.h
 *  - data/px_log_fs.c
 *  
 *  1. Introduction
 *  ===============
 *  
 *  <b>px_log_fs</b> is a basic but **resilient** record-based file system to
 *  store log data on Serial Flash, for example 
 *  [Adesto AT45D DataFlash](http://www.adestotech.com/products/data-flash/).
 *  A file is defined as a collection of fixed-size records that contain the log
 *  data. The records can be navigated forwards or backwards to read data, while
 *  being able to append new data at the same time. Record writing will stop 
 *  when the file is full (PX_LOG_FS_CFG_TYPE_LINEAR option) or newest records will
 *  overwrite oldest records when the file is full (PX_LOG_FS_CFG_TYPE_CIRCULAR
 *  option). Multiple time-stamped files can be created sequentially, but only
 *  the oldest file can be deleted.
 *  
 *  The maximum number of pages used for each file can be limited with the
 *  PX_LOG_FS_CFG_MAX_PAGES option (0 means no limit). The size of each record is 
 *  configured with PX_LOG_FS_CFG_REC_DATA_SIZE. See 'px_log_fs_cfg_template.h'
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
 *  The Serial Flash pages are treated like one big circular buffer.
 *  Files are stored sequentially and record writing will stop (or wrap to
 *  overwrite oldest records) when the start of the oldest file is reached. This
 *  simplified scheme implicitly allows for wear leveling, because each Serial
 *  Flash page is erased once per pass through the circular buffer.
 *  
 *  The file system leverages a characteristic of Serial Flash. When a page is 
 *  erased, all the data bits are reset to 1. When writing a byte, bits can be 
 *  cleared (0), but not reset to 1 again. The following 8-bit marker values are
 *  used:
 *  
 *  ![8-bit markers](log_fs/markers.png)
 *  
 *  @tip_s
 *  By inspecting the bits, it can be observed that a marker can be changed from
 *  FREE to FILE (or RECORD) to BAD, but not the other way around without
 *  erasing the whole page.
 *  @tip_e
 *  
 *  Each page starts with a page header (px_log_fs_page_header_t) which consists of
 *  an 8-bit marker, a 16-bit rolling FILE/RECORD number and an 8-bit CRC. 
 *  For quick indexing or searching a whole page is used to mark the start of a 
 *  file (marker = FILE). Thus only the first 4 bytes of each page is read to 
 *  quickly index the whole file system during px_log_fs_init(). The rolling FILE 
 *  number is used to figure out which is the oldest file and which is the 
 *  newest during px_log_fs_init().
 *  
 *  ![Page header structure](log_fs/page_header.png)
 *  
 *  After the FILE page, records are stored in RECORD pages. The 16-bit rolling
 *  RECORD number (in each page header) is used to figure out which is the 
 *  oldest record page and which is the newest record page if the 
 *  PX_LOG_FS_CFG_TYPE_CIRCULAR option is used. Each record (px_log_fs_rec_block_t)
 *  contains an 8-bit RECORD marker, the record data (fixed size set with 
 *  PX_LOG_FS_CFG_REC_DATA_SIZE) and an 8-bit CRC.
 *  
 *  ![Record structure](log_fs/record.png)
 *  
 *  @warn_s
 *  <b>A record will not span across pages</b>, so there may be unused space at
 *  the end of each page. For example, if a page is 264 bytes and the page 
 *  header takes up 4 bytes, then there are 260 bytes left over for records. If 
 *  4 data bytes are stored in each record, then the total record size is 6 
 *  bytes (including overhead). This means that 43 records can be stored in each
 *  page and 2 bytes will be unused (260 - 43*6 = 2).
 *  @warn_e
 *  
 *  When a file is created (and during writing), the file system ensures that
 *  there is always at least one FREE record space available that can be
 *  written to. Here is an example to illustrate how the file system with the 
 *  PX_LOG_FS_CFG_TYPE_LINEAR option works, i.e. record writing will stop when the 
 *  file system is full.
 *  
 *  4. Linear file example (PX_LOG_FS_CFG_TYPE_LINEAR option)
 *  =========================================================
 *  
 *  ![1. File system empty. All Pages (0- 7) are FREE (data bytes set to 0xFF)](log_fs/linear01.png)
 *  
 *  This example starts with the file system empty, i.e. all of the pages 
 *  have been erased and the data bits reset to 1. This means that the 8-bit
 *  marker at the start of each page is FREE (0xFF).
 *  
 *  ![2. File 00 created. FILE PAGE 00 written to page 2. Page 3 erased](log_fs/linear02.png)
 *  
 *  With an empty file system the first file will start at page 0, but to 
 *  illustrate the circular wrapping nature of the file system, this file starts
 *  at page 2. A file is created and the assigned rolling number is 00. Page 2 
 *  is written with a FILE page header and a timestamp. Page 3 is erased to 
 *  ensure that there is at least one empty record space available that can be 
 *  written to.
 *  
 *  @tip_s
 *  For this example the rolling number is two digits. It starts at 00 and 
 *  increments up to 99 and wraps back to 00.
 *  @tip_e
 *  
 *  ![3. Page 3 header set to RECORD PAGE 00. Two RECORDS (0 & 1) written to page 3](log_fs/linear03.png)
 *  
 *  Two records are written to the open file. A RECORD page header is written 
 *  first, and then two records to page 3.
 *  
 *  ![4. One RECORD (2) written to page 3 (page is now full). Page 4 erased](log_fs/linear04.png)
 *  
 *  One more record is written to page 3 and now the page is full. The file 
 *  system erases the next page to ensure that there is at least one free 
 *  record space.
 *  
 *  ![5. File 01 created. FILE PAGE 01 written to page 4. Page 5 erased](log_fs/linear05.png)
 *  
 *  Logging to File 00 stops and a new file is created (with rolling number 01).
 *  Page 4 is written with a FILE page header and timestamp. Page 5 is erased to
 *  ensure that there is at least one free record space available that can be 
 *  written to.
 *  
 *  ![6. Fifteen RECORDS (0 - 14) written to page 5, 6, 7, 0 & 1. File is full](log_fs/linear06.png)
 *  
 *  Fifteen records are now written to File 01. When record 8 is written to 
 *  page 7, the file system wraps and erase page 0. When record 14 is written, 
 *  the file system will not erase page 2, because it is a FILE page header and
 *  the file is considered to be full.
 *  
 *  ![7. File 00 erased. Page 2 & 3 marker set to BAD (0x00)](log_fs/linear07.png)
 *  
 *  To create space in this example, the oldest file (File 00) is now deleted.
 *  The first byte of each page (page marker) is set to BAD (0x00).
 *  
 *  ![8. Page 2 erased and one RECORD (15) written to page 2](log_fs/linear08.png)
 *  
 *  Another record is written to File 01. The file system erases page 2 and 
 *  first writes a RECORD page header and then record 15. This demonstrates how
 *  the file system reclaims pages that have been marked as BAD (0x00).
 *  
 *  
 *  5. Circular file example (PX_LOG_FS_CFG_TYPE_CIRCULAR option)
 *  =============================================================
 *  
 *  ![1. File system empty. All Pages (0- 7) are FREE (data bytes set to 0xFF)](log_fs/circular01.png)
 *  
 *  This example again starts with the file system empty, i.e. all of the pages 
 *  have been erased and the data bits reset to 1. This means that the 8-bit
 *  marker at the start of each page is FREE (0xFF).
 *  
 *  ![2. File 00 created. FILE PAGE 00 written to page 4. Page 5 erased](log_fs/circular02.png)
 *  
 *  With an empty file system the first file will start at page 0, but to 
 *  illustrate the circular wrapping nature of the file system, this file starts
 *  at page 4. A file is created and the rolling number is 00. Page 4 is written
 *  with a FILE page header and a timestamp. Page 5 is erased to ensure that
 *  there is at least one free record space available that can be written to.
 *  
 *  <i>For this example the rolling number is two digits. It starts at 00 and 
 *  increments up to 99 and wraps back to 00.</i>
 *  
 *  ![3. Eighteen RECORDS (0 - 17) written to page 5,6,7,0,1 & 2. Space available in file](log_fs/circular03.png)
 *  
 *  Eighteen records are now written to File 00. When record 8 is written to 
 *  page 7, the file system wraps and erase page 0. When record 17 is written, 
 *  the file system erase page 3 to ensure that there is at least one free 
 *  record space available that can be written to.
 *  
 *  ![4. Three RECORDS (18 - 20) written to page 3. File is full. Page 5 is erased (marker set to FREE) and oldest RECORDS (0 - 2) are deleted](log_fs/circular04.png)
 *  
 *  Three more records are written to page 3. When record 20 is written, 
 *  the file system will not erase page 4, because it is a FILE page header and
 *  the file is considered to be full. The file system wraps and erases page 5 
 *  (page marker set to FREE). The oldest records (0 - 2) are deleted.
 *  
 *  ![5. Two RECORDS (21 - 22) written to page 5. One FREE record location in page 5](log_fs/circular05.png)
 *  
 *  Two records (21 - 22) are written to page 5. There is still one FREE record
 *  space in page 5 and that is why page 6 is not erased.
 *  
 *  @tip_s
 *  At this stage, it can be observed that the page 6 contains the oldest
 *  records (RECORD PAGE 01) and page 5 contains the newest records (RECORD PAGE
 *  07).
 *  @tip_e
 *  
 *  ![6. Lots of records written to the file. Page 3 contains oldest records (297-299)](log_fs/circular06.png)
 *  
 *  Lots of records are written to the file. Page 3 (RECORD PAGE 98) contains 
 *  the oldest records (297-299). Page 2 (RECORD PAGE 04) contains the newest
 *  record (315).
 *  
 *  6. Finding the start and end using a rolling (wrapping) number
 *  ==============================================================
 *  
 *  To figure out which page contains the oldest entry (START) and which page
 *  contains the newest entry (END), a rolling (or wrapping) number is assigned
 *  to each entry. This means that each consecutive entry is labelled with an
 *  incrementing number. If the number exceeds the maximum, it starts again at
 *  zero (rolls over). For these examples, the rolling number starts at 00 up to
 *  99 and then rolls back to 00.
 *  
 *  Each entry's rolling number is compared with the next valid entry's rolling
 *  number. The largest difference in the rolling numbers indicates that the
 *  END has been found. The next valid entry is per implication the START.
 *  
 *  To be exact, modulo unsigned integer arithmetic is used:
 *  
 *      Diff = (rolling_number_next - rolling_number) mod 100
 *  
 *  Here are a few select test cases to show that it works under various
 *  conditions:
 *  
 *  ![1. END is at page 5 and START is at page 0 (largest diff is 95)](log_fs/start_end_01.png)
 *  
 *  ![2. END is at page 9 and START is at page 4 (largest diff = 95)](log_fs/start_end_02.png)
 *  
 *  ![3. END is at page 10 and START is at page 12 (largest diff = 86)](log_fs/start_end_03.png)
 *  
 *  ![4. END is at page 14 and START is at page 0 (largest diff = 86)](log_fs/start_end_04.png)
 *  
 *  ![5. END is at page 1 and START is at page 3 (largest diff = 86)](log_fs/start_end_05.png)
 *  
 *  ![6. END is at page 2 and START is at page 4 (largest diff = 86)](log_fs/start_end_06.png)
 *  
 *  ![7. END is at page 5 and START is at page 7 (largest diff = 86)](log_fs/start_end_07.png)
 *  
 *  ![8. END is at page 14 and START is at page 0 (largest diff = 86)](log_fs/start_end_08.png)
 *  
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

// Include project specific config. See "px_log_fs_cfg_template.h"
#include "px_log_fs_cfg.h"

// Check that all project specific options have been specified in "px_log_fs_cfg.h"
#if (   !defined(PX_LOG_FS_CFG_PAGE_SIZE    ) \
     || !defined(PX_LOG_FS_CFG_PAGE_START   ) \
     || !defined(PX_LOG_FS_CFG_PAGE_END     ) \
     || !defined(PX_LOG_FS_CFG_REC_DATA_SIZE) \
     || !defined(PX_LOG_FS_CFG_TYPE         ) \
     || !defined(PX_LOG_FS_CFG_MAX_PAGES    )  )
      )
#error "One or more options not defined in 'px_log_fs_cfg.h'"
#endif


#if (PX_LOG_FS_CFG_TYPE != PX_LOG_FS_CFG_TYPE_LINEAR) && (PX_LOG_FS_CFG_TYPE != PX_LOG_FS_CFG_TYPE_CIRCULAR)
#error "Unknown PX_LOG_FS_CFG_TYPE"
#endif
#if (PX_LOG_FS_CFG_MAX_PAGES > (PX_LOG_FS_CFG_PAGE_END - PX_LOG_FS_CFG_PAGE_START + 1))
#error "PX_LOG_FS_CFG_MAX_PAGES too big"
#endif
#if (PX_LOG_FS_CFG_PAGE_START >= PX_LOG_FS_CFG_PAGE_END)
#error "PX_LOG_FS_CFG_PAGE_START must be smaller than PX_LOG_FS_CFG_PAGE_END"
#endif
#if (PX_LOG_FS_CFG_PAGE_END >= 0x8000)
#error "PX_LOG_FS_CFG_PAGE_END too big. Arithmetic will overflow"
#endif
#if (PX_LOG_FS_CFG_PAGE_END + PX_LOG_FS_CFG_MAX_PAGES >= 0xffff)
#error "Arithmetic will overflow. Make value smaller"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// @name Log type option selected in "px_log_fs_cfg.h"
//@{
/// Linear : Record writing will stop when file is full
#define PX_LOG_FS_CFG_TYPE_LINEAR      0
/// Circular : Newest records will overwrite oldest records when file is full
#define PX_LOG_FS_CFG_TYPE_CIRCULAR    1
//@}

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Error codes
typedef enum
{
    PX_LOG_FS_ERR_NONE = 0,                ///< No error
    PX_LOG_FS_ERR_FILE_DOES_NOT_EXIST,     ///< File does not exist
    PX_LOG_FS_ERR_FILE_ALREADY_EXISTS,     ///< File already exists
    PX_LOG_FS_ERR_FILE_INVALID,            ///< Invalid file specified
    PX_LOG_FS_ERR_EMPTY,                   ///< File is empty
    PX_LOG_FS_ERR_FULL,                    ///< File is full
    PX_LOG_FS_ERR_WRITE_FAIL,              ///< Failed to write
    PX_LOG_FS_ERR_NO_FILE,                 ///< No file found
    PX_LOG_FS_ERR_NO_RECORD,               ///< No record found
} px_log_fs_err_t;

/// Page number size definition
typedef uint16_t px_log_fs_page_t;

/// Offset in page size definition
typedef uint16_t px_log_fs_offset_t;

/// File creation date and time
typedef struct
{    
    uint8_t  year;     ///< Years:   0 to 99 (2000 - 2099)
    uint8_t  month;    ///< Months:  1 to 12
    uint8_t  day;      ///< Days:    1 to 31 (depending on month)
    uint8_t  hour;     ///< Hours:   0 to 23
    uint8_t  min;      ///< Minutes: 0 to 59
    uint8_t  sec;      ///< Seconds: 0 to 59
} PX_ATTR_PACKED px_log_fs_time_stamp_t;

/// File info
typedef struct
{
    px_log_fs_page_t          start_page;  ///< File start page
    px_log_fs_time_stamp_t    time_stamp;  ///< File creation time stamp (date and time)
} PX_ATTR_PACKED px_log_fs_file_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 * Initialise log file system.
 *  
 * @retval PX_LOG_FS_ERR_NONE  File system was succesfully indexed
 */
px_log_fs_err_t px_log_fs_init(void);

/**
 *  Create a new file.
 *  
 *  @param time_stamp       User supplied time stamp (date and time).
 *  
 *  @retval PX_LOG_FS_ERR_NONE File succesfully created.
 *  @retval PX_LOG_FS_ERR_FULL File sytem is full (at least 2 free pages are 
 *                          required for a new file).
 */
px_log_fs_err_t px_log_fs_create(const px_log_fs_time_stamp_t * time_stamp);

/**
 *  Write file attribute data.
 *  
 *  A whole page is allocated to mark the start of a file, but only a few bytes
 *  are used. The rest of the page data can be used to store once-off, ancillary
 *  data. The content is reset to 0xff's when the file is created. Bits can be
 *  cleared (0), but not set (1).
 *  
 *  Call px_log_fs_file_attr_data_size() to find out the maximum number of bytes
 *  that can be written.
 *  
 *  @param offset       Offset (0 to px_log_fs_file_attr_data_size()-1)
 *  @param data         Pointer to a buffer containing data to be written
 *  @param nr_of_bytes  Number of bytes to write
 *  
 *  @return size_t      Number of bytes actually written
 */
size_t px_log_fs_file_attr_data_wr(px_log_fs_offset_t offset, 
                                   const void *       data, 
                                   size_t             nr_of_bytes);

/**
 *  Read file attribute data.
 *  
 *  See px_log_fs_file_attr_data_wr().
 *  
 *  @param offset       Offset (0 to px_log_fs_file_attr_data_size()-1)
 *  @param data         Pointer to a buffer where the received data must be
 *                      stored
 *  @param nr_of_bytes  Number of bytes to read
 *  
 *  @return size_t      Number of bytes actually written
 */
size_t px_log_fs_file_attr_data_rd(px_log_fs_offset_t offset, 
                                   void *             data, 
                                   size_t             nr_of_bytes);

/**
 *  Return the maximum size of attribute data that can be read or written.
 *  
 *  @return size_t  Size (in bytes) of attribute data
 */
size_t px_log_fs_file_attr_data_size(void);

/**
 *  Find first (oldest) file.
 *  
 *  @param file     Pointer to buffer that will contain file data (if found)
 *  
 *  @retval PX_LOG_FS_ERR_NONE         Valid file found
 *  @retval PX_LOG_FS_ERR_NO_FILE      No files in file system
 *  @retval PX_LOG_FS_ERR_FILE_INVALID File system corrupt. File page was valid 
 *                                  during px_log_fs_init().
 */
px_log_fs_err_t px_log_fs_file_find_first(px_log_fs_file_t * file);

/**
 *  Find last (newest) file.
 *  
 *  @param file     Pointer to buffer that will contain file data (if found)
 *  
 *  @retval PX_LOG_FS_ERR_NONE         Valid file found
 *  @retval PX_LOG_FS_ERR_NO_FILE      No files in file system
 *  @retval PX_LOG_FS_ERR_FILE_INVALID File system corrupt. File page was valid 
 *                                  during px_log_fs_init().
 */
px_log_fs_err_t px_log_fs_file_find_last(px_log_fs_file_t * file);

/**
 *  Find next (newer) file.
 *  
 *  @param file     Pointer to buffer that will contain file data (if found)
 *  
 *  @retval PX_LOG_FS_ERR_NONE         Valid file found
 *  @retval PX_LOG_FS_ERR_NO_FILE      No more files
 */
px_log_fs_err_t px_log_fs_file_find_next(px_log_fs_file_t * file);

/**
 *  Find previous (older) file.
 *  
 *  @param file     Pointer to buffer that will contain file data (if found)
 *  
 *  @retval PX_LOG_FS_ERR_NONE         Valid file found
 *  @retval PX_LOG_FS_ERR_NO_FILE      No more files
 */
px_log_fs_err_t px_log_fs_file_find_previous(px_log_fs_file_t * file);

/**
 *  Open existing file that has been found using px_log_fs_file_find_first()
 *  method.
 *  
 *  @param file     File data of valid file
 *  
 *  @retval PX_LOG_FS_ERR_NONE         File opened
 *  @retval PX_LOG_FS_ERR_FILE_INVALID File system corrupt
 */
px_log_fs_err_t px_log_fs_open(const px_log_fs_file_t * file);

/**
 *  Delete an existing file that has been found using px_log_fs_file_find_first()
 *  method.
 *  
 *  Only the first (oldest) file may be deleted to avoid fragmentation, which is
 *  not catered for by this file system.
 *  
 *  @param file             File data of valid file
 *  
 *  @retval PX_LOG_FS_ERR_NONE         File was succesfully opened
 *  @retval PX_LOG_FS_ERR_FILE_INVALID Not the oldest file, or file page is invalid
 */
px_log_fs_err_t px_log_fs_file_delete(const px_log_fs_file_t * file);

/**
 *  Read first (oldest) record.
 *  
 *  @param data                     Pointer to buffer where record data must be
 *                                  copied to
 *  @param nr_of_bytes              Number of bytes to copy from record
 *  
 *  @retval PX_LOG_FS_ERR_NONE      Valid record data found and copied into 
 *                                  structure
 *  @retval PX_LOG_FS_ERR_NO_RECORD No record found
 */
px_log_fs_err_t px_log_fs_record_rd_first(void * data, size_t nr_of_bytes);

/**
 *  Read last (newest) record.
 *  
 *  @param data                     Pointer to buffer where record data must be
 *                                  copied to
 *  @param nr_of_bytes              Number of bytes to copy from record
 *  
 *  @retval PX_LOG_FS_ERR_NONE      Valid record data found and copied into 
 *                                  structure
 *  @retval PX_LOG_FS_ERR_NO_RECORD No record found
 */
px_log_fs_err_t px_log_fs_record_rd_last(void * data, size_t nr_of_bytes);

/**
 *  Read next (newer) record.
 *  
 *  @param data                     Pointer to buffer where record data must be
 *                                  copied to
 *  @param nr_of_bytes              Number of bytes to copy from record
 *  
 *  @retval PX_LOG_FS_ERR_NONE      Valid record data found and copied into 
 *                                  structure
 *  @retval PX_LOG_FS_ERR_NO_RECORD No record found
 */
px_log_fs_err_t px_log_fs_record_rd_next(void * data, size_t nr_of_bytes);

/**
 *  Read previous (older) record.
 *  
 *  @param data                     Pointer to buffer where record data must be
 *                                  copied to
 *  @param nr_of_bytes              Number of bytes to copy from record
 *  
 *  @retval PX_LOG_FS_ERR_NONE      Valid record data found and copied into 
 *                                  structure
 *  @retval PX_LOG_FS_ERR_NO_RECORD No record found
 */
px_log_fs_err_t px_log_fs_record_rd_previous(void * data, size_t nr_of_bytes);

/**
 *  Write a record to the file.
 *  
 *  A new record is appended to the end of the list of records. The maximum
 *  number of bytes that can be stored in the record is specified by
 *  PX_LOG_FS_CFG_DATA_SIZE.
 *  
 *  @param data                     Pointer to buffer containing data that must
 *                                  be stored in the record
 *  @param nr_of_bytes              Number of bytes that must be written
 *  
 *  @retval PX_LOG_FS_ERR_NONE         Success
 *  @retval PX_LOG_FS_ERR_FULL         File (or file system) is full
 *  @retval PX_LOG_FS_ERR_WRITE_FAIL   Failed to write record (verify failed)
 */
px_log_fs_err_t px_log_fs_record_wr(const void * data, size_t nr_of_bytes);

/** 
 *  Report log file system info.
 */
void px_log_fs_info(void);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_LOG_FS_H__
