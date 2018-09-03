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

    Title:          px_i2c.h : I2C peripheral driver
    Author(s):      Pieter Conradie
    Creation Date:  2018-03-15

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_i2c.h"
#include "px_board.h"
#include "px_lib_stm32cube.h"

#include "px_dbg.h"
PX_DBG_DECL_NAME("px_i2c")

/* _____LOCAL DEFINITIONS____________________________________________________ */
/// Definition of data for each I2C peripheral
typedef struct px_i2c_data_s
{
    /// I2C peripheral base register address
    I2C_TypeDef * i2c_base_adr; 
    /// Peripheral
    px_i2c_per_t peripheral;
    /// Number of open handles referencing peripheral
    uint8_t open_counter;
} px_i2c_data_t;

/* _____LOCAL VARIABLES______________________________________________________ */
/// Allocate data for each enabled I2C peripheral
#if PX_I2C_CFG_I2C1_EN
static px_i2c_data_t px_i2c1_data;
#endif

#if PX_I2C_CFG_I2C2_EN
static px_i2c_data_t px_i2c2_data;
#endif

/* _____LOCAL FUNCTIONS______________________________________________________ */
static bool px_i2c_init_peripheral(I2C_TypeDef * i2c_base_adr,
                                   px_i2c_per_t  peripheral)
{
    // Enable peripheral clock
    switch(peripheral)
    {
#if PX_I2C_CFG_I2C1_EN
    case PX_I2C_PER_1:
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);
        break;
#endif
#if PX_I2C_CFG_I2C2_EN
    case PX_I2C_PER_2:
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);
        break;
#endif
    default:
        PX_DBG_ERR("Invalid peripheral");
        return false;
    }

    // Set Control register 1
    i2c_base_adr->CR1 = 0;
    // Set Control register 2
    i2c_base_adr->CR2 = 0;
    // Set Timing register
    LL_I2C_SetTiming(i2c_base_adr, PX_I2C_CFG_TIMINGR);

    // Enable I2C
    LL_I2C_Enable(i2c_base_adr);

    // Success
    return true;
}

static void px_i2c_init_peripheral_data(px_i2c_per_t    peripheral,
                                        px_i2c_data_t * i2c_data)
{
    // Set peripheral
    i2c_data->peripheral = peripheral;
    // Set peripheral base address
    switch(peripheral)
    {
#if PX_I2C_CFG_I2C1_EN
    case PX_I2C_PER_1:
        i2c_data->i2c_base_adr = I2C1;
        break;
#endif
#if PX_I2C_CFG_I2C2_EN
    case PX_I2C_PER_2:
        i2c_data->i2c_base_adr = I2C2;
        break;
#endif
    default:
        PX_DBG_ERR("Invalid peripheral");
        return;
    }
    // Clear reference counter
    i2c_data->open_counter = 0;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_i2c_init(void)
{    
    // Initialize peripheral data for each enabled peripheral
#if PX_I2C_CFG_I2C1_EN
    px_i2c_init_peripheral_data(PX_I2C_PER_1, &px_i2c1_data);
#endif
#if PX_I2C_CFG_I2C2_EN
    px_i2c_init_peripheral_data(PX_I2C_PER_2, &px_i2c2_data);
#endif
}

bool px_i2c_open(px_i2c_handle_t * handle,
                 px_i2c_per_t      peripheral,
                 uint8_t           slave_adr)
{
    px_i2c_data_t * i2c_data;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);

    // Handle not initialised
    handle->i2c_data = NULL;

    // Set pointer to peripheral data
    switch(peripheral)
    {
#if PX_I2C_CFG_I2C1_EN
    case PX_I2C_PER_1:
        i2c_data = &px_i2c1_data;
        break;
#endif
#if PX_I2C_CFG_I2C2_EN
    case PX_I2C_PER_2:
        i2c_data = &px_i2c2_data;
        break;
#endif
    default:
        PX_DBG_ERR("Invalid peripheral specified");
        return false;
    }

    // Save 7-bit slave address
    handle->slave_adr = slave_adr;

    // Peripheral closed?
    if(i2c_data->open_counter == 0)
    {
        // Initialise peripheral
        if(!px_i2c_init_peripheral(i2c_data->i2c_base_adr, peripheral))
        {
            // Invalid parameter specified
            return false;
        }
    }

    // Point handle to peripheral
    handle->i2c_data = i2c_data;

    // Success
    i2c_data->open_counter++;
    return true;
}

