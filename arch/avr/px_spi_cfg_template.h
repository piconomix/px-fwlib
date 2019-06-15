#ifndef __PX_SPI_CFG_H__
#define __PX_SPI_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_spi_cfg.h : SPI Peripheral Driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2014-01-15

============================================================================= */

/** 
 *  @addtogroup AVR_SPI
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_board.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Enable/disable support for SPI0 peripheral
#define PX_SPI_CFG_SPI0_EN 1

/// Default value for SPI Control Register used in px_spi_open().
#define PX_SPI_CFG_SPCR \
    ((0<<SPIE) | (1<<SPE) | (0<<DORD) | (1<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0))

/// Default value for "Use double clock rate" used in px_spi_open().
#define PX_SPI_CFG_USE_SPI2X    0

/// Specify default baud rate
#define PX_SPI_CFG_DEFAULT_BAUD         PX_SPI_BAUD_CLK_DIV_2

/// Specify default SPI mode (clock phase and polarity)
#define PX_SPI_CFG_DEFAULT_MODE         PX_SPI_MODE0

/// Specify default SPI data order
#define PX_SPI_CFG_DEFAULT_DATA_ORDER   PX_SPI_DATA_ORDER_MSB

/**
 *  Map PX_SPI_CFG_CS_LO() macro to board_spi_cs_lo() function.
 *  A manual Chip Select function must be implemented, e.g.
 *  
 *      void board_spi_cs_lo(uint8_t cs_id)
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
#define PX_SPI_CFG_CS_LO(cs_id) px_board_spi_cs_lo(cs_id)

/** 
 *  Map PX_SPI_CFG_CS_HI() macro to board_spi_cs_hi() function.
 *  A manual Chip Select function must be implemented, e.g.
 *  
 *      void board_spi_cs_hi(uint8_t cs_id)
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
#define PX_SPI_CFG_CS_HI(cs_id) px_board_spi_cs_hi(cs_id)

/// @}
#endif // #ifndef __PX_SPI_CFG_H__
