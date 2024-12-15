/** @file twim_priv.h
 *
 * @brief       Driver for two-wire interface (I2C) master module (private header).
 * @author      Johnas Cukier
 * @date        January 2023
 *
 */

/**
 * @addtogroup twim
 * @{
 */

#ifndef __TWIM_PRIV_H__
#define __TWIM_PRIV_H__

/***************************************************************************************************************************
 * Includes
 */

#include "twim.h"

/***************************************************************************************************************************
 * Private constants and macros
 */

/***************************************************************************************************************************
 * Private data structures and typedefs
 */

typedef struct
{
    bool                        is_init;
} twim_obj_t ;

/***************************************************************************************************************************
 * Private variables
 */

nrfx_twim_t twim_driver;
nrfx_twim_config_t twim_driver_config;

/***************************************************************************************************************************
 * Private prototypes (interface functions)
 */

/**
 * @brief       Initialize the module.
 * @param[in]   id           Two-wired interface id number.
 * @param[in]   scl_pin      Clock pin.
 * @param[in]   sda_pin      Data pin.
 * @return      Error code.
 */
static error_code_module_t twim_Init( uint8_t id, uint8_t scl_pin, uint8_t sda_pin );

/**
 * @brief       Two-wired interface transmit function.
 * @param[in]   addr         Two-wired interface address.
 * @param[in]   *data        Pointer to data buffer.
 * @param[in]   len          Length of data buffer.
 */
static void twim_Transmit( uint8_t addr, uint8_t *data, uint8_t len );

/**
 * @brief       Two-wired interface receive function.
 * @param[in]   addr         Two-wired interface address.
 * @param[out]  *data        Pointer to data buffer.
 * @param[in]   len          Length of data buffer.
 */
static void twim_Receive( uint8_t addr, uint8_t *data, uint8_t len );

/***************************************************************************************************************************
 * Private prototypes
 */

#endif /* __TWIM_PRIV_H__ */

/**
 * @}
 */
