#ifndef __PX_SPI_CFG_H__
#define __PX_SPI_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_spi_cfg.h : SPI Peripheral Driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2018-03-02

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_board.h"

/* _____DEFINITIONS__________________________________________________________ */
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
 *              PX_GPIO_OUT_SET_HI(PX_GPIO_SPI_CS);
 *              break;
 *  
 *          default:
 *              break;
 *          }
 *      }
 *  
 */
#define PX_SPI_CFG_CS_HI(cs_id)     px_board_spi_cs_hi(cs_id)

#endif
