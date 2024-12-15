/**
 * @addtogroup Driver
 * @{
 * @file      gpio.c
 * @brief     Driver for general purpose input/output interface function.
 * @author    Johnas Cukier
 * @date      Jan 2023
 */

/**
 * @defgroup gpio General Purpose Input/Output driver module
 * @brief     General purpose I/O driver
 * @{
 */

/*************************************************************************************************************************************
 * Includes
*/
#include "gpio_priv.h"

/***************************************************************************************************************************
 * Global variables
 */

gpio_driver_t gpio =
{
    .Init               = &gpio_Init,
    .Write              = &gpio_Write,
    .Read               = &gpio_Read,
};

gpio_obj_t gpio_obj =
{
    .is_init            = false,
};

/*************************************************************************************************************************************
 * Public Functions Definition
 */

error_code_module_t gpio_Init( uint32_t gpio_dir, uint32_t gpio_init )
{
    error_code_module_t error = NO_ERROR;
    uint32_t pin;

    /* Singleton pattern */
    if ( gpio_obj.is_init == false )
    {
        for ( pin = 0; pin < 32; pin++ )
        {
            if ( gpio_dir & 0x1 )
            {
                NRF_P0_NS->DIRSET = ( 1 << pin );
            }
            else
            {
                NRF_P0_NS->DIRCLR = ( 1 << pin );
            }
            if ( gpio_init & 0x1 )
            {
                NRF_P0_NS->OUTSET = ( 1 << pin );
            }
            else
            {
                NRF_P0_NS->OUTCLR = ( 1 << pin );
            }

            gpio_dir >>= 1;
            gpio_init >>= 1;
        }
    }
    else
    {
        error = ERROR_GPIO_ALREADY_INIT;
    }

    return error;
}

void gpio_Write( uint8_t gpio_pin, uint8_t gpio_value )
{
    if ( gpio_value == 0 )
    {
        NRF_P0_NS->OUTCLR = ( 1 << gpio_pin );
    }
    else
    {
        NRF_P0_NS->OUTSET = ( 1 << gpio_pin );
    }
}

void gpio_Read( uint8_t gpio_pin, uint8_t *gpio_value )
{
    *gpio_value = NRF_P0_NS->OUT & ( 1 << gpio_pin ) ? 1 : 0;
}

/**
 * @} gpio
 */

/**
 * @} Driver
 */
