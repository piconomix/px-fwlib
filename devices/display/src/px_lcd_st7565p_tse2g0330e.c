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

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_lcd_st7565p_tse2g0330e.h"
#include "px_board.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("lcd");

/// @name ST7565P commands (Table 16, page 51)
//@{
#define PX_LCD_CMD_DISP_ON_OFF          0xae    ///< (1)  Display ON/OFF
#define PX_LCD_CMD_DISP_START_LINE_SET  0x40    ///< (2)  Display start line set 
#define PX_LCD_CMD_PAGE_ADR_SET         0xb0    ///< (3)  Page address set
#define PX_LCD_CMD_COL_ADR_SET_HI       0x10    ///< (4)  Column address set upper bit
#define PX_LCD_CMD_COL_ADR_SET_LO       0x00    ///< (4)  Column address set lower bit
#define PX_LCD_CMD_ADC_SEL              0xa0    ///< (8)  ADC select
#define PX_LCD_CMD_DISP_NORMAL_REVERSE  0xa6    ///< (9)  Display normal/reverse
#define PX_LCD_CMD_DISP_ALL_ON_OFF      0xa4    ///< (10) Display all points
#define PX_LCD_CMD_BIAS_SET             0xa2    ///< (11) LCD bias set
#define PX_LCD_CMD_RST                  0xe2    ///< (14) Reset
#define PX_LCD_CMD_COM_OUT_MODE_SEL     0xc0    ///< (15) Common output mode select
#define PX_LCD_CMD_PWR_CTL_SET          0x28    ///< (16) Power control set
#define PX_LCD_CMD_V_REG_RES_RATIO_SET  0x20    ///< (17) Vo voltage regulator internal resistor ratio set
#define PX_LCD_CMD_VOL_MODE_SET         0x81    ///< (18) Electronic volume mode set
#define PX_LCD_CMD_VOL_REG_SET          0x00    ///< (18) Electronic volume register set
#define PX_LCD_CMD_STATIC_IND_ON_OFF    0xac    ///< (19) Static indicator ON/OFF
#define PX_LCD_CMD_STATIC_IND_REG_SET   0x00    ///< (19) Static indicator register set
#define PX_LCD_CMD_BOOSTER_RATIO_SET    0xf8    ///< (20) Booster ratio set
//@}

#define PX_LCD_ST7565P_MAX_NR_OF_COLS   132
#define PX_LCD_ST7565P_MAX_NR_OF_ROWS   65

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
///  SPI handle to communicate with LCD
static px_spi_handle_t * px_lcd_spi_handle;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void px_lcd_wr_control_data(uint8_t data)
{
    PX_LOG_D("LCD Cmd 0x%02X", data);

    PX_LCD_CFG_RS_LO();
    px_spi_wr(px_lcd_spi_handle, &data, 1, PX_SPI_FLAG_START_AND_STOP);
    PX_LCD_CFG_RS_HI();
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_lcd_init(px_spi_handle_t * handle)
{
    // Save handle
    px_lcd_spi_handle = handle;

    // Perform hardware reset
    PX_LCD_CFG_RST_LO();
    px_board_delay_ms(50);
    PX_LCD_CFG_RST_HI();

    // Select 1/9 bias
    px_lcd_wr_control_data(PX_LCD_CMD_BIAS_SET | 0);
    // Select normal COM output scan direction
    px_lcd_wr_control_data(PX_LCD_CMD_COM_OUT_MODE_SEL | 0);

#if PX_LCD_CFG_ROT_180_DEG
    // Reverse SEG output direction, because LCD is rotated by 180 deg
    px_lcd_wr_control_data(PX_LCD_CMD_ADC_SEL | 1);
#else
    // Normal SEG output direction
    px_lcd_wr_control_data(PX_LCD_CMD_ADC_SEL | 0);
#endif

    // Set display line adr to 0
    px_lcd_wr_control_data(PX_LCD_CMD_DISP_START_LINE_SET | 0);
    // Enable voltage booster, regulator & follower
    px_lcd_wr_control_data(PX_LCD_CMD_PWR_CTL_SET | 0x7);
    // Regulator resistor select
    px_lcd_wr_control_data(PX_LCD_CMD_V_REG_RES_RATIO_SET | 0x3);
    //set reference voltage mode
    px_lcd_wr_control_data(PX_LCD_CMD_VOL_MODE_SET);
    // set reference voltage
    px_lcd_wr_control_data(47);
    // set normal display
    px_lcd_wr_control_data(PX_LCD_CMD_DISP_ALL_ON_OFF | 0);
    // set reverse display OFF
    px_lcd_wr_control_data(PX_LCD_CMD_DISP_NORMAL_REVERSE | 0);
    // Clear RAM buffer
    px_lcd_clr();
    // turns the display ON
    px_lcd_wr_control_data(PX_LCD_CMD_DISP_ON_OFF | 1);
}

void px_lcd_clr(void)
{
    uint8_t y;
    uint8_t data[PX_LCD_NR_OF_COLS];

    // Set data for whole column to zero
    memset(data, 0, PX_LCD_NR_OF_COLS);

    // Repeat for each page
    for(y=0; y < PX_LCD_NR_OF_PAGES; y++)
    {
        // Select page
        px_lcd_sel_page(y);
        // Start at column 0
        px_lcd_sel_col(0);
        // Clear whole column
        px_lcd_wr_disp_data(data, PX_LCD_NR_OF_COLS);        
    }
}

void px_lcd_sel_col(uint8_t col)
{
#if PX_LCD_CFG_ROT_180_DEG
    col = (PX_LCD_ST7565P_MAX_NR_OF_COLS - PX_LCD_NR_OF_COLS) + col;
#endif

    // Column most significant nibble
    px_lcd_wr_control_data(PX_LCD_CMD_COL_ADR_SET_HI | ((col&0xf0)>>4));
    // Column least significant nibble
    px_lcd_wr_control_data(PX_LCD_CMD_COL_ADR_SET_LO | (col&0x0f));
}

void px_lcd_sel_page(uint8_t page)
{
#if !PX_LCD_CFG_ROT_180_DEG
    page = (PX_LCD_NR_OF_PAGES - 1) - page;
#endif

    px_lcd_wr_control_data(PX_LCD_CMD_PAGE_ADR_SET | page);
}

void px_lcd_wr_disp_u8(uint8_t data)
{
    px_spi_wr(px_lcd_spi_handle, &data, 1, PX_SPI_FLAG_START_AND_STOP);
}

void px_lcd_wr_disp_data(uint8_t * data, size_t nr_of_bytes)
{
    px_spi_wr(px_lcd_spi_handle, data, nr_of_bytes, PX_SPI_FLAG_START_AND_STOP);
}
