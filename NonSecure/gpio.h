/**
 * @addtogroup Driver Driver
 * @{
 *
 * @file      gpio.h
 * @brief     Driver for general purpose input/output interface function (public header)
 * @author    Johnas Cukier
 * @date      January 2023
 */

/**
 * @addtogroup gpio
 * @{
 */
#ifndef __GPIO_H__
#define __GPIO_H__

/***************************************************************************************************************************
 * Includes
 */

#include <stdbool.h>
#include <stdint.h>
#include <nrfx.h>
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
    error_code_module_t ( *Init )( uint32_t gpio_dir, uint32_t gpio_init );
    void ( *Write )( uint8_t gpio_pin, uint8_t gpio_value );
    void ( *Read )( uint8_t gpio_pin, uint8_t *gpio_value );
} const gpio_driver_t;

/***************************************************************************************************************************
 * Public variables
 */

extern gpio_driver_t gpio;

#endif /* __GPIO_H__ */

/**
 * @} gpio
 */

/**
 * @} Driver
 */
