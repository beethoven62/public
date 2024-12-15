/** @file gpio_priv.h
 *
 * @brief       Driver for general purpose input/output interface function (private header).
 * @author      Johnas Cukier
 * @date        January 2023
 *
 */

/**
 * @addtogroup gpio
 * @{
 */

#ifndef __GPIO_PRIV_H__
#define __GPIO_PRIV_H__

/***************************************************************************************************************************
 * Includes
 */

#include "gpio.h"

/***************************************************************************************************************************
 * Private constants and macros
 */

/***************************************************************************************************************************
 * Private data structures and typedefs
 */

typedef struct
{
    bool                        is_init;
} gpio_obj_t ;

/***************************************************************************************************************************
 * Private variables
 */

/***************************************************************************************************************************
 * Private prototypes (interface functions)
 */

/**
 * @brief       Initialize and start the GPIO interface driver.
 * @param[in]   gpio_dir     GPIO pin direction bit map (0 = input, 1 = output).
 * @param[in]   gpio_init    GPIO pin initialization bit map (Input: 0 = HiZ, 1 = pullup; Output: 0 = low, 1 = high).
 * @return      Error code.
 */
static error_code_module_t gpio_Init( uint32_t gpio_dir, uint32_t gpio_init );

/**
 * @brief       General Purpose Input/Output write function.
 * @param[in]   gpio_pin     GPIO pin.
 * @param[in]   gpio_value   GPIO logic level.
 */
static void gpio_Write( uint8_t gpio_pin, uint8_t gpio_value );

/**
 * @brief       Two-wired interface receive function.
 * @brief       General Purpose Input/Output read function.
 * @param[in]   gpio_pin     GPIO pin.
 * @param[out]  gpio_value   GPIO logic level.
 */
static void gpio_Read( uint8_t gpio_pin, uint8_t *gpio_value );

/***************************************************************************************************************************
 * Private prototypes
 */

#endif /* __GPIO_PRIV_H__ */

/**
 * @}
 */
