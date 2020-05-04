#ifndef __PX_I2C_H__
#define __PX_I2C_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          px_i2c.h : I2C peripheral driver
    Author(s):      Pieter Conradie
    Creation Date:  2012-06-25

============================================================================= */

/** 
 *  @ingroup AVR
 *  @defgroup AVR_I2C px_i2c.h : I2C peripheral driver
 *  
 *  Driver to communicate with I2C slaves.
 *  
 *  File(s):
 *  - arch/avr/px_i2c.h
 *  - arch/avr/px_i2c_cfg_template.h
 *  - arch/avr/px_i2c_twi.c  (TWI peripheral version)
 *  - arch/avr/px_i2c_gpio.c (GPIO "bit-bang" version)
 *  
 *  The driver must be configured by supplying a project specific "px_i2c_cfg.h".
 *  "px_i2c_cfg_template.h" can be copied, renamed and modified to supply 
 *  compile time options.
 *  
 *  @par Example:
 *  @include arch/avr/test/px_i2c_test.c
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

// Include project specific configuration. See "px_i2c_cfg_template.h"
#include "px_i2c_cfg.h"

// Check that all project specific options have been specified in "px_i2c_cfg.h"
#if (   !defined(PX_I2C_CFG_I2C0_EN          ) \
     || !defined(PX_I2C_CFG_BIT_RATE_HZ      ) \
     || !defined(PX_I2C_CFG_TWI_PRESCALER_VAL)  )
#error "One or more options not defined in 'px_i2c_cfg.h'"
#endif
#if (PX_I2C_CFG_I2C0_EN > 1)
#error "PX_I2C_CFG_I2Cx_EN must be 0 or 1"
#endif

/// Number of enabled peripherals
#define PX_I2C_CFG_PER_COUNT   (PX_I2C_CFG_I2C0_EN)
#if (PX_I2C_CFG_PER_COUNT == 0)
#error "No peripherals enabled"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Specify I2C peripheral
typedef enum
{
    PX_I2C_NR_0 = 0,
} px_i2c_nr_t;

/// @name I2C bit flags to demarcate the start and end of a transaction
//@{
/// Begin I2C transaction with a START condition
#define PX_I2C_FLAG_START               (1<<0)
/// Finish I2C transaction with a STOP condition
#define PX_I2C_FLAG_STOP                (1<<1)
/// Signal END of current I2C transaction for REP_START to follow
#define PX_I2C_FLAG_END                 (1<<2)
/// Begin I2C transaction with a REPEATED START condition
#define PX_I2C_FLAG_REP_START           (1<<3)
/// Begin and finish an I2C transaction with a START and STOP condition
#define PX_I2C_FLAG_START_AND_STOP      (PX_I2C_FLAG_START + PX_I2C_FLAG_STOP)
/// Begin and finish an I2C transaction with a START and END condition
#define PX_I2C_FLAG_START_AND_END       (PX_I2C_FLAG_START + PX_I2C_FLAG_END)
/// Begin and finish an I2C transaction with a REP START and STOP condition
#define PX_I2C_FLAG_REP_START_AND_STOP  (PX_I2C_FLAG_REP_START + PX_I2C_FLAG_STOP)
/// Begin and finish an I2C transaction with a REP START and END condition
#define PX_I2C_FLAG_REP_START_AND_END   (PX_I2C_FLAG_REP_START + PX_I2C_FLAG_END)
//@}

/// Define I2C handle for a slave
typedef struct
{
    struct px_i2c_per_s * i2c_per;      ///< I2C peripheral data
    uint8_t               slave_adr;    ///< 7-bit I2C slave address
} px_i2c_handle_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise I2C driver.
 */
void px_i2c_init(void);

/** 
 *  Open I2C slave handle.
 *  
 *  @param handle       Pointer to handle data structure
 *  @param i2c_nr       I2C peripheral number
 *  @param slave_adr    7-bit I2C slave address
 *  
 *  @retval false       Error - handle was not opened
 *  @retval true        Success - handle was opened
 */
bool px_i2c_open(px_i2c_handle_t * handle, 
                 px_i2c_nr_t       i2c_nr,
                 uint8_t           slave_adr);

/**
 *  Close specified handle.
 *  
 *  @param handle    Pointer to handle data structure
 *  
 *  @retval true     Success 
 *  @retval false    Specified handle was already closed (or not opened)
 */