bool px_i2c_close(px_i2c_handle_t * handle)
{
    px_i2c_data_t * i2c_data;
    I2C_TypeDef *   i2c_base_adr;    

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    i2c_data = handle->i2c_data;
    // Check that handle is open
    PX_DBG_ASSERT(i2c_data != NULL);
    PX_DBG_ASSERT(i2c_data->open_counter != 0);

    // Get I2C peripheral base register address
    i2c_base_adr = i2c_data->i2c_base_adr;

    // Decrement open count
    i2c_data->open_counter--;

    // More open handles referencing peripheral?
    if(i2c_data->open_counter != 0)
    {
        // Close handle
        handle->i2c_data = NULL;
        // Success
        return true;
    }

    // Disable peripheral
    i2c_base_adr->CR1 = 0;

    // Disable peripheral clock
    switch(i2c_data->peripheral)
    {
#if PX_I2C_CFG_I2C1_EN
    case PX_I2C_PER_1:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C1);
        break;
#endif
#if PX_I2C_CFG_I2C2_EN
    case PX_I2C_PER_2:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C2);
        break;
#endif
    default:
        break;
    }

   // Close handle
   handle->i2c_data = NULL;

    // Success
    return true;
}

bool px_i2c_wr(px_i2c_handle_t * handle,
               const void *      data,
               size_t            nr_of_bytes,
               uint8_t           flags)
{
    px_i2c_data_t * i2c_data;
    I2C_TypeDef *   i2c_base_adr;
    const uint8_t * data_u8 = (const uint8_t *)data;
    uint32_t        i2c_isr;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    i2c_data = handle->i2c_data;
    // Check that handle is open
    PX_DBG_ASSERT(i2c_data != NULL);
    PX_DBG_ASSERT(i2c_data->open_counter != 0);
    // Check that slave address is 7 bits
    PX_DBG_ASSERT(handle->slave_adr < 0x80);

    // Get I2C peripheral base register address
    i2c_base_adr = i2c_data->i2c_base_adr;

    // Must RELOAD bit be cleared?
    if(  (nr_of_bytes <= 1)
       &&(  (flags & PX_I2C_FLAG_STOP) || (flags & PX_I2C_FLAG_END)  )  )
    {
        LL_I2C_DisableReloadMode(i2c_base_adr);
    }
    else
    {
        LL_I2C_EnableReloadMode(i2c_base_adr);
    }
    // Set the number of bytes to transfer
    if(nr_of_bytes > 0)
    {
        LL_I2C_SetTransferSize(i2c_base_adr, 1);
    }
    else
    {
        LL_I2C_SetTransferSize(i2c_base_adr, 0);
    }

    // Generate START or REPEATED START condition?
    if(  (flags & PX_I2C_FLAG_START) || (flags & PX_I2C_FLAG_REP_START)  )
    {
        if(flags & PX_I2C_FLAG_START)
        {
            // I2C bus must be idle for a START condition
            if(LL_I2C_IsActiveFlag_BUSY(i2c_base_adr))
            {
                PX_DBG_ERR("I2C bus is busy with a previous transaction");
                return false;
            }
        }
        else
        {
            // I2C bus must be busy for a REPEATED START condition
            if(!LL_I2C_IsActiveFlag_BUSY(i2c_base_adr))
            {
                PX_DBG_ERR("I2C bus is idle");
                return false;
            }
        }
        // Set slave address
        LL_I2C_SetSlaveAddr(i2c_base_adr, (handle->slave_adr)<<1);
        // Set transfer direction to write
        LL_I2C_SetTransferRequest(i2c_base_adr, LL_I2C_REQUEST_WRITE);        
        // Generate START condition
        LL_I2C_GenerateStartCondition(i2c_base_adr);
        if(flags & PX_I2C_FLAG_START)
        {
            // Wait until I2C bus is busy
            while(!LL_I2C_IsActiveFlag_BUSY(i2c_base_adr))
            {
                ;
            }
        }
        // Wait until START condition has ended
        while(i2c_base_adr->CR2 & I2C_CR2_START)
        {
            ;
        }
        // Received NAK?
        if(LL_I2C_IsActiveFlag_NACK(i2c_base_adr))
        {
            // Clear flag
            LL_I2C_ClearFlag_NACK(i2c_base_adr);
            // I2C slave not present. STOP condition automatically generated
            PX_DBG_WARN("I2C Slave did not ACK SLA+W");
            // Wait until bus is free
            while(LL_I2C_IsActiveFlag_BUSY(i2c_base_adr))
            {
                ;
            }
            return false;
        }
    }

    // Write byte(s)
    while(true)
    {
        // I2C Bus must be busy
        if(!LL_I2C_IsActiveFlag_BUSY(i2c_base_adr))
        {
            PX_DBG_ERR("I2C bus is idle");
            return false;
        }
        // Wait for a flag to be set
        do
        {
            i2c_isr = i2c_base_adr->ISR;
        }
        while((i2c_isr & (I2C_ISR_TXIS | I2C_ISR_NACKF | I2C_ISR_TC | I2C_ISR_TCR | I2C_ISR_BERR)) == 0);

        // Received Bus Error?
        if(i2c_isr & I2C_ISR_BERR)
        {
            // Clear flag
            LL_I2C_ClearFlag_BERR(i2c_base_adr);
            // I2C Bus Error
            PX_DBG_ERR("I2C Bus Error");
            // Wait until bus is free
            while(LL_I2C_IsActiveFlag_BUSY(i2c_base_adr))
            {
                ;
            }
            return false;
        }
        // Received NAK?
        if(i2c_isr & I2C_ISR_NACKF)
        {
            // Clear flag
            LL_I2C_ClearFlag_NACK(i2c_base_adr);
            // I2C slave NACKed byte. STOP condition automatically generated
            PX_DBG_ERR("I2C Slave did not ACK byte");
            // Wait until bus is free
            while(LL_I2C_IsActiveFlag_BUSY(i2c_base_adr))
            {
                ;
            }
            return false;
        }
        // Transmit buffer ready to accept next byte?
        if(i2c_isr & I2C_ISR_TXIS)
        {
            if(nr_of_bytes != 0)
            {
                // Load one byte to be transmitted
                LL_I2C_TransmitData8(i2c_base_adr, *data_u8++);
                // Decrement number of bytes
                nr_of_bytes--;
            }
        }
        // Received TC?
        if(i2c_isr & I2C_ISR_TC)
        {
            // Finished transmitting bytes?
            if(nr_of_bytes == 0)
            {
                // Yes
                break;
            }
        }
        // Received TCR?
        if(i2c_isr & I2C_ISR_TCR)
        {    
            // Finished transmitting bytes?
            if(nr_of_bytes == 0)
            {
                // Yes
                break;
            }
            // Set the number of bytes to transfer
            LL_I2C_SetTransferSize(i2c_base_adr, 1);
        }
    }

    // Generate STOP condition?
    if(flags & PX_I2C_FLAG_STOP)
    {
        // I2C Bus must be busy
        if(!LL_I2C_IsActiveFlag_BUSY(i2c_base_adr))
        {
            PX_DBG_ERR("I2C bus is idle");
            return false;
        }
        // Generate STOP condition
        LL_I2C_GenerateStopCondition(i2c_base_adr);
        // Wait until stop condition has ended
        while(!LL_I2C_IsActiveFlag_STOP(i2c_base_adr))
        {
            ;
        }
        // Clear flag
        LL_I2C_ClearFlag_STOP(i2c_base_adr);
        // Wait until bus is free
        while(LL_I2C_IsActiveFlag_BUSY(i2c_base_adr))
        {
            ;
        }
    }

    // Success
    return true;
}

