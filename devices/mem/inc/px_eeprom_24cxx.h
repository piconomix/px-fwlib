#ifndef __PX_EEPROM_24CXX_H__
#define __PX_EEPROM_24CXX_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          24Cxx I2C EEPROM Driver
    Author(s):      pconradi
    Creation Date:  2012-06-05

============================================================================= */

/** 
 *  @ingroup DEVICES_MEM
 *  @defgroup PX_EEPROM_24CXX px_eeprom_24cxx.h : ST 24Cxx I2C EEPROM Driver
 *  
 *  Driver that communicates with an ST 24Cxx EEPROM using I2C.
 *  
 *  File(s):
 *  - devices/mem/inc/px_eeprom_24cxx.h
 *  - devices/mem/src/px_eeprom_24cxx.c
 *  
 *  Reference:
 *  - [ST 24Cxx] (http://www.st.com/internet/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/DATASHEET/DM00061111.pdf) 16-Kbit serial I2C bus EEPROM datasheet
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// I2C Slave Address (excluding Chip Enable address A10, A9 and A8)
#define PX_EEPROM_24CXX_I2C_SLA_ADR 0x50

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 * Initialise driver.
 *  
 * @param px_i2c_handle    Pointer to I2C driver handle 
 */
extern void px_eeprom_24cxx_init(px_i2c_handle_t * px_i2c_handle);

/**
 * Check if device is present / not busy
 *  
 * @retval true     Device is present / not busy 
 * @retval false    Device is absent  / busy 
 */
extern bool px_eeprom_24cxx_present_or_not_busy(void);

/**
 *  Read one byte from device.
 *  
 *  @param adr      Address of byte to read
 *  @param data     Pointer to location where data byte be stored
 *  
 *  @retval true    Data succesfully read
 *  @retval false   Bus error
 */
extern bool px_eeprom_24cxx_rd_u8(uint16_t adr, uint8_t * data);

/**
 *  Read data from device.
 *  
 *  @param adr          Start address of byte(s) to read
 *  @param data         Pointer to location where data byte(s) be stored
 *  @param nr_of_bytes  Number of bytes to read
 *  
 *  @retval true    Data succesfully read
 *  @retval false   Bus error
 */
extern bool px_eeprom_24cxx_rd_data(uint16_t adr, void * data, size_t nr_of_bytes);

/**
 * Write one byte to device.
 *  
 * px_eeprom_24cxx_present_or_not_busy() must be used to find out when write 
 * operation has finished, or a fixed, maximum delay between operations can be 
 * used.
 *  
 * @param adr   Address of byte to write
 * @param data  Value of byte to write
 *  
 * @return true     Data write operation succesfully started 
 * @retval false    Bus error 
 */
extern bool px_eeprom_24cxx_wr_u8(uint16_t adr, uint8_t data);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