bool px_i2c_close(px_i2c_handle_t * handle);

/**
 *  Perform an I2C write transaction with an I2C slave.
 *  
 *  With the use of flags this function allows concatenation of one or more
 *  writes to an I2C slave to form one big write transaction. It is also
 *  possible to generate a REPEATED START (RESTART) condition by not generating
 *  an intervening STOP condition.
 *  
 *  If the PX_I2C_FLAG_START flag is specified, a START condition is generated
 *  and the 7-bit slave address and WRITE bit is sent on the bus (SLA+W). If a
 *  NAK is received, a STOP condition is generated to return the I2C bus to idle
 *  state.
 *  
 *  The specified number of bytes is sent with the slave supposed to ACK each
 *  byte. If a NAK is received, a STOP condition is generated to return the I2C
 *  bus to idle state.
 *  
 *  If the PX_I2C_FLAG_STOP flag is specified, a STOP condition is generated to
 *  complete the transaction and return the I2C bus to the idle state.
 *  
 *  More than one I2C transaction may be concatenated without releasing the I2C
 *  bus by using a REPEATED START condition. This is acheived by signalling the
 *  end of the current transaction with the PX_I2C_FLAG_END flag and starting
 *  the next transaction with a PX_I2C_FLAG_REP_START flag.
 *  
 *  @param handle       Pointer to handle data structure
 *  @param data         Buffer containing data to write
 *  @param nr_of_bytes  Number of bytes to write to the slave
 *  @param flags        Bit combination of PX_I2C_FLAG_START,
 *                      PX_I2C_FLAG_STOP, PX_I2C_FLAG_REP_START, PX_I2C_FLAG_END
 *                      or nothing (0)
 *  
 *  @retval true        Slave ACK'd all bytes written to it
 *  @retval false       Received a NAK or bus error
 */
bool px_i2c_wr(px_i2c_handle_t * handle,
               const void *      data,
               size_t            nr_of_bytes,
               uint8_t           flags);

/**
 *  Perform an I2C read transaction with an I2C slave.
 *  
 *  With the use of flags this function allows concatenation of one or more
 *  reads from an I2C slave to form one big read transaction. It is also
 *  possible to generate a REPEATED START (RESTART) condition by not generating
 *  an intervening STOP condition.
 *  
 *  If the PX_I2C_FLAG_START flag is specified, A START condition is generated
 *  and the 7-bit slave address and READ bit is sent on the bus (SLA+R).
 *  If a NAK is received, a STOP condition is generated to return the I2C bus to
 *  idle state.
 *  
 *  The specified number of bytes is read from the slave and each byte is
 *  ACKed, except the last one. If the PX_I2C_FLAG_STOP or PX_I2C_FLAG_END flag
 *  is specified, the last byte is NAKed to indicate to the I2C slave that this
 *  is the end of the read transaction, otherwise it is also ACKed to allow the
 *  read transaction to continue.
 *  
 *  If the PX_I2C_FLAG_STOP flag is specified, a STOP condition is generated to
 *  complete the transaction and return the I2C bus to the idle state.
 *  
 *  More than one I2C transaction may be concatenated without releasing the I2C
 *  bus by using a REPEATED START condition. This is acheived by signalling the
 *  end of the current transaction with the PX_I2C_FLAG_END flag and starting
 *  the next transaction with a PX_I2C_FLAG_REP_START flag.
 *  
 *  @param handle           Pointer to handle data structure
 *  @param data             Pointer to a buffer where the received data must be
 *                          stored
 *  @param nr_of_bytes      Number of bytes to read from slave
 *  @param flags            Bit combination of PX_I2C_FLAG_START,
 *                          PX_I2C_FLAG_STOP, PX_I2C_FLAG_REP_START,
 *                          PX_I2C_FLAG_END or nothing (0)
 *  
 *  @retval true            Byte(s) succesfully read
 *  @retval false           Received NAK or bus error
 */
bool px_i2c_rd(px_i2c_handle_t * handle,
               void *            data,
               size_t            nr_of_bytes,
               uint8_t           flags);

/**
 *  Change I2C slave address.
 *  
 *  @param handle       Pointer to handle data structure
 *  @param slave_adr    New 7-bit I2C slave address
 */
void px_i2c_ioctl_change_slave_adr(px_i2c_handle_t * handle,
                                   uint8_t           slave_adr);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_I2C_H__
