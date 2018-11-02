#ifndef __PX_SPI_CFG_H__
#define __PX_SPI_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
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
    
    Title:          px_spi_cfg.h : SPI Peripheral Driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2018-03-02

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_board.h"

/* _____DEFINITIONS _________________________________________________________ */
/// Enable/disable support for SPI1 peripheral
#define PX_SPI_CFG_SPI1_EN 1

/// Enable/disable support for SPI2 peripheral
#define PX_SPI_CFG_SPI2_EN 1

/// Specify default baud rate
#define PX_SPI_CFG_DEFAULT_BAUD         PX_SPI_BAUD_CLK_DIV_32

/// Specify default SPI mode (clock phase and polarity)
#define PX_SPI_CFG_DEFAULT_MODE         PX_SPI_MODE0

/// Specify default SPI data order
#define PX_SPI_CFG_DEFAULT_DATA_ORDER   PX_SPI_DATA_ORDER_MSB

/**
 *  Map PX_SPI_CFG_CS_LO() macro to px_board_spi_cs_lo() function.
 *  A manual Chip Select function must be implemented, e.g.
 *  
 *      void px_board_spi_cs_lo(uint8_t cs_id)
 *      {
 *          switch(cs_id)
 *          {
 *          case BOARD_SPI_CS:
 *              PX_GPIO_PIN_SET_LO(PX_GPIO_SPI_CS);
 *              break;
 *  
 *          default:
 *              break;
 *          }
 *      }
 */
#define PX_SPI_CFG_CS_LO(cs_id)     px_board_spi_cs_lo(cs_id)

/** 
 *  Map PX_SPI_CFG_CS_HI() macro to px_board_spi_cs_hi() function.
 *  A manual Chip Select function must be implemented, e.g.
 *  
 *      void px_board_spi_cs_hi(uint8_t cs_id)
 *      {
 *          switch(cs_id)
 *          {
 *          case BOARD_SPI_CS:
 *              PX_GPIO_PIN_SET_HI(PX_GPIO_SPI_CS);
 *              break;
 *  
 *          default:
 *              break;
 *          }
 *      }
 *  
 */
#define PX_SPI_CFG_CS_HI(cs_id)     px_board_spi_cs_hi(cs_id)

#endif // #ifndef __PX_SPI_CFG_H__
