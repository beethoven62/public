/**
 * @addtogroup Driver Driver
 * @{
 *
 * @file      twim.h
 * @brief     Driver for two-wire interface (I2C) master module (public header)
 * @author    Johnas Cukier
 * @date      January 2023
 */

/**
 * @addtogroup twim
 * @{
 */
#ifndef __TWIM_H__
#define __TWIM_H__

/***************************************************************************************************************************
 * Includes
 */

#include <nrfx_twim.h>
#include "eelcodes.h"

/***************************************************************************************************************************
 * Public constants and macros
 */

/***************************************************************************************************************************
 * Public data structures and typedefs
 */

/**
 * Specifies the public interface functions of the two-wire interface driver.
 */
typedef struct
{
    error_code_module_t ( *Init )( uint8_t id, uint8_t scl_pin, uint8_t sda_pin );
    void ( *Transmit )( uint8_t addr, uint8_t *data, uint8_t len );
    void ( *Receive )( uint8_t addr, uint8_t *data, uint8_t len );
} const twim_driver_t;

/***************************************************************************************************************************
 * Public variables
 */

extern twim_driver_t twim;

#endif /* __TWIM_H__ */

/**
 * @} twim
 */

/**
 * @} Driver
 */
