#ifndef __PX_RING_BUFFER_H__
#define __PX_RING_BUFFER_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2008 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_ring_buf.h : FIFO circular buffer
    Author(s):      Pieter Conradie
    Creation Date:  2008-08-06

============================================================================= */

/** 
 *  @ingroup UTILS
 *  @defgroup PX_RING_BUFFER px_ring_buf.h : FIFO circular buffer
 *  
 *  A data structure that uses a single, fixed-size buffer as if it were
 *  connected end-to-end (circular).
 *  
 *  File(s):
 *  - utils/inc/px_ring_buf.h
 *  - utils/src/px_ring_buf.c
 *  
 *  A fixed-sized array is managed as a FIFO buffer with a "zero-copy" policy,
 *  i.e. data is not shifted (copied) when data is removed or added to the
 *  buffer. If more data is written to the buffer than there is space for, it is
 *  ignored/discarded; no buffer overflow vulnerability.
 *  
 *  In this implementation, the maximum number of bytes that can be stored is
 *  one less than the size of the fixed-size buffer, e.g. if the buffer size is
 *  8 bytes, then a maximum of 7 bytes can be stored.
 *  
 *  Reference:
 *  - http://en.wikipedia.org/wiki/Circular_buffer
 *  
 *  Example:
 *  
 *  @code
 *      Buffer is empty:
 *
 *                   wr
 *                   |
 *      [x][x][x][x][x][x][x][x]
 *                   |
 *                  rd
 *
 *      One byte is written to the buffer ('1'):
 *
 *                      wr
 *                      |
 *      [x][x][x][x][1][x][x][x]
 *                   |
 *                  rd
 *
 *      One byte is read ('1'); buffer is empty again:
 *
 *                      wr
 *                      |
 *      [x][x][x][x][x][x][x][x]
 *                      |
 *                     rd
 *
 *      5 bytes are written ('2','3','4','5','6'); buffer wraps:
 *
 *             wr
 *             |
 *      [5][6][x][x][x][2][3][4]
 *                      |
 *                     rd
 *
 *      2 more bytes are written ('7','8'); buffer is full:
 *
 *                   wr
 *                   |
 *      [5][6][7][8][x][2][3][4]
 *                      |
 *                     rd
 *  @endcode
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdlib.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// Circular buffer index size definition
typedef uint16_t px_ring_buf_idx_t;

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Circular buffer structure
typedef struct
{
    uint8_t *           buf;        ///< Pointer to fixed-size buffer
    px_ring_buf_idx_t   buf_size;   ///< Size of fixed-size buffer
    px_ring_buf_idx_t   wr;         ///< Index that is one ahead of last byte written to the buffer
    px_ring_buf_idx_t   rd;         ///< Index to the first byte to be read from the buffer
} px_ring_buf_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/** 
 *  Initialize the circular buffer.
 *  
 *  @param px_ring_buf      Pointer to the circular buffer object
 *  @param buf              Fixed-size data buffer
 *  @param buf_size         Fixed-size data buffer size
 */
void px_ring_buf_init(px_ring_buf_t *   px_ring_buf,
                      uint8_t *         buf,
                      px_ring_buf_idx_t buf_size);

/** 
 *  See if the circular buffer is empty.
 *  
 *  @param px_ring_buf  Pointer to the circular buffer object
 *  
 *  @retval true            buffer is empty
 *  @retval false           buffer contains data
 */
bool px_ring_buf_is_empty(const px_ring_buf_t * px_ring_buf);

/** 
 *  See if the circular buffer is full.
 *  
 *  @param px_ring_buf  Pointer to the circular buffer object
 *  
 *  @retval true            buffer is full
 *  @retval false           buffer is NOT full
 */
bool px_ring_buf_is_full(const px_ring_buf_t * px_ring_buf);

/** 
 *  Removed all data from circular buffer.
 *  
 *  @param px_ring_buf  Pointer to the circular buffer object
 *  
 */
void px_ring_buf_flush(px_ring_buf_t * px_ring_buf);

/** 
 *  Write (store) a byte in the circular buffer.
 *  
 *  @param px_ring_buf      Pointer to the circular buffer object
 *  @param data             The byte to store in the circular buffer
 *  
 *  @retval true            Byte has been stored in the circular buffer
 *  @retval false           Buffer is full and byte was not stored
 */
bool px_ring_buf_wr_u8(px_ring_buf_t * px_ring_buf,
                       const uint8_t   data);

/** 
 *  Write (store) data in the circular buffer
 *  
 *  @param px_ring_buf          Pointer to the circular buffer object
 *  @param data                 Pointer to array of data to be stored in the
 *                              circular buffer
 *  @param nr_of_bytes_to_wr    Number of data bytes to be written
 *  
 *  @return px_ring_buf_idx_t   The actual number of data bytes stored, which
 *                              may be less than the number specified, because
 *                              the buffer is full.
 */
px_ring_buf_idx_t px_ring_buf_wr(px_ring_buf_t * px_ring_buf, 
                                 const void *    data,
                                 size_t          nr_of_bytes_to_wr);

/** 
 *  Read (retrieve) a byte from the circular buffer.
 *  
 *  @param px_ring_buf      Pointer to the circular buffer object
 *  @param data             Pointer to location where byte must be stored
 *  
 *  @retval true            Valid byte has been retrieved
 *  @retval false           Buffer is empty
 */
bool px_ring_buf_rd_u8(px_ring_buf_t * px_ring_buf,
                       uint8_t *       data);

/** 
 * Read (retrieve) data from the circular buffer.
 *  
 * @param px_ring_buf           Pointer to the circular buffer object
 * @param data                  Pointer to location where data must be stored
 * @param nr_of_bytes_to_rd     Number of bytes to retrieve    
 *  
 * @return px_ring_buf_idx_t    The actual number of bytes retrieved, which may 
 *                              be less than the number specified, because the 
 *                              buffer is empty.
 */
px_ring_buf_idx_t px_ring_buf_rd(px_ring_buf_t * px_ring_buf,
                                 void *          data,
                                 size_t          nr_of_bytes_to_rd);


/** 
 * Peek data from the circular buffer, without advancing the read pointer.
 *  
 * @param px_ring_buf           Pointer to the circular buffer object
 * @param data                  Pointer to location where data must be stored
 * @param nr_of_bytes_to_peek   Number of bytes to peek    
 * @param nr_of_bytes_to_skip   Number of bytes to skip, before peeking    
 *  
 * @return px_ring_buf_idx_t    The actual number of bytes peeked, which may 
 *                              be less than the number specified, because the 
 *                              buffer is empty.
 */
px_ring_buf_idx_t px_ring_buf_peek(px_ring_buf_t * px_ring_buf, 
                                   void *          data,
                                   size_t          nr_of_bytes_to_peek,
                                   size_t          nr_of_bytes_to_skip);



/** 
 * Get free bytes available in buffer, an empty buffer has one less byte than the size
 *  
 * @param px_ring_buf           Pointer to the circular buffer object
 * @return px_ring_buf_idx_t    The actual number of bytes available in the buffer
 */
px_ring_buf_idx_t px_ring_buf_free(px_ring_buf_t * px_ring_buf);


/** 
 * Get the number of bytes in the buffer, how full is the buffer?
 *  
 * @param px_ring_buf           Pointer to the circular buffer object
 * @return px_ring_buf_idx_t    The actual number of bytes in the buffer
 */
px_ring_buf_idx_t px_ring_buf_full(px_ring_buf_t * px_ring_buf);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