bool px_i2c_rd(px_i2c_handle_t * handle,
               void *            data,
               size_t            nr_of_bytes,
               uint8_t           flags)
{
    px_i2c_data_t * i2c_data;
    I2C_TypeDef *   i2c_base_adr;
    uint8_t *       data_u8 = (uint8_t *)data;
    uint32_t        i2c_isr;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    i2c_data = handle->i2c_data;
    // Check that handle is open
    PX_DBG_ASSERT(i2c_data != NULL);
    PX_DBG_ASSERT(i2c_data->open_counter != 0);
    // Check that slave address is 7 bits
    PX_DBG_ASSERT(handle->slave_adr < 0x80);

    // Get I2C peripheral base register address
    i2c_base_adr = i2c_data->i2c_base_adr;

    // Must RELOAD bit be cleared?
    if(  (nr_of_bytes <= 1)
       &&(  (flags & PX_I2C_FLAG_STOP) || (flags & PX_I2C_FLAG_END)  )  )
    {
        LL_I2C_DisableReloadMode(i2c_base_adr);
    }
    else
    {
        LL_I2C_EnableReloadMode(i2c_base_adr);
    }
    // Set the number of bytes to transfer
    if(nr_of_bytes > 0)
    {
        LL_I2C_SetTransferSize(i2c_base_adr, 1);
    }
    else
    {
        LL_I2C_SetTransferSize(i2c_base_adr, 0);
    }

    // Generate START or REPEATED START condition?
    if(  (flags & PX_I2C_FLAG_START) || (flags & PX_I2C_FLAG_REP_START)  )
    {
        if(flags & PX_I2C_FLAG_START)
        {
            // I2C bus must be idle for a START condition
            if(LL_I2C_IsActiveFlag_BUSY(i2c_base_adr))
            {
                PX_DBG_ERR("I2C bus is busy with a previous transaction");
                return false;
            }
        }
        else
        {
            // I2C bus must be busy for a REPEATED START condition
            if(!LL_I2C_IsActiveFlag_BUSY(i2c_base_adr))
            {
                PX_DBG_ERR("I2C bus is idle");
                return false;
            }
        }
        // Set slave address
        LL_I2C_SetSlaveAddr(i2c_base_adr, (handle->slave_adr)<<1);
        // Set transfer direction to read
        LL_I2C_SetTransferRequest(i2c_base_adr, LL_I2C_REQUEST_READ);
        // Generate START condition
        LL_I2C_GenerateStartCondition(i2c_base_adr);
        if(flags & PX_I2C_FLAG_START)
        {
            while(!LL_I2C_IsActiveFlag_BUSY(i2c_base_adr))
            {
                ;
            }
        }
        // Wait until START condition has ended
        while(i2c_base_adr->CR2 & I2C_CR2_START)
        {
            ;
        }
        // Received NAK?
        if(LL_I2C_IsActiveFlag_NACK(i2c_base_adr))
        {
            // Clear flag
            LL_I2C_ClearFlag_NACK(i2c_base_adr);
            // I2C slave not present. STOP condition automatically generated
            PX_DBG_WARN("I2C Slave did not ACK SLA+R");
            // Wait until bus is free
            while(LL_I2C_IsActiveFlag_BUSY(i2c_base_adr))
            {
                ;
            }
            return false;
        }
    }

    // Read byte(s)
    while(true)
    {
        // I2C Bus must be busy
        if(!LL_I2C_IsActiveFlag_BUSY(i2c_base_adr))
        {
            PX_DBG_ERR("I2C bus is idle");
            return false;
        }
        // Wait for a flag to be set
        do
        {
            i2c_isr = i2c_base_adr->ISR;
        }
        while((i2c_isr & (I2C_ISR_RXNE | I2C_ISR_TC | I2C_ISR_TCR | I2C_ISR_NACKF)) == 0);

        // Received byte in buffer?
        if(i2c_isr & I2C_ISR_RXNE)
        {
            if(nr_of_bytes != 0)
            {
                // Fetch byte from receive buffer
                *data_u8++ = LL_I2C_ReceiveData8(i2c_base_adr);
                // Decrement number of bytes
                nr_of_bytes--;
            }
        }        
        // Received NAK?
        if(i2c_isr & I2C_ISR_NACKF)
        {
            // Clear flag
            LL_I2C_ClearFlag_NACK(i2c_base_adr);
            // Received NACKF, but dit not expect it
            PX_DBG_ERR("Received NACKF");
            // Wait until bus is free
            while(LL_I2C_IsActiveFlag_BUSY(i2c_base_adr))
            {
                ;
            }
            return false;
        }
        // Received TC?
        if(i2c_isr & I2C_ISR_TC)
        {
            // Finished receiving bytes?
            if(nr_of_bytes == 0)
            {
                // Yes
                break;
            }
        }
        // Received TCR?
        if(i2c_isr & I2C_ISR_TCR)
        {
            // Must RELOAD bit be cleared?
            if(  (nr_of_bytes == 1)
               &&(  (flags & PX_I2C_FLAG_STOP) || (flags & PX_I2C_FLAG_END)  )  )
            {
                LL_I2C_DisableReloadMode(i2c_base_adr);
            }

            // Finished receiving bytes?
            if(nr_of_bytes == 0)
            {
                // Yes
                break;
            }
            // Set the number of bytes to transfer
            LL_I2C_SetTransferSize(i2c_base_adr, 1);
        }        
    }

    // Generate STOP condition?
    if(flags & PX_I2C_FLAG_STOP)
    {
        // I2C Bus must be busy
        if(!LL_I2C_IsActiveFlag_BUSY(i2c_base_adr))
        {
            PX_DBG_ERR("I2C bus is idle");
            return false;
        }
        // Generate STOP condition
        LL_I2C_GenerateStopCondition(i2c_base_adr);
        // Wait until stop condition has ended
        while(!LL_I2C_IsActiveFlag_STOP(i2c_base_adr))
        {
            ;
        }
        // Clear flag
        LL_I2C_ClearFlag_STOP(i2c_base_adr);
        // Wait until bus is free
        while(LL_I2C_IsActiveFlag_BUSY(i2c_base_adr))
        {
            ;
        }
    }

    // Success
    return true;
}

void px_i2c_change_slave_adr(px_i2c_handle_t * handle,
                             uint8_t           slave_adr)
{
    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Check that slave address is 7 bits
    PX_DBG_ASSERT(slave_adr < 0x80);

    handle->slave_adr = slave_adr;
}
