#ifndef __PX_LCD_ST7565P_TSE2G0330E_H__
#define __PX_LCD_ST7565P_TSE2G0330E_H__
/* =============================================================================
     _____   _____   _____   ____    _   _    ____    __  __   _____  __   __
    |  __ \ |_   _| / ____| / __ \  | \ | |  / __ \  |  \/  | |_   _| \ \ / /
    | |__) |  | |  | |     | |  | | |  \| | | |  | | | \  / |   | |    \ V /
    |  ___/   | |  | |     | |  | | | . ` | | |  | | | |\/| |   | |     > < 
    | |      _| |_ | |____ | |__| | | |\  | | |__| | | |  | |  _| |_   / . \
    |_|     |_____| \_____| \____/  |_| \_|  \____/  |_|  |_| |_____| /_/ \_\

    Copyright (c) 2013 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_lcd_st7565p_tse2g0330e.h : Truly TSE2G0330-E 128x64 monochrome LCD wtih Sitronix ST7565P driver
    Author(s):      Pieter Conradie
    Creation Date:  2013-12-05

============================================================================= */

/** 
 *  @ingroup DEVICES_DISPLAY
 *  @defgroup PX_LCD_ST7565P_TSE2G0330E px_lcd_st7565p_tse2g0330e.h : Truly TSE2G0330-E 128x64 monochrome LCD wtih Sitronix ST7565P driver
 *
 *  File(s):
 *  - devices/display/px_lcd_st7565p_tse2g0330e.h
 *  - devices/display/px_lcd_st7565p_tse2g0330e.c
 *
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_spi.h"

// Include project specific config. See "px_lcd_st7565p_tse2g0330e_cfg_template.h"
#include "px_lcd_st7565p_tse2g0330e_cfg.h"

// Check that all project specific options have been specified in "px_lcd_st7565p_tse2g0330e_cfg.h"
#if (   !defined(PX_LCD_CFG_RST_LO     ) \
     || !defined(PX_LCD_CFG_RST_HI     ) \
     || !defined(PX_LCD_CFG_RS_LO      ) \
     || !defined(PX_LCD_CFG_RS_HI      ) \
     || !defined(PX_LCD_CFG_ROT_180_DEG)  )
#error "One or more options not defined in 'px_lcd_st7565p_tse2g0330e_cfg.h'"
#endif

/* _____DEFINITIONS__________________________________________________________ */
/// Maximum PX_SPI Clock rate
#define PX_LCD_MAX_SPI_CLOCK_HZ  20000000ul
/// PX_SPI Clock / Data phase
#define PX_LCD_SPI_MODE          PX_SPI_MODE3
/// PX_SPI Data order
#define PX_LCD_SPI_DATA_ORDER    PX_SPI_DATA_ORDER_MSB
/// Number of columns
#define PX_LCD_NR_OF_COLS        128
/// Number of rows
#define PX_LCD_NR_OF_ROWS        64
/// Number of pages
#define PX_LCD_NR_OF_PAGES       (PX_LCD_NR_OF_ROWS / 8)

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise LCD driver.
 *  
 *  @param handle       Pointer to SPI handle data structure
 */
void px_lcd_init(px_spi_handle_t * handle);

/**
 *  Clear LCD display.
 *  
 */
void px_lcd_clr(void);

/**
 *  Select LCD page (y) for display data.
 *  
 *  The LCD is divided up into 8 rows of 128 bytes. Each row is called a page.
 *  
 *  @param page     0 to (PX_LCD_NR_OF_PAGES - 1)
 */
void px_lcd_sel_page(uint8_t page);

/**
 *  Select LCD starting column (x) for display data.
 *  
 *  The LCD is divided up into 8 rows of 128 bytes. Each byte in the row maps to
 *  a column value.
 *  
 *  @param col      0 to (PX_LCD_NR_OF_COLS - 1)
 */
void px_lcd_sel_col(uint8_t col);

/**
 *  Write one byte of display data.
 *  
 *  The value will be written to the selected page (@see px_lcd_sel_page)
 *  and selected column (@see px_lcd_sel_col).
 *  
 *  @param data     Value to write to display
 */
void px_lcd_wr_disp_u8(uint8_t data);

/**
 *  Write data to display.
 *  
 *  The data will be written to the selected page (@see px_lcd_sel_page)
 *  and start at the selected column (@see px_lcd_sel_col).
 *  
 *  @note If data is written past the end of the column, it will not
 *  automatically wrap to the next page.
 *  
 *  @param data         Pointer to data to write
 *  @param nr_of_bytes  Number of bytes to write
 */
void px_lcd_wr_disp_data(uint8_t * data, size_t nr_of_bytes);

/* _____MACROS_______________________________________________________________ */

/// @}
#endif
